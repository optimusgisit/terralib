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

/*!
  \file terralib/qt/widgets/datasource/connector/wfs/WFSType.h

  \brief WFS data source type.
*/

#ifndef __TERRALIB_QT_WIDGETS_DATASOURCE_CONNECTOR_WFS_INTERNAL_WFSTYPE_H
#define __TERRALIB_QT_WIDGETS_DATASOURCE_CONNECTOR_WFS_INTERNAL_WFSTYPE_H

#include "Config.h"

// TerraLib
#include <terralib/qt/widgets/datasource/core/DataSourceType.h>

namespace qt_af
{
  namespace plugin
  {
    namespace wfs
    {
      class WFSType : public te::qt::widgets::DataSourceType
      {
        public:

          WFSType();

          ~WFSType();

          std::string getName() const;

          std::string getTitle() const;

          std::string getDescription() const;

          QWidget* getWidget(int widgetType, QWidget* parent = 0, Qt::WindowFlags f = 0) const;

          QIcon getIcon(int iconType) const;
      }; 
    } // end namespace wfs
  }   // end namespace plugin
}     // end namespace qt_af

#endif  // __TERRALIB_QT_WIDGETS_DATASOURCE_CONNECTOR_WFS_INTERNAL_WFSTYPE_H