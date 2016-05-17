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

  \brief This class implements a concrete tool to aggregate geometries.
*/

// TerraLib
#include "../../../common/STLUtils.h"
#include "../../../dataaccess/dataset/ObjectId.h"
#include "../../../dataaccess/dataset/ObjectIdSet.h"
#include "../../../dataaccess/utils/Utils.h"
#include "../../../geometry.h"
#include "../../../qt/af/events/LayerEvents.h"
#include "../../../qt/af/events/MapEvents.h"
#include "../../../qt/widgets/canvas/MapDisplay.h"
#include "../../Feature.h"
#include "../../RepositoryManager.h"
#include "../../Utils.h"
#include "../Renderer.h"
#include "../Utils.h"
#include "../core/command/AddCommand.h"
#include "../core/UndoStackManager.h"
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

te::edit::AggregateAreaTool::AggregateAreaTool(te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer, QObject* parent)
  : CreateLineTool(display, layer, Qt::ArrowCursor, parent),
  m_addWatches(0),
  m_currentIndex(0)
{
}

te::edit::AggregateAreaTool::~AggregateAreaTool()
{
  delete m_feature;
  te::common::FreeContents(m_addWatches);
  m_addWatches.clear();

  UndoStackManager::getInstance().getUndoStack()->clear();
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

  pickFeature(m_layer, GetPosition(e));

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
    if (e->button() != Qt::LeftButton)
      return false;

    if (m_coords.size() < 3) // Can not stop yet...
      return false;

    if (m_feature == 0) // Can not stop yet...
    {
      te::edit::CreateLineTool::clear();
      QMessageBox::critical(m_display, tr("Error"), QString(tr("Error aggregating area to the polygon")));
      return false;
    }

    m_isFinished = true;

    draw();

    storeFeature();

    storeUndoCommand();

    return true;
  }
  catch (std::exception& e)
  {
    QMessageBox::critical(m_display, tr("Error"), QString(tr("Could not join.") + " %1.").arg(e.what()));
    return false;
  }
}

void te::edit::AggregateAreaTool::draw(bool onlyRepository)
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
  renderer.drawRepository(m_layer->getId(), env, m_display->getSRID());

  if (onlyRepository)
  {
    renderer.end();
    m_display->repaint();
    return;
  }

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
    te::gm::Geometry* geoUnion = 0;

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

    if (!polygon->intersects(m_feature->getGeometry()))
      return dynamic_cast<te::gm::Geometry*>(m_feature->getGeometry()->clone());

    geoUnion = convertGeomType(m_layer, unionGeometry(polygon, m_feature->getGeometry()));

    geoUnion->setSRID(m_display->getSRID());

    if (geoUnion->getSRID() == m_layer->getSRID())
      return geoUnion;

    // else, need conversion...
    geoUnion->transform(m_layer->getSRID());

    return geoUnion;

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

void te::edit::AggregateAreaTool::onExtentChanged()
{
  draw();
}

void te::edit::AggregateAreaTool::storeFeature()
{
  RepositoryManager::getInstance().addFeature(m_layer->getId(), m_feature->clone());
}

te::gm::Geometry* te::edit::AggregateAreaTool::unionGeometry(te::gm::Geometry* g1, te::gm::Geometry* g2)
{
  return g1->Union(g2);
}

void te::edit::AggregateAreaTool::pickFeature(const te::map::AbstractLayerPtr& layer, const QPointF& pos)
{
  te::gm::Envelope env = buildEnvelope(pos);

  try
  {
    if (m_feature == 0)
    {
      m_feature = PickFeature(layer, env, m_display->getSRID(), te::edit::GEOMETRY_UPDATE);

      if (m_feature)
        m_oidsSet.insert(m_feature->getId()->getValueAsString());

    }
    else
    {
      Feature* feature = PickFeature(layer, env, m_display->getSRID(), te::edit::GEOMETRY_UPDATE);
      if (feature)
      {
        if (m_oidsSet.find(feature->getId()->clone()->getValueAsString()) == m_oidsSet.end())
        {
          m_oidsSet.insert(feature->getId()->clone()->getValueAsString());
          m_feature = feature;
        }
        else
        {
          if (m_feature->getId()->clone()->getValueAsString() != feature->getId()->clone()->getValueAsString())
            m_feature = feature;
        }
      }
    }

  }
  catch (std::exception& e)
  {
    QMessageBox::critical(m_display, tr("Error"), QString(tr("The geometry cannot be selected from the layer. Details:") + " %1.").arg(e.what()));
  }
}

void te::edit::AggregateAreaTool::storeUndoCommand()
{
  if (m_feature == 0)
    return;

  //ensures that the vector has not repeated features after several clicks on the same
  if (m_addWatches.size())
  {
    if (m_addWatches.at(0)->getGeometry()->equals(m_feature->getGeometry()))
      return;
  }

  //If another feature is selected the stack is cleaned
  for (std::size_t i = 0; i < m_addWatches.size(); i++)
  {
    if (m_addWatches.at(i)->getId()->getValueAsString() != m_feature->getId()->getValueAsString())
    {
      te::common::FreeContents(m_addWatches);
      m_addWatches.clear();
      UndoStackManager::getInstance().getUndoStack()->clear();
      break;
    }
  }

  m_addWatches.push_back(m_feature->clone());

  //If a feature is changed in the middle of the stack, this change ends up being the top of the stack
  if (m_currentIndex < (int)(m_addWatches.size() - 2))
  {
    std::size_t i = 0;
    while (i < m_addWatches.size())
    {
      m_addWatches.pop_back();
      i = (m_currentIndex + 1);
    }
    m_addWatches.push_back(m_feature->clone());
  }

  m_currentIndex = (int)(m_addWatches.size() - 1);

  if (m_addWatches.size() < 2)
    return;

  QUndoCommand* command = new AddCommand(m_addWatches, m_currentIndex, m_layer);
  connect(dynamic_cast<AddCommand*>(command), SIGNAL(geometryAcquired(te::gm::Geometry*, std::vector<te::gm::Coord2D>)), SLOT(onGeometryAcquired(te::gm::Geometry*, std::vector<te::gm::Coord2D>)));

  UndoStackManager::getInstance().addUndoStack(command);
}

void te::edit::AggregateAreaTool::onGeometryAcquired(te::gm::Geometry* geom, std::vector<te::gm::Coord2D> /*coords*/)
{
  if (m_feature == 0)
    return;

  m_feature->setGeometry(geom);

  if (m_currentIndex > -1)
    RepositoryManager::getInstance().addFeature(m_layer->getId(), m_feature->clone());
  else
    RepositoryManager::getInstance().removeFeature(m_layer->getId(), m_feature->getId());

  draw(true);
}
