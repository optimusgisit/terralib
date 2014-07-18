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
  \file GPMBuilder.cpp

  \brief This class defines the GPM Builder class.
*/

// TerraLib Includes
#include "../../dataaccess/datasource/DataSource.h"
#include "../../graph/core/AbstractGraphFactory.h"
#include "../../graph/Globals.h"
#include "GPMBuilder.h"
#include "GPMConstructorAbstractStrategy.h"

// STL Includes
#include <cassert>

te::sa::GPMBuilder::GPMBuilder(GPMConstructorAbstractStrategy* constructor) : 
  m_constructor(constructor), 
  m_gpm(0)
{
}

te::sa::GPMBuilder::~GPMBuilder()
{
  delete m_constructor;
}

bool te::sa::GPMBuilder::setGPMInfo(te::da::DataSourcePtr ds, const std::string& dataSetName, const std::string& attributeName)
{
  //set input data source
  m_ds = ds;

  //create gpm
  m_gpm.reset(new te::sa::GeneralizedProximityMatrix());

  m_gpm->setDataSetName(dataSetName);
  m_gpm->setAttributeName(attributeName);

// graph type
  std::string graphType = te::graph::Globals::sm_factoryGraphTypeDirectedGraph;

// connection info
  std::map<std::string, std::string> connInfo;

// graph information
  std::map<std::string, std::string> graphInfo;
  graphInfo["GRAPH_DATA_SOURCE_TYPE"] = "MEM";
  graphInfo["GRAPH_NAME"] = dataSetName + "_graph";
  graphInfo["GRAPH_DESCRIPTION"] = "Generated by GPM Builder.";

  //create output graph
  m_gpm->setGraph(te::graph::AbstractGraphFactory::make(graphType, connInfo, graphInfo));

  return true;
}

std::auto_ptr<te::sa::GeneralizedProximityMatrix> te::sa::GPMBuilder::build()
{
  assert(m_gpm.get());

  if(m_constructor)
    m_constructor->construct(m_ds.get(), m_gpm.get());

  return m_gpm;
}