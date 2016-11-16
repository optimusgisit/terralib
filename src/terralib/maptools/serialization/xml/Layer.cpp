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
  \file src/terralib/maptools/serialization/xml/Layer.cpp
   
  \brief Auxiliary classes and functions to read layer information from a XML document.
*/

// TerraLib
#include "../../../common/BoostUtils.h"
#include "../../../core/translator/Translator.h"
#include "../../../color/RGBAColor.h"
#include "../../../dataaccess/dataset/AttributeConverterManager.h"
#include "../../../dataaccess/dataset/DataSetTypeConverter.h"
#include "../../../dataaccess/datasource/DataSourceManager.h"
#include "../../../dataaccess/serialization/xml/Serializer.h"
#include "../../../dataaccess/utils/Utils.h"
#include "../../../datatype/DateTimeProperty.h"
#include "../../../datatype/NumericProperty.h"
#include "../../../datatype/Property.h"
#include "../../../datatype/SimpleProperty.h"
#include "../../../datatype/StringProperty.h"
#include "../../../datatype/serialization/xml/Serializer.h"
#include "../../../geometry/Envelope.h"
#include "../../../geometry/GeometryProperty.h"
#include "../../../geometry/serialization/xml/Serializer.h"
#include "../../../se/CoverageStyle.h"
#include "../../../xml/AbstractWriter.h"
#include "../../../xml/Reader.h"
#include "../../../maptools/AbstractLayer.h"
#include "../../../maptools/Chart.h"
#include "../../../maptools/DataSetAdapterLayer.h"
#include "../../../maptools/DataSetLayer.h"
#include "../../../maptools/FolderLayer.h"
#include "../../../maptools/Grouping.h"
#include "../../../maptools/GroupingItem.h"
#include "../../../maptools/QueryLayer.h"
#include "../../../maptools/RasterLayer.h"
#include "../../../se/RasterSymbolizer.h"
#include "../../../se/serialization/xml/Style.h"
#include "../../../se/serialization/xml/Symbolizer.h"
#include "../../Exception.h"
#include "Layer.h"
#include "Utils.h"

// STL
#include <cassert>
#include <memory>

// Boost
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

te::map::AbstractLayer* DataSetLayerReader(te::xml::Reader& reader);
te::map::AbstractLayer* QueryLayerReader(te::xml::Reader& reader);
te::map::AbstractLayer* FolderLayerReader(te::xml::Reader& reader);
te::map::AbstractLayer* RasterLayerReader(te::xml::Reader& reader);
te::map::AbstractLayer* DataSetAdapterLayerReader(te::xml::Reader& reader);
void DataSetLayerWriter(const te::map::AbstractLayer* layer, te::xml::AbstractWriter& writer);
void QueryLayerWriter(const te::map::AbstractLayer* layer, te::xml::AbstractWriter& writer);
void FolderLayerWriter(const te::map::AbstractLayer* layer, te::xml::AbstractWriter& writer);
void RasterLayerWriter(const te::map::AbstractLayer* layer, te::xml::AbstractWriter& writer);
void DataSetAdapterLayerWriter(const te::map::AbstractLayer* layer, te::xml::AbstractWriter& writer);

te::dt::SimpleProperty* GetProperty(std::string name, int dataType, int geomType, int srid)
{
   te::dt::SimpleProperty* simpleProperty = 0;

  switch(dataType)
  {
    case te::dt::BOOLEAN_TYPE:
    case te::dt::CHAR_TYPE:
    case te::dt::DOUBLE_TYPE:
    case te::dt::FLOAT_TYPE:
    case te::dt::INT16_TYPE:
    case te::dt::INT32_TYPE:
    case te::dt::INT64_TYPE:
    case te::dt::UCHAR_TYPE:
    case te::dt::UINT16_TYPE:
    case te::dt::UINT32_TYPE:
    case te::dt::UINT64_TYPE:
    {
      simpleProperty = new te::dt::SimpleProperty(name, dataType);
      break;
    }

    case te::dt::STRING_TYPE:
    {
      simpleProperty = new te::dt::StringProperty(name);
      break;
    }

    case te::dt::NUMERIC_TYPE:
    {
      simpleProperty = new te::dt::NumericProperty(name, 0, 0);
      break;
    }

    case te::dt::DATETIME_TYPE:
    {
      simpleProperty = new te::dt::DateTimeProperty(name);
      break;
    }
        
    case te::dt::GEOMETRY_TYPE:
    {
      simpleProperty = new te::gm::GeometryProperty(name, srid, (te::gm::GeomType)geomType);
      break;
    }

    default:
    {
      simpleProperty = 0;
      return nullptr;
    }
  }

  return simpleProperty;
}

