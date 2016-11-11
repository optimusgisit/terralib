/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/postgis/Utils.cpp
   
  \brief Utility functions for PostgreSQL.  
*/

// TerraLib
#include "../core/utils/HexUtils.h"
#include "../core/translator/Translator.h"
#include "../core/uri/URI.h"
#include "../core/uri/Utils.h"
#include "../dataaccess/dataset/DataSet.h"
#include "../dataaccess/dataset/DataSetType.h"
#include "../datatype/ByteArray.h"
#include "EWKBWriter.h"
#include "Exception.h"
#include "Globals.h"
#include "Utils.h"

// Boost
#include <boost/lexical_cast.hpp>

// libpq
#include <libpq-fe.h>

// helper functions
namespace te
{
  namespace pgis
  {
    inline bool SetColumnDef(std::string& s, const std::string& tname, const te::dt::SimpleProperty* p, bool justDataType = false)
    {
      if(p->isAutoNumber() && p->getType() == te::dt::INT32_TYPE)
        s += "SERIAL";
      else if(p->isAutoNumber() && p->getType() == te::dt::INT64_TYPE)
        s += "BIGSERIAL";
      else
        s += tname;

      if(justDataType)
        return false;
      
      if(p->isRequired())
        s += " NOT NULL";

      if(p->getDefaultValue() && (p->isAutoNumber() == false))
      {
        s += " DEFAULT '";
        s += *(p->getDefaultValue());
        s += "'";
      }

      return p->isAutoNumber();
    }

    inline void SetColumnDef(std::string& s, const te::dt::NumericProperty* p, bool justDataType = false)
    {
      s += Globals::sm_numericTypeName;

      if(p->getPrecision() > 0)
      {
        s += "(";
        s += te::common::Convert2String(p->getPrecision());
        s += ", ";
        s += te::common::Convert2String(p->getScale());
        s += ")";
      }

      if(justDataType)
        return;

      if(p->isRequired())
        s += " NOT NULL";

      if(p->getDefaultValue())
      {
        s += " DEFAULT '";
        s += *(p->getDefaultValue());
        s += "'";
      }
    }

    inline void SetColumnDef(std::string& s, const std::string& tname, const te::dt::StringProperty* p, bool justDataType = false)
    {
      s += tname;

      if(p->size() > 0 && p->size() < 10485760)
      {
        s += "(";
        s += te::common::Convert2String(static_cast<unsigned int>(p->size()));
        s += ")";
      }      

      if(justDataType)
        return;

      if(p->isRequired())
        s += " NOT NULL";

      if(p->getDefaultValue())
      {
        s += " DEFAULT '";
        s += *(p->getDefaultValue());
        s += "'";
      }
    }

    inline void SetColumnDef(std::string& s, const te::dt::ArrayProperty* p, bool justDataType = false)
    {
      te::dt::Property* elementType = p->getElementType();
      SetColumnDef(s, elementType, true);

      s += "[]";

      if(justDataType)
        return;

      if(p->isRequired())
        s += " NOT NULL";

      if(p->getDefaultValue())
      {
        s += " DEFAULT '";
        s += *(p->getDefaultValue());
        s += "'";
      }
    }
  }
}

// Utils implementation
const std::string& te::pgis::GetGeometryName(te::gm::GeomType t)
{
  switch(t)
  {
      case te::gm::PointType:
      case te::gm::PointZType:
      case te::gm::PointZMType:
        return Globals::sm_pointTypeName;
      
      case te::gm::PointMType: 
        return Globals::sm_pointMTypeName;

      case te::gm::LineStringType:
      case te::gm::LineStringZType:
      case te::gm::LineStringZMType:
        return Globals::sm_lineStringTypeName;
      
      case te::gm::LineStringMType: 
        return Globals::sm_lineStringMTypeName; 

      case te::gm::PolygonType:
      case te::gm::PolygonZType:
      case te::gm::PolygonZMType:
        return Globals::sm_polygonTypeName;
      
      case te::gm::PolygonMType: 
        return Globals::sm_polygonMTypeName; 

      case te::gm::GeometryCollectionType:
      case te::gm::GeometryCollectionZType:
      case te::gm::GeometryCollectionZMType:
        return Globals::sm_geometryCollectionTypeName;
      
      case te::gm::GeometryCollectionMType: 
        return Globals::sm_geometryCollectionMTypeName; 

      case te::gm::MultiPointType:
      case te::gm::MultiPointZType:
      case te::gm::MultiPointZMType:
        return Globals::sm_multiPointTypeName;
      
      case te::gm::MultiPointMType: 
        return Globals::sm_multiPointMTypeName; 

      case te::gm::MultiLineStringType:
      case te::gm::MultiLineStringZType:
      case te::gm::MultiLineStringZMType:
        return Globals::sm_multiLineStringTypeName;
      
      case te::gm::MultiLineStringMType: 
        return Globals::sm_multiLineStringMTypeName; 

      case te::gm::MultiPolygonType:
      case te::gm::MultiPolygonZType:
      case te::gm::MultiPolygonZMType:
        return Globals::sm_multiPolygonTypeName;
      
      case te::gm::MultiPolygonMType: 
        return Globals::sm_multiPolygonMTypeName;

      default:
        return Globals::sm_geometryTypeName;
  }
}

