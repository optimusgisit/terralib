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
  \file terralib/qt/plugins/datasource/pgis/PostGISConnectorDialog.cpp

  \brief A dialog window for showing the PostGIS connector widget.
*/

// TerraLib
#include "../../../../core/translator/Translator.h"
#include "../../../../core/uri/URI.h"
#include "../../../../core/uri/Utils.h"
#include "../../../../dataaccess/datasource/DataSource.h"
#include "../../../../dataaccess/datasource/DataSourceFactory.h"
#include "../../../../dataaccess/datasource/DataSourceManager.h"
#include "../../../../dataaccess/datasource/DataSourceInfo.h"
#include "../../../widgets/Exception.h"
#include "PostGISConnectorDialog.h"
#include "ui_PostGISConnectorDialogForm.h"

// Boost
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

// Qt
#include <QMessageBox>

te::qt::plugins::pgis::PostGISConnectorDialog::PostGISConnectorDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f),
    m_ui(new Ui::PostGISConnectorDialogForm)
{
// add controls
  m_ui->setupUi(this);

// init controls
  m_ui->m_advancedConnectionOptionsGroupBox->hide();
  
// connect signal and slots
  connect(m_ui->m_openPushButton, SIGNAL(pressed()), this, SLOT(openPushButtonPressed()));
  connect(m_ui->m_testPushButton, SIGNAL(pressed()), this, SLOT(testPushButtonPressed()));
  connect(m_ui->m_userNameLineEdit, SIGNAL(editingFinished()), this, SLOT(passwordLineEditEditingFinished()));
  connect(m_ui->m_passwordLineEdit, SIGNAL(editingFinished()), this, SLOT(passwordLineEditEditingFinished()));
  connect(m_ui->m_advancedConnectionOptionsCheckBox, SIGNAL(toggled(bool)), this, SLOT(advancedConnectionOptionsCheckBoxToggled(bool)));

  m_ui->m_portLineEdit->setValidator(new QIntValidator(0, 99999, this));

  m_ui->m_helpPushButton->setNameSpace("dpi.inpe.br.plugins");
  m_ui->m_helpPushButton->setPageReference("plugins/pgis/pgis.html");
}

te::qt::plugins::pgis::PostGISConnectorDialog::~PostGISConnectorDialog()
{
}

const te::da::DataSourceInfoPtr& te::qt::plugins::pgis::PostGISConnectorDialog::getDataSource() const
{
  return m_datasource;
}

const te::da::DataSourcePtr& te::qt::plugins::pgis::PostGISConnectorDialog::getDriver() const
{
  return m_driver;
}

void te::qt::plugins::pgis::PostGISConnectorDialog::set(const te::da::DataSourceInfoPtr& ds)
{
  m_datasource = ds;

  if(m_datasource.get() != 0)
  {
    setConnectionInfo(m_datasource->getConnInfoAsString());

    m_ui->m_datasourceTitleLineEdit->setText(QString::fromUtf8(m_datasource->getTitle().c_str()));

    m_ui->m_datasourceDescriptionTextEdit->setText(QString::fromUtf8(m_datasource->getDescription().c_str()));
  }
}

void te::qt::plugins::pgis::PostGISConnectorDialog::openPushButtonPressed()
{
  try
  {
    // Check if driver is loaded
    if(te::da::DataSourceFactory::find("POSTGIS") == 0)
      throw te::qt::widgets::Exception(TE_TR("No data access driver loaded for PostgreSQL + PostGIS data sources!"));

    // Perform connection
    std::unique_ptr<te::da::DataSource> ds = te::da::DataSourceFactory::make("POSTGIS", getConnectionInfo(true));
    ds->open();
    m_driver.reset(ds.release());

    if(m_driver.get() == 0)
      throw te::qt::widgets::Exception(TE_TR("Could not open PostgreSQL + PostGIS data source due to an unknown error!"));

    QString title = m_ui->m_datasourceTitleLineEdit->text().trimmed();

    if(title.isEmpty())
      title = m_ui->m_hostNameLineEdit->text().trimmed() + QString::fromUtf8("@") + m_ui->m_databaseComboBox->currentText().trimmed() + QString::fromUtf8("@") + m_ui->m_userNameLineEdit->text().trimmed();

    if(m_datasource.get() == 0)
    {
      // Create a new data source based on the form data
      m_datasource.reset(new te::da::DataSourceInfo);

      m_datasource->setConnInfo(getConnectionInfo(m_ui->m_savePasswordCheckBox->isChecked()));

      boost::uuids::basic_random_generator<boost::mt19937> gen;
      boost::uuids::uuid u = gen();
      std::string dsId = boost::uuids::to_string(u);

      m_datasource->setId(dsId);
      m_driver->setId(dsId);
      m_datasource->setTitle(title.toUtf8().data());
      m_datasource->setDescription(m_ui->m_datasourceDescriptionTextEdit->toPlainText().toUtf8().data());
      m_datasource->setAccessDriver("POSTGIS");
      m_datasource->setType("POSTGIS");
    }
    else
    {
      m_driver->setId(m_datasource->getId());
      m_datasource->setConnInfo(getConnectionInfo(m_ui->m_savePasswordCheckBox->isChecked()));
      m_datasource->setTitle(title.toUtf8().data());
      m_datasource->setDescription(m_ui->m_datasourceDescriptionTextEdit->toPlainText().toUtf8().data());
    }
  }
  catch(const std::exception& e)
  {
    QMessageBox::warning(this,
                         tr("TerraLib Qt Components"),
                         tr(e.what()));
    return;
  }
  catch(...)
  {
    QMessageBox::warning(this,
                         tr("TerraLib Qt Components"),
                         tr("Unknown error while opening PostgreSQL database!"));
    return;
  }

  accept();
}

