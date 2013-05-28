/*  Copyright (C) 2011-2012 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled applications.

    TerraLib is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraLib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with TerraLib. See COPYING. If not, write to
    TerraLib Team at <terralib-team@terralib.org>.
 */

/*!
  \file terralib/sqlite/Utils.cpp

  \brief Utility functions for the TerraLib SQLite Data Access driver.
*/

// TerraLib
#include "../common/Globals.h"
#include "../common/StringUtils.h"
#include "../common/Translator.h"
#include "../dataaccess/dataset/DataSet.h"
#include "../dataaccess/dataset/DataSetType.h"
#include "../dataaccess/dataset/ForeignKey.h"
#include "../dataaccess/dataset/PrimaryKey.h"
#include "../dataaccess/dataset/UniqueKey.h"
#include "../dataaccess/datasource/DataSource.h"
#include "../datatype/ByteArray.h"
#include "../datatype/DateTimeProperty.h"
#include "../datatype/Enums.h"
#include "../datatype/NumericProperty.h"
#include "../datatype/Property.h"
#include "../datatype/SimpleProperty.h"
#include "../datatype/StringProperty.h"
#include "../geometry/Envelope.h"
#include "../geometry/Geometry.h"
#include "../geometry/GeometryProperty.h"
#include "Exception.h"
#include "EWKBSize.h"
#include "EWKBWriter.h"
#include "Utils.h"

// STL
#include <cassert>
#include <cstring>
#include <fstream>
#include <memory>

// Boost
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

// SQLite
#include <sqlite3.h>

#ifdef TE_ENABLE_SPATIALITE
// SpatiaLite
#include <spatialite/gaiageo.h>
#endif

void te::sqlite::ExecuteScript(sqlite3* db, const char* fileName)
{
  std::ifstream istr;

  istr.open(fileName, std::ios_base::in);

  if(!istr.is_open())
    throw Exception((boost::format(TR_SQLITE("Could not open file: %1%.")) % fileName).str()); 

  try
  {
    PerformCommands(db, istr);
  }
  catch(...)
  {
    istr.close();
    throw;
  }

  istr.close();
}

void te::sqlite::PerformCommands(sqlite3* db, std::istream& istr)
{
  char* buff = (char*)malloc(TE_SQLITE_BUFFER_SCRIPT_SIZE);    // used to buffer a line of SQL command
  char* sql = (char*)malloc(TE_SQLITE_BUFFER_SCRIPT_SIZE);     // used to buffer an entirely SQL command
  size_t buffsize = TE_SQLITE_BUFFER_SCRIPT_SIZE;              // line buffer size (may grow)
  size_t sqlSize = TE_SQLITE_BUFFER_SCRIPT_SIZE;               // sql buffer size (may grow)
  size_t offset = 0;                                        // point to the first available byte in the sql buffer
  size_t priorOffset = 0;                                   // the prior offset value

  while(!istr.eof())
  {
    std::streamsize readbytes = ReadLine(istr, &buff, buffsize);

    if(readbytes == 0)
      break;  // reached end-of-stream

    if(IsAllWhitespace(buff))
      continue;

    if(IsCommandTerminator(buff) && IsComplete(&sql, offset, sqlSize))
      memcpy(buff, ";", 2); // let's change Oracle or SQL Server termination command syntax by SQLite's
    
    std::size_t lineLen = strlen(buff);

    if((lineLen + 2) > (sqlSize - offset))
    {
// we need to reallocate the sql memory
      sql = (char*)realloc(sql, sqlSize + buffsize);
      sqlSize = sqlSize + buffsize;
    }

// if there was a previous line of command, let's insert a \n at the end of it
    if(offset != 0)
      sql[offset++] = '\n';

// copy the line data to the sql buffer
    memcpy(&sql[offset], buff, lineLen + 1);  // we always copy the trailing '\0' in order to make the sql buffer NULL terminated
    priorOffset = offset;
    offset += lineLen;

// we need to check if the string is a complete SQL statement.
    if(offset && ContainsSemicolon(sql + priorOffset, lineLen) && sqlite3_complete(sql))
    {
      try
      {
        PerformCommands(db, sql);
        priorOffset = offset = 0;
      }
      catch(...)
      {
        free(buff);
        free(sql);

        throw;
      }
    }
  }

  if(offset && !IsAllWhitespace(sql))
  {
    boost::format errmsg(TR_SQLITE("Error! incomplete SQL: %1%"));

    errmsg = errmsg % sql;

    free(buff);
    free(sql);

    throw Exception(errmsg.str());
  }


  free(buff);
  free(sql);
}

