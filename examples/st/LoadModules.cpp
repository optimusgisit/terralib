// Examples
#include "STExamples.h"

// TerraLib
#include <terralib/core/utils/Platform.h>
#include <terralib/core/plugin.h>
#include <terralib/common.h>


// STL
#include <iostream>

void LoadModules()
{
  try
  {
    std::string plugins_path = te::core::FindInTerraLibPath("share/terralib/plugins");

    te::core::PluginInfo info;
  
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
  
#ifdef TERRALIB_MOD_POSTGIS_ENABLED
    info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.pgis.teplg.json");
    te::core::PluginManager::instance().insert(info);
    te::core::PluginManager::instance().load(info.name);
#endif
  }
  catch(const boost::exception& e)
  {
    const std::string* err = boost::get_error_info<te::ErrorDescription>(e);
    std::cerr << std::endl << "Failed to load data source drivers: " << *err << std::endl;
  }
  catch(...)
  {
    std::cerr << std::endl << "Failed to load data source drivers: unknow exception!" << std::endl;
  }
}