void te::map::serialize::Layer::reg(const std::string& layerType, const LayerFnctSerializeType& fncts)
{
  m_fncts[layerType] = fncts;
}

te::map::AbstractLayer* te::map::serialize::Layer::read(te::xml::Reader& reader) const
{
  std::string layerType = boost::to_upper_copy(reader.getElementLocalName());

  LayerFnctIdxType::const_iterator it = m_fncts.find(layerType);

  if(it == m_fncts.end())
    throw Exception((boost::format(TE_TR("Could not find a reader for the following layer type: %1%.")) % layerType).str());

  assert(it->second.second);

  return it->second.first(reader);
}

void te::map::serialize::Layer::write(const te::map::AbstractLayer* alayer, te::xml::AbstractWriter& writer) const
{
  assert(alayer);

  LayerFnctIdxType::const_iterator it = m_fncts.find(alayer->getType());

  if(it == m_fncts.end())
    throw Exception((boost::format(TE_TR("Could not find a writer for the following layer type: %1%.")) % alayer->getType()).str());

  assert(it->second.second);

  return it->second.second(alayer, writer);
}

te::map::serialize::Layer::~Layer()
{
}

te::map::serialize::Layer::Layer()
{
  m_fncts["DATASETLAYER"] = std::make_pair(LayerReadFnctType(&DataSetLayerReader), LayerWriteFnctType(&DataSetLayerWriter));
  m_fncts["QUERYLAYER"] = std::make_pair(LayerReadFnctType(&QueryLayerReader), LayerWriteFnctType(&QueryLayerWriter));
  m_fncts["FOLDERLAYER"] = std::make_pair(LayerReadFnctType(&FolderLayerReader), LayerWriteFnctType(&FolderLayerWriter));
  m_fncts["RASTERLAYER"] = std::make_pair(LayerReadFnctType(&RasterLayerReader), LayerWriteFnctType(&RasterLayerWriter));
  m_fncts["DATASETADAPTERLAYER"] = std::make_pair(LayerReadFnctType(&DataSetAdapterLayerReader), LayerWriteFnctType(&DataSetAdapterLayerWriter));
}

te::map::AbstractLayer* DataSetLayerReader(te::xml::Reader& reader)
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

  /* has a Selection Style Element ? */
  std::auto_ptr<te::se::Style> selectionStyle;

  if ((reader.getNodeType() == te::xml::START_ELEMENT) &&
    (reader.getElementLocalName() == "SelectionStyle"))
  {
    reader.next();
    assert(reader.getNodeType() == te::xml::START_ELEMENT);

    selectionStyle.reset(te::se::serialize::Style::getInstance().read(reader));

    assert(reader.getNodeType() == te::xml::END_ELEMENT);
    assert(reader.getElementLocalName() == "SelectionStyle");

    reader.next();
  }

  assert(reader.getNodeType() == te::xml::END_ELEMENT);
  assert(reader.getElementLocalName() == "DataSetLayer");

  reader.next();

  std::auto_ptr<te::map::DataSetLayer> layer(new te::map::DataSetLayer(id, title, 0));
  layer->setExtent(*mbr.get());
  layer->setVisibility(te::map::serialize::GetVisibility(visible));
  layer->setDataSetName(dataset);
  layer->setDataSourceId(datasourceId);
  layer->setRendererType(rendererId);
  layer->setCompositionMode((te::map::CompositionMode)compositionMode);
  layer->setStyle(style.release());
  layer->setSelectionStyle(selectionStyle.release());
  layer->setSRID(srid);
  layer->setEncoding(te::core::CharEncoding::getEncodingType(encoding));
    
  if(grouping)
    layer->setGrouping(grouping);
    
  if(chart.get())
    layer->setChart(chart.release());
    
  return layer.release();
}