void te::sqlite::PerformCommands(sqlite3* db, const char* sql)
{
  sqlite3_stmt *pStmt = 0;      // a pointer to a compiled prepared statement that can be executed using sqlite3_step()
  int rc = SQLITE_OK;           // store the result of executing the queries
  const char* sqlLeftOver = 0;  // tail of unprocessed SQL

  while(sql[0] != '\0' && rc == SQLITE_OK)
  {
    rc = sqlite3_prepare_v2(db, sql, -1, &pStmt, &sqlLeftOver);

    if(rc != SQLITE_OK)
      throw Exception((boost::format(TR_SQLITE("Error preparing the SQL statement. The follwoing error has occurried: %1%.")) % sqlite3_errmsg(db)).str()); 

    if(pStmt == 0) // we need to see if pStmt is NULL beacuse of comments or white-spaces
    {      
      sql = sqlLeftOver;

      while(isspace(sql[0]))
        ++sql;

      continue;
    }

// let's execute the statement
    rc = sqlite3_step(pStmt);

    sqlite3_finalize(pStmt);

    if(rc != SQLITE_ROW && rc != SQLITE_OK && rc != SQLITE_DONE)
    {
      std::string errmsg  = TR_SQLITE("Error processing the SQL statement. The follwoing error has occurried: ");
                  errmsg += sqlite3_errmsg(db);
                  errmsg += ".";

      throw te::common::Exception(errmsg);
    }

    sql = sqlLeftOver;

    while(isspace(sql[0]))
      ++sql;

    rc = SQLITE_OK;
  }
}

std::streamsize te::sqlite::ReadLine(std::istream& istr, char** pbuff, std::size_t& buffsize/*, std::size_t offset*/)
{
  std::streamsize readbytes = 0;

  char* buff = *pbuff;

  do
  {
    istr.getline(buff /*+ offset*/ + readbytes, static_cast<std::streamsize>(buffsize - readbytes) /*- offset*/);

    if(istr.flags() & std::ios::failbit)
    {
      buffsize += TE_SQLITE_BUFFER_SCRIPT_SIZE;
      buff = (char*)realloc(buff, buffsize);
      *pbuff = buff;
      readbytes += istr.gcount();
    }
    else
    {
      readbytes = istr.gcount();
      break;
    }
  }while(true);

  return readbytes;
}

bool te::sqlite::IsAllWhitespace(const char* sqlLine)
{
  for(; *sqlLine; ++sqlLine)
  {
    if(isspace(*(unsigned char*)sqlLine))
      continue;

    if((*sqlLine == '/') && (sqlLine[1] == '*'))
    {
      sqlLine += 2;

      while(*sqlLine && ((*sqlLine != '*') || (sqlLine[1] != '/')))
        ++sqlLine;
    
      if(*sqlLine == 0)
        return false;

      ++sqlLine;

      continue;
    }

    if((*sqlLine == '-') && (sqlLine[1] == '-'))
    {
      sqlLine += 2;
      
      while(*sqlLine && (*sqlLine != '\n'))
        ++sqlLine;

      if(*sqlLine == 0)
        return true;

      continue;
    }

    return false;
  }

  return true;
}

bool te::sqlite::IsCommandTerminator(const char* sqlLine)
{
  while(isspace(*(unsigned char*)sqlLine))
    ++sqlLine;

  if((sqlLine[0] == '/') && IsAllWhitespace(&sqlLine[1]))
    return true;  /* Oracle Style*/

  if(tolower((sqlLine[0]) == 'g') && (tolower(sqlLine[1]) == 'o') && IsAllWhitespace(&sqlLine[2]))
    return true;  /* SQL Server Style */

  return false;
}

