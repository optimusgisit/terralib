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
#include "DataSetTableView.h"

// TerraLib
#include "../../../maptools/AbstractLayer.h"

te::qt::widgets::DataSetTableDockWidget::DataSetTableDockWidget(QWidget* parent) :
QDockWidget(parent, Qt::Widget),
  m_layer(0)
{
  m_view = new DataSetTableView(this);

  setWidget(m_view);
}

te::qt::widgets::DataSetTableDockWidget::~DataSetTableDockWidget()
{
}

void te::qt::widgets::DataSetTableDockWidget::setLayer(te::map::AbstractLayer* layer)
{
  m_layer = layer;

  if(m_layer==0)
    return;

  m_view->setDataSet(m_layer->getData());
  m_view->setLayerSchema(m_layer->getSchema());

  setWindowTitle(m_layer->getTitle().c_str());
}

te::map::AbstractLayer* te::qt::widgets::DataSetTableDockWidget::getLayer() const
{
  return m_layer;
}