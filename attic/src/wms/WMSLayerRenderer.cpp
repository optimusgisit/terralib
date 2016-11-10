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
  \file terralib/wms/WMSLayerRenderer.cpp

  \brief It renders the data associated to a WMS layer.
*/

// TerraLib
#include "../common/progress/TaskProgress.h"
#include "../common/STLUtils.h"
#include "../core/translator/Translator.h"
#include "../dataaccess/utils/Utils.h"
#include "../geometry/Envelope.h"
#include "../geometry/Utils.h"
#include "../maptools/Canvas.h"
#include "../maptools/CanvasConfigurer.h"
#include "../maptools/RasterTransform.h"
#include "../maptools/RasterTransformConfigurer.h"
#include "../raster/Grid.h"
#include "../raster/RasterProperty.h"
#include "../raster/RasterSummary.h"
#include "../raster/RasterSummaryManager.h"
#include "../raster/Utils.h"
#include "../se/CoverageStyle.h"
#include "../se/ImageOutline.h"
#include "../se/RasterSymbolizer.h"
#include "../se/Rule.h"
#include "../se/Utils.h"
#include "../srs/Config.h"
#include "../srs/Converter.h"
#include "Exception.h"
#include "WMSLayer.h"
#include "WMSLayerRenderer.h"

// Boost
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

// STL
#include <cassert>
#include <memory>

te::wms::WMSLayerRenderer::WMSLayerRenderer()
{
}

te::wms::WMSLayerRenderer::~WMSLayerRenderer()
{
}

void te::wms::WMSLayerRenderer::draw(te::map::AbstractLayer* layer,
                                     te::map::Canvas* canvas,
                                     const te::gm::Envelope& bbox,
                                     int srid, 
                                     const double& scale, bool* cancel)
{
  if(!bbox.isValid())
    throw Exception(TE_TR("The requested box is invalid!"));

  assert(layer);
  assert(canvas);

  // Should I render this layer?
  WMSLayer* wmsLayer = dynamic_cast<WMSLayer*>(layer);

  if(wmsLayer == 0)
    throw Exception(TE_TR("Wrong type render type for this layer!"));

  draw(wmsLayer, canvas, bbox, srid);
}

void te::wms::WMSLayerRenderer::draw(WMSLayer* layer, te::map::Canvas* canvas, const te::gm::Envelope& bbox, int srid)
{
  // Check if layer extent intersects the drawing area and so compute bounding box intersection
  te::gm::Envelope reprojectedBBOX(bbox);

  if((layer->getSRID() != TE_UNKNOWN_SRS) && (srid != TE_UNKNOWN_SRS))
  {
    reprojectedBBOX.transform(srid, layer->getSRID());
  }
  else if(layer->getSRID() != srid)
  {
    throw Exception(TE_TR("The layer or map has no valid SRID!"));
  }

  if(!reprojectedBBOX.intersects(layer->getExtent()))
    return;

  te::gm::Envelope ibbox = reprojectedBBOX.intersection(layer->getExtent());
  assert(ibbox.isValid());

  // Get the layer schema
  std::auto_ptr<te::map::LayerSchema> schema(layer->getSchema());
  assert(schema.get());

  // Get the raster property
  te::rst::RasterProperty* rasterProperty = te::da::GetFirstRasterProperty(schema.get());

  // Get the layer style
  te::se::Style* style = layer->getStyle();
  if(style == 0)
  {
    // Try create an appropriate style. i.e. a CoverageStyle
    style = te::se::CreateCoverageStyle(rasterProperty->getBandProperties());

    if(style == 0)
      throw Exception((boost::format(TE_TR("Could not create a default coverage style for the layer %1%!")) % layer->getTitle()).str());

    layer->setStyle(style);
  }

  // Should I render this style?
  te::se::CoverageStyle* cs = dynamic_cast<te::se::CoverageStyle*>(style);
  if(cs == 0)
    throw Exception(TE_TR("The layer style is not a Coverage Style!"));

  // Setup the WMS Layer to request data
  layer->setWidth(canvas->getWidth());
  layer->setHeight(canvas->getHeight());

  // Request the WMS data
  std::auto_ptr<te::da::DataSet> dataset = layer->getData(rasterProperty->getName(), &ibbox, te::gm::INTERSECTS);

  if(dataset.get() == 0)
    throw Exception((boost::format(TE_TR("Could not retrieve the WMS data from the layer %1%!")) % layer->getTitle()).str());

  std::size_t rpos = te::da::GetFirstPropertyPos(dataset.get(), te::dt::RASTER_TYPE);

  // Retrieve the raster
  std::auto_ptr<te::rst::Raster> raster(dataset->getRaster(rpos));
  if(dataset.get() == 0)
    throw Exception((boost::format(TE_TR("Could not retrieve the WMS data from the layer %1%!")) % layer->getTitle()).str());

  // Let's draw!
  drawRaster(layer->getTitle(), raster.get(), canvas, ibbox, layer->getSRID(), bbox, srid, cs);
}

