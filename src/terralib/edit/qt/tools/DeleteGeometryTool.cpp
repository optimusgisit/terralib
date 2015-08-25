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
  \file terralib/edit/qt/tools/DeleteGeometryTool.cpp

  \brief This class implements a concrete tool to move geometries.
*/

// TerraLib
#include "../../../geometry/GeometryProperty.h"
#include "../../../geometry/MultiPolygon.h"
#include "../../../dataaccess/dataset/ObjectIdSet.h"
#include "../../../dataaccess/utils/Utils.h"
#include "../../../qt/widgets/canvas/MapDisplay.h"
#include "../../../qt/widgets/Utils.h"
#include "../../Feature.h"
#include "../../RepositoryManager.h"
#include "../../Utils.h"
#include "../Renderer.h"
#include "../Utils.h"
#include "DeleteGeometryTool.h"

// Qt
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

// STL
#include <cassert>
#include <memory>

te::edit::DeleteGeometryTool::DeleteGeometryTool(te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer, QObject* parent)
  : GeometriesUpdateTool(display, layer.get(), parent)
{}

te::edit::DeleteGeometryTool::~DeleteGeometryTool()
{
  delete m_feature;
}

bool te::edit::DeleteGeometryTool::mousePressEvent(QMouseEvent* e)
{
  m_feature = PickFeature(m_layer, buildEnvelope(e->pos()), m_display->getSRID(), te::edit::GEOMETRY_DELETE);

  if(m_feature == 0)
  {
    QMessageBox::critical(m_display, tr("Error"), QString(tr("The geometry cannot be selected from the layer.")));
    return false;
  }

  storeRemovedFeature();

  return true;
}

bool te::edit::DeleteGeometryTool::mouseMoveEvent(QMouseEvent* e)
{
  return false;
}

bool te::edit::DeleteGeometryTool::mouseReleaseEvent(QMouseEvent* e)
{
  return false;
}

bool te::edit::DeleteGeometryTool::mouseDoubleClickEvent(QMouseEvent* e)
{
  return false;
}

void te::edit::DeleteGeometryTool::reset()
{
  delete m_feature;
  m_feature = 0;
}

void te::edit::DeleteGeometryTool::pickFeature(const te::map::AbstractLayerPtr& layer, const QPointF& pos)
{
  reset();

  try
  {
    std::auto_ptr<te::da::DataSetType> dt(layer->getSchema());

    const te::da::ObjectIdSet* objSet = layer->getSelected();

    std::auto_ptr<te::da::DataSet> ds(layer->getData(objSet));

    te::gm::GeometryProperty* geomProp = te::da::GetFirstGeomProperty(dt.get());

    if (ds->moveNext())
    {

      te::gm::Coord2D coord(0, 0);

      std::auto_ptr<te::gm::Geometry> geom = ds->getGeometry(geomProp->getName());
      te::gm::Envelope auxEnv(*geom->getMBR());

      // Try finds the geometry centroid
      switch (geom->getGeomTypeId())
      {
        case te::gm::PolygonType:
        {
          te::gm::Polygon* p = dynamic_cast<te::gm::Polygon*>(geom.get());
          coord = *p->getCentroidCoord();

          break;
        }
        case te::gm::MultiPolygonType:
        {
          te::gm::MultiPolygon* mp = dynamic_cast<te::gm::MultiPolygon*>(geom.get());
          coord = *mp->getCentroidCoord();

          break;
        }
      }

      // Build the search envelope
      te::gm::Envelope e(coord.getX(), coord.getY(), coord.getX(), coord.getY());

      m_feature = PickFeature(m_layer, e, m_display->getSRID(), te::edit::GEOMETRY_DELETE);

    }

  }
  catch (std::exception& e)
  {
    QMessageBox::critical(m_display, tr("Error"), QString(tr("The geometry cannot be selected from the layer. Details:") + " %1.").arg(e.what()));
  }
}

te::gm::Envelope te::edit::DeleteGeometryTool::buildEnvelope(const QPointF& pos)
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

void te::edit::DeleteGeometryTool::storeRemovedFeature()
{
  RepositoryManager::getInstance().addGeometry(m_layer->getId(), m_feature->getId()->clone(), dynamic_cast<te::gm::Geometry*>(m_feature->getGeometry()->clone()), GEOMETRY_DELETE);

  emit geometriesEdited();
}
