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
  \file terralib/qt/widgets/se/StyleControllerWidget.h

  \brief A widget used to controll the style se element.
*/

#ifndef __TERRALIB_QT_WIDGETS_SE_INTERNAL_STYLECONTROLLERWIDGET_H
#define __TERRALIB_QT_WIDGETS_SE_INTERNAL_STYLECONTROLLERWIDGET_H

// TerraLib
#include "../../../maptools/AbstractLayer.h"
#include "../Config.h"

// Qt
#include <QWidget>

// STL
#include <memory>
#include <string>
#include <vector>

// Forward declaraion
namespace Ui { class StyleControllerWidgetForm; }

namespace te
{
// Forward declarations
  namespace se
  {
    class Style;
    class Symbolizer;
    class Rule;
  }

  namespace qt
  {
    namespace widgets
    {
// Forward declarations
      class StyleExplorer;

      /*!
        \class StyleControllerWidget

        \brief A widget used to controll the style se element.
      */
      class TEQTWIDGETSEXPORT StyleControllerWidget : public QWidget
      {
        Q_OBJECT

        public:

          /** @name Initializer Methods
           *  Methods related to instantiation and destruction.
           */
          //@{

          /*! \brief Constructs a StyleControllerWidget which is a child of parent, with widget flags set to f. */
          StyleControllerWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

          /*! \brief Destructor. */
          ~StyleControllerWidget();

          //@}

        public:

          /*!
            \brief Sets a style element to this widget.

            \param style A valid style element.

            \note The widget will NOT take the ownership of the given style.
            \note The widget will be update based on given style parameters.
          */
          void setLayer(te::map::AbstractLayer* layer, std::string selColor);

          StyleExplorer* getStyleExplorer() const;

          void writeStyle(const te::se::Style* style, std::string path);

          void readStyle(std::string path);

        private:

          bool checkSymbolizer(std::string file);

        protected:

          /*! \brief Updates the widget form based on internal mark element. */
          void updateUi();

        protected slots:

          void onAddTextSymbolizerClicked();

          void onAddSymbolizerClicked();

          void onRemoveSymbolizerClicked();

          void onUpSymbolizerClicked();

          void onDownSymbolizerClicked();

          void onLibraryManagerClicked();

          void onExportClicked();

          void onImportClicked();

          void onMapRefreshClicked();

          void changeLegendIconSize(int size);

          void onVisualStyleChecked(bool state);

          void onSelectionStyleChecked(bool state);

        signals:

          void mapRefresh();

        private:

          std::auto_ptr<Ui::StyleControllerWidgetForm> m_ui; //!< Widget form.
          te::qt::widgets::StyleExplorer* m_explorer;        //!< A style explorer used to explore the style.
          te::se::Style* m_currentStyle;                     //!< current style.

          te::map::AbstractLayer* m_currentLayer;            //!< Current Layer pointer (used for raster symbolizer information)
      }; 

    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_SE_INTERNAL_STYLECONTROLLERWIDGET_H
