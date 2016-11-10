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
  \file terralib/wms/Module.cpp
   
  \brief The TerraLib WMS driver as a plugin.
*/

// TerraLib
#include "../BuildConfig.h"
#include "../core/logger/Logger.h"
#include "../core/translator/Translator.h"
#include "../dataaccess/datasource/DataSourceFactory.h"
#include "../dataaccess/datasource/DataSourceManager.h"
#include "../maptools/serialization/xml/Layer.h"
#include "Config.h"
#include "DataSourceFactory.h"
#include "Module.h"

#ifdef TERRALIB_MOD_XML_ENABLED
  #include "./serialization/xml/Layer.h"
#endif

// GDAL
#include <gdal_priv.h>

// Boost
#include <boost/functional/factory.hpp>
#include <boost/bind.hpp>

te::wms::Module::Module(const te::core::PluginInfo& pluginInfo)
  : te::core::CppPlugin(pluginInfo)
{
}

te::wms::Module::~Module()
{
}

void te::wms::Module::startup()
{
  if(m_initialized)
    return;

  te::da::DataSourceFactory::add(TE_WMS_DRIVER_IDENTIFIER, te::wms::Build);

  GDALAllRegister();

#ifdef TERRALIB_MOD_XML_ENABLED
  // Register serializer methods
  te::map::serialize::Layer::getInstance().reg("WMSLAYER", std::make_pair(te::map::serialize::Layer::LayerReadFnctType(&te::wms::serialize::LayerReader),
                                                                          te::map::serialize::Layer::LayerWriteFnctType(&te::wms::serialize::LayerWriter)));
#endif

  TE_LOG_TRACE(TE_TR("TerraLib WMS driver startup!"));

  m_initialized = true;
}

void te::wms::Module::shutdown()
{
  if(!m_initialized)
    return;

  te::da::DataSourceFactory::remove(TE_WMS_DRIVER_IDENTIFIER);

  te::da::DataSourceManager::getInstance().detachAll(TE_WMS_DRIVER_IDENTIFIER);

  TE_LOG_TRACE(TE_TR("TerraLib WMS driver shutdown!"));

  m_initialized = false;
}

TERRALIB_PLUGIN_CALL_BACK_IMPL(te::wms::Module)
