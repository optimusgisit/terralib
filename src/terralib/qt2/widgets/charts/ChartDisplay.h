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
  \file  terralib/qt/widgets/charts/ChartDisplay.h

  \brief A class to represent a chart display
*/

#ifndef __TERRALIB_QT_WIDGETS_INTERNAL_CHARTDISPLAY_H
#define __TERRALIB_QT_WIDGETS_INTERNAL_CHARTDISPLAY_H

//TerraLib
#include "../Config.h"

//QWT
#include <qwt_plot.h>

//STL
#include <string>

class QwtPlotGrid;
class QwtPlotPicker;

namespace te
{
  namespace color { class RGBAColor; }
  namespace da    { class ObjectIdSet; }
  namespace se    { class Stroke; class Fill; class Font; }

  namespace qt
  {
    namespace widgets
    {

      //Forward declaration
      class ChartStyle;

      /*!
        \class ChartDisplay

        \brief A class to represent a chart display
      */
      class TEQTWIDGETSEXPORT ChartDisplay : public QwtPlot
      {

        Q_OBJECT

        public:

          /*!
            \brief Constructor

            It constructs a default chart display.
          */
          ChartDisplay(QWidget* parent=NULL, QString title = "");

          /*!
            \brief Destructor
          */
          ~ChartDisplay();

          te::qt::widgets::ChartStyle* getStyle();

          void setStyle(te::qt::widgets::ChartStyle* newStyle);

          /*!
            \brief Highlights the objects identified by \a oids

            \param oids The identifiers of plotitems to be highlighted.
          */
          void highlightOIds(const te::da::ObjectIdSet* oids);

          void adjustDisplay();

        protected slots:

          void onPointPicked(const QPointF &pos);

        signals:

          /*!
            \brief Emmit when objects were selected.
          */
          void selected(te::da::ObjectIdSet*, const bool&);

         private:

          te::qt::widgets::ChartStyle*  m_chartStyle;  //!< The display's style.
          QwtPlotGrid*                  m_grid;        //!< The display's grid
          QwtPlotPicker*                m_picker;       //!< The display's picker.
      };
    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_INTERNAL_CHARTDISPLAY_H
