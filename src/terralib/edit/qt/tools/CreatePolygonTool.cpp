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
#include "../../../qt/widgets/canvas/Canvas.h"
#include "../../../qt/widgets/canvas/MapDisplay.h"
#include "../../../qt/widgets/Utils.h"
#include "../Utils.h"
#include "CreatePolygonTool.h"

// Qt
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

// STL
#include <cassert>
#include <memory>

te::edit::CreatePolygonTool::CreatePolygonTool(te::qt::widgets::MapDisplay* display, const QCursor& cursor, QObject* parent) 
  : AbstractTool(display, parent),
    m_continuousMode(false),
    m_isFinished(false)
{
  setCursor(cursor);

  // Signals & slots
  connect(m_display, SIGNAL(extentChanged()), SLOT(onExtentChanged()));
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

  drawPolygon();

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

  return true;
}

void te::edit::CreatePolygonTool::drawPolygon()
{
  const te::gm::Envelope& env = m_display->getExtent();
  if(!env.isValid())
    return;

  // Clear!
  QPixmap* draft = m_display->getDraftPixmap();
  draft->fill(Qt::transparent);

  // Prepares the canvas
  te::qt::widgets::Canvas canvas(m_display->width(), m_display->height());
  canvas.setDevice(draft, false);
  canvas.setWindow(env.m_llx, env.m_lly, env.m_urx, env.m_ury);

  // Let's draw!
  drawPolygon(canvas);

  if(m_continuousMode == false)
    m_coords.pop_back();

  m_display->repaint();
}

void te::edit::CreatePolygonTool::drawPolygon(te::qt::widgets::Canvas& canvas)
{
  if(m_coords.size() < 3)
    return drawLine(canvas);

  // Build the geometry
  te::gm::LinearRing* ring = new te::gm::LinearRing(m_coords.size() + 1, te::gm::LineStringType);
  for(std::size_t i = 0; i < m_coords.size(); ++i)
    ring->setPoint(i, m_coords[i].x, m_coords[i].y);
  ring->setPoint(m_coords.size(), m_coords[0].x, m_coords[0].y); // Closing...

  te::gm::Polygon* polygon = new te::gm::Polygon(1, te::gm::PolygonType);
  polygon->setRingN(0, ring);

  // Let's draw!
  DrawGeometry(&canvas, polygon, m_display->getSRID());

  drawVertexes(canvas);

  delete polygon;
}

void te::edit::CreatePolygonTool::drawLine(te::qt::widgets::Canvas& canvas)
{
  // Build the geometry
  te::gm::LineString* line = new te::gm::LineString(m_coords.size(), te::gm::LineStringType);
  for(std::size_t i = 0; i < m_coords.size(); ++i)
    line->setPoint(i, m_coords[i].x, m_coords[i].y);

  // Let's draw!
  DrawGeometry(&canvas, line, m_display->getSRID());

  drawVertexes(canvas);

  delete line;
}

void te::edit::CreatePolygonTool::drawVertexes(te::qt::widgets::Canvas& canvas)
{
  te::qt::widgets::Config2DrawPoints(&canvas, "circle", 8, Qt::red, Qt::red, 1);

  DrawVertexes(&canvas, m_coords, m_display->getSRID(), m_display->getSRID());
}

void te::edit::CreatePolygonTool::clear()
{
  m_coords.clear();

  QPixmap* draft = m_display->getDraftPixmap();
  draft->fill(Qt::transparent);
    
  m_display->repaint();
}

void te::edit::CreatePolygonTool::onExtentChanged()
{
  if(m_coords.empty())
    return;

  m_coords.push_back(m_lastPos);

  drawPolygon();
}
