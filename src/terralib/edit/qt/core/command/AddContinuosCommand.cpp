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
\file AddContinuosCommand.cpp

\brief

*/

// TerraLib
#include "../../../../dataaccess/dataset/ObjectId.h"
#include "../../../../geometry/Geometry.h"
#include "../../../Utils.h"
#include "../../../RepositoryManager.h"
#include "../../Utils.h"
#include "../../Renderer.h"
#include "../UndoStackManager.h"
#include "AddContinuosCommand.h"

// STL
#include <set>

te::edit::AddContinuosCommand::AddContinuosCommand(std::vector<Feature*>& items, std::vector<te::gm::Coord2D>& coords, int& correntIndex,
  te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer,  QUndoCommand *parent) :
  QUndoCommand(parent)
, m_display(display)
, m_layer(layer)
, m_addItems(items)
, m_coords(coords)
, m_correntIndex(correntIndex)
{
}

te::edit::AddContinuosCommand::~AddContinuosCommand()
{
}

void  te::edit::AddContinuosCommand::undo()
{
  if (m_addItems.empty())
    return;

  m_correntIndex--;

  if (m_correntIndex >= 0)
  {
    m_coords.clear();

    for (std::size_t i = 0; i < m_addItems.at(m_correntIndex)->clone()->getCoords().size(); i++)
      m_coords.push_back(m_addItems.at(m_correntIndex)->clone()->getCoords()[i]);
  }
  else
    m_correntIndex = -1;

  draw();
}

void te::edit::AddContinuosCommand::redo()
{
  bool resultFound = false;

  if (!UndoStackManager::getInstance().getUndoStack())
    return;

  for (int i = 0; i < UndoStackManager::getInstance().getUndoStack()->count(); ++i)
  {
    const QUndoCommand* cmd = UndoStackManager::getInstance().getUndoStack()->command(i);
    if (cmd == this)
    {
      resultFound = true;
    }
  }

  //no makes redo while the command is not on the stack
  if (resultFound)
  {
    m_correntIndex++;
    
    if (m_correntIndex >= (int)m_addItems.size())
    {
      m_correntIndex = (int)m_addItems.size() - 1;
      return;
    }

    m_coords.clear();

    for (std::size_t i = 0; i < m_addItems.at(m_correntIndex)->clone()->getCoords().size(); i++)
      m_coords.push_back(m_addItems.at(m_correntIndex)->clone()->getCoords()[i]);

    draw();
  }

}

void te::edit::AddContinuosCommand::draw()
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
  if(m_correntIndex >= 0)
    renderer.draw(m_addItems[m_correntIndex]->getGeometry(), true);
  else
    draft->fill(Qt::transparent);

  // Draw the layer edited geometries
  renderer.drawRepository(m_layer->getId(), env, m_display->getSRID());

  renderer.end();

  m_display->repaint();

}