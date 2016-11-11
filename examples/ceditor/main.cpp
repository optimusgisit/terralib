/*
  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/examples/ceditor/main.cpp

  \brief Examples for the TerraLib Code Editor Module

  \author Frederico Augusto Bedê
 */

// TerraLib
#include <terralib/common.h>
#include <terralib/core.h>


// Qt
#include <QApplication>
#include <QIcon>

// Example
#include "MainWindow.h"

void LoadModule(std::string m)
{
  std::string mod_name = "share/terralib/plugins/" + m + ".teplg.json";
  std::string plgManifest = te::core::FindInTerraLibPath(mod_name);
  te::core::PluginInfo i(te::core::JSONPluginInfoSerializer(plgManifest));
  te::core::PluginManager::instance().load(i.name);
}

void Initialize()
{
  TerraLib::getInstance().initialize();

  LoadModule("te.da.gdal");
  LoadModule("te.da.ogr");
  LoadModule("te.da.pgis");
  LoadModule("te.vm.lua");
  LoadModule("te.vm.python");
}

void Finalize()
{
  te::core::PluginManager::instance().clear();

  TerraLib::getInstance().finalize();
}

// Main program
int main(int argc, char *argv[])
{
  Initialize();

  std::string iconThemePath = te::core::FindInTerraLibPath("share/terralib/icons");

  QStringList pts = QIcon::themeSearchPaths();

  pts << QString::fromUtf8(iconThemePath.c_str());

  QIcon::setThemeSearchPaths(pts);

  QIcon::setThemeName("terralib");

  QApplication app(argc, argv);

  MainWindow win;

  win.showMaximized();

  int r = app.exec();

  Finalize();

  return r;
}
