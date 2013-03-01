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
  \file terralib/maptools/QueryLayer.cpp

  \brief A layer resulting from a query.
*/

// TerraLib
#include "../dataaccess/query/Select.h"
#include "../se/Style.h"
#include "QueryLayer.h"

const std::string te::map::QueryLayer::sm_type("QUERY_LAYER");

te::map::QueryLayer::QueryLayer(AbstractLayer* parent)
  : AbstractLayer(parent),
    m_query(0),
    m_style(0)
{
}

te::map::QueryLayer::QueryLayer(const std::string& id, AbstractLayer* parent)
  : AbstractLayer(id, parent),
    m_query(0),
    m_style(0)
{
}

te::map::QueryLayer::QueryLayer(const std::string& id,
                                const std::string& title,
                                AbstractLayer* parent)
  : AbstractLayer(id, title, parent),
    m_query(0),
    m_style(0)
{
}

te::map::QueryLayer::~QueryLayer()
{
  delete m_query;
  delete m_style;
}

bool te::map::QueryLayer::isValid() const
{
  return true;
}

void te::map::QueryLayer::draw(Canvas* canvas, const te::gm::Envelope& bbox, int srid)
{
}

const std::string& te::map::QueryLayer::getType() const
{
  return sm_type;
}

te::da::Select* te::map::QueryLayer::getQuery() const
{
  return m_query;
}

void te::map::QueryLayer::setQuery(te::da::Select* s)
{
  delete m_query;

  m_query = s;
}

const std::string& te::map::QueryLayer::getDataSourceId() const
{
  return m_datasourceId;
}

void te::map::QueryLayer::setDataSourceId(const std::string& id)
{
  m_datasourceId = id;
}

const std::string& te::map::QueryLayer::getRendererType() const
{
  return m_rendererType;
}

void te::map::QueryLayer::setRendererType(const std::string& t)
{
  m_rendererType = t;
}

te::se::Style* te::map::QueryLayer::getStyle() const
{
  return m_style;
}

void te::map::QueryLayer::setStyle(te::se::Style* style)
{
  delete m_style;

  m_style = style;
}

