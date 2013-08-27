/*  Copyright (C) 2008-2013 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/gdal/DataSource.cpp

  \brief The implementation of a DataSource that consists of datasets that can be decoded by the GDAL Library.
*/

// TerraLib
#include "../common/StringUtils.h"
#include "../common/Translator.h"
#include "../geometry/Envelope.h"
#include "../dataaccess2/dataset/DataSetType.h"
#include "../raster/Grid.h"
#include "../raster/Raster.h"
#include "../raster/RasterProperty.h"
#include "DataSet.h"
#include "DataSource.h"
#include "Exception.h"
#include "Transactor.h"
#include "Utils.h"

// GDAL
#include <gdal_priv.h>

// Boost
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

te::da::DataSourceCapabilities te::gdal::DataSource::sm_capabilities;

te::gdal::DataSource::DataSource():
  m_straccess(""),
  m_isOpened(false),
  m_transactor(0)
{
}

te::gdal::DataSource::~DataSource()
{
  if (m_transactor)
    delete m_transactor;
}

std::string te::gdal::DataSource::getType() const
{
  return TE_GDAL_DRIVER_IDENTIFIER;
}

const std::map<std::string, std::string>& te::gdal::DataSource::getConnectionInfo() const
{
  return m_connectionInfo;
}

void te::gdal::DataSource::setConnectionInfo(const std::map<std::string, std::string>& connInfo)
{ 
  m_connectionInfo = connInfo;
}

// open methods retrieves the names of the datasets in the data source
void te::gdal::DataSource::open()
{
  if (m_isOpened)
    return;

  if (m_connectionInfo.empty())
      throw Exception((boost::format(TR_GDAL("Empty data source connection information"))).str());
  
  std::map<std::string, std::string>::const_iterator it = m_connectionInfo.find("SOURCE");
  if (it != m_connectionInfo.end())
    m_straccess = it->second;
  else
  {
    it = m_connectionInfo.find("URI");
    if (it != m_connectionInfo.end())
      m_straccess = it->second;
    else
      throw Exception((boost::format(TR_GDAL("Invalid data source connection information"))).str());
  }
  
  m_transactor = new te::gdal::Transactor(m_straccess);
  m_isOpened = true;
}

std::auto_ptr<te::da::DataSourceTransactor> te::gdal::DataSource::getTransactor()
{
  if (!m_isOpened)
    return std::auto_ptr<te::da::DataSourceTransactor>();
  
  return std::auto_ptr<te::da::DataSourceTransactor>(new te::gdal::Transactor(m_straccess));
}

void te::gdal::DataSource::close()
{
  if (m_transactor)
    delete m_transactor;
  m_transactor = 0;
}

bool te::gdal::DataSource::isOpened() const
{
  return (m_isOpened && (m_transactor != 0));
}

bool te::gdal::DataSource::isValid() const
{
  if(m_connectionInfo.empty())
    return false;
  
  std::map<std::string, std::string>::const_iterator it = m_connectionInfo.find("SOURCE");
  
  // if URI used, check if it is a valid directory or file name
  if(it != m_connectionInfo.end())
  {
    if(boost::filesystem::is_directory(it->second))
      return true;    
  }
  else  // if it is another GDAL string let's check it
  {
    it = m_connectionInfo.find("URI");
    
    if(it == m_connectionInfo.end())
      return false;
    
    GDALDataset* gds = static_cast<GDALDataset*>(GDALOpen(it->second.c_str(), GA_ReadOnly));
    
    if(gds)
    {
      GDALClose(gds);
      return true;
    }
  }
  return false;
}

bool te::gdal::DataSource::hasDataSets()
{
  if (m_transactor)
    return m_transactor->hasDataSets();
  
  return false;
}

bool te::gdal::DataSource::dataSetExists(const std::string& name)
{
  assert(!name.empty());
  
  if (m_transactor)
    return m_transactor->dataSetExists(name);
  
  return false;
}

std::size_t te::gdal::DataSource::getNumberOfDataSets()
{
  if (m_transactor)
    return m_transactor->getNumberOfDataSets();
  
  return 0;
}

std::vector<std::string> te::gdal::DataSource::getDataSetNames()
{
  if (m_transactor)
    return m_transactor->getDataSetNames();
  
  return std::vector<std::string>();
}

