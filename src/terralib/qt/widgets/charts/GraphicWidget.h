/*  Copyright (C) 2010-2013 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/qt/widgets/charts/GraphicWidget.h

  \brief A base widget to be used on the graphic settings.
*/

#ifndef __TERRALIB_QT_WIDGETS_INTERNAL_GRAPHICWIDGET_H
#define __TERRALIB_QT_WIDGETS_INTERNAL_GRAPHICWIDGET_H

// QT
#include <QtGui/QWidget>

// TerraLib
#include "../Config.h"

namespace te
{
  namespace qt
  {
    namespace widgets
      {
      /*!
        \class GraphicWidget

        \brief A base widget for settings.
      */
      class TEQTWIDGETSEXPORT GraphicWidget : public QWidget
      {
        public:

          GraphicWidget(QWidget* parent = 0);

          ~GraphicWidget();

        public:

          std::string m_label;

      };
    }   // end namespace widgets
  }     // end namespace qt
}       // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_INTERNAL_GRAPHICWIDGET_H
