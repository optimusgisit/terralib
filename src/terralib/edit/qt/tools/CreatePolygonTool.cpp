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
  \file terralib/edit/qt/tools/CreatePolygonTool.cpp

  \brief This class implements a concrete tool to create polygons.
*/

// TerraLib
#include "../../../geometry/Envelope.h"
#include "../../../geometry/Geometry.h"
#include "../../../geometry/LinearRing.h"
#include "../../../geometry/LineString.h"
#include "../../../geometry/Point.h"
#include "../../../geometry/Polygon.h"
#include "../../../qt/widgets/canvas/MapDisplay.h"
#include "../../../qt/widgets/Utils.h"
#include "../../RepositoryManager.h"
#include "../Renderer.h"
#include "../Utils.h"
#include "CreatePolygonTool.h"

// Qt
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

// STL
#include <cassert>
#include <memory>

te::edit::CreatePolygonTool::CreatePolygonTool(te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer, const QCursor& cursor, QObject* parent) 
  : AbstractTool(display, parent),
    m_layer(layer),
    m_continuousMode(false),
    m_isFinished(false)
{
  setCursor(cursor);

  // Signals & slots
  connect(m_display, SIGNAL(extentChanged()), SLOT(onExtentChanged()));

  draw();
}

te::edit::CreatePolygonTool::~CreatePolygonTool()
{
  QPixmap* draft = m_display->getDraftPixmap();
  draft->fill(Qt::transparent);
}

bool te::edit::CreatePolygonTool::mousePressEvent(QMouseEvent* e)
{
  if(e->button() != Qt::LeftButton)
    return false;

  if(m_isFinished) // Is Finished?! So, start again...
  {
    clear();
    m_isFinished = false;
  }

  QPointF pw = m_display->transform(GetPosition(e));
  m_coords.push_back(te::gm::Coord2D(pw.x(), pw.y()));

  return true;
}

bool te::edit::CreatePolygonTool::mouseMoveEvent(QMouseEvent* e)
{
  if(m_coords.size() < 1 || m_isFinished)
    return false;

  QPointF pos = GetPosition(e);

  QPointF pw = m_display->transform(pos);

  m_coords.push_back(te::gm::Coord2D(pw.x(), pw.y()));
  
  pos += QPointF(0.0001, 0.0001); // To avoid collinear points on polygon

  pw = m_display->transform(pos);

  m_lastPos = te::gm::Coord2D(pw.x(), pw.y());

  Qt::KeyboardModifiers keys = e->modifiers();

  if(keys == Qt::NoModifier)
    m_continuousMode = false;
  else if(keys == Qt::ShiftModifier)
    m_continuousMode = true;

  draw();

  return false;
}

bool te::edit::CreatePolygonTool::mouseReleaseEvent(QMouseEvent* e)
{
  return false;
}

bool te::edit::CreatePolygonTool::mouseDoubleClickEvent(QMouseEvent* e)
{
  if(e->button() != Qt::LeftButton)
    return false;

  if(m_coords.size() < 3) // Can not stop yet...
    return false;

  m_isFinished = true;

  storeNewGeometry();

  return true;
}

void te::edit::CreatePolygonTool::draw()
{
  const te::gm::Envelope& env = m_display->getExtent();
  if(!env.isValid())
    return;

  // Clear!
  QPixmap* draft = m_display->getDraftPixmap();
  draft->fill(Qt::transparent);

  // Initialize the renderer
  Renderer& renderer = Renderer::getInstance();
  renderer.begin(draft, env, m_display->getSRID());

  // Draw the layer edited geometries
  renderer.drawRepository(m_layer->getId(), env, m_display->getSRID());

  if(!m_coords.empty())
  {
    // Draw the geometry being created
    if(m_coords.size() < 3)
      drawLine();
    else
      drawPolygon();

    if(m_continuousMode == false)
      m_coords.pop_back();
  }

  renderer.end();

  m_display->repaint();
}

void te::edit::CreatePolygonTool::drawPolygon()
{
  // Build the geometry
  te::gm::Geometry* polygon = buildPolygon();

  // Draw the current geometry and the vertexes
  Renderer& renderer = Renderer::getInstance();
  renderer.draw(polygon, true);

  delete polygon;
}

void te::edit::CreatePolygonTool::drawLine()
{
  // Build the geometry
  te::gm::Geometry* line = buildLine();

  // Draw the current geometry and the vertexes
  Renderer& renderer = Renderer::getInstance();
  renderer.draw(line, true);

  delete line;
}

void te::edit::CreatePolygonTool::clear()
{
  m_coords.clear();
}

te::gm::Geometry* te::edit::CreatePolygonTool::buildPolygon()
{
  // Build the geometry
  te::gm::LinearRing* ring = new te::gm::LinearRing(m_coords.size() + 1, te::gm::LineStringType);
  for(std::size_t i = 0; i < m_coords.size(); ++i)
    ring->setPoint(i, m_coords[i].x, m_coords[i].y);
  ring->setPoint(m_coords.size(), m_coords[0].x, m_coords[0].y); // Closing...

  te::gm::Polygon* polygon = new te::gm::Polygon(1, te::gm::PolygonType);
  polygon->setRingN(0, ring);

  polygon->setSRID(m_display->getSRID());

  if(polygon->getSRID() == m_layer->getSRID())
    return polygon;

  // else, need conversion...
  polygon->transform(m_layer->getSRID());

  return polygon;
}

te::gm::Geometry* te::edit::CreatePolygonTool::buildLine()
{
  te::gm::LineString* line = new te::gm::LineString(m_coords.size(), te::gm::LineStringType);
  for(std::size_t i = 0; i < m_coords.size(); ++i)
    line->setPoint(i, m_coords[i].x, m_coords[i].y);

  line->setSRID(m_display->getSRID());

  if(line->getSRID() == m_layer->getSRID())
    return line;

  // else, need conversion...
  line->transform(m_layer->getSRID());

  return line;
}

void te::edit::CreatePolygonTool::storeNewGeometry()
{
  RepositoryManager::getInstance().addNewGeometry(m_layer->getId(), buildPolygon());
}

void te::edit::CreatePolygonTool::onExtentChanged()
{
  if(m_coords.empty())
    return;

  m_coords.push_back(m_lastPos);

  draw();
}