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

#include "MainWindow.h"

#include <terralib/common.h>
#include <terralib/core/plugin.h>
#include <terralib/core/utils/Platform.h>


#include <QApplication>
#include <QMessageBox>

//! STL
#include <iostream>

void LoadDrivers()
{
  try
  {
    te::core::PluginInfo info;

    std::string plugins_path = te::core::FindInTerraLibPath("share/terralib/plugins");

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

#ifdef TERRALIB_MOD_GDAL_ENABLED
    info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.gdal.teplg.json");
    te::core::PluginManager::instance().insert(info);
    te::core::PluginManager::instance().load(info.name);
#endif
  }
  catch(const te::common::Exception& e)
  {
    throw e;
  }
}

int main(int argc, char** argv)
{
  try
  {
    TerraLib::getInstance().initialize();
    te::core::plugin::InitializePluginSystem();
    LoadDrivers();

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    app.exec();

  }
  catch(const te::common::Exception& e)
  {
    std::cout << std::endl << "An exception has occurred:" << std::endl;
    std::cout << e.what() << std::endl;
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    return EXIT_FAILURE;
  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An unexpected exception has occurred!" << std::endl;
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    return EXIT_FAILURE;
  }

  te::core::PluginManager::instance().clear();
  te::core::plugin::FinalizePluginSystem();
  // finalize Terralib support
  TerraLib::getInstance().finalize();

  std::cout << "Press Enter to exit..." << std::endl;
  std::cin.get();

  return EXIT_SUCCESS;
}
