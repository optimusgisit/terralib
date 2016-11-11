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

//Terralib
#include "DataSource.h"
#include "Globals.h"
#include "Transactor.h"
#include "Utils.h"

#include "../core/filesystem/FileSystem.h"
#include "../core/translator/Translator.h"
#include "../core/uri/URI.h"
#include "../core/uri/Utils.h"
#include "../dataaccess/query/SQLDialect.h"

// OGR
#include <ogrsf_frmts.h>
#include <ogr_core.h>
#include <gdal_priv.h>
#include <ogr_api.h>
#include <gdal.h>

// Boost
#include <boost/filesystem/operations.hpp>

te::da::SQLDialect* te::ogr::DataSource::sm_myDialect(0);

void GetDataSetTypeCapabilities(te::da::DataSourceCapabilities& caps)
{
  te::da::DataTypeCapabilities dt_caps;

  dt_caps.setSupportInt32(true);
  dt_caps.setSupportInt64(true);
  dt_caps.setSupportArray(true);
  dt_caps.setSupportString(true);
  dt_caps.setSupportDouble(true);
  dt_caps.setSupportNumeric(true);
  dt_caps.setSupportByteArray(true);
  dt_caps.setSupportDateTime(true);
  dt_caps.setSupportGeometry(true);

  caps.setDataTypeCapabilities(dt_caps);
}

void GetQueryCapabilities(te::da::DataSourceCapabilities& caps)
{
  te::da::QueryCapabilities qy_caps;

  // Spatial Topologic Operators
  qy_caps.addSpatialTopologicOperator("st_envelopeintersects");

  // Comparsion Operators
  qy_caps.addComparsionOperator("=");
  qy_caps.addComparsionOperator("<>");
  qy_caps.addComparsionOperator("<");
  qy_caps.addComparsionOperator(">");
  qy_caps.addComparsionOperator("<=");
  qy_caps.addComparsionOperator(">=");
  qy_caps.addComparsionOperator("like");

  qy_caps.addLogicalOperator("and");
  qy_caps.addLogicalOperator("or");

  caps.setQueryCapabilities(qy_caps);
}

void GetCapabilities(GDALDataset* ds, te::da::DataSourceCapabilities& caps)
{
  // DataSet
  if(!ds || ds->GetLayerCount() <= 0)
    return;

  te::da::DataSetCapabilities ds_caps;

  OGRLayer* l = ds->GetLayer(0);
  
  ds_caps.setSupportEfficientMove((l->TestCapability(OLCFastSetNextByIndex)) ? true : false);
  ds_caps.setSupportRandomTraversing((l->TestCapability(OLCRandomRead)) ? true : false);
  ds_caps.setSupportBidirectionalTraversing((l->TestCapability(OLCRandomRead)) ? true : false);
  ds_caps.setSupportEfficientDataSetSize((l->TestCapability(OLCFastGetExtent)) ? true : false);

  caps.setDataSetCapabilities(ds_caps);
}

te::ogr::DataSource::DataSource(const std::string& connInfo)
  : te::da::DataSource(connInfo),
  m_ogrDS(0),
  m_isValid(false),
  m_encoding(te::core::EncodingType::UTF8)
{
}

te::ogr::DataSource::DataSource(const te::core::URI& uri)
  : te::da::DataSource(uri),
  m_ogrDS(0),
  m_isValid(false),
  m_encoding(te::core::EncodingType::UTF8)
{
}

te::ogr::DataSource::~DataSource()
{
  close();
}

std::string te::ogr::DataSource::getType() const
{
  return Globals::m_driverIdentifier;
}

std::auto_ptr<te::da::DataSourceTransactor> te::ogr::DataSource::getTransactor()
{
  return std::auto_ptr<te::da::DataSourceTransactor>(new Transactor(this));
}

void te::ogr::DataSource::open()
{
  close();

  if (!m_uri.isValid())
    throw Exception(TE_TR("There is no valid information about the data source"));

  std::string path = m_uri.host() + m_uri.path();
  std::map<std::string, std::string> kvp = te::core::Expand(m_uri.query());
  std::map<std::string, std::string>::const_iterator it;
  
  if (path.empty())
    throw(Exception(TE_TR("Not enough information to open the data source.")));

  // Retrieve the char encoding
  it = kvp.find("SHAPE_ENCODING");
  if (it != kvp.end())
    CPLSetConfigOption("SHAPE_ENCODING", it->second.c_str());

  if (te::core::FileSystem::exists(path))
    m_ogrDS = (GDALDataset*)GDALOpenEx(path.c_str(), GDAL_OF_UPDATE, NULL, NULL, NULL);
  
  // let's try to open it without update permission
  if (!m_ogrDS)
  {
    m_ogrDS = (GDALDataset*)GDALOpenEx(path.c_str(), GDAL_OF_READONLY, NULL, NULL, NULL);
    if (m_ogrDS)
      m_capabilities.setAccessPolicy(te::common::RAccess);
  }
  else
    m_capabilities.setAccessPolicy(te::common::RWAccess);

  m_isValid = true;

  GetDataSetTypeCapabilities(m_capabilities);
  GetQueryCapabilities(m_capabilities);

  if (m_ogrDS)
    GetCapabilities(m_ogrDS, m_capabilities);
}

