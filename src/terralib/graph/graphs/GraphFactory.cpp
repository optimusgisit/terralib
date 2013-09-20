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
  \file GraphFactory.h

  \brief This is the concrete factory for the commun Graph type.
*/

// TerraLib
#include "../../dataaccess/datasource/DataSource.h"
#include "../../dataaccess/datasource/DataSourceFactory.h"
#include "../cache/AbstractCachePolicyFactory.h"
#include "../drivers/database/DatabaseGraphMetadata.h"
#include "../loader/AbstractGraphLoaderStrategyFactory.h"
#include "../Exception.h"
#include "../Globals.h"
#include "Graph.h"
#include "GraphFactory.h"

// STL
#include <memory>

te::graph::GraphFactory* te::graph::GraphFactory::sm_factory(0);

const std::string& te::graph::GraphFactory::getType() const
{
  return Globals::sm_factoryGraphTypeGraph;
}

void te::graph::GraphFactory::getCreationalParameters(std::vector< std::pair<std::string, std::string> >& params) const
{
  params.push_back(std::pair<std::string, std::string>("GRAPH_DATA_SOURCE_TYPE", ""));
  params.push_back(std::pair<std::string, std::string>("GRAPH_ID", ""));
  params.push_back(std::pair<std::string, std::string>("GRAPH_NAME", ""));
  params.push_back(std::pair<std::string, std::string>("GRAPH_DESCRIPTION", ""));
  params.push_back(std::pair<std::string, std::string>("GRAPH_STORAGE_MODE", ""));
  params.push_back(std::pair<std::string, std::string>("GRAPH_STRATEGY_LOADER", ""));
  params.push_back(std::pair<std::string, std::string>("GRAPH_CACHE_POLICY", ""));
}

void te::graph::GraphFactory::initialize()
{
  finalize();
  sm_factory = new GraphFactory;
}

void te::graph::GraphFactory::finalize()
{
  delete sm_factory;
  sm_factory = 0;
}

te::graph::GraphFactory::GraphFactory()
  : te::graph::AbstractGraphFactory(Globals::sm_factoryGraphTypeGraph)
{
}

te::graph::AbstractGraph* te::graph::GraphFactory::iOpen(const std::map<std::string, std::string>& dsInfo, const std::map<std::string, std::string>& gInfo)
{
  std::map<std::string, std::string>::const_iterator it;
  std::map<std::string, std::string>::const_iterator itend = gInfo.end();

  //create data source
  it = gInfo.find("GRAPH_DATA_SOURCE_TYPE");

  std::auto_ptr<te::da::DataSource> dsPtr;

  if(it != itend)
  {
    dsPtr = te::da::DataSourceFactory::make(it->second); //example: dsType = POSTGIS
    dsPtr->setConnectionInfo(dsInfo);
    dsPtr->open();
  }

  te::da::DataSource* ds = dsPtr.release();

  //create graph metadata
  te::graph::DatabaseGraphMetadata* gMetadata = new te::graph::DatabaseGraphMetadata(ds);

  it = gInfo.find("GRAPH_ID");
  if(it != itend)
  {
    try
    {
      int id = atoi(it->second.c_str());

      gMetadata->load(id);
    }
    catch(const std::exception& e)
    {
      std::string errorMessage = TR_GRAPH("Error saving graph metadata: ");
      errorMessage += e.what();

      throw Exception(errorMessage);
    }
  }
  else
  {
    return 0;
  }

  //create cache policy strategy
  te::graph::AbstractCachePolicy* cp = 0;
  it = gInfo.find("GRAPH_CACHE_POLICY");
  if(it != itend)
  {
    cp = te::graph::AbstractCachePolicyFactory::make(it->second);
  }

  //create graph strategy
  te::graph::AbstractGraphLoaderStrategy* ls = 0;
  it = gInfo.find("GRAPH_STRATEGY_LOADER");
  if(it != itend)
  {
    ls = te::graph::AbstractGraphLoaderStrategyFactory::make(it->second, gMetadata);
  }

  //create graph
  te::graph::AbstractGraph* g = new te::graph::Graph(cp, ls);

  return g;
}

te::graph::AbstractGraph* te::graph::GraphFactory::create(const std::map<std::string, std::string>& dsInfo, const std::map<std::string, std::string>& gInfo)
{
  std::map<std::string, std::string>::const_iterator it;
  std::map<std::string, std::string>::const_iterator itend = gInfo.end();

  //create data source
  it = gInfo.find("GRAPH_DATA_SOURCE_TYPE");

  std::auto_ptr<te::da::DataSource> dsPtr;

  if(it != itend)
  {
    dsPtr = te::da::DataSourceFactory::make(it->second); //example: dsType = POSTGIS
    dsPtr->setConnectionInfo(dsInfo);
    dsPtr->open();
  }

  te::da::DataSource* ds = dsPtr.release();
  
   //create graph metadata
  te::graph::DatabaseGraphMetadata* gMetadata = new te::graph::DatabaseGraphMetadata(ds);

  it = gInfo.find("GRAPH_NAME");
  if(it != itend)
  {
    gMetadata->setName(it->second);
  }

  it = gInfo.find("GRAPH_DESCRIPTION");
  if(it != itend)
  {
    gMetadata->setDescription(it->second);
  }

  it = gInfo.find("GRAPH_STORAGE_MODE");
  if(it != itend)
  {
    if(it->second == TE_GRAPH_STORAGE_MODE_BY_VERTEX)
    {
      gMetadata->setStorageMode(te::graph::Vertex_List);
    }
    else if(it->second == TE_GRAPH_STORAGE_MODE_BY_EDGE)
    {
      gMetadata->setStorageMode(te::graph::Edge_List);
    }
  }

  try
  {
    gMetadata->save();
  }
  catch(const std::exception& e)
  {
    std::string errorMessage = TR_GRAPH("Error saving graph metadata: ");
    errorMessage += e.what();

    throw Exception(errorMessage);
  }

  //create cache policy strategy
  te::graph::AbstractCachePolicy* cp = 0;
  it = gInfo.find("GRAPH_CACHE_POLICY");
  if(it != itend)
  {
    cp = te::graph::AbstractCachePolicyFactory::make(it->second);
  }

  //create graph strategy
  te::graph::AbstractGraphLoaderStrategy* ls = 0;
  it = gInfo.find("GRAPH_STRATEGY_LOADER");
  if(it != itend)
  {
    ls = te::graph::AbstractGraphLoaderStrategyFactory::make(it->second, gMetadata);
  }

  //create graph
  te::graph::AbstractGraph* g = new te::graph::Graph(cp, ls);

  return g;
}

te::graph::AbstractGraph* te::graph::GraphFactory::build()
{
  return new Graph;
}