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
  \file terralib/qt/widgets/connector/wcs/WCSConnectorDialog.h

  \brief ....
*/

#ifndef __TERRALIB_QT_WIDGETS_CONNECTOR_WCS_INTERNAL_WCSCONNECTORDIALOG_H
#define __TERRALIB_QT_WIDGETS_CONNECTOR_WCS_INTERNAL_WCSCONNECTORDIALOG_H

// TerraLib
#include <terralib/dataaccess/Definitions.h>
#include <terralib/qt/widgets/datasource/core/DataSource.h>

// STL
#include <memory>

// Qt
#include <QtGui/QDialog>

namespace Ui { class WCSConnectorDialogForm; }

namespace qt_af
{
  namespace plugin
  {
    namespace wcs
    {
      /*!
        \class WCSConnectorDialog

        \brief ....
      */
      class WCSConnectorDialog : public QDialog
      {
        Q_OBJECT

        public:

          WCSConnectorDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

          ~WCSConnectorDialog();

          const te::qt::widgets::DataSourcePtr& getDataSource() const;

          void set(const te::qt::widgets::DataSourcePtr& ds);

        public slots:

          void openPushButtonPressed();

          void testPushButtonPressed();

          void helpPushButtonPressed();

        private:

          std::auto_ptr<Ui::WCSConnectorDialogForm> m_ui;
          te::qt::widgets::DataSourcePtr m_datasource;
      }; 
    } // end namespace wcs
  }   // end namespace plugin
}     // end namespace qt_af

#endif  // __TERRALIB_QT_WIDGETS_CONNECTOR_WCS_INTERNAL_WCSCONNECTORDIALOG_H