bool te::pgis::SetColumnDef(std::string& s, const te::dt::Property* p, bool justDataType)
{
  int t = p->getType();

  bool ret = false;

  switch(t)
  {
    case te::dt::CHAR_TYPE:
      ret = SetColumnDef(s, Globals::sm_charTypeName, static_cast<const te::dt::SimpleProperty*>(p), justDataType);
    break;

    case te::dt::INT16_TYPE:
      ret = SetColumnDef(s, Globals::sm_int2TypeName, static_cast<const te::dt::SimpleProperty*>(p), justDataType);
    break;

    case te::dt::INT32_TYPE:
      ret = SetColumnDef(s, Globals::sm_intTypeName, static_cast<const te::dt::SimpleProperty*>(p), justDataType);
    break;

    case te::dt::INT64_TYPE:
      ret = SetColumnDef(s, Globals::sm_int8TypeName, static_cast<const te::dt::SimpleProperty*>(p), justDataType);
    break;
 
    case te::dt::NUMERIC_TYPE:
      {
        SetColumnDef(s, static_cast<const te::dt::NumericProperty*>(p), justDataType);
      }
    break;

    case te::dt::DATETIME_TYPE:
      {
        const te::dt::DateTimeProperty* dtp = static_cast<const te::dt::DateTimeProperty*>(p);
        if(dtp->getSubType() == te::dt::DATE)
          SetColumnDef(s, Globals::sm_dateTypeName, dtp, justDataType);
        else if(dtp->getSubType() == te::dt::TIME_DURATION)
          SetColumnDef(s, Globals::sm_timeTypeName, dtp, justDataType);
        else if(dtp->getSubType() == te::dt::TIME_INSTANT)
          SetColumnDef(s, Globals::sm_timeStampTypeName, dtp, justDataType);
        else if(dtp->getSubType() == te::dt::TIME_INSTANT_TZ)
          SetColumnDef(s, Globals::sm_timeStampTZTypeName, dtp, justDataType);
      }
    break;

    case te::dt::FLOAT_TYPE:
      SetColumnDef(s, Globals::sm_floatTypeName, static_cast<const te::dt::SimpleProperty*>(p), justDataType);
    break;

    case te::dt::DOUBLE_TYPE:
      SetColumnDef(s, Globals::sm_doubleTypeName, static_cast<const te::dt::SimpleProperty*>(p), justDataType);
    break;

    case te::dt::STRING_TYPE:
      {
        const te::dt::StringProperty* sp = static_cast<const te::dt::StringProperty*>(p);

        if(sp->getSubType() == te::dt::FIXED_STRING)
          SetColumnDef(s, Globals::sm_fixedcharTypeName, sp, justDataType);
        else if(sp->getSubType() == te::dt::VAR_STRING)
        {
          if(sp->size() > 10485760)
          {
            SetColumnDef(s, Globals::sm_stringTypeName, sp, justDataType);
          }
          else
          {
            SetColumnDef(s, Globals::sm_varcharTypeName, sp, justDataType);
          }
        }
        else
          SetColumnDef(s, Globals::sm_stringTypeName, sp, justDataType);
      }
    break;
             
    case te::dt::BOOLEAN_TYPE:
      SetColumnDef(s, Globals::sm_booleanTypeName, static_cast<const te::dt::SimpleProperty*>(p), justDataType);
    break;

    case te::dt::BYTE_ARRAY_TYPE:
      SetColumnDef(s, Globals::sm_byteArrayTypeName, static_cast<const te::dt::SimpleProperty*>(p), justDataType);
    break;

    case te::dt::GEOMETRY_TYPE:
      SetColumnDef(s, Globals::sm_geometryTypeName, static_cast<const te::dt::SimpleProperty*>(p), justDataType);
    break;

    case te::dt::ARRAY_TYPE:
      SetColumnDef(s, static_cast<const te::dt::ArrayProperty*>(p), justDataType);
    break;

    default:
      throw Exception(TE_TR("The informed type could not be mapped to PostgreSQL type system!"));
    break;
  }  

  return ret;
}

