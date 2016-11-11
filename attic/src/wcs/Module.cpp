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
  \file terralib/wcs/Module.cpp
   
  \brief The TerraLib WCS driver as a plugin.
*/

// TerraLib
#include "../core/logger/Logger.h"
#include "../core/translator/Translator.h"
#include "../dataaccess/datasource/DataSourceFactory.h"
#include "../dataaccess/datasource/DataSourceManager.h"
#include "Config.h"
#include "DataSourceFactory.h"
#include "Module.h"

// GDAL
#include <gdal_priv.h>

// Boost
#include <boost/functional/factory.hpp>
#include <boost/bind.hpp>

te::wcs::Module::Module(const te::core::PluginInfo& pluginInfo)
  : te::core::CppPlugin(pluginInfo)
{
}

te::wcs::Module::~Module()
{
}

void te::wcs::Module::startup()
{
  if(m_initialized)
    return;

  te::da::DataSourceFactory::add(TE_WCS_DRIVER_IDENTIFIER, te::wcs::Build);

  GDALAllRegister();

  TE_LOG_TRACE(TE_TR("TerraLib WCS driver startup!"));

  m_initialized = true;
}

void te::wcs::Module::shutdown()
{
  if(!m_initialized)
    return;

  te::da::DataSourceFactory::remove(TE_WCS_DRIVER_IDENTIFIER);

  te::da::DataSourceManager::getInstance().detachAll(TE_WCS_DRIVER_IDENTIFIER);

  TE_LOG_TRACE(TE_TR("TerraLib WCS driver shutdown!"));

  m_initialized = false;
}

TERRALIB_PLUGIN_CALL_BACK_IMPL(te::wcs::Module)
