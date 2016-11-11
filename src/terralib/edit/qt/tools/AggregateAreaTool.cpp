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
  \file terralib/edit/qt/tools/AggregateAreaTool.cpp

  \brief This class implements a concrete tool to aggregate polygons.
*/

// TerraLib
#include "../../../common/STLUtils.h"
#include "../../../dataaccess/dataset/ObjectId.h"
#include "../../../dataaccess/dataset/ObjectIdSet.h"
#include "../../../dataaccess/utils/Utils.h"
#include "../../../geometry/Utils.h"
#include "../../../qt/af/events/LayerEvents.h"
#include "../../../qt/af/events/MapEvents.h"
#include "../../../qt/widgets/canvas/MapDisplay.h"
#include "../../Feature.h"
#include "../../RepositoryManager.h"
#include "../../Utils.h"
#include "../Renderer.h"
#include "../Utils.h"
#include "AggregateAreaTool.h"

// Qt
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QDebug>

// STL
#include <cassert>
#include <memory>
#include <iostream>

te::edit::AggregateAreaTool::AggregateAreaTool(te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer, 
                                               const te::edit::MouseEventEdition mouseEventToSave, QObject* parent)
  : CreateLineTool(display, layer, Qt::ArrowCursor, parent),
  m_mouseEventToSave(mouseEventToSave),
  m_stack(UndoStackManager::getInstance())
{
  m_display->setCursor(Qt::ArrowCursor);
}

te::edit::AggregateAreaTool::~AggregateAreaTool()
{
}

bool te::edit::AggregateAreaTool::mousePressEvent(QMouseEvent* e)
{
  if (e->button() != Qt::LeftButton)
    return false;

  if (m_isFinished) // Is Finished?! So, start again...
  {
    te::edit::CreateLineTool::clear();
    m_isFinished = false;
  }

  pickFeature(GetPosition(e));

  return te::edit::CreateLineTool::mousePressEvent(e);
}

bool te::edit::AggregateAreaTool::mouseMoveEvent(QMouseEvent* e)
{
  return te::edit::CreateLineTool::mouseMoveEvent(e);
}

bool te::edit::AggregateAreaTool::mouseDoubleClickEvent(QMouseEvent* e)
{
  try
  {
    if (m_mouseEventToSave != te::edit::mouseDoubleClick || Qt::LeftButton != e->button())
      return false;

    return aggregatePolygon();
  }
  catch (std::exception& e)
  {
    QMessageBox::critical(m_display, tr("Error"), QString(tr("Could not join.") + " %1.").arg(e.what()));
    return false;
  }
}

bool te::edit::AggregateAreaTool::mouseReleaseEvent(QMouseEvent* e)
{
  try
  {
    if (m_mouseEventToSave != te::edit::mouseReleaseRightClick || Qt::RightButton != e->button())
      return false;

    return aggregatePolygon();
  }
  catch (std::exception& e)
  {
    QMessageBox::critical(m_display, tr("Error"), QString(tr("Could not join.") + " %1.").arg(e.what()));
    return false;
  }
}

bool te::edit::AggregateAreaTool::aggregatePolygon()
{
  if (m_coords.size() < 3) // Can not stop yet...
    return false;

  if (m_feature == 0) // Can not stop yet...
  {
    te::edit::CreateLineTool::clear();
    QMessageBox::critical(m_display, tr("Error"), QString(tr("Error aggregating area to the polygon")));
    return false;
  }

  m_display->setCursor(Qt::WaitCursor);

  m_isFinished = true;

  draw();

  storeFeature();

  storeUndoCommand();

  te::edit::CreateLineTool::clear();

  emit geometriesEdited();

  m_feature = 0;

  m_display->setCursor(Qt::ArrowCursor);

  return true;
}

void te::edit::AggregateAreaTool::draw()
{
  const te::gm::Envelope& env = m_display->getExtent();
  if (!env.isValid())
    return;

  // Clear!
  QPixmap* draft = m_display->getDraftPixmap();
  draft->fill(Qt::transparent);

  // Initialize the renderer
  Renderer& renderer = Renderer::getInstance();
  renderer.begin(draft, env, m_display->getSRID());

  // Draw the layer edited geometries
  if (!m_coords.empty())
  {
    if (m_coords.size() > 3)
      drawPolygon();

    if (m_continuousMode == false)
      m_coords.pop_back();
  }

  renderer.end();

  m_display->repaint();
  
}