void te::pgis::Convert2PostGIS(PGconn* conn, const te::gm::Geometry* g, std::string& output)
{
  std::size_t size = g->getWkbSize() + 4;
  std::size_t tosize = 0;

  char* ewkb = new char[size];

  EWKBWriter::write(g, ewkb);
  unsigned char* byteArray = PQescapeByteaConn(conn, (unsigned char*)ewkb, size, &tosize);
  delete [] ewkb;

  output += "ST_GeomFromEWKB('";
  output += (char*)byteArray;
  output += "')";

  PQfreemem(byteArray);
}

void te::pgis::ScapeString(PGconn* conn, const std::string& s, std::string& output)
{
  std::size_t length = s.length() ;
  char* to = new char[2 * length + 1];
  PQescapeStringConn(conn, to, s.c_str(), length, 0);
  output += to;
  delete [] to;
}

void te::pgis::Convert2TerraLib(PGresult* result,
                                unsigned int pgisGeomTypeOid,
                                unsigned int pgisRasterTypeOid,
                                std::vector<int>& teTypes)
{
  int ncols = PQnfields(result);

  for(int i = 0; i < ncols; ++i)
  {
    std::auto_ptr<te::dt::Property> p(Convert2TerraLib(i, PQfname(result, i), PQftype(result, i),
                                                       false, 0, false, 0, -1, pgisGeomTypeOid, pgisRasterTypeOid));

    teTypes.push_back(p->getType());
  }
}