void te::qt::plugins::pgis::PostGISConnectorDialog::testPushButtonPressed()
{
  try
  {
    // Check if driver is loaded
    if(te::da::DataSourceFactory::find("POSTGIS") == 0)
      throw te::qt::widgets::Exception(TE_TR("Sorry! No data access driver loaded for PostgreSQL + PostGIS data sources!"));

    // Perform connection
    std::unique_ptr<te::da::DataSource> ds = te::da::DataSourceFactory::make("POSTGIS", getConnectionInfo(true));
    if(ds.get() == 0)
      throw te::qt::widgets::Exception(TE_TR("Could not open PostgreSQL + PostGIS database!"));

    //ds->setConnectionInfo(getConnectionInfo(true));
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
                         tr("Unknown error while testing PostgreSQL + PostGIS data source!"));
  }
}

const std::string te::qt::plugins::pgis::PostGISConnectorDialog::getConnectionInfo(bool getPrivateKeys) const
{
  QString qstr; // Auxiliary string used to hold temporary data

  std::string strURI = "pgsql://"; // The base of the URI

  // get user
  qstr = m_ui->m_userNameLineEdit->text().trimmed();
  strURI += qstr.toUtf8().data();
  strURI += ":";

  //get password
  if(getPrivateKeys)
  {
    qstr = m_ui->m_passwordLineEdit->text().trimmed();
    strURI += qstr.toUtf8().data();
    strURI += "@";
  }

  //get host
  qstr = m_ui->m_hostNameLineEdit->text().trimmed();
  strURI += qstr.isEmpty() ? "localhost" : qstr.toUtf8().data();
  strURI += ":";

  //get port
  qstr = m_ui->m_portLineEdit->text().trimmed();
  strURI += qstr.isEmpty() ? "5432" : qstr.toUtf8().data();
  strURI += "/";

  //get db name 
  qstr = m_ui->m_databaseComboBox->currentText().trimmed();
  strURI += qstr.toUtf8().data();

  //query section
  strURI += "?";

  qstr = m_ui->m_tablesToHideLineEdit->text().trimmed();
  if (!qstr.isEmpty())
  {
    strURI += "PG_HIDE_TABLES=";
    strURI += qstr.toUtf8().data();
    strURI += "&";
  }

  strURI += "PG_CONNECT_TIMEOUT=";
  qstr = qstr.isEmpty() ? "4" : m_ui->m_connectTimeoutSpinBox->text().trimmed();
  strURI += qstr.toUtf8().data();

  // get MaxPoolSize
  strURI += "&PG_MAX_POOL_SIZE=";
  qstr = qstr.isEmpty() ? "4" : m_ui->m_maxPoolSizeSpinBox->text().trimmed();
  strURI += qstr.toUtf8().data();

  // get MinPoolSize
  strURI += "&PG_MIN_POOL_SIZE=";
  qstr = qstr.isEmpty() ? "2" : m_ui->m_minPoolSizeSpinBox->text().trimmed();
  strURI += qstr.toUtf8().data();

  // hide spatial metadata tables?
  strURI += "&PG_HIDE_SPATIAL_METADATA_TABLES=";
  strURI += m_ui->m_hideMetadataTablesCheckBox->isChecked() ? "TRUE" : "FALSE";
  
  // hide raster tables?
  strURI += "&PG_HIDE_RASTER_TABLES=";
  strURI += m_ui->m_hideRasterTablesCheckBox->isChecked() ? "TRUE" : "FALSE";

  qstr = m_ui->m_optionsLineEdit->text().trimmed();
  if(!qstr.isEmpty())
  {
    strURI += "&PG_OPTIONS=";
    strURI += qstr.toUtf8().data();
  }

  return strURI;
}

