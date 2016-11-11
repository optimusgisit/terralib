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
  \file terralib/wms/qt/WMSLayerSelector.h

  \brief A simple widget that allows the selection of datasets from a given WMS server.
*/

#ifndef __TERRALIB_WMS_QT_WMSLAYERSELECTOR_H
#define __TERRALIB_WMS_QT_WMSLAYERSELECTOR_H

// TerraLib
#include "../../dataaccess/datasource/DataSourceInfo.h"
#include "../../qt/widgets/layer/selector/AbstractLayerSelector.h"
#include "../Config.h"

namespace te
{
  namespace wms
  {
    /*!
      \class WMSLayerSelector

      \brief A simple widget that allows the selection of datasets from a given WMS server.
    */
    class TEWMSEXPORT WMSLayerSelector : public te::qt::widgets::AbstractLayerSelector
    {
      public:

        WMSLayerSelector(QWidget* parent = 0, Qt::WindowFlags f = 0);

        ~WMSLayerSelector();

        void set(const std::list<te::da::DataSourceInfoPtr>& datasources);

        std::list<te::map::AbstractLayerPtr> getLayers();

      private:

        std::list<te::da::DataSourceInfoPtr> m_datasources;
    };

  }   // end namespace wms
}     // end namespace te

#endif  // __TERRALIB_WMS_QT_WMSLAYERSELECTOR_H