bool te::sqlite::ContainsSemicolon(const char* sql, std::size_t nbytes)
{
  for(std::size_t i = 0; i < nbytes; ++i)
    if(sql[i] == ';')
      return true;

  return false;
}

bool te::sqlite::IsComplete(char** sql, size_t len, std::size_t& buffsize)
{
  char* psql = *sql;

  if(len == 0)
    return true;

  if(len + 2 > buffsize)
  {
    buffsize += TE_SQLITE_BUFFER_SCRIPT_SIZE;
    psql = (char*)realloc(psql, buffsize);
    *sql = psql;
  }

  psql[len] = ';';
  psql[len + 1] = 0;

  int rc = sqlite3_complete(psql);

  psql[len] = '\0';

  return rc != 0;
}

te::dt::Property* te::sqlite::Convert2TerraLib(int colId,
                                               const std::string& colName,
                                               const std::string& colType,
                                               bool required,
                                               std::string* defaultValue)
{
  te::dt::Property* p = 0;

  std::string uColType = te::common::Convert2UCase(colType);

  if((uColType == "INT") ||
     (uColType == "INTEGER") ||
     (uColType == "TINYINT") ||
     (uColType == "SMALLINT") ||
     (uColType == "MEDIUMINT") ||
     (uColType == "BIGINT") ||
     (uColType == "UNSIGNED BIG INT") ||
     (uColType == "INT2") ||
     (uColType == "INT8"))
  {
    p = new te::dt::SimpleProperty(colName, te::dt::INT64_TYPE, required, defaultValue, colId);
  }
  else if((uColType == "FLOAT")  ||
          (uColType == "DOUBLE") ||
          (uColType == "REAL") ||
          (uColType == "DOUBLE PRECISION"))
  {
    p = new te::dt::SimpleProperty(colName, te::dt::DOUBLE_TYPE, required, defaultValue, colId);
  }
  else if((uColType == "NUMERIC") ||
          (uColType.substr(0, 7) == "DECIMAL"))
  {
    p = new te::dt::NumericProperty(colName, 0, 0, required, defaultValue, colId);
  }
  else if((uColType == "TEXT") ||
          (uColType == "CLOB") ||
          (uColType.substr(0, 4) == "CHAR") ||
          (uColType.substr(0, 7) == "VARCHAR"))
  {
    p = new te::dt::StringProperty(colName, te::dt::STRING, 0, required, defaultValue, colId);
  }
  else if(uColType == "BLOB")
  {
    p = new te::dt::SimpleProperty(colName, te::dt::BYTE_ARRAY_TYPE, required, defaultValue, colId);
  }
  else if(te::gm::Geometry::isGeomType(uColType))
  {
    p = new te::gm::GeometryProperty(colName, -1, te::gm::GeometryType, required, defaultValue, colId, 0);
  }
  else if(uColType == "DATE")
  {
    p = new te::dt::DateTimeProperty(colName, te::dt::DATE, te::dt::UNKNOWN, required, defaultValue, colId);
  }
  else if(uColType == "DATETIME")
  {
    p = new te::dt::DateTimeProperty(colName, te::dt::TIME_INSTANT, te::dt::UNKNOWN, required, defaultValue, colId);
  }
  else if((uColType == "BOOL") ||
     (uColType == "BOOLEAN"))
  {
    p = new te::dt::SimpleProperty(colName, te::dt::BOOLEAN_TYPE, required, defaultValue, colId);
  }
  else// if(uColType == "NULL")
  {
    p = new te::dt::SimpleProperty(colName, te::dt::UNKNOWN_TYPE, required, defaultValue, colId);
  }

  return p;
}

te::da::DataSetType* te::sqlite::Convert2TerraLib(sqlite3_stmt* pStmt)
{
  te::da::DataSetType* dt = new te::da::DataSetType("", 0);

  const int ncols = sqlite3_column_count(pStmt);

  for(int i = 0; i < ncols; ++i)
  {
    const char* t = sqlite3_column_decltype(pStmt, i);
    const char* name = sqlite3_column_name(pStmt, i);

    te::dt::Property* p = Convert2TerraLib(i, name, t != 0 ? t : "", false);

    dt->add(p);
  }

  return dt;
}

