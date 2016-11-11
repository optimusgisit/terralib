#include "Layer.h"

#include "../../../../../../geometry/Envelope.h"
#include "../../../../../../geometry/serialization/xml/Serializer.h"
#include "../../../../../../maptools/serialization/xml/Utils.h"
#include "../../../../../../xml/AbstractWriter.h"
#include "../../../../../../xml/Reader.h"
#include "../../../../../../core/encoding/CharEncoding.h"
#include "../../WMSLayer.h"


te::map::AbstractLayer *te::ws::ogc::wms::serialize::LayerReader(te::xml::Reader &reader)
{
  std::string id = reader.getAttr("id");

  /* Title Element */
  reader.next();
  std::string title = te::map::serialize::ReadLayerTitle(reader);

  /* Visible Element */
  reader.next();
  std::string visible = te::map::serialize::ReadLayerVisibility(reader);

  /* Enconding Element */
  reader.next();
  std::string encodingStr = te::map::serialize::ReadLayerEncoding(reader);

  te::core::EncodingType encoding = te::core::CharEncoding::getEncodingType(encodingStr);

  /* DataSetName Element */
  reader.next();
  std::string dataset = te::map::serialize::ReadDataSetName(reader);

  /* DataSourceId Element */
  reader.next();
  std::string datasourceId = te::map::serialize::ReadDataSourceId(reader);

  /* SRID Element */
  reader.next();
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

  /* WMSGetMapRequest Element*/
  reader.next();
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "WMSGetMapRequest");

  te::ws::ogc::wms::WMSGetMapRequest request = GetMapRequestReader(reader);

  reader.next();
  assert(reader.getElementLocalName() == "OGCWMSLayer");

  std::auto_ptr<te::ws::ogc::wms::WMSLayer> wmsLayer (new te::ws::ogc::wms::WMSLayer(id, title));
  wmsLayer->setDataSetName(dataset);
  wmsLayer->setDataSourceId(datasourceId);
  wmsLayer->setEncoding(encoding);
  wmsLayer->setSRID(srid);
  wmsLayer->setExtent(*mbr.get());
  wmsLayer->setRendererType(rendererId);
  wmsLayer->setVisibility(te::map::serialize::GetVisibility(visible));
  wmsLayer->setGetMapRequest(request);

  reader.next();

  return wmsLayer.release();
}

void te::ws::ogc::wms::serialize::LayerWriter(const te::map::AbstractLayer *alayer, te::xml::AbstractWriter &writer)
{
  const te::ws::ogc::wms::WMSLayer* layer = dynamic_cast<const te::ws::ogc::wms::WMSLayer*>(alayer);

  if(layer == 0)
    return;

  te::ws::ogc::wms::WMSGetMapRequest request = layer->getRequest();

  writer.writeStartElement("te_map:OGCWMSLayer");

  // Write mandatory attributes.
  writer.writeAttribute("width", request.m_width);
  writer.writeAttribute("height", request.m_height);
  writer.writeAttribute("format", request.m_format);

  // Write optional attributes.
  if(!request.m_bgColor.empty())
    writer.writeAttribute("bgColor", request.m_bgColor);

  if(!request.m_time.empty())
    writer.writeAttribute("time", request.m_time);

  if(request.m_transparent)
    writer.writeAttribute("transparent", request.m_transparent);


  te::map::serialize::WriteAbstractLayer(layer, writer);

  writer.writeElement("te_map:DataSetName", layer->getDataSetName());
  writer.writeElement("te_map:DataSourceId", layer->getDataSourceId());
  writer.writeElement("te_map:SRID", layer->getSRID());
  te::serialize::xml::SaveExtent(layer->getExtent(), writer);
  writer.writeElement("te_map:RendererId", layer->getRendererType());

  writer.writeStartElement("te_map:WMSGetMapRequest");

  writer.writeStartElement("te_map:Layers");

  for (size_t i = 0; i < request.m_layers.size(); i++)
  {
    writer.writeElement("te_map:LayerName", request.m_layers[i]);
  }

  writer.writeEndElement("te_map:Layers");

  writer.writeStartElement("te_map:BoundingBox");
  writer.writeAttribute("CRS", request.m_boundingBox.m_crs);
  writer.writeAttribute("minx", request.m_boundingBox.m_minX);
  writer.writeAttribute("miny", request.m_boundingBox.m_minY);
  writer.writeAttribute("maxx", request.m_boundingBox.m_maxX);
  writer.writeAttribute("maxy", request.m_boundingBox.m_maxY);
  writer.writeEndElement("te_map:BoundingBox");

  writer.writeStartElement("te_map:Styles");

  for (size_t i = 0; i < request.m_styles.size(); i++)
  {
    writer.writeElement("te_map:StyleName", request.m_styles[i]);
  }

  writer.writeEndElement("te_map:Styles");

  writer.writeEndElement("te_map:WMSGetMapRequest");

  writer.writeEndElement("te_map:OGCWMSLayer");
}

