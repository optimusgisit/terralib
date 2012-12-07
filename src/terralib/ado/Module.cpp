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

//! TerraLib
#include "../common/Logger.h"
#include "../common/Translator.h"
#include "../dataaccess/datasource/DataSourceManager.h"
#include "DataSource.h"
#include "DataSourceFactory.h"
#include "Globals.h"
#include "Module.h"


const std::string te::ado::Module::sm_unknownTypeName("UNKNOWN");
const std::string te::ado::Module::sm_int2TypeName("INT2");
const std::string te::ado::Module::sm_intTypeName("INT");
const std::string te::ado::Module::sm_int8TypeName("INT8");
const std::string te::ado::Module::sm_numericTypeName("NUMERIC");
const std::string te::ado::Module::sm_dateTypeName("DATE");
const std::string te::ado::Module::sm_timeTypeName("TIME");
const std::string te::ado::Module::sm_timeTZTypeName("TIMETZ");
const std::string te::ado::Module::sm_timeStampTypeName("TIMESTAMP");
const std::string te::ado::Module::sm_timeStampTZTypeName("TIMESTAMPTZ");
const std::string te::ado::Module::sm_floatTypeName("FLOAT4");
const std::string te::ado::Module::sm_doubleTypeName("FLOAT8");
const std::string te::ado::Module::sm_booleanTypeName("BOOL");
const std::string te::ado::Module::sm_charTypeName("CHAR");
const std::string te::ado::Module::sm_varcharTypeName("VARCHAR");
const std::string te::ado::Module::sm_stringTypeName("TEXT");
const std::string te::ado::Module::sm_byteArrayTypeName("BYTEA");
const std::string te::ado::Module::sm_pointTypeName("POINT");
const std::string te::ado::Module::sm_pointMTypeName("POINTM");
const std::string te::ado::Module::sm_lineStringTypeName("LINESTRING");
const std::string te::ado::Module::sm_lineStringMTypeName("LINESTRINGM");
const std::string te::ado::Module::sm_polygonTypeName("POLYGON");
const std::string te::ado::Module::sm_polygonMTypeName("POLYGONM");
const std::string te::ado::Module::sm_geometryCollectionTypeName("GEOMETRYCOLLECTION");
const std::string te::ado::Module::sm_geometryCollectionMTypeName("GEOMETRYCOLLECTIONM");
const std::string te::ado::Module::sm_multiPointTypeName("MULTIPOINT");
const std::string te::ado::Module::sm_multiPointMTypeName("MULTIPOINTM");
const std::string te::ado::Module::sm_multiLineStringTypeName("MULTILINESTRING");
const std::string te::ado::Module::sm_multiLineStringMTypeName("MULTILINESTRINGM");
const std::string te::ado::Module::sm_multiPolygonTypeName("MULTIPOLYGON");
const std::string te::ado::Module::sm_multiPolygonMTypeName("MULTIPOLYGONM");
const std::string te::ado::Module::sm_geometryTypeName("GEOMETRY");

te::ado::Module::Module(const te::plugin::PluginInfo& pluginInfo)
  : te::plugin::Plugin(pluginInfo)
{
}

te::ado::Module::~Module()
{
}

void te::ado::Module::startup()
{
  if(m_initialized)
    return;

// it initializes the Translator support for the TerraLib ADO driver support
  TE_ADD_TEXT_DOMAIN(TE_ADO_TEXT_DOMAIN, TE_ADO_TEXT_DOMAIN_DIR, "UTF-8");

// it initializes the OGR Factory support
  DataSourceFactory::initialize();

  TE_LOG_TRACE(TR_ADO("TerraLib ADO driver startup!"));

  m_initialized = true;
}

void te::ado::Module::shutdown()
{
  if(!m_initialized)
    return;

// it finalizes the OGR factory support.
  te::ado::DataSourceFactory::finalize();

// free OGR registered drivers
  te::da::DataSourceManager::getInstance().detachAll(ADO_DRIVER_IDENTIFIER);
 
  TE_LOG_TRACE(TR_ADO("TerraLib OGR driver shutdown!"));

  m_initialized = false;
}

PLUGIN_CALL_BACK_IMPL(te::ado::Module)
