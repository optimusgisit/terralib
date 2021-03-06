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

  \brief This class implements a concrete tool to remove geometries.
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

// STL
#include <cassert>
#include <memory>

te::edit::DeleteGeometryTool::DeleteGeometryTool(te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer, QObject* parent)
  : GeometriesUpdateTool(display, layer.get(), parent)
{
  updateCursor();
}

te::edit::DeleteGeometryTool::~DeleteGeometryTool()
{
}

bool te::edit::DeleteGeometryTool::mousePressEvent(QMouseEvent* e)
{
  if (e->button() != Qt::LeftButton)
    return false;

  m_feature = PickFeature(m_layer, buildEnvelope(e->pos()), m_display->getSRID(), te::edit::TO_DELETE);

  if (m_feature == 0)
  {
    QMessageBox::critical(m_display, tr("Error"), QString(tr("The geometry cannot be selected from the layer.")));
    return false;
  }
  else
  {
    storeFeature();
  }

  return true;

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

void te::edit::DeleteGeometryTool::storeFeature()
{
  te::edit::FeatureType op = te::edit::TO_DELETE;

  if (RepositoryManager::getInstance().hasIdentify(m_layer->getId(), m_feature->getId()->clone()) == true)
  {
    switch (m_feature->clone()->getType())
    {
      case te::edit::TO_DELETE:

        m_feature->setType(te::edit::TO_UPDATE);
        op = te::edit::TO_UPDATE;

        break;

      case te::edit::TO_UPDATE:

        m_feature->setType(te::edit::TO_DELETE);
        op = te::edit::TO_DELETE;

        break;

      default:
        return;
    }
  }

  RepositoryManager::getInstance().addGeometry(m_layer->getId(), m_feature->getId()->clone(), dynamic_cast<te::gm::Geometry*>(m_feature->getGeometry()->clone()), op);

  emit geometriesEdited();

}

void te::edit::DeleteGeometryTool::updateCursor()
{
  m_display->setCursor(Qt::ArrowCursor);
}