/*  Copyright (C) 2011-2012 National Institute For Space Research (INPE) - Brazil.

    This file is part of TerraView - A GIS Application.

    TerraView is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraView is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TerraLib Code Editor. See COPYING. If not, write to
    TerraLib Team at <terralib-team@dpi.inpe.br>.
 */

/*!
  \file terraview/main.cpp

  \brief It contains the main routine of TerraView.
*/

#include "MainWindow.h"

//! Qt include files
#include <QtGui/QApplication>
#include <QStyle>

//! STL include files
#include <exception>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  int waitVal = EXIT_FAILURE;
  //QString spaths(ICON_THEME_PATH);

  //QStringList paths = spaths.split(";");

  //QIcon::setThemeName("terralib");
  //QIcon::setThemeSearchPaths(paths);

//  app.setStyleSheet();

  try
  {
// initialize the application
    MainWindow dlg;
    dlg.showMaximized();

    waitVal = app.exec();
  }
  catch(const std::exception& /*e*/)
  {
    return EXIT_FAILURE;
  }
  catch(...)
  {
    return EXIT_FAILURE;
  }

  return waitVal;
}

