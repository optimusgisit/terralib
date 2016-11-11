#include "LoadModules.h"

#include "Config.h"

#include <terralib/core/utils/Platform.h>
#include <terralib/core/plugin.h>
#include <terralib/common.h>


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

#ifdef TERRALIB_MOD_POSTGIS_ENABLED
  info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.pgis.teplg.json");
  te::core::PluginManager::instance().insert(info);
  te::core::PluginManager::instance().load(info.name);
#endif

#ifdef TERRALIB_MOD_GRIB_ENABLED
  info = te::core::JSONPluginInfoSerializer(plugins_path + "/plugin_grib_info.xml");
  te::core::PluginManager::instance().insert(info);
  te::core::PluginManager::instance().load(info.name);
#endif
}
