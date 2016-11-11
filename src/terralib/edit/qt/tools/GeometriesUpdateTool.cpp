/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

  This file is part of the TerraLib - a Framework for building GIS enabled applications.

  TerraLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See them_initialPosition
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraLib. See COPYING. If not, write to
  TerraLib Team at <terralib-team@terralib.org>.
*/

/*!
\file GeometriesUpdateTool.cpp

\brief

*/

// TerraLib
#include "../../../dataaccess/dataset/ObjectId.h"
#include "../../../geometry/Geometry.h"
#include "../../../maptools/AbstractLayer.h"
#include "../../Feature.h"
#include "../../RepositoryManager.h"
#include "../core/UndoStackManager.h"
#include "GeometriesUpdateTool.h"

te::edit::GeometriesUpdateTool::GeometriesUpdateTool(te::qt::widgets::MapDisplay* display, te::map::AbstractLayer* layer, QObject* parent) :
  te::qt::widgets::AbstractTool(display, parent),
  m_isInUse(true),
  m_feature(0),
  m_layer(layer)
{}

te::edit::GeometriesUpdateTool::~GeometriesUpdateTool()
{
  emit toolDeleted();
}

void te::edit::GeometriesUpdateTool::setLayer(te::map::AbstractLayer* layer)
{
  m_layer = layer;
}

std::string te::edit::GeometriesUpdateTool::getLayerId()
{
  return m_layer->getId();
}

void te::edit::GeometriesUpdateTool::saveGeometry(te::gm::Geometry* gm, const int& op)
{
  RepositoryManager::getInstance().addGeometry(m_layer->getId(), gm, (FeatureType) op);

  m_feature = RepositoryManager::getInstance().getFeature(m_layer->getId(), *gm->getMBR(), gm->getSRID());

  emit geometriesEdited();
}

bool te::edit::GeometriesUpdateTool::isInUse() const
{
  return m_isInUse;
}

void te::edit::GeometriesUpdateTool::setInUse(const bool& status)
{
  m_isInUse = status;
}

void te::edit::GeometriesUpdateTool::resetVisualizationTool()
{
  m_feature = 0;
}

void te::edit::GeometriesUpdateTool::storeFeature()
{
  RepositoryManager::getInstance().addFeature(m_layer->getId(), m_feature->clone());

  emit geometriesEdited();
}

void te::edit::GeometriesUpdateTool::storeUndoCommand()
{
  return;
}

void te::edit::GeometriesUpdateTool::onUndoFeedback()
{
  return;
}