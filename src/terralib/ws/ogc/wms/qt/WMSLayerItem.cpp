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

#include "WMSLayerItem.h"


te::ws::ogc::wms::qt::WMSLayerItem::WMSLayerItem(const te::ws::ogc::wms::Layer& rootLayer, QTreeWidget *parent)
  : QTreeWidgetItem(parent)
{
  m_layer = rootLayer;

  std::string nameValue = m_layer.m_title.empty() ? m_layer.m_name : m_layer.m_title;

  this->setText(0, QString(nameValue.c_str()));
  this->setText(1, QString(m_layer.m_abstract.c_str()));

  if(m_layer.m_queryable)
  {
    this->setFlags(this->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    this->setCheckState(0, Qt::Unchecked);
  }
  else
  {
    this->setFlags(this->flags() | Qt::ItemIsSelectable);
  }

  for(size_t i = 0; i < m_layer.m_layers.size(); i++)
  {
    WMSLayerItem* child = new WMSLayerItem(m_layer.m_layers[i]);
    this->addChild(child);
  }

  this->buildDefaultRequest();
}

te::ws::ogc::wms::qt::WMSLayerItem::WMSLayerItem(const te::ws::ogc::wms::Layer& layer)
  : QTreeWidgetItem()
{
  m_layer = layer;

  std::string nameValue = m_layer.m_title.empty() ? m_layer.m_name : m_layer.m_title;

  this->setText(0, QString(nameValue.c_str()));
  this->setText(1, QString(m_layer.m_abstract.c_str()));

  if(m_layer.m_queryable)
  {
    this->setFlags(this->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    this->setCheckState(0, Qt::Unchecked);
  }
  else
  {
    this->setFlags(this->flags() | Qt::ItemIsSelectable);
  }

  for(size_t i = 0; i < m_layer.m_layers.size(); i++)
  {
    WMSLayerItem* child = new WMSLayerItem(m_layer.m_layers[i]);
    this->addChild(child);
  }

  this->buildDefaultRequest();
}

te::ws::ogc::wms::qt::WMSLayerItem::~WMSLayerItem()
{

}

const te::ws::ogc::wms::Layer &te::ws::ogc::wms::qt::WMSLayerItem::getLayer() const
{
  return m_layer;
}

const te::ws::ogc::wms::WMSGetMapRequest& te::ws::ogc::wms::qt::WMSLayerItem::getRequest() const
{
  return m_request;
}

void te::ws::ogc::wms::qt::WMSLayerItem::setRequest(const te::ws::ogc::wms::WMSGetMapRequest &request)
{
  m_request = request;
}

void te::ws::ogc::wms::qt::WMSLayerItem::buildDefaultRequest()
{
  m_request = te::ws::ogc::wms::WMSGetMapRequest();

  m_request.m_layers.push_back(m_layer.m_name);

  if(m_layer.m_styles.size() > 0)
  {
    m_request.m_styles.push_back(m_layer.m_styles[0].m_name);
  }

  for(size_t i = 0; i < m_layer.m_boundingBoxes.size(); i++)
  {
    if(m_layer.m_boundingBoxes[i].m_crs.find("EPSG:") == std::string::npos)
    {
      continue;
    }
    else
    {
      te::ws::ogc::wms::BoundingBox bbox = m_layer.m_boundingBoxes[i];
      m_request.m_srs = bbox.m_crs;
      m_request.m_boundingBox = bbox;
      break;
    }
  }
}