void te::edit::AggregateAreaTool::drawPolygon()
{
  // Build the geometry
  m_feature->setGeometry(buildPolygon());

  // Draw the current geometry and the vertexes
  Renderer& renderer = Renderer::getInstance();
  renderer.draw(m_feature->getGeometry(), true);

}

te::gm::Geometry* te::edit::AggregateAreaTool::buildPolygon()
{
  te::gm::Geometry* geoAggregate = 0;

  // Build the geometry
  te::gm::LinearRing* ring = new te::gm::LinearRing(m_coords.size() + 1, te::gm::LineStringType);
  for (std::size_t i = 0; i < m_coords.size(); ++i)
    ring->setPoint(i, m_coords[i].x, m_coords[i].y);
  ring->setPoint(m_coords.size(), m_coords[0].x, m_coords[0].y); // Closing...

  te::gm::Polygon* polygon = new te::gm::Polygon(1, te::gm::PolygonType);
  polygon->setRingN(0, ring);

  polygon->setSRID(m_display->getSRID());

  if (polygon->getSRID() != m_layer->getSRID())
    polygon->transform(m_layer->getSRID());

  if (polygon->getSRID() != m_feature->getGeometry()->getSRID())
    m_feature->getGeometry()->transform(polygon->getSRID());

  te::gm::Geometry* aggr = te::gm::Validate(polygon);

  if (!aggr->intersects(m_feature->getGeometry()))
  { 
    m_isFinished = false;
    return dynamic_cast<te::gm::Geometry*>(m_feature->getGeometry()->clone());
  }

  geoAggregate = ConvertGeomType(m_layer, unionGeometry(aggr, m_feature->getGeometry()));

  geoAggregate->setSRID(m_display->getSRID());

  if (geoAggregate->getSRID() == m_layer->getSRID())
    return geoAggregate;

  // else, need conversion...
  geoAggregate->transform(m_layer->getSRID());

  return geoAggregate;

}

te::gm::Envelope te::edit::AggregateAreaTool::buildEnvelope(const QPointF& pos)
{
  QPointF pixelOffset(4.0, 4.0);

  QRectF rect(pos - pixelOffset, pos + pixelOffset);

  // Converts rect boundary to world coordinates
  QPointF ll(rect.left(), rect.bottom());
  QPointF ur(rect.right(), rect.top());
  ll = m_display->transform(ll);
  ur = m_display->transform(ur);

  te::gm::Envelope env(ll.x(), ll.y(), ur.x(), ur.y());

  return env;
}

void te::edit::AggregateAreaTool::storeFeature()
{
  RepositoryManager::getInstance().addFeature(m_layer->getId(), m_feature->clone());
}

te::gm::Geometry* te::edit::AggregateAreaTool::unionGeometry(te::gm::Geometry* g1, te::gm::Geometry* g2)
{
  return g1->Union(g2);
}

void te::edit::AggregateAreaTool::pickFeature(const QPointF& pos)
{
  m_display->setCursor(Qt::WaitCursor);

  te::gm::Envelope env = buildEnvelope(pos);

  try
  {
    if (m_feature == 0)
      m_feature = PickFeature(m_layer, env, m_display->getSRID(), te::edit::TO_UPDATE);

    m_display->setCursor(Qt::ArrowCursor);
  }
  catch (std::exception& e)
  {
    m_display->setCursor(Qt::ArrowCursor);
    QMessageBox::critical(m_display, tr("Error"), QString(tr("The geometry cannot be selected from the layer. Details:") + " %1.").arg(e.what()));
  }
}

void te::edit::AggregateAreaTool::storeUndoCommand()
{
  if (m_feature == 0)
    return;

  if (!m_isFinished)
    return;

  m_stack.addWatch(m_feature->clone());

  QUndoCommand* command = new AddCommand(m_display, m_layer, m_feature->clone()->getId());
  m_stack.addUndoStack(command);
}