int te::sqlite::IsRasterTable(const std::set<std::string>& rasters, const std::string& tblName)
{
  std::size_t pos = tblName.rfind("_metadata");
      
  if(pos != std::string::npos)
  {
    std::string tblPrefix = tblName.substr(0, pos);

    if(rasters.find(tblPrefix) != rasters.end())
      return TE_SQLITE_IS_RASTER_METADATA_TABLE;
  }

  pos = tblName.rfind("_rasters");
      
  if(pos != std::string::npos)
  {
    std::string tblPrefix = tblName.substr(0, pos);

    if(rasters.find(tblPrefix) != rasters.end())
      return TE_SQLITE_IS_RASTER_TABLE;
  }

  return TE_SQLITE_IS_NOT_RASTER_TABLE;
}

int te::sqlite::GetConnectionFlags(const std::map<std::string, std::string>& connInfo)
{
  int flags = 0;

  if((connInfo.find("SQLITE_OPEN_NOMUTEX") != connInfo.end()) &&
     (te::common::Convert2UCase(connInfo.find("SQLITE_OPEN_NOMUTEX")->second) == "TRUE"))
    flags |= SQLITE_OPEN_NOMUTEX;

  if((connInfo.find("SQLITE_OPEN_FULLMUTEX") != connInfo.end()) &&
     (te::common::Convert2UCase(connInfo.find("SQLITE_OPEN_FULLMUTEX")->second) == "TRUE"))
    flags |= SQLITE_OPEN_FULLMUTEX;

  if((connInfo.find("SQLITE_OPEN_SHAREDCACHE") != connInfo.end()) &&
     (te::common::Convert2UCase(connInfo.find("SQLITE_OPEN_SHAREDCACHE")->second) == "TRUE"))
    flags |= SQLITE_OPEN_SHAREDCACHE;

  if((connInfo.find("SQLITE_OPEN_PRIVATECACHE") != connInfo.end()) &&
     (te::common::Convert2UCase(connInfo.find("SQLITE_OPEN_PRIVATECACHE")->second) == "TRUE"))
    flags |= SQLITE_OPEN_PRIVATECACHE;

  if((connInfo.find("SQLITE_OPEN_READWRITE") != connInfo.end()) &&
     (te::common::Convert2UCase(connInfo.find("SQLITE_OPEN_READWRITE")->second) == "TRUE"))
    flags |= SQLITE_OPEN_READWRITE;
  else if((connInfo.find("SQLITE_OPEN_READONLY") != connInfo.end()) &&
     (te::common::Convert2UCase(connInfo.find("SQLITE_OPEN_READONLY")->second) == "TRUE"))
    flags |= SQLITE_OPEN_READONLY;

  if((connInfo.find("SQLITE_OPEN_CREATE") != connInfo.end()) &&
     (te::common::Convert2UCase(connInfo.find("SQLITE_OPEN_CREATE")->second) == "TRUE"))
    flags |= SQLITE_OPEN_CREATE;

  if((connInfo.find("SQLITE_OPEN_URI") != connInfo.end()) &&
     (te::common::Convert2UCase(connInfo.find("SQLITE_OPEN_URI")->second) == "TRUE"))
    flags |= SQLITE_OPEN_CREATE;

  return flags;
}

bool te::sqlite::Exists(const std::map<std::string, std::string>& dbInfo)
{
// get any additional flag
  int flags = GetConnectionFlags(dbInfo);

// assure at least read-only is present
  if(flags == 0)
    flags = SQLITE_OPEN_READONLY;

// removes any creational bit if present
  flags &= 0xFFFFFFFB;

// get file name (remember: it can not be a null value!)
  std::string fileName;

  {
    std::map<std::string, std::string>::const_iterator it = dbInfo.find("SQLITE_FILE");

    if(it != dbInfo.end())
      fileName = it->second;
    else
      throw Exception(TR_SQLITE("To check the existence of an SQLite database you must inform its file name or URI!"));
  }

// get VFS parameter
  std::string vfs;

  {
    std::map<std::string, std::string>::const_iterator it = dbInfo.find("SQLITE_VFS");

    if(it != dbInfo.end())
      vfs = it->second;
  }

// check database existence
  sqlite3* db = 0;

  int ret = sqlite3_open_v2(fileName.c_str(), &db, flags, vfs.empty() ? 0 : vfs.c_str());

  sqlite3_close(db);

  return (ret == SQLITE_OK);
}

