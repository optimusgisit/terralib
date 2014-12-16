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
  \file terralib/addressgeocoding/qt/MainwindowDialog.h

  \brief A main window to address geocoding operation
*/

#ifndef __TERRALIB_ADDRESSGEOCODING_INTERNAL_MAINWINDOWDIALOG_H
#define __TERRALIB_ADDRESSGEOCODING_INTERNAL_MAINWINDOWDIALOG_H

// TerraLib
#include "../../dataaccess/datasource/DataSource.h"
#include "../../dataaccess/datasource/DataSourceInfo.h"
#include "../../datatype/Property.h"
#include "../../maptools/AbstractLayer.h"
#include "../Config.h"

// STL
#include <list>
#include <map>
#include <memory>

// Qt
#include <QDialog>

namespace Ui { class MainWindowDialogForm; }

// Forward declarations
class QModelIndex;

namespace te
{
  namespace addressgeocoding
  {
    class TEADDRESSGEOCODINGEXPORT MainWindowDialog : public QDialog
    {
      Q_OBJECT

      public:

        MainWindowDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

        ~MainWindowDialog();

        /*!
          \brief Set the layer that can be used

          \param layers   List of AbstractLayerPtr
        */
        void setLayers(std::list<te::map::AbstractLayerPtr> layers);

        /*!
          \brief Get the generated layer.

          \return the generated layer.
        */
        te::map::AbstractLayerPtr getLayer();

      protected slots:

        void onLayerComboBoxChanged(int index);

				void onConfigureLayerClicked();

				void onConfigureAddressClicked();

        void onTargetDatasourceToolButtonPressed();

        void onTargetFileToolButtonPressed();

        void onHelpPushButtonClicked();

        void onOkPushButtonClicked();

        void onCancelPushButtonClicked();

      private:

        std::auto_ptr<Ui::MainWindowDialogForm> m_ui;
        std::list<te::map::AbstractLayerPtr> m_layers;                //!< List of layers.
        te::map::AbstractLayerPtr m_selectedLayer;                    //!< List of layers.
        te::da::DataSourcePtr m_layerDataSource;
        te::map::AbstractLayerPtr m_newColumnLayer;                   //!< Layer used for tsVector column.
        std::vector<te::dt::Property*> m_properties;                  //!< Properties related to the selected Layer
        te::da::DataSourceInfoPtr m_outputDatasource;                 //!< DataSource information.
        te::map::AbstractLayerPtr m_resultLayer;                      //!< Generated Layer.

        te::da::DataSourcePtr m_addressDataSource;
        std::string m_addressFile;
        std::string m_streetType;
        std::string m_streetTitle;
        std::string m_streetName;
        std::string m_streetNumber;
        std::string m_streetNeighborhood;
        std::string m_streetPostalCode;

    };
  }   // end namespace addressgeocoding
}     // end namespace te

#endif  // __TERRALIB_ADDRESSGEOCODING_INTERNAL_MAINWINDOWDIALOG_H