/*  Copyright (C) 2001-20013 National Institute For Space Research (INPE) - Brazil.

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

  \brief An example of using the TerraLib ADO Driver.
 */

// TerraLib
#include <terralib/common/TerraLib.h>
#include <terralib/dataaccess.h>
#include <terralib/plugin.h>

// Example
#include "ADOExamples.h"

// STL
#include <cassert>
#include <cstdlib>
#include <exception>
#include <iostream>

int main(int /*argc*/, char** /*argv*/)
{
  try
  {
    // Initialize Terralib support
    TerraLib::getInstance().initialize();
    
    LoadModules();

    // Set the minimum server connection information needed to connect to the database server
    std::map<std::string, std::string> connInfo;
    connInfo["Provider"] = "Microsoft.Jet.OLEDB.4.0";
    connInfo["host"] = "localhost";
    connInfo["user"] = "";
    connInfo["password"] = "";
    //connInfo["dbname"] = "C:\\Curso_TerraView\\BD_TView\\Modelo_Dados.mdb";
    //connInfo["dbname"] = "C:\\Users\\juan\\Documents\\ADO_Examples\\Northwind.mdb";
    connInfo["dbname"] = ""TE_DATA_EXAMPLE_LOCALE"/data/mdb/Northwind.mdb";
   
    // Create a data source using the data source factory
    te::da::DataSource* ds = te::da::DataSourceFactory::make("ADO");

    // Open the data source using the connection information given above
    ds->open(connInfo);

    // Show the information about datasets stored in the data source catalog
    PrintCatalog(ds);

    // Retrieve and then print the datasets stored in the data source
    PrintDataSets(ds);
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
  
  te::plugin::PluginManager::getInstance().unloadAll();
  
  TerraLib::getInstance().finalize();
  
  std::cout << "Press Enter to exit..." << std::endl;
  std::cin.get();
  
  return EXIT_SUCCESS;
}
