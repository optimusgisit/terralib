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
  \file terralib/wms/serialization/xml/Config.h
   
  \brief Auxiliary classes and functions to read/write WMS layers from a XML document.
*/

// TerraLib
#include "../../../common/URL.h"
#include "../../../geometry/serialization/xml/Serializer.h"
#include "../../../maptools/serialization/xml/Utils.h"
#include "../../../se/serialization/xml/Style.h"
#include "../../../xml/AbstractWriter.h"
#include "../../../xml/Reader.h"
#include "../../WMSLayer.h"
#include "Layer.h"

// STL
#include <cassert>

te::map::AbstractLayer* te::wms::serialize::LayerReader(te::xml::Reader& reader)
{
  std::string id = reader.getAttr("id");

  /* Title Element */
  reader.next();
  std::string title = te::map::serialize::ReadLayerTitle(reader);
  reader.next();

  /* Visible Element */
  std::string visible = te::map::serialize::ReadLayerVisibility(reader);
  reader.next();

  /* Encoding Element */
  std::string encoding = te::map::serialize::ReadLayerEncoding(reader);
  if (!encoding.empty())
    reader.next();
  else
    encoding = te::core::CharEncoding::getEncodingName(te::core::EncodingType::UTF8);

  /* Grouping */
  te::map::Grouping* grouping = te::map::serialize::ReadLayerGrouping(reader);

  /* Chart */
  std::auto_ptr<te::map::Chart> chart(te::map::serialize::ReadLayerChart(reader));

  /* DataSetName Element */
  std::string dataset = te::map::serialize::ReadDataSetName(reader);
  reader.next();

  /* DataSourceId Element */
  std::string datasourceId = te::map::serialize::ReadDataSourceId(reader);
  reader.next();

  /* SRID Element */
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "SRID");
  reader.next();
  assert(reader.getNodeType() == te::xml::VALUE);
  int srid = reader.getElementValueAsInt32();
  reader.next();
  assert(reader.getNodeType() == te::xml::END_ELEMENT);

  /* Extent Element */
  reader.next();
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "Extent");
  std::auto_ptr<te::gm::Envelope> mbr(te::serialize::xml::ReadExtent(reader));

  /* RendererId Element */
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "RendererId");
  reader.next();
  assert(reader.getNodeType() == te::xml::VALUE);
  std::string rendererId = reader.getElementValue();
  reader.next();
  assert(reader.getNodeType() == te::xml::END_ELEMENT);

  /* Composition Mode Element */
  reader.next();
  int compositionMode = te::map::SourceOver;
  if(reader.getNodeType() == te::xml::START_ELEMENT && reader.getElementLocalName() == "CompositionMode")
  {
    reader.next();
    assert(reader.getNodeType() == te::xml::VALUE);
    compositionMode = reader.getElementValueAsInt32();
    reader.next();
    assert(reader.getNodeType() == te::xml::END_ELEMENT);
    reader.next();
  }

  /* has a Style Element ? */
  std::auto_ptr<te::se::Style> style;

  if((reader.getNodeType() == te::xml::START_ELEMENT) &&
     (reader.getElementLocalName() == "Style"))
  {
    reader.next();
    assert(reader.getNodeType() == te::xml::START_ELEMENT);

    style.reset(te::se::serialize::Style::getInstance().read(reader));

    assert(reader.getNodeType() == te::xml::END_ELEMENT);
    assert(reader.getElementLocalName() == "Style");

    reader.next();
  }

  assert(reader.getNodeType() == te::xml::END_ELEMENT);
  assert(reader.getElementLocalName() == "WMSLayer");

  reader.next();

  std::auto_ptr<te::wms::WMSLayer> layer(new te::wms::WMSLayer(id, title, 0));
  layer->setSRID(srid);
  layer->setExtent(*mbr.get());
  layer->setVisibility(te::map::serialize::GetVisibility(visible));
  layer->setDataSetName(dataset);
  layer->setDataSourceId(datasourceId);
  layer->setRendererType(rendererId);
  layer->setCompositionMode((te::map::CompositionMode)compositionMode);
  layer->setStyle(style.release());
    
  if(grouping)
    layer->setGrouping(grouping);
    
  if(chart.get())
    layer->setChart(chart.release());
    
  return layer.release();
}

void te::wms::serialize::LayerWriter(const te::map::AbstractLayer* alayer, te::xml::AbstractWriter& writer)
{
  const te::wms::WMSLayer* layer = dynamic_cast<const te::wms::WMSLayer*>(alayer);

  if(layer == 0)
    return;

  writer.writeStartElement("te_map:WMSLayer");

  te::map::serialize::WriteAbstractLayer(layer, writer);

  writer.writeElement("te_map:DataSetName", layer->getDataSetName());
  writer.writeElement("te_map:DataSourceId", layer->getDataSourceId());
  writer.writeElement("te_map:SRID", layer->getSRID());
  te::serialize::xml::SaveExtent(layer->getExtent(), writer);
  writer.writeElement("te_map:RendererId", layer->getRendererType());
  writer.writeElement("te_map:CompositionMode", (int)layer->getCompositionMode());

  if(layer->getStyle())
  {
    writer.writeStartElement("te_map:Style");

    te::se::serialize::Style::getInstance().write(layer->getStyle(), writer);

    writer.writeEndElement("te_map:Style");
  }

  writer.writeEndElement("te_map:WMSLayer");
}
