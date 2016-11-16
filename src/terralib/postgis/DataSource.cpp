/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled applications.

    TerraLib is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published byF
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
  \file terralib/postgis/DataSource.cpp

  \brief Implementation of the data source for the PostGIS driver.
*/

// TerraLib
#include "../common/StringUtils.h"
#include "../core/translator/Translator.h"
#include "../core/uri/URI.h"
#include "../core/uri/Utils.h"
#include "../dataaccess/dataset/CheckConstraint.h"
#include "../dataaccess/dataset/DataSet.h"
#include "../dataaccess/dataset/ForeignKey.h"
#include "../dataaccess/dataset/Index.h"
#include "../dataaccess/dataset/PrimaryKey.h"
#include "../dataaccess/dataset/Sequence.h"
#include "../dataaccess/dataset/UniqueKey.h"
#include "../dataaccess/datasource/ScopedTransaction.h"
#include "../dataaccess/query/Select.h"
#include "../dataaccess/query/SQLDialect.h"
#include "../dataaccess/utils/Utils.h"
#include "../datatype/Array.h"
#include "../datatype/Property.h"
#include "../datatype/SimpleData.h"
#include "../geometry/GeometryProperty.h"
#include "../geometry/Utils.h"
#include "../raster/Grid.h"
#include "../raster/BandProperty.h"
#include "../raster/RasterProperty.h"
#include "../geometry/Geometry.h"
#include "Connection.h"
#include "ConnectionPool.h"
#include "DataSource.h"
#include "DataSet.h"
#include "Exception.h"
#include "Globals.h"
#include "PreparedQuery.h"
#include "SQLVisitor.h"
#include "Transactor.h"
#include "Utils.h"

// STL
#include <cassert>
#include <memory>

// Boost
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

// libpq
#include <libpq-fe.h>

te::pgis::DataSource::DataSource(const std::string& connInfo)
  : te::da::DataSource(connInfo),
  m_pool(0),
  m_geomTypeOid(0),
  m_rasterTypeOid(0),
  m_timeIsInteger(true)
{
  m_pool = new ConnectionPool(this);
}

te::pgis::DataSource::DataSource(const te::core::URI& uri)
  : te::da::DataSource(uri),
    m_pool(0),
    m_geomTypeOid(0),
    m_rasterTypeOid(0),
    m_timeIsInteger(true)
{
  m_pool = new ConnectionPool(this);
}

te::pgis::DataSource::~DataSource()
{
  delete m_pool;
}

std::string te::pgis::DataSource::getType() const
{
  return PGIS_DRIVER_IDENTIFIER;
}

std::auto_ptr<te::da::DataSourceTransactor> te::pgis::DataSource::getTransactor()
{
  Connection* conn = m_pool->getConnection();

  return std::auto_ptr<te::da::DataSourceTransactor>(new te::pgis::Transactor(this, conn));
}

te::pgis::Connection* te::pgis::DataSource::getConnection()
{
  return m_pool->getConnection();
}

 void te::pgis::DataSource::closeConnection(Connection* conn)
{
  m_pool->release(conn);
}

bool te::pgis::DataSource::isTimeAnInteger()
{
  return m_timeIsInteger;
}

void te::pgis::DataSource::setTimeAsInteger(bool timeIsInteger)
{
  m_timeIsInteger = timeIsInteger;
}

void te::pgis::DataSource::open()
{
  // Assure we are in a closed state
  close();

  m_pool->initialize();

  std::auto_ptr<te::da::DataSourceTransactor> t = getTransactor();
  te::pgis::Transactor* pgt = static_cast<te::pgis::Transactor*>(t.get());

  // Find the PostGIS types
  m_geomTypeOid = pgt->getGeomTypeId();
  m_rasterTypeOid = pgt->getRasterTypeId();

  // Find the PostgreSQL current schema of the connection
  pgt->getDatabaseInfo(m_currentSchema);
}

void te::pgis::DataSource::close()
{
  m_pool->finalize();
}

bool te::pgis::DataSource::isOpened() const
{
  return m_pool->isInitialized();
}

bool te::pgis::DataSource::isValid() const
{
  return m_pool->isValid();
}

const te::da::DataSourceCapabilities& te::pgis::DataSource::getCapabilities() const
{
  return *te::pgis::Globals::sm_capabilities;
}

const te::da::SQLDialect* te::pgis::DataSource::getDialect() const
{
  return te::pgis::Globals::sm_queryDialect;
}

unsigned int te::pgis::DataSource::getGeomTypeId() const
{
  return m_geomTypeOid;
}