te::map::AbstractLayer* QueryLayerReader(te::xml::Reader& reader)
{
  std::string id = reader.getAttr("id");

 /* Title Element */
  reader.next();
  std::string title = te::map::serialize::ReadLayerTitle(reader);

  /* Visible Element */
  reader.next();
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

  /* Query Element */
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "Select");
  te::da::Select* query = te::serialize::xml::ReadSelect(reader);

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

  /* has a Selection Style Element ? */
  std::auto_ptr<te::se::Style> selectionStyle;

  if ((reader.getNodeType() == te::xml::START_ELEMENT) &&
    (reader.getElementLocalName() == "SelectionStyle"))
  {
    reader.next();
    assert(reader.getNodeType() == te::xml::START_ELEMENT);

    selectionStyle.reset(te::se::serialize::Style::getInstance().read(reader));

    assert(reader.getNodeType() == te::xml::END_ELEMENT);
    assert(reader.getElementLocalName() == "SelectionStyle");

    reader.next();
  }

  assert(reader.getNodeType() == te::xml::END_ELEMENT);
  assert(reader.getElementLocalName() == "QueryLayer");

  reader.next();

  std::auto_ptr<te::map::QueryLayer> layer(new te::map::QueryLayer(id, title, 0));
  layer->setDataSourceId(datasourceId);
  layer->setSRID(srid);
  layer->setExtent(*mbr.get());
  layer->setVisibility(te::map::serialize::GetVisibility(visible));
  layer->setQuery(query);
  layer->setDataSourceId(datasourceId);
  layer->setRendererType(rendererId);
  layer->setCompositionMode((te::map::CompositionMode)compositionMode);
  layer->setStyle(style.release());
  layer->setSelectionStyle(selectionStyle.release());
  layer->setEncoding(te::core::CharEncoding::getEncodingType(encoding));

  if(grouping)
    layer->setGrouping(grouping);

  if(chart.get())
    layer->setChart(chart.release());

  return layer.release();
}

te::map::AbstractLayer* FolderLayerReader(te::xml::Reader& reader)
{
  std::string id = reader.getAttr("id");

  /* Title Element */
  reader.next();
  std::string title = te::map::serialize::ReadLayerTitle(reader);

  /* Visible Element */
  reader.next();
  std::string visible = te::map::serialize::ReadLayerVisibility(reader);

  reader.next();
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "LayerList");

  reader.next();

  std::auto_ptr<te::map::FolderLayer> flayer(new te::map::FolderLayer(id, title, 0));

  flayer->setVisibility(te::map::serialize::GetVisibility(visible));

  const te::map::serialize::Layer& lserial = te::map::serialize::Layer::getInstance();

  while((reader.getNodeType() != te::xml::END_ELEMENT) &&
        (reader.getElementLocalName() != "LayerList"))
  {
    te::map::AbstractLayerPtr layer(lserial.read(reader));

    if(layer.get() == 0)
      break;

    flayer->add(layer);
  }

  reader.next();
  assert(reader.getNodeType() == te::xml::END_ELEMENT);
  assert(reader.getElementLocalName() == "FolderLayer");

  reader.next();

  return flayer.release();
}

