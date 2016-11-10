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
  \file terralib/wms/WMS2Layer.cpp

  \brief A functor that converts a dataset from WMS server to a TerraLib WMSLayer.
*/

// TerraLib
#include "../core/translator/Translator.h"
#include "../dataaccess/dataset/DataSetType.h"
#include "../dataaccess/datasource/DataSourceManager.h"
#include "../dataaccess/datasource/DataSourceTransactor.h"
#include "../dataaccess/utils/Utils.h"
#include "../geometry/Envelope.h"
#include "../maptools/AbstractLayer.h"
#include "../raster/Grid.h"
#include "../raster/RasterProperty.h"
#include "../se/Utils.h"
#include "../srs/Config.h"
#include "Exception.h"
#include "WMS2Layer.h"

// Boost
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

te::wms::WMS2Layer::WMS2Layer(const std::string& datasourceId)
  : m_datasourceId(datasourceId)
{
}

te::wms::WMSLayerPtr te::wms::WMS2Layer::operator()(const te::da::DataSetTypePtr& dataset) const
{
  static boost::uuids::basic_random_generator<boost::mt19937> gen;

  if(dataset.get() == 0)
    throw Exception(TE_TR("Can not convert a NULL dataset to a WMS layer!"));

  boost::uuids::uuid u = gen();
  std::string id = boost::uuids::to_string(u);

  std::string title = dataset->getTitle().empty() ? dataset->getName() : dataset->getTitle();

  WMSLayerPtr layer(new WMSLayer(id, title));
  layer->setDataSetName(dataset->getName());
  layer->setDataSourceId(m_datasourceId);
  layer->setVisibility(te::map::NOT_VISIBLE);

  if(dataset->size() == 0)
  {
    te::da::DataSourcePtr ds(te::da::DataSourceManager::getInstance().find(m_datasourceId));
    te::da::LoadProperties(dataset.get(), m_datasourceId);
  }

  assert(dataset->hasRaster());

  te::rst::Grid* grid = te::da::GetFirstRasterProperty(dataset.get())->getGrid();
  layer->setSRID(grid->getSRID());
  layer->setExtent(*(grid->getExtent()));
  layer->setStyle(te::se::CreateCoverageStyle(te::da::GetFirstRasterProperty(dataset.get())->getBandProperties()));

  return layer;
}
