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
  \file terralib/qt/widgets/rp/RpToolsWidget.h

  \brief This file has the RpToolsWidget class.
*/

#ifndef __TERRALIB_QT_WIDGETS_RP_INTERNAL_ROTOOLSWIDGET_H
#define __TERRALIB_QT_WIDGETS_RP_INTERNAL_ROTOOLSWIDGET_H

// TerraLib
#include "../../../geometry/Envelope.h"
#ifndef Q_MOC_RUN
#include "../../../geometry/Polygon.h"
#include "../../../maptools/AbstractLayer.h"
#endif
#include "../../../maptools/Enums.h"
#include "../Config.h"

// STL
#include <memory>

// Qt
#include <QComboBox>
#include <QWidget>
#include <qactiongroup.h>

namespace Ui { class RpToolsWidgetForm; }

namespace te
{
  namespace gm  { class Envelope; }
  namespace rst { class Raster; }
  namespace se  { class RasterSymbolizer; }

  namespace qt
  {
    namespace widgets
    {
      class AbstractTool;
      class EyeBirdMapDisplayWidget;
      class MapDisplay;

      /*!
        \class RpToolsWidget

        \brief This class is used to navigate over a DataSetLayer (having a raster representation)
           and given a set of tools, such as, zoom in, zoom out, pan, recompose.
        Two new tools as created for raster interaction:
        - point clicked
        - geom definition

        \sa RasterFactory
      */
      class TEQTWIDGETSEXPORT RpToolsWidget : public QWidget
      {
        Q_OBJECT

        public:

          RpToolsWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

          ~RpToolsWidget();

        public:

          /*!
            \brief This method is used to set the selected layer
            
            \param layer The layer ptr 

            \note This layer MUST HAVE a valid raster object.
          */
          void set(te::map::AbstractLayerPtr layer, bool setFullScaleBox = false);

          void setVectorial(te::map::AbstractLayerPtr layer);

          void setExtent(te::gm::Envelope env);

          void removeVectorial();

          te::gm::Envelope getCurrentExtent();

          void setMapDisplay(te::qt::widgets::MapDisplay* mapDisplay);

          void setActionGroup(QActionGroup* actionGroup);

          te::rst::Raster* getExtentRaster(bool fullScale = false);

          void drawRaster(te::rst::Raster* rst, te::se::Style* style = 0);

          void hideEditionTools(bool hide);

          void hideUndoTool(bool hide);

          void hideRedoTool(bool hide);

          void hidePickerTool(bool hide);

          void hideGeomTool(bool hide);

          void hideBoxTool(bool hide);

          void hideInfoTool(bool hide);

          void enableUndoAction();

          void enableRedoAction();

          void enablePickerAction();

          void enableGeomAction();

          void enableBoxAction();

          void enableInfoAction();

          void setSelectionMode(bool mode);

          void setPointPickedTool(bool checked);

          protected slots:

          void onEnvelopeAcquired(te::gm::Envelope env);

          void onGeomAquired(te::gm::Polygon* poly);

          void onPointPicked(QPointF& point);

          void onMapDisplayExtentChanged();

          void onUndoToggled(bool checked);

          void onRedoToggled(bool checked);

          void onPointPickerToggled(bool checked);
          
          void onGeomToggled(bool checked);

          void onBoxToggled(bool checked);

          void onReadPixelToggled(bool checked);

          void onRecomposeClicked();

          void onPreviewClicked();

          void onVSliderChanged(int value);

          void onHSliderChanged(int value);

        signals:

          void previewClicked();

          void mapDisplayExtentChanged();

          void pointPicked(double x, double y);

          void geomAquired(te::gm::Polygon* poly);

          void envelopeAcquired(te::gm::Envelope env);

        protected:

          void setComboBoxText(QComboBox* cb, std::string value);

          void drawOverlay();

        private:

          std::auto_ptr<Ui::RpToolsWidgetForm> m_ui;

          te::map::AbstractLayerPtr m_layer;
          te::se::RasterSymbolizer* m_symbolizer;

          te::qt::widgets::MapDisplay* m_mapDisplay;

          QActionGroup* m_actionGroup;

          int m_currentColumn;                                       //!< The column position of mouse in map display.
          int m_currentRow;                                          //!< The row position of mouse in map display.

          bool m_checkedTool;

          te::map::Visibility m_visibility;

          QCursor m_pointCursor;

          QPixmap* m_draftOriginal;
      };

    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_RP_INTERNAL_ROTOOLSWIDGET_H

