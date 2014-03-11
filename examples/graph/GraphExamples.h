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
  \file GraphExamples.h

  \brief These routines show how to use the Graph module.
 */

#ifndef __TERRALIB_EXAMPLES_INTERNAL_GRAPH_EXAMPLES_H
#define __TERRALIB_EXAMPLES_INTERNAL_GRAPH_EXAMPLES_H

//TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/geometry/Envelope.h>
#include <terralib/raster/Raster.h>
#include "../Config.h"

//STL
#include <map>
#include <memory>
#include <string>

/*! \brief It loads the data source drivers. */
void LoadModules();

/*! \brief Creates a LDD GRAPH. */
void CreateLDDGraph(bool draw);

/*! \brief Creates a GPM with adjacency strategy GRAPH. */
void CreateGPMAdjacencyGraph(bool draw);

/*! \brief Creates a GPM with distance strategy GRAPH. */
void CreateGPMDistanceGraph(bool draw);

/*! \brief Auxiliar functions for load a raster. */
std::auto_ptr<te::rst::Raster> OpenRaster(const std::string& pathName, const int& srid);

/*! \brief Auxiliar functions for load a data source. */
std::auto_ptr<te::da::DataSource> OpenDataSource(std::map<std::string, std::string> connInfo, std::string dsType);

/*! \brief Auxiliar functions for load a org data source. */
std::auto_ptr<te::da::DataSource> OpenOGRDataSource(const std::string& pathName);

/*! \brief Auxiliar functions used to get a dataset extent. */
std::auto_ptr<te::gm::Envelope> getDataSetExtent(te::da::DataSource* ds, std::string dataSetName);

#endif
