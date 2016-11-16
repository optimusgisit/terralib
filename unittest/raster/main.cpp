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
  \file terralib/unittest/raster/main.cpp

  \brief Main file of test suit for the Raster Module.
*/

// TerraLib
#include <terralib/raster_fw.h>
#include <terralib/common/Module.h>
#include <terralib/core/plugin.h>
#include "Config.h"
#include "LoadModules.h"

// STL
#include <cstdlib>

// Boost
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

bool init_unit_test()
{
  return true;
}

int main(int argc, char *argv[])
{
  /* Initialize Terralib platform */
  TerraLib::getInstance().initialize();
  te::core::plugin::InitializePluginSystem();
    
  LoadModules();

  int resultStatus = boost::unit_test::unit_test_main(init_unit_test, argc, argv);
  
  /* Finalize TerraLib Plataform */
  te::core::plugin::FinalizePluginSystem();
  TerraLib::getInstance().finalize();  

  return resultStatus;
}