te::map::AbstractLayer* RasterLayerReader(te::xml::Reader& reader)
{
  std::string id = reader.getAttr("id");

  /* Title Element */
  reader.next();
  std::string title = te::map::serialize::ReadLayerTitle(reader);

  /* Visible Element */
  reader.next();
  std::string visible = te::map::serialize::ReadLayerVisibility(reader);

  /* DataSetName Element */
  std::string dataset = te::map::serialize::ReadDataSetName(reader);
  reader.next();

  /* DataSourceId Element */
  std::string datasourceId = te::map::serialize::ReadDataSourceId(reader);
  reader.next();

  /* ConnectionInfo Element */
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "ConnectionInfo");

  std::map<std::string, std::string> conninfo;

  while(reader.next() &&
        (reader.getNodeType() == te::xml::START_ELEMENT) &&
        (reader.getElementLocalName() == "Parameter"))
  {
    // Parameter Name
    reader.next();
    assert(reader.getNodeType() == te::xml::START_ELEMENT);
    assert(reader.getElementLocalName() == "Name");
    reader.next();
    assert(reader.getNodeType() == te::xml::VALUE);
    std::string paramName = reader.getElementValue();
    reader.next();
    assert(reader.getNodeType() == te::xml::END_ELEMENT);

    // Parameter Value
    reader.next();
    assert(reader.getNodeType() == te::xml::START_ELEMENT);
    assert(reader.getElementLocalName() == "Value");
    reader.next();
    assert(reader.getNodeType() == te::xml::VALUE);
    std::string paramValue = reader.getElementValue();
    reader.next();
    assert(reader.getNodeType() == te::xml::END_ELEMENT);

    conninfo[paramName] = paramValue;

    reader.next();
    assert(reader.getNodeType() == te::xml::END_ELEMENT);
  }

  assert(reader.getNodeType() == te::xml::END_ELEMENT); // End of ConnectionInfo Element

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
  assert(reader.getElementLocalName() == "RasterLayer");

  reader.next();

  std::auto_ptr<te::map::RasterLayer> layer(new te::map::RasterLayer(id, title, 0));
  layer->setDataSetName(dataset);
  layer->setDataSourceId(datasourceId);
  layer->setSRID(srid);
  layer->setExtent(*mbr.get());
  layer->setRasterInfo(conninfo);
  layer->setVisibility(te::map::serialize::GetVisibility(visible));
  layer->setRendererType(rendererId);
  layer->setCompositionMode((te::map::CompositionMode)compositionMode);
  layer->setStyle(dynamic_cast<te::se::CoverageStyle*>(style.release()));

  return layer.release();
}

