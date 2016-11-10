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
  \file terralib/wms/WMSLayer.cpp

  \brief A layer with reference to a WMS Layer.
*/

// TerraLib
#include "../core/translator/Translator.h"
#include "../dataaccess/utils/Utils.h"
#include "../maptools/AbstractRenderer.h"
#include "../maptools/RendererFactory.h"
#include "Exception.h"
#include "WMSLayer.h"

// Boost
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

const std::string te::wms::WMSLayer::sm_type("WMSLAYER");

te::wms::WMSLayer::WMSLayer(te::map::AbstractLayer* parent)
  : te::map::AbstractLayer(parent),
    m_rendererType("WMS_LAYER_RENDERER"),
    m_width(boost::lexical_cast<std::size_t>(TE_WMS_DEFAULT_WIDTH)),
    m_height(boost::lexical_cast<std::size_t>(TE_WMS_DEFAULT_HEIGHT)),
    m_format(TE_WMS_DEFAULT_IMAGE_FORMAT),
    m_schema(0)
{
}

te::wms::WMSLayer::WMSLayer(const std::string& id, te::map::AbstractLayer* parent)
  : te::map::AbstractLayer(id, parent),
    m_rendererType("WMS_LAYER_RENDERER"),
    m_width(boost::lexical_cast<std::size_t>(TE_WMS_DEFAULT_WIDTH)),
    m_height(boost::lexical_cast<std::size_t>(TE_WMS_DEFAULT_HEIGHT)),
    m_format(TE_WMS_DEFAULT_IMAGE_FORMAT),
    m_schema(0)
{
}

te::wms::WMSLayer::WMSLayer(const std::string& id,
                            const std::string& title,
                            te::map::AbstractLayer* parent)
  : te::map::AbstractLayer(id, title, parent),
    m_rendererType("WMS_LAYER_RENDERER"),
    m_width(boost::lexical_cast<std::size_t>(TE_WMS_DEFAULT_WIDTH)),
    m_height(boost::lexical_cast<std::size_t>(TE_WMS_DEFAULT_HEIGHT)),
    m_format(TE_WMS_DEFAULT_IMAGE_FORMAT),
    m_schema(0)
{
}

te::wms::WMSLayer::~WMSLayer()
{
}

std::auto_ptr<te::map::LayerSchema> te::wms::WMSLayer::getSchema() const
{
  if(m_schema.get())
    return std::auto_ptr<te::map::LayerSchema>(static_cast<te::map::LayerSchema*>(m_schema->clone()));

  assert(!m_datasetName.empty());

  te::da::DataSourcePtr ds = te::da::GetDataSource(m_datasourceId, true);

  m_schema = ds->getDataSetType(m_datasetName);

  return std::auto_ptr<te::map::LayerSchema>(static_cast<te::map::LayerSchema*>(m_schema->clone()));
}

std::auto_ptr<te::da::DataSet> te::wms::WMSLayer::getData(te::common::TraverseType travType,
                                                          const te::common::AccessPolicy accessPolicy) const
{
  std::string request = buildRequest();

  te::da::DataSourcePtr ds = te::da::GetDataSource(m_datasourceId, true);

  return ds->getDataSet(request, travType, accessPolicy);
}

std::auto_ptr<te::da::DataSet> te::wms::WMSLayer::getData(const std::string& propertyName,
                                                          const te::gm::Envelope* e,
                                                          te::gm::SpatialRelation r,
                                                          te::common::TraverseType travType,
                                                          const te::common::AccessPolicy accessPolicy) const
{
  std::string request = buildRequest();

  te::da::DataSourcePtr ds = te::da::GetDataSource(m_datasourceId, true);

  return ds->getDataSet(request, propertyName, e, r, travType, accessPolicy);
}

std::auto_ptr<te::da::DataSet> te::wms::WMSLayer::getData(const std::string& propertyName,
                                                          const te::gm::Geometry* g,
                                                          te::gm::SpatialRelation r,
                                                          te::common::TraverseType travType,
                                                          const te::common::AccessPolicy accessPolicy) const
{
  std::string request = buildRequest();

  te::da::DataSourcePtr ds = te::da::GetDataSource(m_datasourceId, true);

  return ds->getDataSet(request, propertyName, g, r, travType, accessPolicy);
}

std::auto_ptr<te::da::DataSet> te::wms::WMSLayer::getData(te::da::Expression* restriction,
                                                          te::common::TraverseType travType,
                                                          const te::common::AccessPolicy accessPolicy) const
{
  throw Exception(TE_TR("Query operations are not supported by the WMS Layer!"));
}

std::auto_ptr<te::da::DataSet> te::wms::WMSLayer::getData(const te::da::ObjectIdSet* oids,
                                                          te::common::TraverseType travType,
                                                          const te::common::AccessPolicy accessPolicy) const
{
  throw Exception(TE_TR("The ObjectIdSet concept is not supported by the WMS Layer!"));
}

bool te::wms::WMSLayer::isValid() const
{
  if(m_datasourceId.empty() || m_datasetName.empty())
    return false;

  if(m_width == 0 || m_height == 0 || m_format.empty())
    return false;

  te::da::DataSourcePtr ds;
  try
  {
    ds = te::da::GetDataSource(m_datasourceId, true);
  }
  catch(...)
  {
    return false;
  }

  if(ds.get() == 0 || !ds->isValid() || !ds->isOpened())
    return false;

  return true;
}

void te::wms::WMSLayer::draw(te::map::Canvas* canvas, const te::gm::Envelope& bbox, int srid, const double& scale, bool* cancel)
{
  if(m_rendererType.empty())
    throw Exception((boost::format(TE_TR("Could not draw the WMS layer %1%. The renderer type is empty!")) % getTitle()).str());

  // Try get the defined renderer
  std::auto_ptr<te::map::AbstractRenderer> renderer(te::map::RendererFactory::make(m_rendererType));
  if(renderer.get() == 0)
    throw Exception((boost::format(TE_TR("Could not draw the WMS layer %1%. The renderer %2% could not be created!")) % getTitle() % m_rendererType).str());

  renderer->draw(this, canvas, bbox, srid, scale, cancel);
}

const std::string& te::wms::WMSLayer::getType() const
{
  return sm_type;
}

const std::string& te::wms::WMSLayer::getDataSetName() const
{
  return m_datasetName;
}

void te::wms::WMSLayer::setDataSetName(const std::string& name)
{
  m_datasetName = name;
}

const std::string& te::wms::WMSLayer::getDataSourceId() const
{
  return m_datasourceId;
}

void te::wms::WMSLayer::setDataSourceId(const std::string& id)
{
  m_datasourceId = id;
}

const std::string& te::wms::WMSLayer::getRendererType() const
{
  return m_rendererType;
}

void te::wms::WMSLayer::setRendererType(const std::string& t)
{
  m_rendererType = t;
}

void te::wms::WMSLayer::setWidth(const std::size_t& width)
{
  m_width = width;
}

void te::wms::WMSLayer::setHeight(const std::size_t& height)
{
  m_height = height;
}

void te::wms::WMSLayer::setFormat(const std::string& format)
{
  m_format = format;
}

std::string te::wms::WMSLayer::buildRequest() const
{
  assert(!m_datasetName.empty());
  assert(m_width > 0);
  assert(m_height > 0);
  assert(!m_format.empty());

  std::string request = "LAYER=" + m_datasetName;
  request += "&WIDTH=" + boost::lexical_cast<std::string>(m_width);
  request += "&HEIGHT=" + boost::lexical_cast<std::string>(m_height);
  request += "&FORMAT=" + m_format;

  return request;
}