void te::ogr::DataSource::close()
{
  if (m_ogrDS)
    //OGRDataSource::DestroyDataSource(m_ogrDS);
    GDALClose(m_ogrDS);

  m_ogrDS = 0;  
  
  m_isValid = false;
}

bool te::ogr::DataSource::isOpened() const
{
  return m_ogrDS != 0;    
}

bool te::ogr::DataSource::isValid() const
{
  return m_isValid;
}

const te::da::DataSourceCapabilities& te::ogr::DataSource::getCapabilities() const
{
  return m_capabilities;
}

const te::da::SQLDialect* te::ogr::DataSource::getDialect() const
{
  return sm_myDialect;
}

void te::ogr::DataSource::setDialect(te::da::SQLDialect* dialect)
{
  if(sm_myDialect != 0)
    delete sm_myDialect;

  sm_myDialect = dialect;
}

GDALDataset* te::ogr::DataSource::getOGRDataSource()
{
  return m_ogrDS;
}

void te::ogr::DataSource::createOGRDataSource()
{
  if (!m_ogrDS)
  {
    if (!m_uri.isValid())
      throw Exception(TE_TR("There is no valid information about the data source"));

    std::string path = m_uri.host() + m_uri.path();
    std::map<std::string, std::string> kvp = te::core::Expand(m_uri.query());
    std::map<std::string, std::string>::const_iterator it;

    if (path.empty())
      throw(Exception(TE_TR("Not enough information to create data set.")));

    boost::filesystem::path bpath(path);
    std::string dir = bpath.parent_path().string();
    if (!dir.empty() && !te::core::FileSystem::exists(dir))
      te::core::FileSystem::createDirectory(dir);

    // Retrieve the char encoding
    it = kvp.find("SHAPE_ENCODING");
    if (it != kvp.end())
      CPLSetConfigOption("SHAPE_ENCODING", it->second.c_str());

    GDALDriverManager* driverManager = GetGDALDriverManager();
    GDALDriver* driver;

    it = kvp.find("DRIVER");
    if (it != kvp.end())
      driver = driverManager->GetDriverByName(it->second.c_str());
    else
      driver = driverManager->GetDriverByName(GetDriverName(path).c_str());

    if (driver == 0)
      throw(Exception(TE_TR("Driver not found.")));

    if (!OGR_Dr_TestCapability(driver, ODrCCreateDataSource))
      throw(Exception(TE_TR("The driver has no capability for creating a datasource!")));

    char** papszOptions = 0;
    it = kvp.begin();
    while (it != kvp.end())
    {
      papszOptions = CSLSetNameValue(papszOptions, it->first.c_str(), it->second.c_str());
      ++it;
    }

    m_ogrDS = driver->Create(path.c_str(), 0, 0, 0, GDT_Unknown, papszOptions);

    if (papszOptions)
      CSLDestroy(papszOptions);
  }

  if (!m_ogrDS)
    throw(Exception(TE_TR("Error creating the dataset!")));
}

void te::ogr::DataSource::create(const std::string& connInfo)
{
  te::core::URI auxURI(connInfo);

  std::string path = auxURI.path();
  //m_connectionInfo["URI"] = path;

  createOGRDataSource();
  close();
}

te::core::EncodingType te::ogr::DataSource::getEncoding()
{
  return m_encoding;
}

void te::ogr::DataSource::setEncoding(const te::core::EncodingType& et)
{
  m_encoding = et;
}

void te::ogr::DataSource::createDataSet(te::da::DataSetType* dt, const std::map<std::string, std::string>& options)
{
  std::auto_ptr<te::da::DataSourceTransactor> t = getTransactor();
  return t->createDataSet(dt, options);
}

void te::ogr::DataSource::drop(const std::string& connInfo)
{
  te::core::URI auxURI(connInfo);

  std::string path = auxURI.path();

  if (m_ogrDS != 0 && path.compare(m_ogrDS->GetDescription()) == 0)
    close();

  GDALDriverManager* driverManager = GetGDALDriverManager();
  GDALDriver* driver = driverManager->GetDriverByName(GetDriverName(path).c_str());

  if (driver == 0)
    throw(Exception(TE_TR("Driver not found!")));

  if (!OGR_Dr_TestCapability(driver, ODrCDeleteDataSource))
    throw(Exception(TE_TR("The driver has no drop capability!")));

  GDALClose(driver);
}

bool te::ogr::DataSource::exists(const std::string& connInfo)
{
  te::core::URI auxURI(connInfo);
  std::string path = auxURI.path();

  return te::core::FileSystem::exists(path);
}

std::vector<std::string> te::ogr::DataSource::getDataSourceNames(const std::string& connInfo)
{
  te::core::URI auxURI(connInfo);

  std::string path = auxURI.path();

  std::vector<std::string> names;

  if (!path.empty())
  {
    names.push_back(path);
  }

  return names;
}

te::da::DataSource* te::ogr::Build(const te::core::URI& uri)
{
  te::da::DataSource* ds = new DataSource(uri);
  return ds;
}
