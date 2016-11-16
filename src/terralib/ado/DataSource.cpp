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
  \file terralib/ado/DataSource.cpp

  \brief Implementation of the data source class for the ADO driver.
*/

// TerraLib
#include "../common/Exception.h"
#include "../core/filesystem/FileSystem.h"
#include "../core/translator/Translator.h"
#include "../core/uri/Utils.h"
#include "../dataaccess/datasource/DataSourceTransactor.h"
#include "../dataaccess/query/SQLDialect.h"
#include "../datatype/StringProperty.h"
#include "Connection.h"
#include "DataSource.h"

#include "Globals.h"
#include "Transactor.h"
#include "Utils.h"

// STL
#include <cassert>
#include <iostream>
#include <memory>

// Boost
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

inline void TESTHR(HRESULT hr)
{
  if(FAILED(hr))
    _com_issue_error(hr);
}

te::ado::DataSource::DataSource(const std::string& connInfo)
  : te::da::DataSource(connInfo),
    m_isOpened(false)
{
}

te::ado::DataSource::DataSource(const te::core::URI& uri)
  : te::da::DataSource(uri),
    m_isOpened(false)
{
}

te::ado::DataSource::~DataSource()
{
}

std::string te::ado::DataSource::getType() const
{
  return ADO_DRIVER_IDENTIFIER;
}

std::auto_ptr<te::da::DataSourceTransactor> te::ado::DataSource::getTransactor()
{
  return std::auto_ptr<te::da::DataSourceTransactor>(new te::ado::Transactor(this));
}

void te::ado::DataSource::open()
{
// assure we are in a closed state
  close();

  std::string connInfo = MakeConnectionStr(m_uri);

  std::auto_ptr<Connection> conn(new te::ado::Connection(connInfo));

  _ConnectionPtr connection = conn->getConn();

  loadGeometryColumnsCache(connection);

  m_isOpened = true;
}

void te::ado::DataSource::close()
{
  m_isOpened = false;
}

bool te::ado::DataSource::isOpened() const
{
  return m_isOpened;
}

bool te::ado::DataSource::isValid() const
{
  return m_isOpened;
}

const te::da::DataSourceCapabilities& te::ado::DataSource::getCapabilities() const
{
  assert(te::ado::Globals::sm_capabilities);
  return *te::ado::Globals::sm_capabilities;
}

const te::da::SQLDialect* te::ado::DataSource::getDialect() const
{
  return te::ado::Globals::sm_queryDialect;
}

const std::map<std::string, std::string>& te::ado::DataSource::getGeomColumns() const
{
  return m_geomColumns;
}

void te::ado::DataSource::registerGeometryColumn(const std::string& datasetName,
                                                 const std::string& geomColName)
{
  boost::lock_guard<boost::mutex> lock(m_mtx);

  m_geomColumns[datasetName] = geomColName;
}

bool te::ado::DataSource::isGeometryColumn(const std::string& datasetName,
                                           const std::string& colName) const
{
  boost::lock_guard<boost::mutex> lock(m_mtx);

  std::map<std::string, std::string>::const_iterator it = m_geomColumns.find(datasetName);

  if(it != m_geomColumns.end())
    return it->second == colName;

  return false;
}

void te::ado::DataSource::create(const std::string& connInfo)
{
  te::core::URI auxURI(connInfo);
  std::map<std::string, std::string> kvp = te::core::Expand(auxURI.query());
  std::string connSTR = te::ado::MakeConnectionStr(auxURI);

  // Create the new database
  ADOX::_CatalogPtr pCatalog = 0;

  pCatalog.CreateInstance(__uuidof(ADOX::Catalog));

  try
  {
    pCatalog->Create(connSTR.c_str());
  }
  catch(_com_error& e)
  {
    throw te::common::Exception((LPCSTR)e.ErrorMessage());
  }
  std::string createMetaTables = kvp["CREATE_OGC_METADATA_TABLES"];

  if(!createMetaTables.empty() && createMetaTables == "TRUE")
  {
    // Create the geometry_columns dataset
    te::da::DataSetType* geomColsDt = new te::da::DataSetType("geometry_columns");

    geomColsDt->add(new te::dt::StringProperty("f_table_catalog", te::dt::VAR_STRING, 256));
    geomColsDt->add(new te::dt::StringProperty("f_table_schema", te::dt::VAR_STRING, 256));
    geomColsDt->add(new te::dt::StringProperty("f_table_name", te::dt::VAR_STRING, 256));
    geomColsDt->add(new te::dt::StringProperty("f_geometry_column", te::dt::VAR_STRING, 256));
    geomColsDt->add(new te::dt::SimpleProperty("coord_dimension", te::dt::INT32_TYPE));
    geomColsDt->add(new te::dt::SimpleProperty("srid", te::dt::INT32_TYPE));
    geomColsDt->add(new te::dt::StringProperty("type", te::dt::VAR_STRING, 30));

    std::map<std::string, std::string> op;

    createDataSet(geomColsDt, op);
  }

  close();
}

void te::ado::DataSource::drop(const std::string& connInfo)
{
  if(!exists(connInfo))
    throw te::common::Exception(TE_TR("The data source doesn't exist!"));

  te::core::URI auxURI(connInfo);

  std::string auxPath = auxURI.host() + auxURI.path();

  boost::filesystem::path path(auxPath);

  if(te::core::FileSystem::remove(path.string()) == false)
    throw te::common::Exception(TE_TR("The data source could not be dropped!"));
}

bool te::ado::DataSource::exists(const std::string& connInfo)
{
  te::core::URI auxURI(connInfo);

  std::string auxPath = auxURI.host() + auxURI.path();

  boost::filesystem::path path(auxPath);

  return te::core::FileSystem::exists(path.string());
}

std::vector<std::string> te::ado::DataSource::getDataSourceNames(const std::string& connInfo)
{
  te::core::URI auxURI(connInfo);

  std::string path = auxURI.host() + auxURI.path();

  std::vector<std::string> names;

  if (!path.empty())
  {
    names.push_back(path);
  }

  return names;
}

void te::ado::DataSource::loadGeometryColumnsCache(_ConnectionPtr& adoConn)
{
  boost::lock_guard<boost::mutex> lock(m_mtx);

  m_geomColumns.clear();

  _RecordsetPtr recordset;

  TESTHR(recordset.CreateInstance(__uuidof(Recordset)));
  
  std::string query = "SELECT * FROM geometry_columns";

  try
  {
    recordset->Open(query.c_str(), _variant_t((IDispatch *)adoConn), adOpenDynamic, adLockReadOnly, adCmdText);

    while(!recordset->EndOfFile)
    {
      std::string tablename = (LPCSTR)(_bstr_t)recordset->GetFields()->GetItem("f_table_name")->GetValue();

      std::string columnName = (LPCSTR)(_bstr_t)recordset->GetFields()->GetItem("f_geometry_column")->GetValue();

      m_geomColumns[tablename] = columnName;

      recordset->MoveNext();
    }
  }
  catch(_com_error& e)
  {
    throw te::common::Exception((LPCSTR)e.ErrorMessage());
  }
}

