/*  Copyright (C) 2001-2010 National Institute For Space Research (INPE) - Brazil.

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
  \file LayerExplorerModel.cpp

  \brief It provides an implementation for an item model classes based on te::qt::widgets::AbstractLayerItem.
 */

// TerraLib
#include "../../../maptools/AbstractLayer.h"
#include "../../../maptools/Layer.h"
#include "AbstractLayerItemFactory.h"
#include "LayerExplorerModel.h"

// Qt
#include <QtCore/QByteArray>
#include <QtCore/QMimeData>
#include <QtGui/QApplication>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

te::qt::widgets::LayerExplorerModel::LayerExplorerModel(te::map::AbstractLayer* layer, QObject* parent)
  : QAbstractItemModel(parent),
    m_rootItem(0),
    m_dndOperation(false)
{
  te::qt::widgets::AbstractLayerItem* layerItem = 0;
  m_rootItem = AbstractLayerItemFactory::make(layer->getType(),
                   std::pair<te::map::AbstractLayer*, te::qt::widgets::AbstractLayerItem*>(layer, layerItem));
}

te::qt::widgets::LayerExplorerModel::~LayerExplorerModel()
{
  delete m_rootItem;
}

QModelIndex te::qt::widgets::LayerExplorerModel::index(int row, int column, const QModelIndex& parent) const
{
  if(!hasIndex(row, column, parent))
    return QModelIndex();

  te::qt::widgets::AbstractLayerItem* parentItem = 0;

  if(parent.isValid())
    parentItem = static_cast<te::qt::widgets::AbstractLayerItem*>(parent.internalPointer());
  else
    parentItem = m_rootItem;

  if (parentItem == 0)
    return QModelIndex();

  te::qt::widgets::AbstractLayerItem* item = static_cast<te::qt::widgets::AbstractLayerItem*>(parentItem->getChildren().at(row));

  if(item)
    return createIndex(row, column, item);

  return QModelIndex();
}

QModelIndex te::qt::widgets::LayerExplorerModel::parent(const QModelIndex& index) const
{
  if(!index.isValid())
    return QModelIndex();

  te::qt::widgets::AbstractLayerItem* childItem = static_cast<te::qt::widgets::AbstractLayerItem*>(index.internalPointer());

  if (childItem == 0)
    return QModelIndex();

  te::qt::widgets::AbstractLayerItem* parentItem = static_cast<te::qt::widgets::AbstractLayerItem*>(childItem->parent());

  if(parentItem == m_rootItem)
    return QModelIndex();

  if (parentItem == 0)
    return QModelIndex();

  size_t row = parentItem->getIndex();

  return createIndex(row, 0, parentItem);
}

int te::qt::widgets::LayerExplorerModel::rowCount(const QModelIndex& parent) const
{
  if(parent.column() > 0)
    return 0;

  te::qt::widgets::AbstractLayerItem* parentItem = 0; 

 if(parent.isValid())
    parentItem = static_cast<te::qt::widgets::AbstractLayerItem*>(parent.internalPointer());
  else
    parentItem = m_rootItem;

 if (parentItem == 0)
   return 0;

  return parentItem->getChildren().count();
}

int te::qt::widgets::LayerExplorerModel::columnCount(const QModelIndex& /*parent*/) const
{
 return 1;
}

QVariant te::qt::widgets::LayerExplorerModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
    return QVariant();

  te::qt::widgets::AbstractLayerItem* item = static_cast<te::qt::widgets::AbstractLayerItem*>(index.internalPointer());
  
  if(item == 0)
    return QVariant();

  return item->data(role);
}

bool te::qt::widgets::LayerExplorerModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if((!index.isValid()) || (index.column() > 0))
    return false;

  te::qt::widgets::AbstractLayerItem* item = static_cast<te::qt::widgets::AbstractLayerItem*>(index.internalPointer());

  if(item == 0)
    return false;

  if(role == Qt::EditRole)
  {
    item->getRefLayer()->setTitle(value.toString().toStdString());
    emit dataChanged(index, index);
    return true;
  }
  else if(role == Qt::CheckStateRole)
  {
    // Get item visibility
    bool ok = false;
    Qt::CheckState checkState = static_cast<Qt::CheckState>(value.toInt(&ok));

    if(!ok)
      return false;

    if (item->getRefLayer() == 0)
      return false;

    // Set the reference layer visibility associated to the item
    if(checkState == Qt::Checked)
      item->getRefLayer()->setVisibility(te::map::VISIBLE);
    else if(checkState == Qt::Unchecked)
      item->getRefLayer()->setVisibility(te::map::NOT_VISIBLE);
    else if (checkState == Qt::PartiallyChecked)
      item->getRefLayer()->setVisibility(te::map::PARTIALLY_VISIBLE);

    emit dataChanged(index, index);

    // Emit the dataChanged signal for the ascendants indexes
    QModelIndex ascendentIndex = parent(index);
    while(ascendentIndex.isValid())
    {
      emit dataChanged(ascendentIndex, ascendentIndex);
      ascendentIndex = parent(ascendentIndex);
    }

    // Emit the dataChanged signal for the descendants indexes
    dataChangedForDescendantsIndexes(index);

    return true;
  }

  return true;
}

QVariant te::qt::widgets::LayerExplorerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
  {
    if(section == 0)
    {
      te::qt::widgets::AbstractLayerItem* root = static_cast<te::qt::widgets::AbstractLayerItem*>(m_rootItem);

      if (root == 0 || root->getRefLayer() == 0)
        return QVariant();
      
      return QVariant(root->getRefLayer()->getTitle().c_str());
    }
  }

  return QVariant();
}

