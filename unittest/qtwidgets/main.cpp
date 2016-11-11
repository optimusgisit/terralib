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

// TerraLib
#include <terralib/color.h>
#include <terralib/common.h>
#include <terralib/dataaccess.h>
#include <terralib/geometry.h>
#include <terralib/maptools.h>
#include <terralib/postgis.h>
#include <terralib/qt/widgets.h>

#include <terralib/common/TerraLib.h>
#include <terralib/plugin.h>

// cppUnit
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/XmlOutputter.h>

void LoadModules()
{
  try
  {      
    te::plugin::PluginInfo* info;
      
    info = te::plugin::GetInstalledPlugin(TE_PLUGINS_PATH + std::string("/te.da.ogr.teplg.json"));
    te::plugin::PluginManager::getInstance().add(info); 

    info = te::plugin::GetInstalledPlugin(TE_PLUGINS_PATH + std::string("/te.da.gdal.teplg.json"));
    te::plugin::PluginManager::getInstance().add(info);
       
    info = te::plugin::GetInstalledPlugin(TE_PLUGINS_PATH + std::string("/te.da.pgis.teplg.json"));
    te::plugin::PluginManager::getInstance().add(info);       
  
    te::plugin::PluginManager::getInstance().loadAll(); 
  }
  catch(const te::common::Exception& e)
  {
    throw e;
  }
}


int main(int /*argc*/, char** /*argv*/)
{  
  
  try
  {
    // initialize Terralib support
    TerraLib::getInstance().initialize();
    
    LoadModules();
    // it creates the event manager and test controller
    CPPUNIT_NS::TestResult controller;
    
    // it adds a listener that colllects test result 
    CPPUNIT_NS::TestResultCollector result;
    
    controller.addListener(&result);
    
    // it adds a listener that print dots as test run.
    CPPUNIT_NS::BriefTestProgressListener progress;
    
    controller.addListener(&progress);
    
    // it adds the top suite to the test runner
    CppUnit::Test* suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
    
    CPPUNIT_NS::TestRunner runner;
    
    runner.addTest(suite);
    
    runner.run(controller);
    
    CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
    
    outputter.write();
    
    // Printing testResult in XML file 
    // The testResult_*.xml files will be saved at TERRALIB_REPORT_DIR directory.
    // The styleSheet 'report.xsl' should be at this directory (found originally at <third-party-lib>\cppunit-1.12.1\contrib\xml-xsl).
    // The "data.zip" (downloaded) containing the data used in unit tests should be at TERRALIB_DATA_DIR 

    CPPUNIT_NS::OFileStream file2( TE_OUTPUT_REPORT_DIR "/testsResult_qtwidgets_xml.xml" );
    CPPUNIT_NS::XmlOutputter xml( &result, file2 );
    xml.setStyleSheet( "report.xsl" );
    xml.write();
    file2.close();

    te::plugin::PluginManager::getInstance().unloadAll();
    TerraLib::getInstance().finalize();
    
    bool resultStatus = result.wasSuccessful();
    
    return resultStatus ? EXIT_SUCCESS : EXIT_FAILURE;  
  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occuried: " << e.what() << std::endl;
    
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    
    return EXIT_FAILURE;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occuried!" << std::endl;
    
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    
    return EXIT_FAILURE;
  }    

}