te::map::AbstractLayer* DataSetAdapterLayerReader(te::xml::Reader& reader)
{
  std::string id = reader.getAttr("id");

  /* Title Element */
  reader.next();
  std::string title = te::map::serialize::ReadLayerTitle(reader);

  /* Visible Element */
  reader.next();
  std::string visible = te::map::serialize::ReadLayerVisibility(reader);
  reader.next();

  /* Encoding Element */
  std::string encoding = te::map::serialize::ReadLayerEncoding(reader);
  reader.next();

  /* DataSetName Element */
  std::string dataSetName = te::map::serialize::ReadDataSetName(reader);
  reader.next();

  /* DataSourceId Element */
  std::string dataSourceId = te::map::serialize::ReadDataSourceId(reader);
  reader.next();

  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "RendererType");

  reader.next();

  assert(reader.getNodeType() == te::xml::VALUE);

  std::string rendererType = reader.getElementValue();

  reader.next();

  assert(reader.getNodeType() == te::xml::END_ELEMENT); //RendererType

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

  /* has a Selection Style Element ? */
  std::auto_ptr<te::se::Style> selectionStyle;

  if ((reader.getNodeType() == te::xml::START_ELEMENT) &&
    (reader.getElementLocalName() == "SelectionStyle"))
  {
    reader.next();
    assert(reader.getNodeType() == te::xml::START_ELEMENT);

    selectionStyle.reset(te::se::serialize::Style::getInstance().read(reader));

    assert(reader.getNodeType() == te::xml::END_ELEMENT);
    assert(reader.getElementLocalName() == "SelectionStyle");

    reader.next();
  }

  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "Converter");

  reader.next();

  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "InDataSetTypeName");

  reader.next();

  std::string inDataSetName = reader.getElementValue();

  reader.next();

  assert(reader.getNodeType() == te::xml::END_ELEMENT); // InDataSetTypeName

  reader.next();

  std::vector<std::pair<std::string, std::vector<std::size_t> > > props;

  while(reader.getNodeType() == te::xml::START_ELEMENT &&
        reader.getElementLocalName() == "OutPropInfo")
  {
    reader.next();

    assert(reader.getNodeType() == te::xml::START_ELEMENT);
    assert(reader.getElementLocalName() == "type");

    reader.next();

    assert(reader.getNodeType() == te::xml::VALUE);

    int type = reader.getElementValueAsInt32();

    reader.next();

    assert(reader.getNodeType() == te::xml::END_ELEMENT); // type

    reader.next();

    assert(reader.getNodeType() == te::xml::START_ELEMENT);
    assert(reader.getElementLocalName() == "name");

    reader.next();

    assert(reader.getNodeType() == te::xml::VALUE);

    std::string name = reader.getElementValue();

    reader.next();

    assert(reader.getNodeType() == te::xml::END_ELEMENT); // name

    reader.next();

    int geomType;
    int srid;

    assert(reader.getNodeType() == te::xml::START_ELEMENT);
    assert(reader.getElementLocalName() == "geomType");

    reader.next();

    assert(reader.getNodeType() == te::xml::VALUE);

    geomType = reader.getElementValueAsInt32();

    reader.next();

    assert(reader.getNodeType() == te::xml::END_ELEMENT); // geomType

    reader.next();

    assert(reader.getNodeType() == te::xml::START_ELEMENT);
    assert(reader.getElementLocalName() == "srid");

    reader.next();

    assert(reader.getNodeType() == te::xml::VALUE);

    srid = reader.getElementValueAsInt32();

    reader.next();

    assert(reader.getNodeType() == te::xml::END_ELEMENT); // srid

    reader.next();

    assert(reader.getNodeType() == te::xml::END_ELEMENT); // OutPropInfo

    reader.next();

    std::pair<std::string, std::vector<std::size_t> > prop;
    prop.first = name;
    std::vector<std::size_t> typeVec;
    typeVec.push_back(type);
    typeVec.push_back(geomType);
    typeVec.push_back(srid);
    prop.second = typeVec;
    props.push_back(prop);
  }

  std::vector<std::vector<std::size_t> > propertyIndexes;

  while(reader.getNodeType() == te::xml::START_ELEMENT &&
        reader.getElementLocalName() == "PropertyIndex")
  {

    int outIdx;
    outIdx = reader.getAttrAsInt32("OutIdx");

    reader.next();

    std::vector<std::size_t> inIdxs;

    while(reader.getNodeType() == te::xml::START_ELEMENT &&
          reader.getElementLocalName() == "InIdx")
    {
      reader.next();

      assert(reader.getNodeType() == te::xml::VALUE);

      inIdxs.push_back(reader.getElementValueAsInt32());

      reader.next();

      assert(reader.getNodeType() == te::xml::END_ELEMENT); // InIdx

      reader.next();
    }

    assert(reader.getNodeType() == te::xml::END_ELEMENT); // InIdx

    propertyIndexes.push_back(inIdxs);

    reader.next();
  }

  std::vector<std::string> functionsNames;

  while(reader.getNodeType() == te::xml::START_ELEMENT &&
        reader.getElementLocalName() == "FunctionName")
  {

    reader.next();

    assert(reader.getNodeType() == te::xml::VALUE);

    functionsNames.push_back(reader.getElementValue());

    reader.next();

    assert(reader.getNodeType() == te::xml::END_ELEMENT); // FunctionName

    reader.next();
  }

  assert(reader.getNodeType() == te::xml::END_ELEMENT); // Converter

  reader.next();

  assert(reader.getNodeType() == te::xml::END_ELEMENT); // DataSetAdapterLayer

  reader.next();

  te::map::DataSetAdapterLayer* result = new te::map::DataSetAdapterLayer(id);
  result->setTitle(title);

  result->setVisibility(te::map::serialize::GetVisibility(visible));
  result->setDataSetName(dataSetName);
  result->setDataSourceId(dataSourceId);
  result->setRendererType(rendererType);
  result->setCompositionMode((te::map::CompositionMode)compositionMode);
  result->setStyle(style.release());
  result->setSelectionStyle(selectionStyle.release());
  
  te::da::DataSourcePtr ds = te::da::GetDataSource(dataSourceId);
  std::auto_ptr<te::da::DataSetType> dst = ds->getDataSetType(dataSetName);

  std::auto_ptr<te::da::DataSetTypeConverter> converter(new te::da::DataSetTypeConverter(dst.get()));
  
  for(std::size_t i = 0; i < propertyIndexes.size(); ++i)
  {
    converter->add(propertyIndexes[i], GetProperty(props[i].first, props[i].second[0], props[i].second[1], props[i].second[2]), functionsNames[i]);
  }

  std::vector<te::dt::Property*> convertedProps = converter->getResult()->getProperties();

  te::da::PrimaryKey* pkIn = dst->getPrimaryKey();
  if(pkIn)
  {
    te::da::PrimaryKey* pk = new te::da::PrimaryKey(converter->getResult());

    std::vector<te::dt::Property*> pkProps = pkIn->getProperties();
    for(std::size_t i = 0; i < convertedProps.size(); ++i)
    {
      for(std::size_t t = 0; t < pkProps.size(); ++t)
      {
        if(convertedProps[i]->getName() == pkProps[t]->getName())
          pk->add(convertedProps[i]);
          
      }
    }
  }

  result->setConverter(converter);

  te::gm::Envelope* env = new te::gm::Envelope;

  std::auto_ptr<te::da::DataSet> dataset = result->getData();

  te::gm::GeometryProperty* gp = te::da::GetFirstGeomProperty(result->getConverter()->getResult());

  if(gp)
  {
    while(dataset->moveNext())
    {
      te::gm::Geometry* geom = dataset->getGeometry(gp->getName()).release();
      assert(geom);
      env->Union(*geom->getMBR());
    }
    result->setSRID(gp->getSRID());
  }

  result->setExtent(*env);

  return result;
}