void te::sqlite::GetHiddenTables(const te::da::DataSource* ds, std::vector<std::string>& tables)
{
  const std::map<std::string, std::string>& connInfo = ds->getConnectionInfo();

  std::map<std::string, std::string>::const_iterator it = connInfo.find("SQLITE_HIDE_SPATIAL_METADATA_TABLES");
  std::map<std::string, std::string>::const_iterator itend = connInfo.end();

  if((it != itend) && (te::common::Convert2UCase(it->second) == "TRUE"))
  {
    tables.push_back("geom_cols_ref_sys");
    tables.push_back("geometry_columns");
    tables.push_back("geometry_columns_auth");
    tables.push_back("spatial_ref_sys");
    tables.push_back("views_geometry_columns");
    tables.push_back("virts_geometry_columns");
  }

  it = connInfo.find("SQLITE_HIDE_TABLES");

  if((it != itend) && (it->second.empty() == false))
    te::common::Tokenize(it->second, tables, ",");

  return;
}

int te::sqlite::Convert2TerraLibCategory(const std::string& category)
{
  if(category == "table")
    return te::da::TABLE_TYPE;
  else if(category == "view")
    return te::da::VIEW_TYPE;
  else if(category == "index")
    return te::da::INDEX_TYPE;
  else if(category == "trigger")
    return te::da::TRIGGER_TYPE;
  else
    return te::da::UNKNOWN_DATASET_TYPE;
}

te::gm::GeomType te::sqlite::Convert2TerraLibGeomType(const std::string& geomType, const std::string& dimension)
{
  int gtype = 0;

#ifdef TE_ENABLE_SPATIALITE
  if(geomType == "MULTIPOLYGON")
  {
    gtype = GAIA_MULTIPOLYGON;
  }
  else if(geomType == "POINT")
  {
    gtype = GAIA_POINT;
  }
  else if(geomType == "LINESTRING")
  {
    gtype = GAIA_LINESTRING;
  }
  else if(geomType == "POLYGON")
  {
    gtype = GAIA_POLYGON;
  }
  else if(geomType == "MULTILINESTRING")
  {
    gtype = GAIA_MULTILINESTRING;
  }
  else if(geomType == "MULTIPOINT")
  {
    gtype = GAIA_MULTIPOINT;
  }
  else if(geomType == "GEOMETRYCOLLECTION")
  {
    gtype = GAIA_GEOMETRYCOLLECTION;
  }
  else if(geomType == "GEOMETRY")
  {
    gtype = te::gm::GeometryType;
  }
  else
    throw Exception(TR_SQLITE("TerraLib SpatiaLite driver doesn't support this geometric type!"));

  if(dimension == "XY" || dimension == "2")
  {
    return static_cast<te::gm::GeomType>(gtype);
  }
  else if(dimension == "XYZ" || dimension == "3")
  {
    return static_cast<te::gm::GeomType>(gtype + 1000);
  }
  else if(dimension == "XYM")
  {
    return static_cast<te::gm::GeomType>(gtype + 2000);
  }
  else if(dimension == "XYZM")
  {
    return static_cast<te::gm::GeomType>(gtype + 3000);
  }
  else
    throw Exception(TR_SQLITE("TerraLib SpatiaLite driver doesn't support this dimension!"));
#else
  return static_cast<te::gm::GeomType>(gtype);
#endif  // TE_ENABLE_SPATIALITE
}

