/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of TerraView - A GIS Application.

    TerraView is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraView is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TerraLib Code Editor. See COPYING. If not, write to
    TerraLib Team at <terralib-team@dpi.inpe.br>.
 */

/*! 
  \file terralib/qt/af/connectors/MapDisplay.h

  \brief A connector of the te::qt::widgets::MapDisplay class to the Application Framework.
*/

#ifndef __TERRALIB_QT_AF_CONNECTORS_INTERNAL_MAPDISPLAY_H
#define __TERRALIB_QT_AF_CONNECTORS_INTERNAL_MAPDISPLAY_H

// Terralib
#ifndef Q_MOC_RUN
#include "../../../maptools/AbstractLayer.h"
#endif
#include "../Config.h"

// Qt
#include <QObject>
#include <QPoint>
#include <QString>
#include <QMap>
#include <QColor>
#include <QMenu>
#include <QPixmap>

class QAction;

namespace te
{
  namespace da
  {
    class DataSet;
  }

  namespace qt
  {
    namespace widgets
    {
      class EyeBirdMapDisplayWidget;
      class MapDisplay;
      class ZoomInMapDisplayWidget;
    }

    namespace af
    {
      class ApplicationController;

      namespace evt
      {
        // Forward declaration
        struct Event;
      }

      /*!
        \class MapDisplay
        
        \brief A connector of the te::qt::widgets::MapDisplay class to the Application Framework.

        It is used to listen events sent by the application framework.
        It is a proxy for the te::qt::widgets::MapDisplay.
      */
      class TEQTAFEXPORT MapDisplay : public QObject
      {
        Q_OBJECT

        public:

          /*!
            \brief Constructor.

            \param display te::qt::widgets::MapDisplay to be listened.
          */
          MapDisplay(te::qt::widgets::MapDisplay* display, te::qt::af::ApplicationController* app);

          /*! \brief destructor. */
          ~MapDisplay();

          te::qt::widgets::MapDisplay* getDisplay();

          void setZoomInDisplay(te::qt::widgets::ZoomInMapDisplayWidget* display);

          void setEyeBirdDisplay(te::qt::widgets::EyeBirdMapDisplayWidget* display);

          bool eventFilter(QObject* watched, QEvent* e);

          /*!
            \brief It draws the given layer list.

            \param layers The layer list.
          */
          void draw(const std::list<te::map::AbstractLayerPtr>& layers);

          /*! \brief It clears the map display. */
          void clear();

          void nextExtent();

          void previousExtent();

          void fit(const std::list<te::map::AbstractLayerPtr>& layers);

        protected slots:

          void onCoordTracked(QPointF& coordinate);

          void onDrawLayersFinished(const QMap<QString, QString>& errors);

          void onExtentChanged();

          /*!
            \brief Listener to the application framewrork events.

            \param e An event sent by framework.
          */
          void onApplicationTriggered(te::qt::af::evt::Event* e);

          /*!
           \brief Returns the pan selected flag;

           \return Flag of the pan to selected operation.
          */
          bool isPanToSelectedEnabled();

        protected:

          void drawLayersSelection(const std::list<te::map::AbstractLayerPtr>& layers);

          void drawLayerSelection(te::map::AbstractLayerPtr layer);

          void drawDataSet(te::da::DataSet* dataset, const std::string& geomPropertyName, int srid, const QColor& color, bool isLinked = false);

          void drawDataSet(te::da::DataSet* dataset, const std::string& geomPropertyName, int srid, te::se::Style* style, bool isLinked = false);

          void configSRS(const std::list<te::map::AbstractLayerPtr>& layers);

          std::list<te::map::AbstractLayerPtr> getSelectedLayer();

        signals:

          void hasNextExtent(bool value);

          void hasPreviousExtent(bool value);

          void triggered(te::qt::af::evt::Event* e);

        protected:

          std::list<te::map::AbstractLayerPtr> getVisibleLayers();


          te::qt::af::ApplicationController* m_app;                  //!< Pointer to applicatin controller;
          te::qt::widgets::MapDisplay* m_display;                    //!< Pointer to a component te::qt::widgets::MapDisplay.
          QMenu* m_menu;                                             //!< The map display popup menu.
          QPixmap m_lastDisplayContent;                              //!< The last map display content. i.e. a "clean" pixmap.
          std::vector<te::gm::Envelope> m_extentStack;               //!< The stack of MapDisplay extents.
          int m_currentExtentIndex;                                  //!< The current extent index.
          int m_extentStackMaxSize;                                  //!< The max size of the extent stack. Used on previousExtent and nextExtent.

          te::qt::widgets::ZoomInMapDisplayWidget* m_zoomInDisplay;   //!< Pointer to a component that represents a zoom in display.
          te::qt::widgets::EyeBirdMapDisplayWidget* m_eyeBirdDisplay; //!< Pointer to a component that represents a eye bird display.

          QAction* m_pantoSelectedAction;                             //!< Action to enable / disable pan to selected operation.
      };
    }
  }
}

#endif // __TERRALIB_QT_AF_CONNECTORS_INTERNAL_MAPDISPLAY_H
