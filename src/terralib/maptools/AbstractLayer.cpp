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
  \file terralib/maptools/AbstractLayer.cpp

  \brief This is the base class for layers.
 */

// TerraLib
#include "../dataaccess/dataset/ObjectIdSet.h"
#include "../se/Style.h"
#include "../srs/Config.h"
#include "AbstractLayer.h"
#include "Chart.h"
#include "Grouping.h"

te::map::AbstractLayer::AbstractLayer(AbstractLayer* parent)
  : te::common::TreeItem(parent),
    m_srid(TE_UNKNOWN_SRS),
    m_visibility(NOT_VISIBLE),
    m_visibilityChanged(false),
    m_selected(0),
    m_style(0),
    m_selectionStyle(0),
    m_grouping(0),
    m_chart(0),
    m_compositionMode(te::map::SourceOver),
    m_encoding(te::core::EncodingType::UTF8)
{
}

te::map::AbstractLayer::AbstractLayer(const std::string& id, AbstractLayer* parent)
  : te::common::TreeItem(parent),
    m_id(id),
    m_srid(TE_UNKNOWN_SRS),
    m_visibility(NOT_VISIBLE),
    m_visibilityChanged(false),
    m_selected(0),
    m_style(0),
    m_selectionStyle(0),
    m_grouping(0),
    m_chart(0),
    m_compositionMode(te::map::SourceOver),
    m_encoding(te::core::EncodingType::UTF8)
{
}

te::map::AbstractLayer::AbstractLayer(const std::string& id,
                                      const std::string& title,
                                      AbstractLayer* parent)
  : te::common::TreeItem(parent),
    m_id(id),
    m_title(title),
    m_srid(TE_UNKNOWN_SRS),
    m_visibility(NOT_VISIBLE),
    m_visibilityChanged(false),
    m_selected(0),
    m_style(0),
    m_selectionStyle(0),
    m_grouping(0),
    m_chart(0),
    m_compositionMode(te::map::SourceOver),
    m_encoding(te::core::EncodingType::UTF8)
{
}

te::map::AbstractLayer::~AbstractLayer()
{
  delete m_selected;
  delete m_style;
  delete m_selectionStyle;
  delete m_grouping;
  delete m_chart;
}

const std::string& te::map::AbstractLayer::getId() const
{
  return m_id;
}

void te::map::AbstractLayer::setId(const std::string& id)
{
  m_id = id;
}

const std::string& te::map::AbstractLayer::getTitle() const
{
  return m_title;
}

void te::map::AbstractLayer::setTitle(const std::string& title)
{
  m_title = title;
}

std::vector<te::map::AbstractLayer*> te::map::AbstractLayer::getDescendants()
{
  std::vector<AbstractLayer*> layers;
  std::vector<AbstractLayer*> childrenLayers;

  std::size_t numChildren = getChildrenCount();
  for(std::size_t i = 0; i < numChildren; ++i)
  {
    AbstractLayer* childLayer = static_cast<AbstractLayer*>(getChild(i).get());
    layers.push_back(childLayer);

    if(childLayer->hasChildren())
    {
      childrenLayers = childLayer->getDescendants();
      for(std::size_t i = 0; i < childrenLayers.size(); ++i)
        layers.push_back(childrenLayers[i]);
    }
  }

  return layers;
}

std::vector<te::map::AbstractLayer*> te::map::AbstractLayer::getAncestors()
{
  std::vector<AbstractLayer*> layers;

  AbstractLayer* parentLayer = static_cast<AbstractLayer*>(getParent());
  while(parentLayer)
  {
    layers.push_back(parentLayer);
    parentLayer = static_cast<AbstractLayer*>(parentLayer->getParent());
  }

  return layers;
}

te::map::Visibility te::map::AbstractLayer::getVisibility() const
{
  return m_visibility;
}

void te::map::AbstractLayer::setVisibility(Visibility v)
{
  Visibility prevVisibility = m_visibility;

  m_visibilityChanged = false;

  m_visibility = v;

  if(m_visibility != prevVisibility)
    m_visibilityChanged = true;
}

bool te::map::AbstractLayer::hasVisibilityChanged()
{
  return m_visibilityChanged;
}

void te::map::AbstractLayer::setVisibilityAsChanged(bool visChanged)
{
  m_visibilityChanged = visChanged;
}

