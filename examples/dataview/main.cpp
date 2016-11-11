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

#include "TabularViewerEx.h"

//! TerraLib include files
#include <terralib/common/TerraLib.h>
#include <terralib/common/Exception.h>
#include <terralib/qt/widgets/dataview/HLDelegateDecorator.h>

//#include <terralib/qt/widgets/dataview/TabularViewer.h>

//! Qt include files
#include <QApplication>
#include <QIcon>

void LoadDrivers()
{
  te::plugin::PluginInfo* info = te::plugin::GetInstalledPlugin(TE_PLUGINS_PATH + std::string("/te.da.ogr.teplg.json"));
  te::plugin::PluginManager::getInstance().add(info);   
  
  te::plugin::PluginManager::getInstance().loadAll();
}

int main(int argc, char** argv)
{
  int res;

  try
  {
    TerraLib::getInstance().initialize();
    
    LoadDrivers();
    

    QApplication app(argc, argv);

    QString spaths = std::string(ICON_THEME_PATH).c_str();
    QStringList paths = spaths.split(";");

    QIcon::setThemeName(ICON_THEME);
    QIcon::setThemeSearchPaths(paths);

    TabularViewerEx win;
    win.show();

    res = app.exec();
  }
  catch(te::common::Exception&)
  {
    return -1;
  }

  te::plugin::PluginManager::getInstance().unloadAll();

  TerraLib::getInstance().finalize();

  return res;
}
