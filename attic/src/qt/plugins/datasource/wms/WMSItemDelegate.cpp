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

#include "WMSItemDelegate.h"

// TerraLib
#include "../../../widgets/layer/explorer/LayerItem.h"

te::qt::plugins::wms::WMSItemDelegate::WMSItemDelegate(QStyledItemDelegate* decorated, QObject* parent) :
  te::common::Decorator<QStyledItemDelegate>(decorated, false)
{
  setParent(parent);

  m_icon = QIcon::fromTheme("datasource-wms");
}

te::qt::plugins::wms::WMSItemDelegate::~WMSItemDelegate()
{

}

void te::qt::plugins::wms::WMSItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  te::qt::widgets::TreeItem* item = static_cast<te::qt::widgets::TreeItem*>(index.internalPointer());

  if(item->getType() == "LAYER")
  {
    te::qt::widgets::LayerItem* li = (te::qt::widgets::LayerItem*)item;

    if(li->getLayer()->isValid() && li->getLayer()->getType() == "WMSLAYER")
    {
      QStyleOptionViewItem opt = option;
      opt.decorationSize = QSize(20, 20);
      opt.features |= QStyleOptionViewItem::HasDecoration;

      opt.icon = m_icon;

      QStyledItemDelegate::paint(painter, opt, index);

      return;
    }
  }

  if(m_decorated != 0)
    m_decorated->paint(painter, option, index);
  else
    QStyledItemDelegate::paint(painter, option, index);
}