void DataSetLayerWriter(const te::map::AbstractLayer* alayer, te::xml::AbstractWriter& writer)
{
  const te::map::DataSetLayer* layer = dynamic_cast<const te::map::DataSetLayer*>(alayer);

  if(layer == 0)
    return;

  writer.writeStartElement("te_map:DataSetLayer");

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

  if (layer->getSelectionStyle())
  {
    writer.writeStartElement("te_map:SelectionStyle");

    te::se::serialize::Style::getInstance().write(layer->getSelectionStyle(), writer);

    writer.writeEndElement("te_map:SelectionStyle");
  }

  writer.writeEndElement("te_map:DataSetLayer");
}

void QueryLayerWriter(const te::map::AbstractLayer* alayer, te::xml::AbstractWriter& writer)
{
  const te::map::QueryLayer* layer = dynamic_cast<const te::map::QueryLayer*>(alayer);

  if(layer == 0)
    return;

  writer.writeStartElement("te_map:QueryLayer");

  te::map::serialize::WriteAbstractLayer(layer, writer);

  te::serialize::xml::Save(layer->getQuery(), writer);

  if (layer->getDataSetName() != "")
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

  if (layer->getSelectionStyle())
  {
    writer.writeStartElement("te_map:SelectionStyle");

    te::se::serialize::Style::getInstance().write(layer->getSelectionStyle(), writer);

    writer.writeEndElement("te_map:SelectionStyle");
  }

  writer.writeEndElement("te_map:QueryLayer");
}

void FolderLayerWriter(const te::map::AbstractLayer* alayer, te::xml::AbstractWriter& writer)
{
  const te::map::FolderLayer* folderLayer = static_cast<const te::map::FolderLayer*>(alayer);

  writer.writeStartElement("te_map:FolderLayer");

  writer.writeAttribute("id", folderLayer->getId());
  writer.writeElement("te_map:Title", folderLayer->getTitle());
  writer.writeElement("te_map:Visible", te::map::serialize::GetVisibility(folderLayer->getVisibility()));

  writer.writeStartElement("te_map:LayerList");
 
  size_t count = folderLayer->getChildrenCount();

  for(size_t i=0; i<count; i++)
    te::map::serialize::Layer::getInstance().write((const te::map::AbstractLayer*) folderLayer->getChild(i).get(), writer);

  writer.writeEndElement("te_map:LayerList");

  writer.writeEndElement("te_map:FolderLayer");
}