te::ws::ogc::wms::WMSGetMapRequest te::ws::ogc::wms::serialize::GetMapRequestReader(te::xml::Reader &reader)
{
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "WMSGetMapRequest");

  te::ws::ogc::wms::WMSGetMapRequest request;
  std::vector<std::string> layerNames;

  for(size_t i = 0; i < reader.getNumberOfAttrs(); i++)
  {
    if(reader.getAttrLocalName(i) == "width")
    {
      request.m_width = reader.getAttrAsInt32(i);
    }
    else if(reader.getAttrLocalName(i) == "height")
    {
      request.m_height = reader.getAttrAsInt32(i);
    }
    else if(reader.getAttrLocalName(i) == "format")
    {
      request.m_format = reader.getAttr(i);
    }
    else if(reader.getAttrLocalName(i) == "bgColor")
    {
      request.m_bgColor = reader.getAttr(i);
    }
    else if(reader.getAttrLocalName(i) == "time")
    {
      request.m_time = reader.getAttr(i);
    }
    else if(reader.getAttrLocalName(i) == "transparent")
    {
      request.m_transparent = (bool) reader.getAttrAsInt32(i);
    }
  }

  // Layers List Element
  reader.next();
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "Layers");

  while(reader.next() && (reader.getElementLocalName() != "Layers" && reader.getNodeType() != te::xml::END_ELEMENT))
  {
    if(reader.getElementLocalName() == "LayerName" && reader.getNodeType() == te::xml::VALUE)
    {
      layerNames.push_back(reader.getElementValue());
    }
  }

  reader.next();
  request.m_layers = layerNames;

  // BoundingBox Element
  reader.next();
  assert(reader.getElementLocalName() == "BoundingBox");

  te::ws::ogc::wms::BoundingBox bbox;

  for(size_t i = 0; i < reader.getNumberOfAttrs(); i++)
  {
    if(reader.getAttrLocalName(i) == "CRS")
      bbox.m_crs = reader.getAttr("CRS");
    else if (reader.getAttrLocalName(i) == "minx")
      bbox.m_minX = reader.getAttrAsDouble("minx");
    else if (reader.getAttrLocalName(i) == "miny")
      bbox.m_minY = reader.getAttrAsDouble("miny");
    else if (reader.getAttrLocalName(i) == "maxx")
      bbox.m_maxX = reader.getAttrAsDouble("maxx");
    else if (reader.getAttrLocalName(i) == "maxy")
      bbox.m_maxY = reader.getAttrAsDouble("maxy");
  }

  reader.next();
  request.m_boundingBox = bbox;
  request.m_srs = bbox.m_crs;

  // Styles List Element
  reader.next();
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "Styles");

  std::vector<std::string> styleNames;

  while(reader.next() && (reader.getElementLocalName() != "Styles" && reader.getNodeType() != te::xml::END_ELEMENT))
  {
    if(reader.getElementLocalName() == "StyleName" && reader.getNodeType() == te::xml::VALUE)
    {
      styleNames.push_back(reader.getElementValue());
    }
  }

  reader.next();
  request.m_styles = styleNames;

  reader.next();
  assert(reader.getNodeType() == te::xml::END_ELEMENT);
  assert(reader.getElementLocalName() == "WMSGetMapRequest");

  return request;
}
