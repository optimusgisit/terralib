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
  \file terralib/qt/plugins/datasource/wms/WMSConnectorDialog.cpp

  \brief A dialog window for showing the WMS connector widget.
*/

// TerraLib
#include "../../../../core/translator/Translator.h"
#include "../../../../core/uri/URI.h"
#include "../../../../core/utils/URI.h"
#include "../../../../dataaccess/datasource/DataSource.h"
#include "../../../../dataaccess/datasource/DataSourceFactory.h"
#include "../../../../dataaccess/datasource/DataSourceInfo.h"
#include "../../../../dataaccess/datasource/DataSourceManager.h"
#include "../../../widgets/Exception.h"
#include "WMSConnectorDialog.h"
#include "ui_WMSConnectorDialogForm.h"

// Boost
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

// Qt
#include <QMessageBox>

te::qt::plugins::wms::WMSConnectorDialog::WMSConnectorDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f),
    m_ui(new Ui::WMSConnectorDialogForm)
{
// add controls
  m_ui->setupUi(this);

// connect signal and slots
  connect(m_ui->m_openPushButton, SIGNAL(pressed()), this, SLOT(openPushButtonPressed()));
  connect(m_ui->m_testPushButton, SIGNAL(pressed()), this, SLOT(testPushButtonPressed()));
  connect(m_ui->m_helpPushButton, SIGNAL(pressed()), this, SLOT(helpPushButtonPressed()));
}

te::qt::plugins::wms::WMSConnectorDialog::~WMSConnectorDialog()
{
}

const te::da::DataSourceInfoPtr& te::qt::plugins::wms::WMSConnectorDialog::getDataSource() const
{
  return m_datasource;
}

const te::da::DataSourcePtr& te::qt::plugins::wms::WMSConnectorDialog::getDriver() const
{
  return m_driver;
}

void te::qt::plugins::wms::WMSConnectorDialog::set(const te::da::DataSourceInfoPtr& ds)
{
  m_datasource = ds;

  if(m_datasource.get() != 0)
  {
    const te::core::URI& connInfo = m_datasource->getConnInfo();

    if(!connInfo.uri().empty())
      m_ui->m_serverLineEdit->setText(QString::fromUtf8(connInfo.uri().c_str()));

    m_ui->m_datasourceTitleLineEdit->setText(QString::fromUtf8(m_datasource->getTitle().c_str()));

    m_ui->m_dataSourceDescriptionTextEdit->setText(QString::fromUtf8(m_datasource->getDescription().c_str()));
  }
}

void te::qt::plugins::wms::WMSConnectorDialog::openPushButtonPressed()
{
  try
  {
    // Check if driver is loaded
    if(te::da::DataSourceFactory::find("WMS") == 0)
      throw te::qt::widgets::Exception(TE_TR("Sorry! No data access driver loaded for WMS data sources!"));

    // Get the data source connection info based on form data
    std::string dsInfo = getConnectionInfo();

    // Perform connection
    std::unique_ptr<te::da::DataSource> ds = te::da::DataSourceFactory::make("WMS", dsInfo);
    ds->open();
    m_driver.reset(ds.release());

    if(m_driver.get() == 0)
      throw te::qt::widgets::Exception(TE_TR("Could not open WMS data source due to an unknown error!"));

    QString title = m_ui->m_datasourceTitleLineEdit->text().trimmed();

    if(title.isEmpty())
      title = m_ui->m_serverLineEdit->text().trimmed();

    if(m_datasource.get() == 0)
    {
      // Create a new data source based on the form data
      m_datasource.reset(new te::da::DataSourceInfo);

      m_datasource->setConnInfo(dsInfo);

      boost::uuids::basic_random_generator<boost::mt19937> gen;
      boost::uuids::uuid u = gen();
      std::string dsId = boost::uuids::to_string(u);

      m_datasource->setId(dsId);
      m_driver->setId(dsId);
      m_datasource->setTitle(title.toUtf8().data());
      m_datasource->setDescription(m_ui->m_dataSourceDescriptionTextEdit->toPlainText().trimmed().toUtf8().data());
      m_datasource->setAccessDriver("WMS");
      m_datasource->setType("WMS");
    }
    else
    {
      m_driver->setId(m_datasource->getId());
      m_datasource->setConnInfo(dsInfo);
      m_datasource->setTitle(title.toUtf8().data());
      m_datasource->setDescription(m_ui->m_dataSourceDescriptionTextEdit->toPlainText().trimmed().toUtf8().data());
    }
  }
  catch(const std::exception& e)
  {
    QMessageBox::warning(this,
                         tr("TerraLib Qt Components"),
                         tr(e.what()));
  }
  catch(...)
  {
    QMessageBox::warning(this,
                         tr("TerraLib Qt Components"),
                         tr("Unknown error while opening WMS data source!"));
  }

  accept();
}

void te::qt::plugins::wms::WMSConnectorDialog::testPushButtonPressed()
{
  try
  {
    // Check if driver is loaded
    if(te::da::DataSourceFactory::find("WMS") == 0)
      throw te::qt::widgets::Exception(TE_TR("Sorry! No data access driver loaded for WMS data sources!"));

    // Perform connection
    std::unique_ptr<te::da::DataSource> ds(te::da::DataSourceFactory::make("WMS", getConnectionInfo()));

    if(ds.get() == 0)
      throw te::qt::widgets::Exception(TE_TR("Could not open WMS server!"));

    ds->open();

    QMessageBox::information(this,
                             tr("TerraLib Qt Components"),
                             tr("Data source is ok!"));

    ds->close();
  }
  catch(const std::exception& e)
  {
    QMessageBox::warning(this,
                         tr("TerraLib Qt Components"),
                         tr(e.what()));
  }
  catch(...)
  {
    QMessageBox::warning(this,
                         tr("TerraLib Qt Components"),
                         tr("Unknown error while testing WMS data source!"));
  }
}

void te::qt::plugins::wms::WMSConnectorDialog::helpPushButtonPressed()
{
  QMessageBox::warning(this,
                       tr("TerraLib Qt Components"),
                       tr("Not implemented yet!\nWe will provide it soon!"));
}

const std::string te::qt::plugins::wms::WMSConnectorDialog::getConnectionInfo() const
{
  QString qstr; // Auxiliary string used to hold temporary data

  qstr = m_ui->m_serverLineEdit->text().trimmed();
  std::string strURI("WMS:");

  if (qstr.isEmpty())
    throw te::qt::widgets::Exception(TE_TR("Please select a feature file first!"));

  strURI += qstr.toUtf8().data();

  return strURI;
}
