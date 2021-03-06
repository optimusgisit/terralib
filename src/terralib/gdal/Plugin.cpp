/*
  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

  This file is part of the TerraLib - a Framework for building GIS enabled
  applications.

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
  \file terralib/gdal/Plugin.cpp

  \brief Plugin interface for dynamic loading of GDAL Driver for TerraLib.
*/

#include "../BuildConfig.h"
#include "../Defines.h"
#include "../core/filesystem/FileSystem.h"
#include "../core/logger/Logger.h"
#include "../core/plugin/CppPlugin.h"
#include "../core/translator/Translator.h"
#include "../core/utils/Platform.h"
#include "../dataaccess/datasource/DataSourceCapabilities.h"
#include "../dataaccess/datasource/DataSourceFactory.h"
#include "../dataaccess/datasource/DataSourceManager.h"
#include "Config.h"
#include "DataSourceFactory.h"
#include "RasterFactory.h"
#include "Utils.h"

// STL
#include <cstdlib>

// Boost
#include <boost/filesystem.hpp>

// GDAL
#include <gdal_priv.h>
namespace te
{
  namespace gdal
  {
    TERRALIB_CPP_PLUGIN_BEGIN(Plugin)

    TERRALIB_CPP_PLUGIN_STARTUP
    {
      if(m_initialized)
        return;

      // initializing the static mutex
      getStaticMutex();

      // for all platforms, first look at an environment variable
      // defined by macro TERRALIB_GDAL_DATA.
      // note: TERRALIB_GDAL_DATA is detected by CMAKE.
      // note: installed versions on developers machine may look for this
      // version of GDAL
      std::string gdal_data_dir(TERRALIB_GDAL_DATA);

      // if the above variable is not set or it points to an invalid directory
      if(gdal_data_dir.empty() ||
         !te::core::FileSystem::isDirectory(gdal_data_dir))
      {
        // search for GDAL in a PATH relative to TerraLib.
        // note: each SO will look in a different folder
        #if defined(TE_PLATFORM) && defined(TE_PLATFORMCODE_MSWINDOWS)
        #if TE_PLATFORM == TE_PLATFORMCODE_MSWINDOWS
          gdal_data_dir = te::core::FindInTerraLibPath("/share/data");
        #elif TE_PLATFORM == TE_PLATFORMCODE_APPLE
          gdal_data_dir = te::core::FindInTerraLibPath("/share/gdal");
        #elif TE_PLATFORM == TE_PLATFORMCODE_LINUX
          gdal_data_dir = te::core::FindInTerraLibPath("/share/gdal");
        #else
        #error "unsupported plataform: please, contact terralib-team@terralib.org"
        #endif
        #else
        #error \
            "the macro TE_PLATFORM is not set, please, contact terralib-team@terralib.org"
        #endif
      }

      if(!gdal_data_dir.empty())
      {
        CPLSetConfigOption("GDAL_DATA", gdal_data_dir.c_str());
      }

      CPLSetConfigOption("GDAL_PAM_ENABLED", "NO");

      te::da::DataSourceFactory::add(TE_GDAL_DRIVER_IDENTIFIER,
                                     te::gdal::Build);

      GDALAllRegister();

      RasterFactory::initialize();

      // DataType Capabilites
      te::da::DataTypeCapabilities dataTypeCapabilities;
      dataTypeCapabilities.setSupportRaster(true);

      // DataSetType Capabilites
      te::da::DataSetTypeCapabilities dataSetTypeCapabilities;  // All false

      // DataSet Capabilities
      te::da::DataSetCapabilities dataSetCapabilities;
      dataSetCapabilities.setSupportEfficientDataSetSize(true);

      // Query Capabilities
      te::da::QueryCapabilities queryCapabilities;  // All false

      // DataSouce Capabilities
      te::da::DataSourceCapabilities capabilities;
      capabilities.setSupportAll();
      capabilities.setSupportTransactions(false);
      capabilities.setDataTypeCapabilities(dataTypeCapabilities);
      capabilities.setDataSetTypeCapabilities(dataSetTypeCapabilities);
      capabilities.setDataSetCapabilities(dataSetCapabilities);
      capabilities.setQueryCapabilities(queryCapabilities);
      capabilities.setAccessPolicy(te::common::RWAccess);

      // Supported file extensions capability

      std::set<std::string> supportedRasterExtensionsSet;
      std::set<std::string> supportedVectorExtensionsSet;

      for(std::map<std::string, DriverMetadata>::const_iterator it =
              GetGDALDriversMetadata().begin();
          it != GetGDALDriversMetadata().end(); ++it)
      {
        if(!it->second.m_extensions.empty())
        {
          for(std::size_t extensionsIdx = 0;
              extensionsIdx < it->second.m_extensions.size(); ++extensionsIdx)
          {
            if(it->second.m_isRaster)
            {
              if(supportedRasterExtensionsSet.find(
                     it->second.m_extensions[extensionsIdx]) ==
                 supportedRasterExtensionsSet.end())
              {
                supportedRasterExtensionsSet.insert(
                    it->second.m_extensions[extensionsIdx]);
              }
            }

            if(it->second.m_isVector)
            {
              if(supportedVectorExtensionsSet.find(
                     it->second.m_extensions[extensionsIdx]) ==
                 supportedVectorExtensionsSet.end())
              {
                supportedVectorExtensionsSet.insert(
                    it->second.m_extensions[extensionsIdx]);
              }
            }
          }
        }
      }

      std::string supportedRasterExtensionsStr;

      for(std::set<std::string>::const_iterator it =
              supportedRasterExtensionsSet.begin();
          it != supportedRasterExtensionsSet.end(); ++it)
      {
        if(!supportedRasterExtensionsStr.empty())
        {
          supportedRasterExtensionsStr.append(";");
        }
        supportedRasterExtensionsStr.append(*it);
      }

      std::string supportedVectorExtensionsStr;

      for(std::set<std::string>::const_iterator it =
              supportedVectorExtensionsSet.begin();
          it != supportedVectorExtensionsSet.end(); ++it)
      {
        if(!supportedVectorExtensionsStr.empty())
        {
          supportedVectorExtensionsStr.append(";");
        }
        supportedVectorExtensionsStr.append(*it);
      }

      capabilities.addSpecificCapability("SUPPORTED_RASTER_EXTENSIONS",
                                         supportedRasterExtensionsStr);
      capabilities.addSpecificCapability("SUPPORTED_VECTOR_EXTENSIONS",
                                         supportedVectorExtensionsStr);

      te::gdal::DataSource::setCapabilities(capabilities);

      TE_LOG_TRACE(TE_TR("TerraLib GDAL driver startup!"));

      m_initialized = true;
    }

    TERRALIB_CPP_PLUGIN_SHUTDOWN
    {
      if(!m_initialized)
        return;

      //! it finalizes the GDAL factory support.
      te::da::DataSourceFactory::remove(TE_GDAL_DRIVER_IDENTIFIER);

      RasterFactory::finalize();

      //! free GDAL registered drivers
      te::da::DataSourceManager::getInstance().detachAll(
          TE_GDAL_DRIVER_IDENTIFIER);

      TE_LOG_TRACE(TE_TR("TerraLib GDAL driver shutdown!"));

      m_initialized = false;
    }

    TERRALIB_CPP_PLUGIN_END(te::gdal::Plugin)
  }
}
