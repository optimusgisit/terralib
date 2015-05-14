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
  \file terralib/wms/qt/WMSLayerItem.h

  \brief The class that represents a WMS layer item in a LayerTreeModel.
*/

#ifndef __TERRALIB_WMS_QT_WMSLAYERITEM_H
#define __TERRALIB_WMS_QT_WMSLAYERITEM_H

// TerraLib
#include "../../qt/widgets/layer/explorer/AbstractTreeItem.h"
#include "../Config.h"
#include "../WMSLayer.h"

// Qt
#include <QtCore/QString>

namespace te
{
  namespace wms
  {
    class TEWMSEXPORT WMSLayerItem : public te::qt::widgets::AbstractTreeItem
    {
      Q_OBJECT

      public:

        WMSLayerItem(const te::map::AbstractLayerPtr& l, QObject* parent = 0);

        ~WMSLayerItem();

        int columnCount() const;

        QVariant data(int column, int role) const;

        QMenu* getMenu(QWidget* parent = 0) const;

        bool canFetchMore() const;

        Qt::ItemFlags flags() const;

        void fetchMore();

        bool hasChildren() const;

        bool setData(int column, const QVariant& value, int role = Qt::EditRole);

        te::map::AbstractLayerPtr getLayer() const;

        /*!
          \brief It returns the item type: "WMS_LAYER_ITEM".

          \return The item type: "WMS_LAYER_ITEM".
        */
        const std::string getItemType() const;

        QString buildToolTip() const;

      private:

        bool hasColorMapItem() const;

      private:

        WMSLayerPtr m_layer;
    };

  }   // end namespace wms
}     // end namespace te

#endif  // __TERRALIB_WMS_QT_WMSLAYERITEM_H
