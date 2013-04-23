/*  Copyright (C) 2001-2013 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/vp/qt/DataSetLayerItem.cpp

  \brief The class that represents a dataset layer item in a LayerTreeModel.
*/

// TerraLib
#include "../../common/Translator.h"
#include "../../maptools/DataSetLayer.h"
#include "../core/Exception.h"
#include "DataSetLayerItem.h"

// Qt
#include <QtGui/QMenu>
#include <QtGui/QWidget>

te::vp::DataSetLayerItem::DataSetLayerItem(const te::map::AbstractLayerPtr& l, QObject* parent)
  : AbstractLayerTreeItem(parent)
{
  m_layer = boost::dynamic_pointer_cast<te::map::DataSetLayer>(l);
}

te::vp::DataSetLayerItem::~DataSetLayerItem()
{
}

int te::vp::DataSetLayerItem::columnCount() const
{
  return 1;
}

QVariant te::vp::DataSetLayerItem::data(int /*column*/, int role) const
{
  //if(role == Qt::DecorationRole)
  //  return QVariant(QIcon::fromTheme("dataset-layer"));

  if(role == Qt::DisplayRole)
    return QVariant(QString::fromStdString(m_layer->getTitle()));

  if(role == Qt::CheckStateRole)
    return QVariant(m_layer->getVisibility() == te::map::VISIBLE ? Qt::Checked : Qt::Unchecked);

  return QVariant();
}

QMenu* te::vp::DataSetLayerItem::getMenu(QWidget* /*parent*/) const
{
  //QMenu* m = new QMenu(parent);

  //QAction* aOpenDataSource = m->addAction(tr("&Open data source"));

  //connect(aOpenDataSource, SIGNAL(triggered()), this, SLOT(openDataSource()));

  //return m;
  return 0;
}

bool te::vp::DataSetLayerItem::canFetchMore() const
{
  return !children().isEmpty();
}

Qt::ItemFlags te::vp::DataSetLayerItem::flags() const
{
  return Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;;
}

void te::vp::DataSetLayerItem::fetchMore()
{
}

bool te::vp::DataSetLayerItem::hasChildren() const
{
  return !children().isEmpty();
}

bool te::vp::DataSetLayerItem::setData(const QVariant& value, int role)
{
  if(role == Qt::CheckStateRole)
  {
    bool ok = false;
    Qt::CheckState checkState = static_cast<Qt::CheckState>(value.toInt(&ok));

    if(!ok)
      return false;

    if(checkState == Qt::Checked)
      m_layer->setVisibility(te::map::VISIBLE);
    else if(checkState == Qt::Unchecked)
      m_layer->setVisibility(te::map::NOT_VISIBLE);

    return true;
  }

  return false;
}

te::map::AbstractLayerPtr te::vp::DataSetLayerItem::getLayer() const
{
  return m_layer;
}

//te::vp::AbstractLayerTreeItem* te::vp::DataSetLayerItem::clone(QObject* parent)
//{
//  return new DataSetLayerItem(m_layer, parent);
//}