void RasterLayerWriter(const te::map::AbstractLayer* alayer, te::xml::AbstractWriter& writer)
{
  const te::map::RasterLayer* layer = dynamic_cast<const te::map::RasterLayer*>(alayer);

  if(layer == 0)
    return;

  writer.writeStartElement("te_map:RasterLayer");

  te::map::serialize::WriteAbstractLayer(layer, writer);

  writer.writeStartElement("te_map:ConnectionInfo");
  std::map<std::string, std::string> info = layer->getRasterInfo();
  std::map<std::string, std::string>::iterator conIt;

  for(conIt=info.begin(); conIt!=info.end(); ++conIt)
  {
    writer.writeStartElement("te_map:Parameter");

    writer.writeStartElement("te_map:Name");
    writer.writeValue(conIt->first);
    writer.writeEndElement("te_map:Name");

    writer.writeStartElement("te_map:Value");
    writer.writeValue(conIt->second);
    writer.writeEndElement("te_map:Value");

    writer.writeEndElement("te_map:Parameter");
  }
  writer.writeEndElement("te_map:ConnectionInfo");

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

  writer.writeEndElement("te_map:RasterLayer");
}

void DataSetAdapterLayerWriter(const te::map::AbstractLayer* alayer, te::xml::AbstractWriter& writer)
{
  const te::map::DataSetAdapterLayer* layer = dynamic_cast<const te::map::DataSetAdapterLayer*>(alayer);

  if(layer == 0)
    return;

  writer.writeStartElement("te_map:DataSetAdapterLayer");

  te::map::serialize::WriteAbstractLayer(layer, writer);

  writer.writeElement("te_map:DataSetName", layer->getDataSetName());
  writer.writeElement("te_map:DataSourceId", layer->getDataSourceId());
  writer.writeElement("te_map:RendererType", layer->getRendererType());
  writer.writeElement("te_map:CompositionMode", (int)layer->getCompositionMode());

  if(layer->getStyle())
  {
    writer.writeStartElement("te_map:Style");

    te::se::serialize::Style::getInstance().write(layer->getStyle(), writer);

    writer.writeEndElement("te_map:Style");
  }

  if (layer->getSelectionStyle())
  {
    writer.writeStartElement("te_map:SelectionStyle");

    te::se::serialize::Style::getInstance().write(layer->getSelectionStyle(), writer);

    writer.writeEndElement("te_map:SelectionStyle");
  }

  te::da::DataSetTypeConverter* converter = layer->getConverter();

  writer.writeStartElement("te_map:Converter");

  writer.writeElement("te_map:InDataSetTypeName", converter->getConvertee()->getName());

  te::da::DataSetType* resultDt = converter->getResult();

  std::vector<te::dt::Property*> outProps = resultDt->getProperties();

  for(std::size_t i = 0; i < outProps.size(); ++i)
  {
    writer.writeStartElement("te_map:OutPropInfo");

    writer.writeElement("te_map:type", outProps[i]->getType());
    writer.writeElement("te_map:name", outProps[i]->getName());

    if(outProps[i]->getType() == te::dt::GEOMETRY_TYPE)
    {
      te::gm::GeometryProperty* geomProp = dynamic_cast<te::gm::GeometryProperty*>(outProps[i]);

      writer.writeElement("te_map:geomType", geomProp->getGeometryType());
      writer.writeElement("te_map:srid", geomProp->getSRID());
    }
    else
    {
      writer.writeElement("te_map:geomType", -1);
      writer.writeElement("te_map:srid", 0);
    }

    writer.writeEndElement("te_map:OutPropInfo");
    
  }

  // PropertyIndexes
  std::vector<std::vector<std::size_t> > propertyIndexes = converter->getConvertedPropertyIndexes();

  for(std::size_t i = 0; i < propertyIndexes.size(); ++i)
  {
    writer.writeStartElement("te_map:PropertyIndex");

    writer.writeAttribute("OutIdx", (int)i);

    std::vector<std::size_t> inputPropertiesIdx = propertyIndexes[i];

    for(std::size_t j = 0; j < inputPropertiesIdx.size(); ++j)
      writer.writeElement("te_map:InIdx", (int)inputPropertiesIdx[j]);

    writer.writeEndElement("te_map:PropertyIndex");
  }

  std::vector<te::da::AttributeConverter> converters = converter->getConverters();
  for(std::size_t i = 0; i < converters.size(); ++i)
  {
    std::string convName = layer->getConverter()->getConverterName(i);

    writer.writeElement("te_map:FunctionName", convName);
  }

  writer.writeEndElement("te_map:Converter");

  writer.writeEndElement("te_map:DataSetAdapterLayer");
}
