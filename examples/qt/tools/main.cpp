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

  \brief Example for the TerraLib Qt Tools.
 */

// Example
#include "MainWindow.h"

// TerraLib
#include <terralib/core/utils/Platform.h>
#include <terralib/core/plugin.h>
#include <terralib/common.h>


// Qt
#include <QMetaType>
#include <QApplication>
#include <QMessageBox>

// STL
#include <exception>
#include <iostream>

void LoadModules()
{
    te::core::PluginInfo info;

    std::string plugins_path = te::core::FindInTerraLibPath("share/terralib/plugins");

#ifdef TERRALIB_MOD_OGR_ENABLED
    info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.ogr.teplg.json");
    te::core::PluginManager::instance().insert(info);
    te::core::PluginManager::instance().load(info.name);
#endif

#ifdef TERRALIB_MOD_GDAL_ENABLED
    info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.gdal.teplg.json");
    te::core::PluginManager::instance().insert(info);
    te::core::PluginManager::instance().load(info.name);
#endif
}


int main(int argc, char** argv)
{
// initialize Terralib support
  TerraLib::getInstance().initialize();
  te::core::plugin::InitializePluginSystem();
  try
  {
    QApplication app(argc, argv);

    qRegisterMetaType<QImage>("QImage");

    // Adjusting icons theme
    QStringList ithemes = QIcon::themeSearchPaths();
    ithemes.push_back(te::core::FindInTerraLibPath("share/terralib/icons").c_str());
    QIcon::setThemeName("terralib");
    QIcon::setThemeSearchPaths(ithemes);

    LoadModules();

    // Instructions
    QString text = QString::fromUtf8("<p>Here you have a Map Display component associated with a set of geographic tools.\
                                        <p>There are somes tools always active. e.g.:\
                                        <ul><li><b>CoordTracking:</b> move the mouse on Map Display and see the tracked geographic coordinate on status bar;</li>\
                                            <li><b>ZoomWheel:</b> use the mouse wheel to performs a zoom on MapDisplay;</li>\
                                            <li><b>ZoomKeyboard:</b> you can use the keys (+) / (Up) or (-) / (Down) to performs a zoom also.</li>\
                                        </ul>\
                                        <p>Some tools can be activated using the tool bar. e.g.\
                                        <ul><li><b>Pan:</b> click on MapDisplay using the left mouse button. Keep the button hold and move. Release the button to perform a pan;</li>\
                                            <li><b>ZoomIn:</b> click on MapDisplay using the left mouse button. You will perform a zoom in on clicked point;</li>\
                                            <li><b>ZoomOut:</b> click on MapDisplay using the left mouse button. You will perform a zoom out on clicked point;</li>\
                                            <li><b>ZoomArea:</b> click on MapDisplay using the left mouse button. Keep the button hold and move to define a boundary rectangle. Release the button to perform a zoom in on this area;</li>\
                                            <li><b>Measure:</b> you can measure distance, area and angles. Try it!\
                                                         Select the specified tool. Click on MapDisplay using the left mouse button to fix a point. Move the mouse to see a geometry and the measured value.\
                                                         Double click left button to stop the measuring.</li>\
                                        </ul>\
                                        <p>Finally, this example also provides a tool called <b>Selection</b>.<br>\
                                           The only purpose of this tool is to show how you can implement a new tool.<br>\
                                           Do not consider it as a final application!<br>\
                                           So, select it and click with the left button. You will see the feature attributes.\
                                        ");

    // Show instructions
    QMessageBox instructions;
    instructions.setWindowTitle("Information");
    instructions.setText(text);
    instructions.setTextFormat(Qt::RichText);
    instructions.setWindowModality(Qt::NonModal);
    instructions.move(0, 0);
    instructions.show();

    // Example MainWindow
    MainWindow window;
    window.show();

    app.exec();
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred!" << std::endl;

    return EXIT_FAILURE;
  }

  te::core::PluginManager::instance().clear();
  te::core::plugin::FinalizePluginSystem();
// finalize Terralib support
  TerraLib::getInstance().finalize();

  return EXIT_SUCCESS;
}
