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
\file terralib/mnt/qt/TINGenerationDialog.h

\brief A dialog TIN generation
*/
#ifndef __TERRALIB_MNT_INTERNAL_TINGENERATIONDIALOG_H
#define __TERRALIB_MNT_INTERNAL_TINGENERATIONDIALOG_H

// Terralib
#ifndef Q_MOC_RUN
#include "../../common/UnitOfMeasure.h"
#include "../../dataaccess/datasource/DataSource.h"
#include "../../dataaccess/datasource/DataSourceInfo.h"
#include "../../maptools/AbstractLayer.h"
#endif
#include "../core/Config.h"

// STL
#include <list>
#include <map>
#include <memory>

// Qt
#include <QDialog>

namespace Ui { class TINGenerationDialogForm; }

namespace te
{
  namespace mnt
  {
    class TEMNTEXPORT TINGenerationDialog : public QDialog
    {
      Q_OBJECT

    public:
      TINGenerationDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

      ~TINGenerationDialog();

      /*!
      \brief Set the layer that can be used
      \param layers   List of AbstractLayerPtr
      */
      void setLayers(std::list<te::map::AbstractLayerPtr> layers);

      te::map::AbstractLayerPtr getLayer();

      void setSRID(int newSRID);

      protected slots:

      void onInputIsolinesToolButtonClicked();
      void onIsolinesComboBoxChanged(int index);
      void onInputSamplesToolButtonClicked();
      void onSamplesComboBoxChanged(int index);
      void onScalePushButtonClicked();
      void onYesToggled();
      void onNoToggled();
      void onBreakLinesComboBoxChanged(int index);
      void onInputBreaklineToolButtonClicked();
      void onTargetDatasourceToolButtonPressed();
      void onTargetFileToolButtonPressed();
      void onHelpPushButtonClicked();
      void onOkPushButtonClicked();
      void onCancelPushButtonClicked();
      void onSrsToolButtonClicked();

    private:

      std::auto_ptr<Ui::TINGenerationDialogForm> m_ui;

      te::da::DataSourceInfoPtr m_outputDatasource;                                     //!< DataSource information.
      std::string m_outputArchive;                                                      //!< Archive information.
      bool m_toFile;
      std::list<te::map::AbstractLayerPtr> m_layers;                                    //!< List of layers.
      te::map::AbstractLayerPtr m_isolinesLayer;                                        //!< Isolines layer
      te::map::AbstractLayerPtr m_samplesLayer;                                        //!< Points layer
      te::map::AbstractLayerPtr m_breaklinesLayer;                                        //!< BreakLines layer
      te::map::AbstractLayerPtr m_outputLayer;                                          //!< Generated Layer.
      te::da::DataSourcePtr     m_isolinesDataSource;   //!< Isolines DataSourcePtr
      te::da::DataSourcePtr     m_samplesDataSource;   //!< Samples DataSourcePtr
      te::da::DataSourcePtr     m_breakDataSource;   //!< BreakLines DataSourcePtr
      std::string m_isoSetName;  //!< Isolines DataSetLayer name
      std::string m_sampleSetName;  //!< Samples DataSetLayer name
      std::string m_breakSetName;  //!< BreakLines DataSetLayer name
      double m_scale;         //!<Triangulation scale.
      double m_tol;           //!<Triangulation lines simplification tolerance.
      double m_breaktol;      //!<Triangulation breaklines simplification tolerance.
      double m_distance;      //!<Triangulation lines simplification maximum distance.
      double m_edgeSize;      //!<Triangulation edges minimum size.

      int m_isosrid;
      int m_samplesrid;
      int m_outsrid;
    };
  }
}

#endif
