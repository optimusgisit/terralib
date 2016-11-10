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

#ifndef __TERRALIB_QT_PLUGINS_DATASOURCE_WMS_INTERNAL_ITEMDELEGATE_H
#define __TERRALIB_QT_PLUGINS_DATASOURCE_WMS_INTERNAL_ITEMDELEGATE_H

// TerraLib
#include "../../../../common/Decorator.h"

// Qt
#include <QIcon>
#include <QStyledItemDelegate>

namespace te
{
  namespace qt
  {
    // Forward declarations
    namespace plugins
    {
      namespace wms
      {
        class WMSItemDelegate : public te::common::Decorator<QStyledItemDelegate>
        {
        public:
          WMSItemDelegate(QStyledItemDelegate* decorated, QObject* parent = 0);

          ~WMSItemDelegate();

          void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

        protected:
          QIcon m_icon;
        };
      }
    }
  }
}

#endif //__TERRALIB_QT_PLUGINS_DATASOURCE_WMS_INTERNAL_ITEMDELEGATE_H
