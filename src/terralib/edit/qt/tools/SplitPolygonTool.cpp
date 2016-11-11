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
  \file terralib/edit/qt/tools/SplitPolygonTool.cpp

  \brief This class implements a concrete tool to split polygons.
*/

// TerraLib
#include "../../../common/STLUtils.h"
#include "../../../geometry/Envelope.h"
#include "../../../dataaccess/utils/Utils.h"
#include "../../../qt/widgets/canvas/MapDisplay.h"
#include "../../../qt/widgets/Utils.h"
#include "../../Feature.h"
#include "../../Repository.h"
#include "../../RepositoryManager.h"
#include "../../Utils.h"
#include "../Renderer.h"
#include "../Utils.h"
#include "SplitPolygonTool.h"

// Qt
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

// STL
#include <cassert>
#include <memory>

te::edit::SplitPolygonTool::SplitPolygonTool(te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer, 
                                             const te::edit::MouseEventEdition mouseEventToSave, QObject* parent)
  : CreateLineTool(display, layer, Qt::ArrowCursor, parent),
  m_tol(0.000001),
  m_oidSet(0),
  m_vecFeature(0),
  m_mouseEventToSave(mouseEventToSave)
{
  pickFeatures();

  draw();
}

te::edit::SplitPolygonTool::~SplitPolygonTool()
{
  te::common::FreeContents(m_vecFeature);
  m_vecFeature.clear();

  if (m_oidSet)
    m_oidSet = 0;

  m_layer->clearSelected();
}

bool te::edit::SplitPolygonTool::mousePressEvent(QMouseEvent* e)
{
  if (e->button() != Qt::LeftButton)
    return false;

  if (m_isFinished) // Is Finished?! So, start again...
  {
    resetVisualizationTool();
    m_isFinished = false;

    pickFeatures();
  }

  return te::edit::CreateLineTool::mousePressEvent(e);
}

bool te::edit::SplitPolygonTool::mouseMoveEvent(QMouseEvent* e)
{
  return te::edit::CreateLineTool::mouseMoveEvent(e);
}

bool te::edit::SplitPolygonTool::mouseDoubleClickEvent(QMouseEvent* e)
{
  if (m_mouseEventToSave != te::edit::mouseDoubleClick || Qt::LeftButton != e->button())
    return false;

  startSplit();

  return true;
}

bool te::edit::SplitPolygonTool::mouseReleaseEvent(QMouseEvent* e)
{
  if (m_mouseEventToSave != te::edit::mouseReleaseRightClick || Qt::RightButton != e->button())
    return false;

  startSplit();

  return true;
}

void te::edit::SplitPolygonTool::startSplit()
{
  if (m_coords.size() < 2) // Can not stop yet...
    return;

  m_isFinished = true;

  if (m_oidSet == 0)
    m_oidSet = new te::da::ObjectIdSet();

  for (std::size_t i = 0; i < m_vecFeature.size(); ++i)
  {
    if (RepositoryManager::getInstance().hasIdentify(m_layer->getId(), m_vecFeature[i]->getId()))
    {
      RepositoryManager::getInstance().removeFeature(m_layer->getId(), m_vecFeature[i]->getId());
      splitPolygon(i);
    }
  }

  te::edit::CreateLineTool::clear();

  if (!m_oidSet->size())
    return;

  emit geometriesEdited();
}

void te::edit::SplitPolygonTool::splitPolygon(std::size_t index)
{
  int srid = m_vecFeature.at(index)->getGeometry()->getSRID();
  std::vector<te::gm::Polygon*> outPolygons;

  std::auto_ptr<te::gm::Geometry> geom_bounds;
  geom_bounds.reset(m_vecFeature.at(index)->getGeometry()->getBoundary());
  geom_bounds->setSRID(srid);

  std::auto_ptr<te::gm::Geometry> blade_in;
  blade_in.reset(te::edit::CreateLineTool::buildLine());
  blade_in->setSRID(srid);

  std::auto_ptr<te::gm::Geometry> vgeoms;
  vgeoms.reset(geom_bounds->Union(blade_in.get()));
  vgeoms->setSRID(srid);

  te::gm::Polygonizer(vgeoms.get(), outPolygons);

  for (std::size_t i = 0; i < outPolygons.size(); ++i)
  {
    outPolygons.at(i)->setSRID(m_layer->getSRID());
    if (outPolygons.at(i)->getSRID() != m_display->getSRID())
      outPolygons.at(i)->transform(m_display->getSRID());
  }

  m_vecFeature.at(index)->getGeometry()->setSRID(m_layer->getSRID());
  if (m_vecFeature.at(index)->getGeometry()->getSRID() != m_display->getSRID())
    m_vecFeature.at(index)->getGeometry()->transform(m_display->getSRID());

  std::size_t i = 1;
  while (i < outPolygons.size())
  {
    if (!outPolygons.at(i)->equals(m_vecFeature.at(index)->getGeometry()) &&
         outPolygons.at(i)->coveredBy(m_vecFeature.at(index)->getGeometry()->buffer(m_tol)))
    {
      Feature* f = new Feature(GenerateId(), outPolygons.at(i), te::edit::TO_ADD);
      RepositoryManager::getInstance().addFeature(m_layer->getId(), f->clone());

      m_oidSet->add(f->getId());
    }

    i++;
  }

  if (m_oidSet->size() && 
      outPolygons.at(0)->coveredBy(m_vecFeature.at(index)->getGeometry()->buffer(m_tol)))
  {
    m_vecFeature.at(index)->setGeometry(outPolygons.at(0));
    RepositoryManager::getInstance().addFeature(m_layer->getId(), m_vecFeature.at(index)->clone());

    m_oidSet->add(m_vecFeature.at(index)->getId());
  }

}

void te::edit::SplitPolygonTool::draw()
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

  renderer.end();

  m_display->repaint();
}

void te::edit::SplitPolygonTool::resetVisualizationTool()
{
  te::edit::Repository* repo = te::edit::RepositoryManager::getInstance().getRepository(m_layer->getId());

  if (m_oidSet)
  {
    std::set<te::da::ObjectId*, te::common::LessCmp<te::da::ObjectId*> >::const_iterator it;
    for (it = m_oidSet->begin(); it != m_oidSet->end(); ++it)
    {
      if (repo->hasIdentifier(*it))
        repo->remove((*it));
    }
  }

  te::edit::CreateLineTool::clear();
  m_isFinished = true;
  m_oidSet = 0;
}

void te::edit::SplitPolygonTool::pickFeatures()
{
  try
  {
    std::auto_ptr<te::da::DataSet> ds(m_layer->getData(m_layer->getSelected()));
    std::size_t gpos = te::da::GetFirstSpatialPropertyPos(ds.get());

    std::vector<std::string> oidPropertyNames;
    te::da::GetOIDPropertyNames(m_layer->getSchema().get(), oidPropertyNames);

    m_vecFeature.clear();

    ds->moveBeforeFirst();
    while (ds->moveNext())
    {
      std::auto_ptr<te::gm::Geometry> g(ds->getGeometry(gpos));

      Feature* f = new Feature(te::da::GenerateOID(ds.get(), oidPropertyNames), g.release(), te::edit::TO_UPDATE);
      
      m_vecFeature.push_back(f);
      RepositoryManager::getInstance().addFeature(m_layer->getId(), f->clone());
    }

  }
  catch (std::exception& e)
  {
    QMessageBox::critical(m_display, tr("Error"), QString(tr("The geometry cannot be selected from the layer. Details:") + " %1.").arg(e.what()));
  }
}