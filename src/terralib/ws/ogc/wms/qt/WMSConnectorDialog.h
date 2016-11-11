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
  \file terralib/ws/ogc/wms/qt/WMSConnectorDialog.h

  \brief A dialog window for showing the WMS connector widget.
*/

#ifndef __TERRALIB_WS_OGC_WMS_QT_WMSCONNECTORDIALOG_H
#define __TERRALIB_WS_OGC_WMS_QT_WMSCONNECTORDIALOG_H

//TerraLib
#include "Config.h"
#include "../../../../qt/widgets/datasource/connector/AbstractDataSourceConnector.h"

// TerraLib
#ifndef Q_MOC_RUN
#include "../../../../dataaccess/datasource/DataSource.h"
#include "../../../../dataaccess/datasource/DataSourceInfo.h"
#endif

//Qt
#include <QDialog>

namespace Ui { class WMSConnectorDialogForm; }

namespace te
{
  namespace ws
  {
    namespace ogc
    {
      namespace wms
      {
        namespace qt
        {
          /*!
            \class WMSConnectorDialog

            \brief A dialog window for showing the WMS connector widget.
          */
          class TEOGCWMSQTEXPORT WMSConnectorDialog : public QDialog
          {
            Q_OBJECT
          public:

            WMSConnectorDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

            ~WMSConnectorDialog();

            const te::da::DataSourceInfoPtr& getDataSource() const;

            const te::da::DataSourcePtr& getDriver() const;

            void set(const te::da::DataSourceInfoPtr& ds);

          public slots:

            void openPushButtonPressed();

            void testPushButtonPressed();

            void helpPushButtonPressed();

            void authCheckBoxStateChanged(const int& state);

          private:

            const std::string getConnectionInfo() const;

          private:

            std::unique_ptr<Ui::WMSConnectorDialogForm> m_ui;
            te::da::DataSourceInfoPtr m_datasource;
            te::da::DataSourcePtr m_driver;
          };
        }
      }
    }
  }
}

#endif //__TERRALIB_WS_OGC_WMS_QT_WMSCONNECTORDIALOG_H