te::da::DataSetTypePtr te::gdal::DataSource::getDataSetType(const std::string& name)
{
  assert(!name.empty());
  
  if (m_transactor)
    return m_transactor->getDataSetType(name);
  
  te::da::DataSetType* ptr = 0;
  return te::da::DataSetTypePtr(ptr);
}

std::auto_ptr<te::da::DataSet> te::gdal::DataSource::getDataSet(const std::string& name, 
                                                                te::common::TraverseType travType) 
{
  assert(!name.empty());
  
  if (m_transactor)
    return m_transactor->getDataSet(name,travType); 
  
  return std::auto_ptr<te::da::DataSet>();
}

std::size_t te::gdal::DataSource::getNumberOfProperties(const std::string& datasetName)
{
  assert(!datasetName.empty());
  
  if (m_transactor)
    return m_transactor->getNumberOfProperties(datasetName);
  
  return 0;
}

boost::ptr_vector<te::dt::Property> te::gdal::DataSource::getProperties(const std::string& datasetName)
{
  assert(!datasetName.empty());
  
  if (m_transactor)
    return m_transactor->getProperties(datasetName);
  
  return boost::ptr_vector<te::dt::Property>();
}

bool te::gdal::DataSource::propertyExists(const std::string& datasetName, const std::string& name)
{  
  assert(!datasetName.empty());
  
  if (m_transactor)
    return m_transactor->propertyExists(datasetName,name);
  
  return false;
}

std::auto_ptr<te::dt::Property> te::gdal::DataSource::getProperty(const std::string& datasetName, const std::string& propertyName)
{
  assert(!datasetName.empty());
  
  if (m_transactor)
    return m_transactor->getProperty(datasetName, propertyName);
  
  return std::auto_ptr<te::dt::Property>();
}


std::auto_ptr<te::dt::Property> te::gdal::DataSource::getProperty(const std::string& datasetName, std::size_t propertyPos)
{
  assert(!datasetName.empty());
  
  if (m_transactor)
    return m_transactor->getProperty(datasetName,propertyPos);
  
  return std::auto_ptr<te::dt::Property>();
}

std::auto_ptr<te::gm::Envelope> te::gdal::DataSource::getExtent(const std::string& datasetName, std::size_t propertyPos)
{
  assert(!datasetName.empty());
  
  if (m_transactor)
    return m_transactor->getExtent(datasetName,propertyPos);
  
  return std::auto_ptr<te::gm::Envelope>();
}

std::auto_ptr<te::gm::Envelope> te::gdal::DataSource::getExtent(const std::string& datasetName, const std::string& propertyName)
{
  assert(!datasetName.empty());
  assert(!propertyName.empty());
  
  if (m_transactor)
    return m_transactor->getExtent(datasetName,propertyName);
  
  return std::auto_ptr<te::gm::Envelope>();
}

const te::da::DataSourceCapabilities& te::gdal::DataSource::getCapabilities() const
{
  return sm_capabilities;
}

std::auto_ptr<te::da::DataSet> te::gdal::DataSource::getDataSet(const std::string& name,
                                                                const std::string& propertyName,
                                                                const te::gm::Envelope* /*e*/,
                                                                te::gm::SpatialRelation /*r*/,
                                                                te::common::TraverseType travType)
{
  return getDataSet(name, travType);
}

std::auto_ptr<te::da::DataSet>  te::gdal::DataSource::getDataSet(const std::string& name,
                                                                 const std::string& propertyName,
                                                                 const te::gm::Geometry* /*g*/,
                                                                 te::gm::SpatialRelation /*r*/,
                                                                 te::common::TraverseType travType)
{
  return getDataSet(name, travType);
}

bool te::gdal::DataSource::isDataSetNameValid(const std::string& datasetName)
{
  assert(!datasetName.empty());
  
  if (m_transactor)
    return m_transactor->isDataSetNameValid(datasetName);
  
  return false;
}

bool te::gdal::DataSource::isPropertyNameValid(const std::string& propertyName)
{
  assert(!propertyName.empty());
  
  if (m_transactor)
    return m_transactor->isDataSetNameValid(propertyName);
  
  return false;
}

