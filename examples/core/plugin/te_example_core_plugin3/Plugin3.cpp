/*
  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

  This file is part of the TerraLib - a Framework for building GIS enabled
  applications.

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
  \file terralib/examples/core/plugin/terralib_example_core_plugin3/Plugin3.cpp

  \brief Example of a C++ Library that will be loaded as a plugin.

  \author Matheus Cavassan Zaglia
*/

// TerraLib
#include <terralib/core/plugin/CppPlugin.h>

// STL
#include <iostream>

TERRALIB_CPP_PLUGIN_BEGIN(Plugin3)

TERRALIB_CPP_PLUGIN_STARTUP
{
  if(m_initialized)
    return;
  
  std::cout << "Plugin3 startup" << std::endl;
  
  m_initialized = true;
}

TERRALIB_CPP_PLUGIN_SHUTDOWN
{
  std::cout << "Plugin3 shutdown" << std::endl;
  
  m_initialized = false;
}

TERRALIB_CPP_PLUGIN_END(Plugin3)
