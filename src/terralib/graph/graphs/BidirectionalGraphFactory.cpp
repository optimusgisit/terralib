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
  \file BidirectionalGraphFactory.cpp

  \brief This is the concrete factory for the bidirectional Graph type.
*/

// TerraLib
#include "../core/GraphMetadata.h"
#include "../Globals.h"
#include "../Exception.h"
#include "BidirectionalGraph.h"
#include "BidirectionalGraphFactory.h"

// STL
#include <memory>

te::graph::BidirectionalGraphFactory* te::graph::BidirectionalGraphFactory::sm_factory(0);

const std::string& te::graph::BidirectionalGraphFactory::getType() const
{
  return Globals::sm_factoryGraphTypeBidirectionalGraph;
}

void te::graph::BidirectionalGraphFactory::getCreationalParameters(std::vector< std::pair<std::string, std::string> >& params) const
{
  params.push_back(std::pair<std::string, std::string>("GRAPH_DATA_SOURCE_TYPE", ""));
  params.push_back(std::pair<std::string, std::string>("GRAPH_ID", ""));
  params.push_back(std::pair<std::string, std::string>("GRAPH_NAME", ""));
  params.push_back(std::pair<std::string, std::string>("GRAPH_DESCRIPTION", ""));
  params.push_back(std::pair<std::string, std::string>("GRAPH_STORAGE_MODE", ""));
  params.push_back(std::pair<std::string, std::string>("GRAPH_STRATEGY_LOADER", ""));
  params.push_back(std::pair<std::string, std::string>("GRAPH_CACHE_POLICY", ""));
}

void te::graph::BidirectionalGraphFactory::initialize()
{
  finalize();
  sm_factory = new BidirectionalGraphFactory;
}

void te::graph::BidirectionalGraphFactory::finalize()
{
  delete sm_factory;
  sm_factory = 0;
}

te::graph::BidirectionalGraphFactory::BidirectionalGraphFactory()
  : te::graph::AbstractGraphFactory(Globals::sm_factoryGraphTypeBidirectionalGraph)
{
}

te::graph::AbstractGraph* te::graph::BidirectionalGraphFactory::iOpen(const std::string& dsInfo, const std::map<std::string, std::string>& gInfo)
{
   //create graph metadata
  te::graph::GraphMetadata* gMetadata = getMetadata(dsInfo, gInfo);

  if(gMetadata->m_memoryGraph)
    return 0;

  //get graph id
  int id = getId(gInfo);

  try
  {
    gMetadata->load(id);
  }
  catch(const std::exception& e)
  {
    std::string errorMessage = TE_TR("Error opening graph metadata: ");
    errorMessage += e.what();

    throw Exception(errorMessage);
  }

  //create cache policy strategy
  te::graph::AbstractCachePolicy* cp = getCachePolicy(gInfo);

  //create graph strategy
  te::graph::AbstractGraphLoaderStrategy* ls = getLoaderStrategy(gInfo, gMetadata);

  //create graph
  te::graph::AbstractGraph* g = new te::graph::BidirectionalGraph(cp, ls);

  return g;
}

te::graph::AbstractGraph* te::graph::BidirectionalGraphFactory::create(const std::string& dsInfo, const std::map<std::string, std::string>& gInfo)
{
  //create graph metadata
  te::graph::GraphMetadata* gMetadata = getMetadata(dsInfo, gInfo);

  setMetadataInformation(gInfo, gMetadata);

  try
  {
    gMetadata->save();
  }
  catch(const std::exception& e)
  {
    std::string errorMessage = TE_TR("Error saving graph metadata: ");
    errorMessage += e.what();

    throw Exception(errorMessage);
  }

  //create cache policy strategy
  te::graph::AbstractCachePolicy* cp = getCachePolicy(gInfo);

  //create graph strategy
  te::graph::AbstractGraphLoaderStrategy* ls = getLoaderStrategy(gInfo, gMetadata);

  //create graph
  te::graph::AbstractGraph* g = 0;

  if(gMetadata->m_memoryGraph)
    g = new te::graph::BidirectionalGraph(gMetadata);
  else
    g = new te::graph::BidirectionalGraph(cp, ls);

  return g;
}

te::graph::AbstractGraph* te::graph::BidirectionalGraphFactory::build()
{
  return new Graph;
}