/*  Copyright (C) 2011-2012 National Institute For Space Research (INPE) - Brazil.

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

#include "DataSetTableDockWidget.h"

// TerraLib
#include "../../../maptools/AbstractLayer.h"
#include "../../widgets/table/DataSetTableView.h"
#include "../../widgets/utils/ScopedCursor.h"
#include "../events/LayerEvents.h"
#include "../ApplicationController.h"

te::qt::af::DataSetTableDockWidget::DataSetTableDockWidget(QWidget* parent)
  : QDockWidget(parent, Qt::Widget),
    m_layer(0)
{
  m_view = new te::qt::widgets::DataSetTableView(this);

  setWidget(m_view);

  setAttribute(Qt::WA_DeleteOnClose, true);

  connect (m_view, SIGNAL(selectOIds(te::da::ObjectIdSet*, const bool&)), SLOT(selectionChanged(te::da::ObjectIdSet*, const bool&)));
  connect (m_view, SIGNAL(deselectOIds(te::da::ObjectIdSet*)), SLOT(removeSelectedOIds(te::da::ObjectIdSet*)));
}

te::qt::af::DataSetTableDockWidget::~DataSetTableDockWidget()
{
  emit closed(this);
}

void te::qt::af::DataSetTableDockWidget::setLayer(te::map::AbstractLayer* layer)
{
  m_layer = layer;

  if(m_layer == 0)
    return;

  te::qt::widgets::ScopedCursor cursor(Qt::WaitCursor);

  m_view->setDataSet(m_layer->getData());
  m_view->setLayerSchema(m_layer->getSchema(true));

  if(layer->getSelected() != 0)
    m_view->highlightOIds(layer->getSelected());

  setWindowTitle(m_layer->getTitle().c_str());
}

te::map::AbstractLayer* te::qt::af::DataSetTableDockWidget::getLayer() const
{
  return m_layer;
}

void te::qt::af::DataSetTableDockWidget::onApplicationTriggered(te::qt::af::evt::Event* evt)
{
  switch(evt->m_id)
  {
    case te::qt::af::evt::LAYER_SELECTION_CHANGED:
    {
      te::qt::af::evt::LayerSelectionChanged* ev = static_cast<te::qt::af::evt::LayerSelectionChanged*>(evt);

      if(ev->m_layer->getId() == m_layer->getId())
        m_view->highlightOIds(ev->m_layer->getSelected());
    }
    break;
  }
}

void te::qt::af::DataSetTableDockWidget::selectionChanged(te::da::ObjectIdSet* oids, const bool& add)
{
  if (!add)
    m_layer->clearSelected();

  m_layer->select(oids);

  te::qt::af::evt::LayerSelectionChanged e(m_layer);
  ApplicationController::getInstance().broadcast(&e);
}

void te::qt::af::DataSetTableDockWidget::removeSelectedOIds(te::da::ObjectIdSet* oids)
{
  if(m_layer->getSelected() != 0)
    m_layer->deselect(oids);
}