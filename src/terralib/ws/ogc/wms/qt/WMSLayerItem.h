/*  Copyright (C) 2016 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/ws/ogc/wms/qt/WMSLayerItem.h

  \brief QTreeWidgetItem extension to WMS Layers.

  \author Emerson Moraes
*/

#ifndef __TERRALIB_WS_OGC_WMS_QT_WMSLAYERITEM_H
#define __TERRALIB_WS_OGC_WMS_QT_WMSLAYERITEM_H

#include "Config.h"


//TerraLib
#include "../client/DataTypes.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace te
{
  namespace ws
  {
    namespace ogc
    {
      namespace wms
      {

        namespace qt
        {

        /*!
         * \brief The WMSLayerItem class is an extension of QTreeWidgetItem
         * to handle WMS Layer creation.
         */
        class WMSLayerItem : public QTreeWidgetItem
        {
        public:

          WMSLayerItem(const te::ws::ogc::wms::Layer& rootLayer, QTreeWidget* parent);

          WMSLayerItem(const te::ws::ogc::wms::Layer& layer);

          ~WMSLayerItem();

          const te::ws::ogc::wms::Layer& getLayer() const;

          const te::ws::ogc::wms::WMSGetMapRequest& getRequest() const;

          void setRequest(const te::ws::ogc::wms::WMSGetMapRequest& request);

        private:

          void buildDefaultRequest();

        private:

          te::ws::ogc::wms::Layer            m_layer;

          te::ws::ogc::wms::WMSGetMapRequest m_request;
        };

        }
      }
    }
  }
}

#endif // __TERRALIB_WS_OGC_WMS_QT_WMSLAYERITEM_H
