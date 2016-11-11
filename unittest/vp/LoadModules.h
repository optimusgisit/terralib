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
  \file LoadModules.h
 
  \brief Load terralib modules.
 */

#ifndef __TERRALIB_UNITTEST_VP_LOADMODULES_H
#define __TERRALIB_UNITTEST_VP_LOADMODULES_H

#include <terralib/core/utils/Platform.h>
#include <terralib/core/plugin.h>
#include <terralib/common.h>
#include "Config.h"

/*!
  \brief Load terralib modules.
 */

void LoadModules()
{
  std::string plugins_path = te::core::FindInTerraLibPath("share/terralib/plugins");
  
  te::core::PluginInfo info;

#ifdef TERRALIB_MOD_OGR_ENABLED
  info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.ogr.teplg.json");
  te::core::PluginManager::instance().insert(info); 
  te::core::PluginManager::instance().load(info.name);
#endif

#ifdef TERRALIB_MOD_POSTGIS_ENABLED
  info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.pgis.teplg.json");
  te::core::PluginManager::instance().insert(info);
  te::core::PluginManager::instance().load(info.name);
#endif
}

#endif  // __TERRALIB_UNITTEST_VP_LOADMODULES_H
