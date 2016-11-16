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
  \file terralib/qt/widgets/se/StyleDockWidget.h

  \brief A dock widget used control the geographic data style using SE elements and a property browser to show its properties.
*/

#ifndef __TERRALIB_QT_WIDGETS_SE_INTERNAL_STYLEDOCKWIDGET_H
#define __TERRALIB_QT_WIDGETS_SE_INTERNAL_STYLEDOCKWIDGET_H

// TerraLib
#include "../../../se.h"
#include "../Config.h"

// Qt
#include <QDockWidget>
#include <QTabWidget>

namespace te
{
  namespace map {class AbstractLayer; }

  namespace se { class Symbolizer; }

  namespace qt
  {
    namespace widgets
    {
      // Forward declarations
      class RuleProperty;
      class PolygonSymbolizerProperty;
      class LineSymbolizerProperty;
      class PointSymbolizerProperty;
      class TextSymbolizerProperty;
      class RasterSymbolizerWidget;
      class StyleControllerWidget;

      /*!
        \class StyleDockWidget

        \brief A dock widget used control the geographic data style using SE elements and a property browser to show its properties.
      */
      class TEQTWIDGETSEXPORT StyleDockWidget : public QDockWidget
      {
        Q_OBJECT

        public:

          /** @name Initializer Methods
           *  Methods related to instantiation and destruction.
           */
          //@{

          /*! \brief Constructs a StyleDockWidget. */
          StyleDockWidget(QWidget* parent = 0, Qt::WindowFlags flags = 0);

          /*! \brief Destructor. */
          ~StyleDockWidget();

          //@}

        public:

          /*!
            \brief Sets a style element to this widget.

            \param style A valid style element.
            \param layer A valid layer element.

            \note The widget will NOT take the ownership of the given pointers.
          */
          void setLayer(te::map::AbstractLayer* layer, std::string selColor);

          void setTabStatus(bool status);

        protected:

          void updateUi();

          QWidget* buildUi();

        protected slots:  

          void onRuleSelected(te::se::Rule* r);

          void onSymbolizerSelected(te::se::Symbolizer* s);

          void onStyleImported(te::se::Style* style, bool isVisual);

          void onPolygonSymbolizerChanged();
          void onLineSymbolizerChanged();
          void onPointSymbolizerChanged();
          void onRasterSymbolizerChanged();
          void onTextSymbolizerChanged();
          void onMapRefresh();

        signals:

          void symbolChanged(te::map::AbstractLayer* l);

          void symbolizerChanged(te::se::Symbolizer* s);

          void repaintMapDisplay();

        private:

          QTabWidget* m_tabWidget;

          te::qt::widgets::RuleProperty* m_ruleWidget;
          te::qt::widgets::PolygonSymbolizerProperty* m_polyWidget;
          te::qt::widgets::LineSymbolizerProperty* m_lineWidget;
          te::qt::widgets::PointSymbolizerProperty* m_pointWidget;
          te::qt::widgets::RasterSymbolizerWidget* m_rasterWidget;
          te::qt::widgets::StyleControllerWidget* m_styleController;
          te::qt::widgets::TextSymbolizerProperty* m_textWidget;

          te::map::AbstractLayer* m_currentLayer;                     //!< Current Layer pointer (used for raster symbolizer information)
          std::string m_selColor;
      };

    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_SE_INTERNAL_STYLEDOCKWIDGET_H
