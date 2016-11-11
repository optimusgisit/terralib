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
  \file terralib/gdal/DataSource.cpp

  \brief The implementation of a DataSource that consists of datasets that can be decoded by the GDAL Library.
*/

// TerraLib
#include "../common/StringUtils.h"
#include "../core/filesystem/FileSystem.h"
#include "../core/translator/Translator.h"
#include "../core/uri/URI.h"
#include "../core/uri/Utils.h"
#include "../geometry/Envelope.h"
#include "../dataaccess/dataset/DataSetType.h"
#include "../raster/Grid.h"
#include "../raster/Raster.h"
#include "../raster/RasterProperty.h"
#include "DataSet.h"
#include "DataSource.h"
#include "Exception.h"
#include "Transactor.h"
#include "Utils.h"
#include "DataSetUseCounter.h"

// GDAL
#include <gdal_priv.h>

// Boost
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

te::da::DataSourceCapabilities te::gdal::DataSource::sm_capabilities;

te::gdal::DataSource::DataSource(const std::string& connInfo)
  : te::da::DataSource(connInfo),
  m_straccess(""),
  m_isOpened(false)
{
}

te::gdal::DataSource::DataSource(const te::core::URI& uri)
  : te::da::DataSource(uri),
  m_straccess(""),
  m_isOpened(false)
{
}

te::gdal::DataSource::~DataSource()
{}

std::string te::gdal::DataSource::getType() const
{
  return TE_GDAL_DRIVER_IDENTIFIER;
}

// open methods retrieves the names of the datasets in the data source
void te::gdal::DataSource::open()
{
  if (m_isOpened)
    return;

  if (!m_uri.isValid())
    throw Exception((boost::format(TE_TR("Invalid data source connection information"))).str());

  std::string source = m_uri.host() + m_uri.path();

  if(source.empty())
    throw Exception((boost::format(TE_TR("Not enough information to open the data source."))).str());
  else
    m_straccess = source;

  m_isOpened = true;
}

void te::gdal::DataSource::close()
{
}

bool te::gdal::DataSource::isOpened() const
{
  return m_isOpened;
}

bool te::gdal::DataSource::isValid() const
{
  if (!m_uri.isValid())
    return false;

  std::string source = m_uri.host() + m_uri.path();

  if(!source.empty())
  {
    //Checking if it is a valid directory or file name
    if(te::core::FileSystem::isDirectory(source))
      return true;
    else if(te::core::FileSystem::isRegularFile(source))
      return true;

    // if it is another GDAL string let's check it
    DataSetUseCounter dsUseCounter(GetParentDataSetName(source),
      DataSetsManager::MultipleAccessType);

    GDALDataset* gds = static_cast<GDALDataset*>(GDALOpen(source.c_str(), GA_ReadOnly));

    if (gds)
    {
      GDALClose(gds);

      return true;
    }
  }
  return false;
}

const te::da::DataSourceCapabilities& te::gdal::DataSource::getCapabilities() const
{
  return sm_capabilities;
}

std::auto_ptr<te::da::DataSourceTransactor> te::gdal::DataSource::getTransactor()
{
  if (!m_isOpened)
    throw Exception((boost::format(TE_TR("Data source is not open."))).str());
  
  return std::auto_ptr<te::da::DataSourceTransactor>(new te::gdal::Transactor(m_straccess));
}

void te::gdal::DataSource::setCapabilities(const te::da::DataSourceCapabilities& capabilities)
{
  sm_capabilities = capabilities;
}

void te::gdal::DataSource::create(const std::string& connInfo)
{
  //Auxiliary URI
  te::core::URI auxURI(connInfo);

  if (!auxURI.isValid())
    throw Exception((boost::format(TE_TR("Invalid data source connection information"))).str());

  // create the needed directory
  std::string path = auxURI.host() + auxURI.path();
  if(!path.empty())
  {
    try 
    {      
      if(!te::core::FileSystem::isDirectory(path))
        te::core::FileSystem::createDirectory(path);
      else
      {
        throw Exception((boost::format(TE_TR("Data source creation is supported only for directory data sources"))).str());
      }
    } 
    catch(const boost::filesystem::filesystem_error& e) 
    { 
      throw Exception((boost::format(TE_TR("Could not create the data source due to the following error: %1%.")) % e.what()).str());
    }
  }
  else 
  {
    throw Exception((boost::format(TE_TR("Empty data source connection information"))).str());
  }
}

bool te::gdal::DataSource::exists(const std::string& connInfo)
{
  //Auxiliary URI
  te::core::URI auxURI(connInfo);

  if (!auxURI.isValid())
    throw Exception((boost::format(TE_TR("Empty ou invalid data source connection information"))).str());

  // create the needed directory
  std::string path = auxURI.host() + auxURI.path();
  if (!path.empty())
  {
    if (te::core::FileSystem::exists(path) && te::core::FileSystem::isDirectory(path)) // expects a directory?
      return true;
    else if(boost::filesystem::exists(path) && boost::filesystem::is_regular_file(path)) // expects a file?
    {
      DataSetUseCounter dsUseCounter(GetParentDataSetName(path), DataSetsManager::MultipleAccessType);

      GDALDataset* gds = static_cast<GDALDataset*>(GDALOpen(path.c_str(), GA_ReadOnly));

      if (gds)
      {
        GDALClose(gds);
        return true;
      }
      else return false;
    }
    else
      return false; // it is an invalid file
  }
  else
    return false; // Path is empty
}
 
void te::gdal::DataSource::drop(const std::string& connInfo)
{
  //Auxiliary URI
  te::core::URI auxURI(connInfo);

  std::string path = auxURI.host() + auxURI.path();
  if (!path.empty())
  {
    try
    {
      te::core::FileSystem::remove(path);
    }
    catch (const boost::filesystem::filesystem_error& /*e*/)
    {
    }
  }
  else
    return;  // nothing to be done 

}

std::vector<std::string> te::gdal::DataSource::getDataSourceNames(const std::string& connInfo)
{
  //Auxiliary URI
  te::core::URI auxURI(connInfo);

  std::string path = auxURI.host() + auxURI.path();

  std::vector<std::string> dsnames;
  
  if(!path.empty())
  {
    dsnames.push_back(path);
  }
  else
    throw Exception((boost::format(TE_TR("Empty ou invalid data source connection information"))).str());
  
  return dsnames;
}
