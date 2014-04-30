/*  Copyright (C) 2001-2009 National Institute For Space Research (INPE) - Brazil.

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
#include "../common/Logger.h"
#include "../common/Translator.h"
#include "../dataaccess/datasource/DataSourceFactory.h"
#include "../dataaccess/datasource/DataSourceManager.h"
#include "../plugin/PluginInfo.h"
#include "../qt/widgets/layer/explorer/AbstractTreeItemFactory.h"
#include "./qt/WMSLayerItem.h"
#include "Config.h"
#include "DataSourceFactory.h"
#include "Module.h"

// GDAL
#include <gdal_priv.h>

// Boost
#include <boost/functional/factory.hpp>
#include <boost/bind.hpp>

te::wms::Module::Module(const te::plugin::PluginInfo& pluginInfo)
  : te::plugin::Plugin(pluginInfo)
{
}

te::wms::Module::~Module()
{
}

void te::wms::Module::startup()
{
  if(m_initialized)
    return;

// it initializes the Translator support for the TerraLib WMS driver support
  TE_ADD_TEXT_DOMAIN(TE_WMS_TEXT_DOMAIN, TE_WMS_TEXT_DOMAIN_DIR, "UTF-8");

  te::da::DataSourceFactory::add(TE_WMS_DRIVER_IDENTIFIER, te::wms::Build);

  GDALAllRegister();

  te::qt::widgets::AbstractTreeItemFactory::reg("WMSLAYER", boost::bind(boost::factory<WMSLayerItem*>(),_1, _2));
  
  TE_LOG_TRACE(TR_WMS("TerraLib WMS driver startup!"));

  m_initialized = true;
}

void te::wms::Module::shutdown()
{
  if(!m_initialized)
    return;

  te::da::DataSourceFactory::remove(TE_WMS_DRIVER_IDENTIFIER);

  te::da::DataSourceManager::getInstance().detachAll(TE_WMS_DRIVER_IDENTIFIER);

  TE_LOG_TRACE(TR_WMS("TerraLib WMS driver shutdown!"));

  m_initialized = false;
}

PLUGIN_CALL_BACK_IMPL(te::wms::Module)