/*
  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/examples/vm/main.cpp

  \brief Examples for the Terralib Virtual Machine Module

  \author Frederico Augusto Bedê Teotônio
*/

// TerraLib
#include <terralib/common/TerraLib.h>
#include <terralib/core/lib/Library.h>
#include <terralib/core/utils/Platform.h>
#include <terralib/plugin/PluginInfo.h>
#include <terralib/plugin/PluginManager.h>
#include <terralib/plugin/Utils.h>
#include <terralib/vm/core/VirtualMachine.h>
#include <terralib/vm/core/VirtualMachineManager.h>

#include <memory>


void LoadModule(std::string m)
{
  std::string mod_name = "share/terralib/plugins/" + m + ".teplg";
  std::string plgManifest = te::core::FindInTerraLibPath(mod_name);

  std::unique_ptr<te::plugin::PluginInfo> i(te::plugin::GetInstalledPlugin(plgManifest));

  te::plugin::PluginManager::getInstance().load(*i.get());
}

void LoadModules()
{
  TerraLib::getInstance().initialize();

  LoadModule("te.da.gdal");
  LoadModule("te.da.ogr");
  LoadModule("te.da.pgis");
  LoadModule("te.vm.lua");
}


int main(int argc, char *argv[])
{
  LoadModules();

  std::string luaScript = te::core::FindInTerraLibPath("share/terralib/examples/lua/geometry.lua");

  te::vm::core::VirtualMachine* vm = te::vm::core::VirtualMachineManager::instance().get("lua");

  vm->build(luaScript);

  vm->execute();

  te::plugin::PluginManager::getInstance().unloadAll();

  TerraLib::getInstance().finalize();

  getchar();

  return EXIT_SUCCESS;
}
