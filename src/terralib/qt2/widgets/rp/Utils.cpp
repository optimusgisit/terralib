/*  Copyright (C) 2008-2011 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/qt/widgets/rp/Utils.h

  \brief Utility functions for the qt widgets rp Module.
*/

// TerraLib
#include "../../../dataaccess2/utils/Utils.h"
#include "../../../maptools2/RasterLayer.h"
#include "../../../raster/Raster.h"
#include "Utils.h"

// Boost
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>


te::map::AbstractLayerPtr te::qt::widgets::createLayer(const std::string& layerName, 
                                                       const std::map<std::string, std::string>& connInfo)
{
  te::map::AbstractLayerPtr layer;

  static boost::uuids::basic_random_generator<boost::mt19937> gen;

  boost::uuids::uuid layerU = gen();
  std::string layerId = boost::uuids::to_string(layerU);

  te::map::RasterLayer* rLayer = new te::map::RasterLayer(layerId, layerName);

  rLayer->setRasterInfo(connInfo);
  rLayer->setVisibility(te::map::VISIBLE);
  rLayer->setRendererType("RASTER_LAYER_RENDERER");

  layer.reset(rLayer);

  return layer;
}
