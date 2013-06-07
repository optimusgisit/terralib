/*  Copyright (C) 2008-2013 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/dataaccess/memory/Module.cpp

  \brief This singleton defines the TerraLib In-Memory module entry.
*/

// TerraLib
#include "../../common/Logger.h"
#include "../../common/TerraLib.h"
#include "../../common/Translator.h"
#include "../core/datasource/DataSourceCapabilities.h"
#include "../core/datasource/DataSourceFactory.h"
#include "Config.h"
#include "DataSourceFactory.h"
#include "Module.h"

const te::da::mem::Module& sm_module = te::da::mem::Module::getInstance();

te::da::mem::Module::Module()
{
  TerraLib::Module m = { TE_MEMORY_MODULE_NAME,
                         te::da::mem::Module::initialize,
                         te::da::mem::Module::finalize
                       };

// initialize TerraLib singleton
  TerraLib::getInstance().add(m);

// it initializes the Translator support for the TerraLib Color module
 TE_ADD_TEXT_DOMAIN(TE_MEMORY_TEXT_DOMAIN, TE_MEMORY_TEXT_DOMAIN_DIR, "UTF-8");
}

te::da::mem::Module::~Module()
{
  TerraLib::getInstance().remove(TE_MEMORY_MODULE_NAME);
}

void te::da::mem::Module::initialize()
{
// register data source factory
  te::da::core::DataSourceFactory::add(TE_MEMORY_DRIVER_IDENTIFIER, te::da::mem::Build);

  //// DataSource Capabilities
  //te::da::DataSourceCapabilities capabilities;
  //capabilities.setAccessPolicy(te::common::RWAccess);
  //capabilities.setSupportDataSetPesistenceAPI(true);
  //capabilities.setSupportDataSetTypePesistenceAPI(true);

  //// DataType Capabilities
  //te::da::DataTypeCapabilities dataTypeCapabilities;
  //dataTypeCapabilities.setSupportAll();
  //capabilities.setDataTypeCapabilities(dataTypeCapabilities);

  //// DataSetType Capabilites
  //te::da::DataSetTypeCapabilities dataSetTypeCapabilities;
  //dataSetTypeCapabilities.setSupportAll();
  //dataSetTypeCapabilities.setSupportForeingKey(false);
  //dataSetTypeCapabilities.setSupportSequence(false);
  //dataSetTypeCapabilities.setSupportCheckConstraints(false);
  //dataSetTypeCapabilities.setSupportQuadTreeIndex(false);
  //capabilities.setDataSetTypeCapabilities(dataSetTypeCapabilities);

  //// DataSet Capabilities
  //te::da::DataSetCapabilities dataSetCapabilities;
  //dataSetCapabilities.setSupportAll();
  //capabilities.setDataSetCapabilities(dataSetCapabilities);

  //// Query Capabilities
  //te::da::QueryCapabilities queryCapabilities;
  //capabilities.setQueryCapabilities(queryCapabilities);

  //DataSource::setCapabilities(capabilities);

  TE_LOG_TRACE(TR_MEMORY("TerraLib In-Memory driver initialized!"));
}

void te::da::mem::Module::finalize()
{
// unregister data source factory
  te::da::core::DataSourceFactory::remove(TE_MEMORY_DRIVER_IDENTIFIER);

  TE_LOG_TRACE(TR_MEMORY("TerraLib In-Memory driver finalized!"));
}

