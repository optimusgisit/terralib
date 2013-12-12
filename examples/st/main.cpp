/*  Copyright (C) 2001-2009 National Institute For Space Research (INPE) - Brazil.

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

  \brief Examples about TerraLib ST Module.
*/

// TerraLib
#include <terralib/common.h>
#include <terralib/dataaccess.h>
#include <terralib/st.h>
#include <terralib/plugin.h>

// STL
#include <cassert>
#include <cstdlib>
#include <exception>
#include <iostream>

#include "STExamples.h"

int main(int /*argc*/, char** /*argv*/)
{    
  try
  {
    TerraLib::getInstance().initialize();

    //Load Modules and Plugins
    LoadModules();

<<<<<<< HEAD
    CoverageSeriesExamples();

    //TrajectoryExamplesFromKML();
     
=======
    //Initialize STDataLoader support
    te::st::STDataLoader::initialize();
    
    //Examples of trajectories
	  //TrajectoryExamples();
		
    //Examples of coverage series
	  CoverageSeriesExamples();

    //Finalize STDataLoader support
    te::st::STDataLoader::finalize();

>>>>>>> 5.0.0-alpha
    te::plugin::PluginManager::getInstance().unloadAll();

    TerraLib::getInstance().finalize();
  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurried: " << e.what() << std::endl;

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    return EXIT_FAILURE;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurried!" << std::endl;

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    return EXIT_FAILURE;
  }

  std::cout << "Press Enter to exit..." << std::endl;
  std::cin.get();

  return EXIT_SUCCESS;
}


