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

  \brief A list of examples for the TerraLib Widgets.
 */

// TerraLib
#include <terralib/core/utils/Platform.h>
#include <terralib/common.h>
#include <terralib/core/plugin.h>

#include "SymbologyWidgets.h"

// STL
#include <exception>
#include <iostream>

void LoadGDALModule()
{
  try
  {
    //te::plugin::PluginInfo info; 
    //info.m_name = "te.da.gdal";
    //info.m_displayName = "GDAL DataSource Driver";
    //info.m_description = "This data source driver supports spatial data managed by GDAL";
    //info.m_engine = "C++";
    //info.m_folder = TE_PLUGINS_PATH;
    //
    //std::pair<std::string, std::string> rsc("SharedLibraryName", "terralib_gdal");
    //
    //info.m_resources.push_back(rsc);
    //
    //te::plugin::PluginManager::getInstance().load(info);
    //te::plugin::PluginInfo* info = te::core::JSONPluginInfoSerializer(TE_PLUGINS_PATH + std::string("/te.da.gdal.teplg.json"));
    
    std::string plugins_path = te::core::FindInTerraLibPath("share/terralib/plugins");

#ifdef TERRALIB_MOD_GDAL_ENABLED
    te::core::PluginInfo info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.gdal.teplg.json");
    te::core::PluginManager::instance().insert(info);    
    te::core::PluginManager::instance().load(info.name);
#endif
  }
  catch(...)
  {
    std::cout << std::endl << "Failed to load data source drivers: unknow exception!" << std::endl;
  }
}

int main(int /*argc*/, char** /*argv*/)
{
// initialize Terralib support
  TerraLib::getInstance().initialize();
  te::core::plugin::InitializePluginSystem();
  try
  {
    LoadGDALModule();
    
    SymbologyWidgets();

    RasterSymbologyWidgets();
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

// finalize Terralib support
  te::core::PluginManager::instance().clear();
  te::core::plugin::FinalizePluginSystem();
  TerraLib::getInstance().finalize();


  return EXIT_SUCCESS;
}