unsigned int te::pgis::DataSource::getRasterTypeId() const
{
  return m_rasterTypeOid;
}

const std::string& te::pgis::DataSource::getCurrentSchema() const
{
  return m_currentSchema;
}

te::pgis::ConnectionPool* te::pgis::DataSource::getConnPool() const
{
  return m_pool;
}

void te::pgis::DataSource::create(const std::string& connInfo)
{
  // get an auxiliary data source to create the new database
  std::auto_ptr<DataSource> dsPGIS(new DataSource(connInfo));
  dsPGIS->open();

  std::map<std::string, std::string> kvp = te::core::Expand(dsPGIS->getConnectionInfo().query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  std::map<std::string, std::string>::const_iterator itend = kvp.end();

// create a database based on the connection information
  std::string sql = "CREATE DATABASE ";

  it = kvp.find("PG_NEWDB_NAME");
  if (it != itend && !it->second.empty())
    sql += "\"" + it->second + "\"";
  else
    throw Exception(TE_TR("The database could not be created due the missing parameter: PG_NEWDB_NAME!"));

  it = kvp.find("PG_NEWDB_TEMPLATE");
  if(it != itend && !it->second.empty())
    sql += " TEMPLATE = " + it->second;

  it = kvp.find("PG_NEWDB_OWNER");
  if (it != itend && !it->second.empty())
    sql += " OWNER = " + it->second;

  sql += " ENCODING = '" + te::core::CharEncoding::getEncodingName(te::core::EncodingType::UTF8) + "'";

  it = kvp.find("PG_NEWDB_TABLESPACE");
  if (it != itend && !it->second.empty())
    sql += " TABLESPACE = " + it->second;

  it = kvp.find("PG_NEWDB_CONN_LIMIT");
  if (it != itend && !it->second.empty())
    sql += " CONNECTION LIMIT = " + it->second;

  dsPGIS->execute(sql);
  
  // check if new database has postgis extension enabled
  std::auto_ptr<te::da::DataSet> result(dsPGIS->query("SELECT extname, extversion FROM pg_extension WHERE extname = 'postgis'"));

  if (!result->moveNext())
  {
    // no PostGIS extension found, let's try to enable it!
    dsPGIS->execute("CREATE EXTENSION postgis");
  }

  dsPGIS->close();
}

void te::pgis::DataSource::drop(const std::string& connInfo)
{
  // Get an auxiliary data source
  std::auto_ptr<DataSource> ds(new DataSource(connInfo));

  ds->open();

  // Drop the database
  std::string sql = "DROP DATABASE ";
  std::map<std::string, std::string> kvp = te::core::Expand(ds->getConnectionInfo().query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  std::map<std::string, std::string>::const_iterator itend = kvp.end();
  
  it = kvp.find("PG_DB_TO_DROP");
  if (it == itend || it->second.empty())
    throw Exception(TE_TR("Could not drop the database due the missing parameter: PG_DB_TO_DROP!"));

  if((it->second == "postgres") || (it->second == "template_postgis"))
    throw Exception(TE_TR("The database postgres or template_postgis is not allowed to be dropped!"));

  sql += it->second;

  ds->execute(sql);

  ds->close();
}

bool te::pgis::DataSource::exists(const std::string& connInfo)
{
  // Get an auxiliary data source
  std::auto_ptr<DataSource> ds(new DataSource(connInfo));
  ds->open();

  std::map<std::string, std::string> kvp = te::core::Expand(ds->getConnectionInfo().query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  std::map<std::string, std::string>::const_iterator itend = kvp.end();

  it = kvp.find("PG_CHECK_DB_EXISTENCE");

  if (it == itend || it->second.empty())
    throw Exception(TE_TR("Could not check the PostgreSQL database existence due the missing parameter : PG_CHECK_DB_EXISTENCE!"));

  std::string sql("SELECT * FROM pg_database WHERE datname = '");
  sql += it->second;
  sql += "'";

  std::auto_ptr<te::da::DataSet> database(ds->query(sql));

  ds->close();

  return database->moveNext();
}

std::vector<std::string> te::pgis::DataSource::getDataSourceNames(const std::string& connInfo)
{
  // Get an auxiliary data source
  std::auto_ptr<DataSource> ds(new DataSource(connInfo));
  ds->open();

  std::string sql("SELECT datname FROM pg_database");

  std::auto_ptr<te::da::DataSet> dataset(ds->query(sql));

  std::vector<std::string> dataSourceNames;

  while(dataset->moveNext())
    dataSourceNames.push_back(dataset->getString(0));

  ds->close();

  return dataSourceNames;
}