void te::sqlite::Convert2SpatiaLiteGeom(const te::gm::GeomType t, std::string& geomType, std::string& dimension)
{
  switch(t)
  {
    case te::gm::PointType :
    case te::gm::PointZType :
    case te::gm::PointMType :
    case te::gm::PointZMType :
      geomType = "POINT";
    break;

    case te::gm::LineStringType :
    case te::gm::LineStringZType :
    case te::gm::LineStringMType :
    case te::gm::LineStringZMType :
      geomType = "LINESTRING";
    break;

    case te::gm::PolygonType :
    case te::gm::PolygonZType :
    case te::gm::PolygonMType :
    case te::gm::PolygonZMType :
      geomType = "POLYGON";
    break;

    case te::gm::MultiPointType :
    case te::gm::MultiPointZType :
    case te::gm::MultiPointMType :
    case te::gm::MultiPointZMType :
      geomType = "MULTIPOINT";
    break;

    case te::gm::MultiLineStringType :
    case te::gm::MultiLineStringZType :
    case te::gm::MultiLineStringMType :
    case te::gm::MultiLineStringZMType :
      geomType = "MULTILINESTRING";
    break;

    case te::gm::MultiPolygonType :
    case te::gm::MultiPolygonZType :
    case te::gm::MultiPolygonMType :
    case te::gm::MultiPolygonZMType :
      geomType = "MULTIPOLYGON";
    break;

    case te::gm::GeometryCollectionType :
    case te::gm::GeometryCollectionZType :
    case te::gm::GeometryCollectionMType :
    case te::gm::GeometryCollectionZMType :
      geomType = "GEOMETRYCOLLECTION";
    break;

    case te::gm::GeometryType :
    case te::gm::GeometryZType :
    case te::gm::GeometryMType :
    case te::gm::GeometryZMType :
      geomType = "GEOMETRY";
    break;

    default:
      throw Exception(TR_SQLITE("This geometric type is not supported by SpatiaLite!"));
  }

  if((t & 0xF00) == 0xB00)    // it is zm
    dimension = "XYZM";
  else if((t & 0x0F00) == 0x300)   // it is z
    dimension = "XYZ";
  else if((t & 0xF00) == 0x700)  // it is m
    dimension = "XYM";
  else
    dimension = "XY";
}

std::string te::sqlite::Convert2SQLCreate(const te::dt::Property* p)
{
  std::string sql = p->getName();

  sql += " ";

  sql += GetSQLType(p);

  const te::dt::SimpleProperty* sp = static_cast<const te::dt::SimpleProperty*>(p);

  if(sp->isRequired())
    sql += " NOT NULL";

  if(p->getParent())
  {
    const te::da::DataSetType* dt = static_cast<const te::da::DataSetType*>(p->getParent());

    if(dt->getPrimaryKey() && (dt->getPrimaryKey()->getProperties().size() == 1) && dt->getPrimaryKey()->has(p))
    {
      sql += " PRIMARY KEY";
    }
  }

  return sql;
}

std::string te::sqlite::GetSQLType(const te::dt::Property* p)
{
  std::string sql;

  switch(p->getType())
  {
    case te::dt::CHAR_TYPE :
    case te::dt::UCHAR_TYPE :
    case te::dt::INT16_TYPE :
    case te::dt::INT32_TYPE :
    case te::dt::INT64_TYPE :
      sql = "INTEGER";
    break;

    case te::dt::FLOAT_TYPE :
    case te::dt::DOUBLE_TYPE :
      sql = "REAL";
    break;
    
    case te::dt::STRING_TYPE :
      sql = "TEXT";
    break;

    case te::dt::NUMERIC_TYPE :
      sql = "NUMERIC";
    break;

    case te::dt::BYTE_ARRAY_TYPE :
      sql = "BLOB";
    break;

    case te::dt::DATETIME_TYPE :
      sql = static_cast<const te::dt::DateTimeProperty*>(p)->getSubType() == te::dt::TIME_INSTANT ? "DATETIME" : "DATE";
    break;

    case te::dt::GEOMETRY_TYPE :
    {
      const te::gm::GeometryProperty* gp = static_cast<const te::gm::GeometryProperty*>(p);

      std::string gtype;
      std::string cdim;

      Convert2SpatiaLiteGeom(gp->getGeometryType(), gtype, cdim);

      sql = gtype;
    }
    break;

    default:
      throw Exception((boost::format(TR_SQLITE("The TerraLib data type %1% can not be converted to SQLite type system!")) % p->getType()).str());
  }

  return sql;
}