std::vector<std::string> te::gdal::DataSource::getPropertyNames(const std::string& datasetName)
{
  assert(!datasetName.empty());
  
  if (m_transactor)
    return m_transactor->getPropertyNames(datasetName);
  
  return std::vector<std::string>();
}

void te::gdal::DataSource::createDataSet(te::da::DataSetType* dt,
                                         const std::map<std::string, std::string>& options) 
{ 
  if (m_transactor)
    return m_transactor->createDataSet(dt,options);
}

void te::gdal::DataSource::cloneDataSet(const std::string& name,
                                        const std::string& cloneName,
                                        const std::map<std::string, std::string>& options)
{
  if (m_transactor)
    return m_transactor->cloneDataSet(name, cloneName, options);
}

void te::gdal::DataSource::dropDataSet(const std::string& name)
{
  assert(!name.empty());
  
  if (m_transactor)
    return m_transactor->dropDataSet(name);
}

//parei aqui!!!!
void te::gdal::DataSource::renameDataSet(const std::string& name, const std::string& newName)
{
  assert(!name.empty());
  assert(!newName.empty());
  
  if (m_transactor)
    return m_transactor->renameDataSet(name,newName);
}

void te::gdal::DataSource::setCapabilities(const te::da::DataSourceCapabilities& capabilities)
{
  sm_capabilities = capabilities;
}

void te::gdal::DataSource::create(const std::map<std::string, std::string>& dsInfo)
{
  m_connectionInfo = dsInfo;
  
  // create the needed directory
  std::map<std::string, std::string>::const_iterator it = m_connectionInfo.find("SOURCE");
  if(it != m_connectionInfo.end())
  {
    try 
    {      
      if(!boost::filesystem::is_directory(it->second))
        boost::filesystem::create_directory(it->second);
    } 
    catch(const boost::filesystem::filesystem_error& e) 
    { 
      throw Exception((boost::format(TR_GDAL("Could not create the data source due to the following error: %1%.")) % e.what()).str());
    }
  }
  else 
  {
    throw Exception((boost::format(TR_GDAL("Data source creation is supported only for directory data sources"))).str());
  }
}

bool te::gdal::DataSource::exists(const std::map<std::string, std::string>& dsInfo)
{
  std::map<std::string, std::string>::const_iterator it = dsInfo.find("SOURCE"); // expects a directory
  if(it != dsInfo.end())   
  {
    if (boost::filesystem::exists(it->second) && boost::filesystem::is_directory(it->second)) 
      return true;
    
    return false;
  }
  
  it = dsInfo.find("URI"); // expects a file?
  if(it != dsInfo.end())   
  {  
    GDALDataset* gds = static_cast<GDALDataset*>(GDALOpen(it->second.c_str(), GA_ReadOnly));
  
    if (gds)
    {
      GDALClose(gds);
      return true;
    }
  }
  
  return false; // it is an invalid file
}
 
void te::gdal::DataSource::drop(const std::map<std::string, std::string>& dsInfo)
{
  std::map<std::string, std::string>::const_iterator it = dsInfo.find("URI");
  if (it == dsInfo.end())
    it = dsInfo.find("SOURCE");
  
  if (it == dsInfo.end())
    return;  // nothing to be done
  
  try 
  {    
    boost::filesystem::remove(it->second);
  }
  catch(const boost::filesystem::filesystem_error& e) 
  { 
    throw Exception((boost::format(TR_GDAL("Could not drop the data source due to the following error: %1%.")) % e.what()).str());
  }
}

std::vector<std::string> te::gdal::DataSource::getDataSourceNames(const std::map<std::string, std::string>& dsInfo)
{
  assert(!dsInfo.empty());
  
  std::vector<std::string> dsnames;
  
  std::map<std::string, std::string>::const_iterator it = dsInfo.find("URI");
  if (it == dsInfo.end())
    it = dsInfo.find("SOURCE");
  
  if (it != dsInfo.end())
    dsnames.push_back(it->second);
  
  return dsnames;
}

std::vector<std::string> te::gdal::DataSource::getEncodings(const std::map<std::string, std::string>& dsInfo)
{
  return std::vector<std::string>();
}
