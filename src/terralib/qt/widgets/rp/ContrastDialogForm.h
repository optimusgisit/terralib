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
  \file terralib/qt/widgets/rp/ContrastDialogForm.h

  \brief This file defines a class for a Contrast dialog.
*/

#ifndef __TERRALIB_QT_WIDGETS_RP_INTERNAL_CONTRASTDIALOGFORM_H
#define __TERRALIB_QT_WIDGETS_RP_INTERNAL_CONTRASTDIALOGFORM_H

// TerraLib
#ifndef Q_MOC_RUN
#include "../../../maptools/AbstractLayer.h"
#endif
#include "../../../rp/Contrast.h"
#include "../../../se/ChannelSelection.h"
#include "../../../qt/widgets/canvas/MapDisplay.h"
#include "../Config.h"
#include "OutputRasterWidget.h"

// STL
#include <memory>

// Qt
#include <qcombobox.h>
#include <QDialog>

// Forward declaration
namespace Ui { class ContrastDialogForm; }

namespace te
{
  namespace qt
  {
    namespace af
    {
      namespace evt
      {
        struct Event;
      }
    }

    namespace widgets
    {
      class RasterHistogramWidget;
      class RasterNavigatorWidget;

      /*!
        \class ContrastDialogForm

        \brief This class is GUI used to define the contrast parameters for the RP constast operation.
      */
      class TEQTWIDGETSEXPORT ContrastDialogForm : public QDialog
      {
        Q_OBJECT

        public:

          ContrastDialogForm(QWidget* parent = 0);

          ~ContrastDialogForm();

        public:

          void resetWindow(te::map::AbstractLayerPtr layer);

          /*!
            \brief This method is used to set the selected layer for contrast operation
            
            \param layer The layer ptr 

            \note This layer MUST HAVE a valid raster object.
          */
          void set(te::map::AbstractLayerPtr layer);

          void setConfigurations();

          void setMapDisplay(te::qt::widgets::MapDisplay* mapDisplay);

          te::map::AbstractLayerPtr get();

          te::rp::Contrast::InputParameters getInputParams();

          te::rp::Contrast::OutputParameters getOutputParams();

          te::map::AbstractLayerPtr getOutputLayer();

        protected:

          void fillContrastTypes();

          te::se::ChannelSelection* getChannelSelection();

          void listBands();

          void applyPreview();

          void drawPreview(te::rst::Raster* raster);

          void getRasterFromROI();

          bool execute();

          void closeEvent(QCloseEvent* e);

        protected slots:

          void apply();

          void onAllImageRadioButtonToggled(bool isChecked);

          void onBlueComboBoxCurrentIndexChanged(int index);

          void onContrastTypeComboBoxActivated(int index);

          void onCellClicked(int row, int column);

          void onCellDoubleClicked(int row, int column);

          void onCellChanged(int row, int column);

          void onGreenComboBoxCurrentIndexChanged(int index);

          void onMinValueSelected(int value, int band);

          void onMinValueSelected(double value, int band);

          void onMaxValueSelected(int value, int band);

          void onMaxValueSelected(double value, int band);

          void onMonoComboBoxCurrentIndexChanged(int index);

          void onNewROIPushButtonClicked();

          void onHistogramCheckBoxToggled(bool isChecked);

          void onOkPushButtonClicked();

          void onPreviewCheckBoxToggled(bool isChecked);

          void onRedComboBoxCurrentIndexChanged(int index);

          void onRoiRadioButtonToggled(bool isChecked);

          void onVisibleAreaRadioButtonToggled(bool isChecked);

          void drawHistogram();

          void onApplicationTriggered(te::qt::af::evt::Event* e);

          void onMapDisplayExtentChanged();

          void onEnvelopeAcquired(te::gm::Envelope env);

        signals:

          void geomAquired(te::gm::Polygon* poly);

          void addLayer(te::map::AbstractLayerPtr layer);

          void closeTool();

        private:

          std::unique_ptr<Ui::ContrastDialogForm> m_ui;
          std::unique_ptr<te::qt::widgets::RasterNavigatorWidget> m_navigator;
          std::unique_ptr<te::qt::widgets::RasterHistogramWidget> m_histogramWidget;

          QWidget* m_parent;
          te::map::AbstractLayerPtr m_layer;
          te::map::AbstractLayerPtr m_outputLayer;
          te::qt::widgets::MapDisplay* m_mapDisplay;
          te::qt::widgets::Canvas* m_canvas;
          te::rst::Raster* m_raster;
          te::rst::Raster* m_previewRaster;
          te::gm::Polygon* m_polygon;
          te::gm::Geometry* m_geom;
          te::se::ChannelSelection* m_channelSelect;
          std::vector<double> m_minValue;
          std::vector<double> m_maxValue;
          te::qt::widgets::OutputRasterWidget* m_outputWidget;
          std::string m_lastText;
          bool m_contrast;

          QComboBox* m_monoComboBox;
          QComboBox* m_redComboBox;
          QComboBox* m_greenComboBox;
          QComboBox* m_blueComboBox;
      };

    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_RP_INTERNAL_CONTRASTDIALOGFORM_H
