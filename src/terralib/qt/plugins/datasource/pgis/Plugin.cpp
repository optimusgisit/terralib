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
  \file terralib/qt/plugins/datasource/pgis/Plugin.cpp

  \brief Plugin implementation for the PostGIS data source widget.
*/

// TerraLib
#include "../../../../common/Config.h"
#include "../../../../core/translator/Translator.h"
#include "../../../../core/logger/Logger.h"
#include "../../../../dataaccess/datasource/DataSourceInfoManager.h"
#include "../../../widgets/datasource/core/DataSourceTypeManager.h"
#include "PostGISType.h"
#include "Plugin.h"

te::qt::plugins::pgis::Plugin::Plugin(const te::core::PluginInfo& pluginInfo)
  : te::core::CppPlugin(pluginInfo)
{
}

te::qt::plugins::pgis::Plugin::~Plugin() 
{
}

void te::qt::plugins::pgis::Plugin::startup()
{
  if(m_initialized)
    return;

  te::qt::widgets::DataSourceTypeManager::getInstance().add(new PostGISType);

  TE_LOG_TRACE(TE_TR("TerraLib Qt PostGIS widget startup!"));

  m_initialized = true;
}

void te::qt::plugins::pgis::Plugin::shutdown()
{
  if(!m_initialized)
    return;

  te::da::DataSourceInfoManager::getInstance().removeByType("POSTGIS");
  te::qt::widgets::DataSourceTypeManager::getInstance().remove("POSTGIS");

  TE_LOG_TRACE(TE_TR("TerraLib Qt PostGIS widget shutdown!"));

  m_initialized = false;
}

TERRALIB_PLUGIN_CALL_BACK_IMPL(te::qt::plugins::pgis::Plugin)