void te::qt::plugins::pgis::PostGISConnectorDialog::setConnectionInfo(const std::string& connInfo)
{
  te::core::URI uri(connInfo);
  std::map<std::string, std::string> kvp = te::core::Expand(uri.query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  std::map<std::string, std::string>::const_iterator itend = kvp.end();

  it = kvp.find("PG_HOST_ADDR");
  if (it != itend && !it->second.empty())
    m_ui->m_hostNameLineEdit->setText(QString::fromUtf8(it->second.c_str()));

  std::string port = uri.port();
  if(!port.empty())
    m_ui->m_portLineEdit->setText(QString::fromUtf8(port.c_str()));

  std::string dbName = uri.path().substr(1, uri.path().length());
  if(!dbName.empty())
  {
    int pos = m_ui->m_databaseComboBox->findText(QString::fromUtf8(dbName.c_str()));

    if(pos != -1)
      m_ui->m_databaseComboBox->setCurrentIndex(pos);
    else
    {
      m_ui->m_databaseComboBox->addItem(QString::fromUtf8(dbName.c_str()));
      m_ui->m_databaseComboBox->setCurrentIndex(0);
    }
  }

  std::string user = uri.user();
  if(!user.empty())
    m_ui->m_userNameLineEdit->setText(QString::fromUtf8(user.c_str()));

  std::string pass = uri.password();
  if (!pass.empty())
    m_ui->m_passwordLineEdit->setText(QString::fromUtf8(pass.c_str()));

  it = kvp.find("PG_CONNECT_TIMEOUT");
  if (it != itend && !it->second.empty())
    m_ui->m_connectTimeoutSpinBox->setValue(boost::lexical_cast<int>(it->second.c_str()));
  
  it = kvp.find("PG_MIN_POOL_SIZE");
  if (it != itend && !it->second.empty())
    m_ui->m_minPoolSizeSpinBox->setValue(boost::lexical_cast<int>(it->second.c_str()));

  it = kvp.find("PG_MAX_POOL_SIZE");
  if (it != itend && !it->second.empty())
    m_ui->m_maxPoolSizeSpinBox->setValue(boost::lexical_cast<int>(it->second.c_str()));

  it = kvp.find("PG_OPTIONS");
  if (it != itend && !it->second.empty())
    m_ui->m_optionsLineEdit->setText(QString::fromUtf8(it->second.c_str()));

  it = kvp.find("PG_HIDE_SPATIAL_METADATA_TABLES");
    m_ui->m_hideMetadataTablesCheckBox->setChecked((it != itend && !it->second.empty()) && (boost::to_upper_copy(it->second) == "TRUE"));

  it = kvp.find("PG_HIDE_RASTER_TABLES");
    m_ui->m_hideRasterTablesCheckBox->setChecked((it != itend && !it->second.empty()) && (boost::to_upper_copy(it->second) == "TRUE"));

  it = kvp.find("PG_HIDE_TABLES");
  if(it != itend && !it->second.empty())
    m_ui->m_tablesToHideLineEdit->setText(QString::fromUtf8(it->second.c_str()));
}

void te::qt::plugins::pgis::PostGISConnectorDialog::advancedConnectionOptionsCheckBoxToggled(bool t)
{
  m_ui->m_advancedConnectionOptionsGroupBox->setVisible(t);
}

void te::qt::plugins::pgis::PostGISConnectorDialog::passwordLineEditEditingFinished()
{
  if(m_ui->m_userNameLineEdit->text() != "" || m_ui->m_passwordLineEdit->text() != "")
  {
    try{
       std::string connInfo = getConnectionInfo(true);

      //get current informations
      std::string curDb = "";
      
      if(!m_ui->m_databaseComboBox->currentText().isEmpty())
        curDb = m_ui->m_databaseComboBox->currentText().toUtf8().data();

      m_ui->m_databaseComboBox->clear();

      // Get DataSources
      std::vector<std::string> dbNames = te::da::DataSource::getDataSourceNames("POSTGIS", connInfo);

      if(!dbNames.empty())
      {
        setDatabasesNames(dbNames);

        if(!curDb.empty())
        {
          int idx =  m_ui->m_databaseComboBox->findText(curDb.c_str(), Qt::MatchExactly);

          if(idx != -1)
            m_ui->m_databaseComboBox->setCurrentIndex(idx);
        }
      }
    }
    catch(...)
    {
    }
  }
}

void te::qt::plugins::pgis::PostGISConnectorDialog::setDatabasesNames(std::vector<std::string> names)
{
  std::sort(names.begin(), names.end());

  for(std::size_t i = 0; i < names.size(); ++i)
  {
    m_ui->m_databaseComboBox->addItem(names[i].c_str());
  }
}
