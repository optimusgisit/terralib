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
  \file terralib/vp/qt/UnionDialog.h

  \brief Union operation dialog.
*/

#ifndef __TERRALIB_VP_QT_INTERNAL_UNIONDIALOG_H
#define __TERRALIB_VP_QT_INTERNAL_UNIONDIALOG_H

// TerraLib
#include "../../dataaccess/datasource/DataSourceInfo.h"
#include "../../maptools/AbstractLayer.h"
#include "../AlgorithmParams.h"
#include "../Config.h"

// STL
#include <memory>

// Qt
#include <QDialog>

namespace Ui { class UnionDialogForm; }

namespace te
{
  namespace qt
  {
    namespace widgets
    {
      class DoubleListWidget;
    }
  }

  namespace vp
  {
    /*!
      \class UnionDialog

      \brief A dialog used to execute vector union.
    */
    class TEVPEXPORT UnionDialog : public QDialog
    {
      Q_OBJECT

      public:

        UnionDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

        ~UnionDialog();

        struct DataStruct
        {
          DataStruct()
          {
            m_dataSetType = 0;
            m_dataSet = 0;
          }

          te::da::DataSetType* m_dataSetType;
          te::da::DataSet* m_dataSet;
        };

        /*!
          \brief Set the layer that can be used

          \param layers   List of AbstractLayerPtr
        */
        void setLayers(std::list<te::map::AbstractLayerPtr> layers);

        te::map::AbstractLayerPtr getLayer();

      protected:
       te::da::Select* getSelectQueryFromLayer(te::map::AbstractLayerPtr layer,
                                               bool onlySelectedObjects,
                                               int srid = 0);

       DataStruct getDataStructFromLayer(te::map::AbstractLayerPtr layer,
                                         bool onlySelectedObjects,
                                         int srid = 0);

       std::vector<std::pair<std::string, std::string> >
       getSelectedProperties();

        void updateFirstLayerComboBox();

        void updateSecondLayerComboBox();

        void updateDoubleListWidget();

      protected slots:

        void onFirstLayerComboBoxChanged(int index);

        void onSecondLayerComboBoxChanged(int index);

        void onTargetDatasourceToolButtonPressed();

        void onTargetFileToolButtonPressed();

        void onOkPushButtonClicked();
        
        void onCancelPushButtonClicked();

      private:

        std::auto_ptr<Ui::UnionDialogForm> m_ui;
        std::list<te::map::AbstractLayerPtr> m_layers;    //!< The vector layers in Layer Explorer.
        te::map::AbstractLayerPtr m_firstSelectedLayer;   //!< Input layer selected.
        te::map::AbstractLayerPtr m_secondSelectedLayer;  //!< Union layer selected.

        std::vector<te::vp::InputParams> m_inputParams;   //!< A vector of input parameters.
        te::vp::AlgorithmParams* m_params;                //!< Algorithm parameters.

        te::da::DataSourceInfoPtr m_outputDatasource;     //!< DataSource information.
        te::map::AbstractLayerPtr m_outputLayer;          //!< Generated Layer.
        
        bool m_toFile;                                    //!< The result is in a file?

        std::unique_ptr<te::qt::widgets::DoubleListWidget> m_doubleListWidget;
    };
  }   // end namespace vp
}     // end namespace te

#endif  // __TERRALIB_VP_QT_INTERNAL_UNIONDIALOG_H