std::string te::pgis::MakeConnectionStr(const te::core::URI& connInfo)
{
  std::string connStr, aux;
  std::map<std::string, std::string> kvp = te::core::Expand(connInfo.query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  std::map<std::string, std::string>::const_iterator itend = kvp.end();

  aux = connInfo.host();
  if (!aux.empty())
    connStr += " host = " + aux;

  it = kvp.find("PG_HOST_ADDR");
  if (it != itend && !it->second.empty())
    connStr += " hostaddr = " + it->second;

  aux = connInfo.port();
  if (!aux.empty())
    connStr += " port = " + aux;

  aux = connInfo.path().substr(1, connInfo.path().length());
  if (!aux.empty())
    connStr += " dbname = " + aux;

  aux = connInfo.user();
  if (!aux.empty())
    connStr += " user = " + aux;

  aux = connInfo.password();
  if (!aux.empty())
    connStr += " password = " + aux;

  it = kvp.find("PG_CONNECT_TIMEOUT");
  if (it != itend && !it->second.empty())
    connStr += " connect_timeout = " + it->second;

  it = kvp.find("PG_OPTIONS");
  if (it != itend && !it->second.empty())
    connStr += " options = " + it->second;

  it = kvp.find("PG_SSL_MODE");
  if (it != itend && !it->second.empty())
    connStr += " sslmode = " + it->second;

  it = kvp.find("PG_KRBSRVNAME");
  if (it != itend && !it->second.empty())
    connStr += " krbsrvname = " + it->second;

  it = kvp.find("PG_GSSLIB");
  if (it != itend && !it->second.empty())
    connStr += " gsslib = " + it->second;

  return connStr;
}

void te::pgis::SplitTableName(const std::string& fullName, const std::string* defaultSchema, std::string& schemaName, std::string& tableName)
{
  assert(defaultSchema);

// split schema-name.table-name if needed
  size_t pos = fullName.find(".");

  if(pos == std::string::npos)
  {
    tableName = fullName;
    schemaName = *defaultSchema;
  }
  else
  {
    tableName = fullName.substr(pos + 1);
    schemaName = fullName.substr(0, pos);    
  }
}

std::string te::pgis::GetBindableWhereSQL(const std::vector<te::dt::Property*>& properties, const std::size_t offset)
{
  std::string wheresql;

  const std::size_t size = properties.size();

  for(std::size_t i = 0; i < size; ++i)
  {
    if(i != 0)
      wheresql += " AND ";

    wheresql += properties[i]->getName();

    wheresql += " = $" + boost::lexical_cast<std::string>(i + 1 + offset);
  }

  return wheresql;
}

std::string te::pgis::GetSQLBindValues(std::size_t nproperties)
{
  std::string valueNames("(");

  for(std::size_t i = 0; i < nproperties; ++i)
  {
    if(i != 0)
      valueNames += ",";

    valueNames += "$"  + boost::lexical_cast<std::string>(i + 1);
  }

  valueNames += ")";

  return valueNames;
}

std::string te::pgis::GetBindableUpdateSQL(const std::vector<te::dt::Property*>& properties)
{
  std::string sql;

  const std::size_t size = properties.size();

  for(std::size_t i = 0; i < size; ++i)
  {
    if(i != 0)
      sql += ", ";

    sql += properties[i]->getName();

    sql += " = $" + boost::lexical_cast<std::string>(i + 1);
  }

  return sql;
}

std::string te::pgis::GetSQLValues(const te::da::DataSetType* dt, te::da::DataSet* d, PGconn *conn)
{
  std::string values("(");

  const std::size_t np = dt->size();

  for(std::size_t i = 0; i < np; ++i)
  {
    if(i != 0)
      values += ",";

    values += GetSQLValue(dt->getProperty(i), i, d, conn);
  }

  values += ")";

  return values;
}

std::string te::pgis::GetSQLValue(const te::dt::Property* p, std::size_t propertyPos, te::da::DataSet* d, PGconn *conn)
{
  std::string value;

  switch(p->getType())
  {
    case te::dt::CHAR_TYPE :
      value += d->getChar(propertyPos);
    break;

    case te::dt::INT16_TYPE :
      value += boost::lexical_cast<std::string>(d->getInt16(propertyPos));
    break;

    case te::dt::INT32_TYPE :
      value += boost::lexical_cast<std::string>(d->getInt32(propertyPos));
    break;

    case te::dt::INT64_TYPE :
      value += boost::lexical_cast<std::string>(d->getInt64(propertyPos));
    break;

    case te::dt::BOOLEAN_TYPE :
      value += d->getBool(propertyPos) ? "1" : "0";
    break;

    case te::dt::FLOAT_TYPE :
      value += boost::lexical_cast<std::string>(d->getFloat(propertyPos));
    break;

    case te::dt::DOUBLE_TYPE :
      value += boost::lexical_cast<std::string>(d->getDouble(propertyPos));
    break;

    case te::dt::NUMERIC_TYPE :
      value += d->getNumeric(propertyPos);
    break;

    case te::dt::STRING_TYPE :
      {
        std::string dvalue = d->getString(propertyPos);

        char* valueto = new char[dvalue.length() * 2  + 1];

        int pgerror = 0;

        PQescapeStringConn(conn, valueto, dvalue.c_str(), dvalue.length(), &pgerror);

        if(pgerror != 0)
        {
          delete [] valueto;
          throw Exception(TE_TR("Could not escape string!"));
        }

        value += "'";
        value += valueto;
        value += "'";

        delete [] valueto;
      }
    break;

    case te::dt::BYTE_ARRAY_TYPE :
      {
        //std::auto_ptr<te::dt::ByteArray> ba(d->getByteArray(propertyPos));
        //char* hexba = new char[ba->bytesUsed() * 2  + 1];
        //te::common::Binary2Hex(ba->getData(), ba->bytesUsed(), hexba);

        //value += "E'\\\\x";
        //value += hexba;
        //value += "'";

        //delete [] hexba;

        std::auto_ptr<te::dt::ByteArray> ba(d->getByteArray(propertyPos));

        std::size_t tolen;

        unsigned char* pgba = PQescapeByteaConn(conn, (const unsigned char*)(ba->getData()), ba->bytesUsed(), &tolen);

        value += "'";
        value += (char*)pgba;
        value += "'";

        PQfreemem(pgba);
      }
    break;

    case te::dt::GEOMETRY_TYPE :
      {
        std::auto_ptr<te::gm::Geometry> geom(d->getGeometry(propertyPos));

// get ewkb
        std::size_t ewkbsize = geom->getWkbSize() + 4;

        char* ewkb = new char[ewkbsize];

        EWKBWriter::write(geom.get(), ewkb);

        char* hewkb = new char[ewkbsize * 2  + 1];

        te::core::Binary2Hex(ewkb, ewkbsize, hewkb);

        //value += "GeomFromEWKB(E'\\\\x";
        value += "'";
        value += hewkb;
        value += "'";
        //value += "')";

        delete [] ewkb;
        delete [] hewkb;
      }
    break;

    case te::dt::DATETIME_TYPE :
      {
        std::auto_ptr<te::dt::DateTime> dt(d->getDateTime(propertyPos));
        value += "'";
        value += dt->toString();
        value += "'";
      }
    break;

    default :
      throw Exception(TE_TR("The TerraLib data type is not supported by the PostgreSQL driver!"));
  }

  return value;
}

std::string te::pgis::GetLoadDataRow(const te::da::DataSetType* dt, te::da::DataSet* d, PGconn *conn)
{
  std::string values;

  const std::size_t np = dt->size();

  for(std::size_t i = 0; i < np; ++i)
  {
    if(i != 0)
      values += ",";

    values += GetSQLValue(dt->getProperty(i), i, d, conn);
  }

  values += "\n";

  return values;
}