void te::wms::WMSLayerRenderer::drawRaster(const std::string& layerTitle, te::rst::Raster* raster, te::map::Canvas* canvas, const te::gm::Envelope& bbox, int bboxSRID,
                                           const te::gm::Envelope& visibleArea, int srid, te::se::CoverageStyle* style)
{
  assert(raster);
  assert(canvas);
  assert(bbox.isValid());
  assert(visibleArea.isValid());
  assert(style);

  // Build the grid canvas. i.e. a grid with canvas dimensions and requested mbr
  te::gm::Envelope* gmbr = new te::gm::Envelope(visibleArea);
  std::auto_ptr<te::rst::Grid> gridCanvas(new te::rst::Grid(static_cast<unsigned int>(canvas->getWidth()), static_cast<unsigned int>(canvas->getHeight()), gmbr, srid));

  // Create a raster transform
  te::map::RasterTransform rasterTransform(raster, 0);

  // Check band data type
  if(raster->getBandDataType(0) != te::dt::UCHAR_TYPE)
  {
    // Raster min / max values
    const te::rst::RasterSummary* rsMin = te::rst::RasterSummaryManager::getInstance().get(raster, te::rst::SUMMARY_MIN);
    const te::rst::RasterSummary* rsMax = te::rst::RasterSummaryManager::getInstance().get(raster, te::rst::SUMMARY_MAX);
    const std::complex<double>* cmin = rsMin->at(0).m_minVal;
    const std::complex<double>* cmax = rsMax->at(0).m_maxVal;
    double min = cmin->real();
    double max = cmax->real();

    rasterTransform.setLinearTransfParameters(min, max, 0, 255);
  }
  else
  {
    rasterTransform.setLinearTransfParameters(0, 255, 0, 255);
  }

  // Get the raster symbolizer
  std::size_t nRules = style->getRules().size();
  assert(nRules >= 1);

  // For while, consider one rule
  const te::se::Rule* rule = style->getRule(0);

  const std::vector<te::se::Symbolizer*>& symbolizers = rule->getSymbolizers();
  assert(!symbolizers.empty());

  // For while, consider one raster symbolizer
  te::se::RasterSymbolizer* rasterSymbolizer = dynamic_cast<te::se::RasterSymbolizer*>(symbolizers[0]);
  assert(rasterSymbolizer);

  // Configure the raster transformation based on the raster symbolizer
  te::map::RasterTransformConfigurer rtc(rasterSymbolizer, &rasterTransform);
  rtc.configure();

  // Adjusting to deal with transparency...
  if(raster->getNumberOfBands() > 3)
  {
    rasterTransform.getRGBMap()[te::map::RasterTransform::ALPHA_CHANNEL] = 3;
    rasterTransform.setTransfFunction(te::map::RasterTransform::EXTRACT2RGBA_TRANSF);
  }

  // Verify if is necessary convert the raster to the given srid
  bool needRemap = false;
  if((bboxSRID != TE_UNKNOWN_SRS) && (srid != TE_UNKNOWN_SRS) && (bboxSRID != srid))
    needRemap = true;

  // Build task message
  std::string message = TE_TR("Drawing the WMS layer");
  !layerTitle.empty() ? message += " " + layerTitle + ". " : message += "...";

  // Create the draw task
  te::common::TaskProgress task(message, te::common::TaskProgress::DRAW, gridCanvas->getNumberOfRows());

  // Create a RGBA array that will be drawn in the canvas. i.e. This array is the result of the render process.
  //te::color::RGBAColor** rgba = new te::color::RGBAColor*[gridCanvas->getNumberOfRows()];

  // Create a RGBA array that will be drawn in the canvas. i.e. This array represents a row of the render process.
  te::color::RGBAColor** row = new te::color::RGBAColor*[1];
  te::color::RGBAColor* columns = new te::color::RGBAColor[gridCanvas->getNumberOfColumns()];
  row[0] = columns;

  // Create a SRS converter
  std::auto_ptr<te::srs::Converter> converter(new te::srs::Converter());
  if(needRemap)
  {
    converter->setSourceSRID(srid);
    converter->setTargetSRID(bboxSRID);
  }

  // Fill the result RGBA array
  for(unsigned int r = 0; r < gridCanvas->getNumberOfRows(); ++r)
  {
    for(unsigned int c = 0; c < gridCanvas->getNumberOfColumns(); ++c)
    {
      te::gm::Coord2D inputGeo = gridCanvas->gridToGeo(c, r);

      if(needRemap)
        converter->convert(inputGeo.x, inputGeo.y, inputGeo.x, inputGeo.y);

      te::gm::Coord2D outputGrid = raster->getGrid()->geoToGrid(inputGeo.x, inputGeo.y);

      // TODO: round or truncate?
      int x = te::rst::Round(outputGrid.x);
      int y = te::rst::Round(outputGrid.y);

      te::color::RGBAColor color(255, 255, 255, 0);

      if((x >= 0 && x < (int)(raster->getNumberOfColumns())) &&
         (y >= 0 && y < (int)(raster->getNumberOfRows())))
           color = rasterTransform.apply(x, y);

      columns[c] = color;
    }

    //rgba[r] = columns;

    if(!task.isActive())
    {
      // Draw the part of result
      //canvas->drawImage(0, 0, rgba, canvas->getWidth(), r + 1);
      canvas->drawImage(0, r, row, canvas->getWidth(), 1);

      // Free memory
      //te::common::Free(rgba, r + 1);
      te::common::Free(row, 1);

      return;
    }

    canvas->drawImage(0, r, row, canvas->getWidth(), 1);

    task.pulse();
  }

  // Put the result in the canvas
  //canvas->drawImage(0, 0, rgba, canvas->getWidth(), canvas->getHeight());

  // Free memory
  //te::common::Free(rgba, gridCanvas->getNumberOfRows());
  te::common::Free(row, 1);

  // Image outline
  if(rasterSymbolizer->getImageOutline() == 0)
    return;

  // Get the symbolizer that will be used to draw the image outline
  te::se::Symbolizer* outlineSymbolizer = rasterSymbolizer->getImageOutline()->getSymbolizer();
  if(outlineSymbolizer == 0)
    return;

  // Create a canvas configurer
  te::map::CanvasConfigurer cc(canvas);
  cc.config(outlineSymbolizer);

  // Creates the image outline
  std::auto_ptr<te::gm::Geometry> geom(te::gm::GetGeomFromEnvelope(raster->getExtent(), bboxSRID));
  if(needRemap)
  {
    geom->setSRID(bboxSRID);
    geom->transform(srid);
  }

  canvas->draw(geom.get());
}