void te::map::AbstractLayer::updateVisibilityOfAncestors()
{
  AbstractLayer* parent = static_cast<AbstractLayer*>(getParent());
  while(parent)
  {
    bool allVisible = true;
    bool allNotVisible = true;

    Visibility prevParentVisibility = parent->getVisibility();
    parent->m_visibilityChanged = false;

    int numChildren = parent->getChildrenCount();
    std::vector<AbstractLayer*> childrenVec(numChildren);

    for(int i = 0; i < numChildren; ++i)
    {
      childrenVec[i] = static_cast<AbstractLayer*>(parent->getChild(i).get());

      if(childrenVec[i]->getVisibility() == te::map::NOT_VISIBLE)
        allVisible = false;
      else if(childrenVec[i]->getVisibility() == te::map::VISIBLE)
        allNotVisible = false;
      else if(childrenVec[i]->getVisibility() == te::map::PARTIALLY_VISIBLE)
      {
        allVisible = false;
        allNotVisible = false;
      }
    }

    // Set parent visibility;
    Visibility parentVisibility = te::map::PARTIALLY_VISIBLE;
    if(allVisible)
      parentVisibility = te::map::VISIBLE;
    else if(allNotVisible)
      parentVisibility = te::map::NOT_VISIBLE;

    parent->m_visibility = parentVisibility;

    if(parentVisibility != prevParentVisibility)
      parent->m_visibilityChanged = true;

    parent = static_cast<AbstractLayer*>(parent->getParent());
  }
}

void te::map::AbstractLayer::updateVisibility()
{
}

const te::gm::Envelope& te::map::AbstractLayer::getExtent() const
{
  return m_mbr;
}

void te::map::AbstractLayer::setExtent(const te::gm::Envelope& mbr)
{
  m_mbr = mbr;
}

int te::map::AbstractLayer::getSRID() const
{
  return m_srid;
}

void te::map::AbstractLayer::setSRID(int srid)
{
  m_srid = srid;
}

void te::map::AbstractLayer::select(te::da::ObjectIdSet* oids)
{
  assert(oids);

  if(m_selected == 0)
  {
    m_selected = oids;
    return;
  }

  if(m_selected->size() == 0)
  {
    delete m_selected;
    m_selected = oids;
    return;
  }

  assert(m_selected);

  m_selected->Union(oids);
}

const te::da::ObjectIdSet* te::map::AbstractLayer::getSelected() const
{
  return m_selected;
}

void te::map::AbstractLayer::clearSelected()
{
  if(m_selected)
    m_selected->clear();
}

void te::map::AbstractLayer::deselect(const te::da::ObjectIdSet* oids)
{
  assert(m_selected);

  if(m_selected->size() == 0)
    return;

  m_selected->difference(oids);
}

te::se::Style* te::map::AbstractLayer::getStyle() const
{
  return m_style;
}

void te::map::AbstractLayer::setStyle(te::se::Style* style)
{
  delete m_style;
  m_style = style;
}

te::se::Style* te::map::AbstractLayer::getSelectionStyle() const
{
  return m_selectionStyle;
}

void te::map::AbstractLayer::setSelectionStyle(te::se::Style* style)
{
  delete m_selectionStyle;
  m_selectionStyle = style;
}

te::map::Grouping* te::map::AbstractLayer::getGrouping() const
{
  return m_grouping;
}

void te::map::AbstractLayer::setGrouping(te::map::Grouping* grouping)
{
  delete m_grouping;
  m_grouping = grouping;
}

te::map::Chart* te::map::AbstractLayer::getChart() const
{
  return m_chart;
}

void te::map::AbstractLayer::setChart(te::map::Chart* chart)
{
  delete m_chart;
  m_chart = chart;
}

const std::string& te::map::AbstractLayer::getGeomPropertyName() const
{
  return m_geomPropertyName;
}

void te::map::AbstractLayer::setGeomPropertytName(const std::string& name)
{
  m_geomPropertyName = name;
}

te::map::CompositionMode te::map::AbstractLayer::getCompositionMode() const
{
  return m_compositionMode;
}

void te::map::AbstractLayer::setCompositionMode(te::map::CompositionMode mode)
{
  m_compositionMode = mode;
}

te::core::EncodingType te::map::AbstractLayer::getEncoding() const
{
  return m_encoding;
}

void te::map::AbstractLayer::setEncoding(te::core::EncodingType et)
{
  m_encoding = et;
}

void te::map::AbstractLayer::setOutOfDate()
{
}

const std::string& te::map::AbstractLayer::getDataSourceId() const
{
  return m_datasourceId;
}

void te::map::AbstractLayer::setDataSourceId(const std::string& id)
{
  m_datasourceId = id;
}

const std::string& te::map::AbstractLayer::getDataSetName() const
{
  return m_datasetName;
}

void te::map::AbstractLayer::setDataSetName(const std::string& name)
{
  m_datasetName = name;
}