Qt::ItemFlags te::qt::widgets::LayerExplorerModel::flags(const QModelIndex &index) const
{
  if(!index.isValid())
    return Qt::ItemIsDropEnabled;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |
         Qt::ItemIsDropEnabled | Qt::ItemIsUserCheckable;
}

Qt::DropActions te::qt::widgets::LayerExplorerModel::supportedDropActions() const
{
  Qt::KeyboardModifiers keyboardModifiers = QApplication::keyboardModifiers();

  if (keyboardModifiers == Qt::ControlModifier)
     return Qt::CopyAction;
  
  return Qt::MoveAction;
}

QStringList te::qt::widgets::LayerExplorerModel::mimeTypes() const
{
  QStringList types;
  types << "application/layer-explorer";
  return types;
}

QMimeData* te::qt::widgets::LayerExplorerModel::mimeData(const QModelIndexList& indexes) const
{
  QString s;
  m_dragIndex = indexes[0];

  QMimeData *mimeData = new QMimeData();
  te::qt::widgets::AbstractLayerItem* item = static_cast<te::qt::widgets::AbstractLayerItem*>(indexes[0].internalPointer());
  te::map::AbstractLayer* al = item->getRefLayer();
  s.setNum((qulonglong)al);
  QByteArray encodedData(s.toStdString().c_str());
  mimeData->setData("application/layer-explorer", encodedData);

  return mimeData;
}

bool te::qt::widgets::LayerExplorerModel::dropMimeData(const QMimeData* data, Qt::DropAction action, 
                  int row, int column, const QModelIndex& dropIndex)
{
  m_dndOperation = true;

  if(action == Qt::IgnoreAction)
    return true;

  if(column > 0)
    return false;

  if(!data->hasFormat("application/layer-explorer"))
    return false;

  // Get the drop item
  te::qt::widgets::AbstractLayerItem* dropItem = getItem(dropIndex);

  // Get the drop parent item
  te::qt::widgets::AbstractLayerItem* dropParentItem = static_cast<te::qt::widgets::AbstractLayerItem*>(dropItem->parent());

  if(dropParentItem == 0)
    return false;

  // Get the drag parent item
  te::qt::widgets::AbstractLayerItem* dragParentItem = getItem(m_dragIndex.parent());

  // Get the drag row
  int dragRow = m_dragIndex.row();

  // Get the drop row
  int dropRow = dropIndex.row();

  // Check if the parent of the drag item is the same as the drop item parent
  if(dragParentItem == dropParentItem)
  {
    if(dragRow == dropRow)
      return true;

    // Remove the dragged item
    beginRemoveRows(m_dragIndex.parent(), dragRow, dragRow);
    dragParentItem->removeChildren(dragRow, 1);
    endRemoveRows();

    // Insert the dragged item at the dropped item position
    insertRows(dropRow, 1, dropIndex.parent());

    emit dragDropEnded(m_dragIndex, dropIndex);
  }

  return true;
}

bool te::qt::widgets::LayerExplorerModel::insertRows(int row, int count, const QModelIndex& parent)
{
  te::qt::widgets::AbstractLayerItem* parentLayerItem;

  if(parent.isValid())
    parentLayerItem = getItem(parent);
  else
    parentLayerItem = static_cast<te::qt::widgets::AbstractLayerItem*>(m_rootItem);

  beginInsertRows(parent, row, row + count - 1);
  te::qt::widgets::AbstractLayerItem* layerItem;

  for (int i = 0; i < count; ++i)
  {
    if(m_childItemsToBeInserted.empty() == false)
      layerItem = m_childItemsToBeInserted[i];
    else
      layerItem = getItem(m_dragIndex);
    parentLayerItem->addChild(row++, layerItem);
  }

  endInsertRows();
  return true;
}

bool	te::qt::widgets::LayerExplorerModel::removeRows(int row, int count, const QModelIndex& parent)
{
  if(m_dndOperation == true)
  {
    m_dndOperation = false;
    return true;
  }

  te::qt::widgets::AbstractLayerItem* parentLayerItem;

  if(parent.isValid())
    parentLayerItem = static_cast<te::qt::widgets::AbstractLayerItem*>(parent.internalPointer());
  else
    parentLayerItem = static_cast<te::qt::widgets::AbstractLayerItem*>(m_rootItem);

  beginRemoveRows(parent, row, row+count-1);
  parentLayerItem->removeChildren(row, count);
  endRemoveRows();
  return true;
}

te::qt::widgets::AbstractLayerItem* te::qt::widgets::LayerExplorerModel::getItem(const QModelIndex& index)
{
  if(index.isValid())
    return static_cast<te::qt::widgets::AbstractLayerItem*>(index.internalPointer());
  else
    return m_rootItem;
}


void te::qt::widgets::LayerExplorerModel::resetModel()
{
  reset();
}

void te::qt::widgets::LayerExplorerModel::setItemsToBeInserted(std::vector<te::qt::widgets::AbstractLayerItem*> items)
{
  m_childItemsToBeInserted = items;
}

void te::qt::widgets::LayerExplorerModel::dataChangedForDescendantsIndexes(const QModelIndex& parentIndex)
{
  if (parentIndex.isValid() == false)
    return;

  int rows = rowCount(parentIndex);

  for (int i = 0; i < rows; ++i)
  {
    QModelIndex idx = index(i, 0, parentIndex);

    emit dataChanged(idx, idx);
    dataChangedForDescendantsIndexes(idx);
  }
}
