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
  \file terralib/qt/widgets/connector/DataSourceInfo2.h

  \brief A widget that groups datasource identificatin and connection information.
*/

#ifndef __TERRALIB_QT_WIDGETS_CONNECTOR_INTERNAL_DATASOURCEINFO2_H
#define __TERRALIB_QT_WIDGETS_CONNECTOR_INTERNAL_DATASOURCEINFO2_H

// TerraLib
#include "../../Config.h"

// STL
#include <map>
#include <string>

// Qt
#include <QtGui/QWidget>

namespace te
{
  namespace qt
  {
    namespace widgets
    {
      /*!
        \class DataSourceInfo2

        \brief A widget that groups datasource identificatin and connection information.
      */
      class TEQTWIDGETSEXPORT DataSourceInfo2 : public QWidget
      {
        public:

          DataSourceInfo2(QWidget* parent = 0, Qt::WindowFlags f = 0);

          ~DataSourceInfo2();

      }; 
    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_CONNECTOR_INTERNAL_DATASOURCEINFO2_H
