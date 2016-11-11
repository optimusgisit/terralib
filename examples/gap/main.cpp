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
  \file main.cpp

  \brief A list of examples for the TerraLib Spatial Reference Systems Module.
 */

// TerraLib
#include <terralib/common.h>
#include <terralib/common/TerraLib.h>
#include <terralib/core/utils/Platform.h>
#include <terralib/core/plugin.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>


#include "GAP.h"

void LoadModules()
{
  te::core::PluginInfo info;

  std::string plugins_path = te::core::FindInTerraLibPath("share/terralib/plugins");

#ifdef TERRALIB_MOD_POSTGIS_ENABLED
  info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.pgis.teplg.json");
  te::core::PluginManager::instance().insert(info);
  te::core::PluginManager::instance().load(info.name);
#endif

#ifdef TERRALIB_MOD_GDAL_ENABLED
  info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.gdal.teplg.json");
  te::core::PluginManager::instance().insert(info);
  te::core::PluginManager::instance().load(info.name);
#endif

#ifdef TERRALIB_MOD_OGR_ENABLED
  info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.ogr.teplg.json");
  te::core::PluginManager::instance().insert(info);
  te::core::PluginManager::instance().load(info.name);
#endif

#ifdef TERRALIB_MOD_MYSQL_ENABLED
  info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.mysql.teplg.json");
  te::core::PluginManager::instance().insert(info);
  te::core::PluginManager::instance().load(info.name);
#endif

#ifdef TERRALIB_MOD_SQLITE_ENABLED
  info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.sqlite.teplg.json");
  te::core::PluginManager::instance().insert(info);
  te::core::PluginManager::instance().load(info.name);
#endif
}

int main(int argc, char** argv)
{
// initialize Terralib support
// It initializes all the data source drivers (see LoadModule.cpp)
  try
  {
    TerraLib::getInstance().initialize();
    te::core::plugin::InitializePluginSystem();
    LoadModules();

    //Aggregation examples

    std::cout << std::endl << "GLUE " << std::endl;
    GAP g;

    g.run();
 

    te::core::PluginManager::instance().clear();
    te::core::plugin::FinalizePluginSystem();
    TerraLib::getInstance().finalize();
  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurred: " << e.what() << std::endl;

    return EXIT_FAILURE;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred!" << std::endl;

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
