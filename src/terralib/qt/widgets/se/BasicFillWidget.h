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
  \file terralib/qt/widgets/se/BasicFillWidget.h

  \brief A widget used to build a basic fill element.
*/

#ifndef __TERRALIB_QT_WIDGETS_SE_INTERNAL_BASICFILLWIDGET_H
#define __TERRALIB_QT_WIDGETS_SE_INTERNAL_BASICFILLWIDGET_H

// TerraLib
#include "../Config.h"

// Qt
#include <QtGui/QWidget>

// STL
#include <memory>

// Forward declaraion
namespace Ui { class BasicFillWidgetForm; }

namespace te
{
// Forward declarations
  namespace se
  {
    class Fill;
  }

  namespace qt
  {
    namespace widgets
    {
      /*!
        \class BasicFillWidget

        \brief A widget used to build a basic fill element.
      */
      class TEQTWIDGETSEXPORT BasicFillWidget : public QWidget
      {
        Q_OBJECT

        public:

          BasicFillWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

          ~BasicFillWidget();

        public:

          void setFill(const te::se::Fill* fill);

          te::se::Fill* getFill() const;

        protected:

          void initialize();

          void updateUi();

          void updateUiFillColor();

        protected slots:

          void onFillColorPushButtonClicked();

          void onFillOpacitySliderValueChanged(int value);

        signals:

          void fillChanged();

        private:

          std::auto_ptr<Ui::BasicFillWidgetForm> m_ui;

          QColor m_color;

          te::se::Fill* m_fill;
      };

    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_SE_INTERNAL_BASICFILLWIDGET_H
