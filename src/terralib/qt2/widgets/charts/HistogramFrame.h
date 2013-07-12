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
  \file terralib/qt/widgets/charts/HistogramFrame.h

  \brief A frame used to adjust a histogram's chart parameters, weather it is about it's data or it's visual style.
*/

#ifndef __TERRALIB_QT_WIDGETS_INTERNAL_HISTOGRAMFRAME_H
#define __TERRALIB_QT_WIDGETS_INTERNAL_HISTOGRAMFRAME_H

// TerraLib
#include "../Config.h"
#include "ChartWidget.h"

#include <memory>

class QwtPlotSeriesItem;

namespace Ui { class HistogramFrameWidgetForm; }

namespace te
{
  namespace qt
  {
    namespace widgets
      {
      //Forward declarations
      class HistogramChart;
      class HistogramStyleWidget;
      /*!
        \class HistogramFrame

        \brief A frame for setting display options.
      */
      class TEQTWIDGETSEXPORT HistogramFrame : public ChartWidget
      {
        Q_OBJECT

        public:

          HistogramFrame(QWidget* parent = 0);

          ~HistogramFrame();

          QwtPlotSeriesItem* getChart();

          void setChart(QwtPlotSeriesItem* newChart);

        private:

          std::auto_ptr<Ui::HistogramFrameWidgetForm>  m_ui;           //!< The widget Form.
          te::qt::widgets::HistogramChart*             m_chart;        //!< The chart that will be editted by this widget.
          te::qt::widgets::HistogramStyleWidget*       m_styleWidget;  //!< The widget used to configure the histogram's style.
      };
    }   // end namespace widgets
  }     // end namespace qt
}       // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_INTERNAL_HISTOGRAMFRAME_H