std::string te::sqlite::GetSQLValueNames(const te::da::DataSetType* dt)
{
  std::string valueNames("(");

  const std::size_t np = dt->size();

  for(std::size_t i = 0; i < np; ++i)
  {
    if(i != 0)
      valueNames += ",";

    valueNames += dt->getProperty(i)->getName();
  }

  valueNames += ")";

  return valueNames;
}

std::string te::sqlite::GetBindableWhereSQL(std::vector<te::dt::Property*>::const_iterator itBegin, std::vector<te::dt::Property*>::const_iterator itEnd)
{
  std::string wheresql;

  int i = 0;

  while(itBegin != itEnd)
  {
    if(i != 0)
      wheresql += " AND ";

    wheresql += (*itBegin)->getName();

    wheresql += " = ?";

    ++itBegin;
    ++i;
  }

  return wheresql;
}

std::string te::sqlite::GetBindableUpdateSQL(const std::vector<te::dt::Property*>& properties)
{
  std::string sql;

  const std::size_t size = properties.size();

  for(std::size_t i = 0; i < size; ++i)
  {
    if(i != 0)
      sql += ", ";

    sql += properties[i]->getName();

    sql += " = ?";
  }

  return sql;
}

std::string te::sqlite::GetSQLBindValues(const te::da::DataSet* dataset)
{
  std::string valueNames("(");

  const std::size_t np = dataset->getNumProperties();

  for(std::size_t i = 0; i != np; ++i)
  {
    if(i != 0)
      valueNames += ",";

    valueNames += "?";
  }

  valueNames += ")";

  return valueNames;
}

std::string te::sqlite::GetBindableSpatialRelation(const std::string& colName, const te::gm::SpatialRelation r)
{
  std::string filter;

  switch(r)
  {
    case te::gm::INTERSECTS :
      filter = "Intersects(";
    break;

    case te::gm::WITHIN :
      filter = "Within(";
    break;

    case te::gm::CONTAINS :
      filter = "Contains(";
    break;

    case te::gm::TOUCHES :
      filter = "Touches(";
    break;

    case te::gm::EQUALS :
      filter = "Equals(";
    break;

    case te::gm::OVERLAPS :
      filter = "Overlaps(";
    break;

    case te::gm::CROSSES :
      filter = "Crosses(";
    break;

    case te::gm::DISJOINT :
      filter = "Disjoint(";
    break;

    default:
      throw Exception(TR_SQLITE("Invalid rectangle relation for SQLite driver!"));
  }

  filter += colName;

  filter += ", ?)";

  return filter;
}

std::string te::sqlite::GetRtreeFilter(const te::gm::Envelope* e, const te::gm::SpatialRelation r)
{
  std::string filter;

  switch(r)
  {
    case te::gm::INTERSECTS :
    case te::gm::TOUCHES :
    case te::gm::OVERLAPS :
    case te::gm::CROSSES :
    case te::gm::EQUALS :
      filter = "RTreeIntersects(";
    break;

    case te::gm::WITHIN :
      filter = "RTreeWithin(";
    break;

    case te::gm::CONTAINS :
      filter = "RTreeContain(";
    break;

    default:
      throw Exception(TR_SQLITE("Invalid rectangle relation for SQLite driver!"));
  }

  filter += boost::lexical_cast<std::string>(e->m_llx);
  filter += ", ";
  filter += boost::lexical_cast<std::string>(e->m_lly);
  filter += ", ";
  filter += boost::lexical_cast<std::string>(e->m_urx);
  filter += ", ";
  filter += boost::lexical_cast<std::string>(e->m_ury);
  filter += ")";

  return filter;
}

te::da::FKActionType te::sqlite::GetAction(const std::string& action)
{
  if(action == "NO ACTION")
  {
    return te::da::NO_ACTION;
  }
  else if(action == "RESTRICT")
  {
    return te::da::RESTRICT;
  }
  else if(action == "CASCADE")
  {
    return te::da::CASCADE;
  }
  else if(action == "SET NULL")
  {
    return te::da::SET_NULL;
  }
  else //if(action == "SET DEFAULT")
  {
    return te::da::SET_DEFAULT;
  }
}

