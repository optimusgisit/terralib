#include "WMSConnectorDialog.h"
#include "ui_WMSConnectorDialogForm.h"

//TerraLib
#include "../../../../core/translator/Translator.h"
#include "../../../../core/uri/Utils.h"
#include "../../../../dataaccess/datasource/DataSource.h"
#include "../../../../dataaccess/datasource/DataSourceFactory.h"
#include "../../../../dataaccess/datasource/DataSourceInfo.h"
#include "../../../../dataaccess/datasource/DataSourceManager.h"
#include "../../../core/Exception.h"
#include "../../../core/CurlWrapper.h"
#include "../../../../qt/af/ApplicationController.h"

// Boost
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

//Qt
#include <QMessageBox>

te::ws::ogc::wms::qt::WMSConnectorDialog::WMSConnectorDialog(QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f),
    m_ui(new Ui::WMSConnectorDialogForm)
{
  m_ui->setupUi(this);

  m_ui->m_authCheckBox->setCheckState(Qt::Unchecked);

  connect(m_ui->m_authCheckBox, SIGNAL(stateChanged(int)), this, SLOT(authCheckBoxStateChanged(int)));
  connect(m_ui->m_openPushButton, SIGNAL(pressed()), this, SLOT(openPushButtonPressed()));
  connect(m_ui->m_testPushButton, SIGNAL(pressed()), this, SLOT(testPushButtonPressed()));
  connect(m_ui->m_helpPushButton, SIGNAL(pressed()), this, SLOT(helpPushButtonPressed()));
}

te::ws::ogc::wms::qt::WMSConnectorDialog::~WMSConnectorDialog()
{

}

const te::da::DataSourceInfoPtr &te::ws::ogc::wms::qt::WMSConnectorDialog::getDataSource() const
{
  return m_datasource;
}

const te::da::DataSourcePtr &te::ws::ogc::wms::qt::WMSConnectorDialog::getDriver() const
{
  return m_driver;
}

void te::ws::ogc::wms::qt::WMSConnectorDialog::set(const te::da::DataSourceInfoPtr &ds)
{
  m_datasource = ds;

  if(m_datasource.get() != 0)
  {
    const te::core::URI& connInfo = m_datasource->getConnInfo();

    if (!connInfo.uri().empty())
      m_ui->m_serverLineEdit->setText(QString::fromUtf8(connInfo.uri().c_str()));

    m_ui->m_datasourceTitleLineEdit->setText(QString::fromUtf8(m_datasource->getTitle().c_str()));

    m_ui->m_dataSourceDescriptionTextEdit->setText(QString::fromUtf8(m_datasource->getDescription().c_str()));
  }
}

void te::ws::ogc::wms::qt::WMSConnectorDialog::openPushButtonPressed()
{
  try
  {
    // Check if driver is loaded
    if(te::da::DataSourceFactory::find("WMS2") == 0)
      throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("Sorry! No data access driver loaded for WMS data sources!"));

    // Get the data source connection info based on form data
    std::string dsInfo = getConnectionInfo();

    // Perform connection
    std::unique_ptr<te::da::DataSource> ds = te::da::DataSourceFactory::make("WMS2", dsInfo);

    ds->open();
    m_driver.reset(ds.release());

    if(m_driver.get() == 0)
      throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("Could not open WMS data source due to an unknown error!"));

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
      m_datasource->setAccessDriver("WMS2");
      m_datasource->setType("WMS2");
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
    return;
  }
  catch(...)
  {
    QMessageBox::warning(this,
                         tr("TerraLib Qt Components"),
                         tr("Unknown error while opening WMS data source!"));
    return;
  }

  accept();
}

void te::ws::ogc::wms::qt::WMSConnectorDialog::testPushButtonPressed()
{
  try
  {
    // Check if driver is loaded
    if(te::da::DataSourceFactory::find("WMS2") == 0)
      throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("Sorry! No data access driver loaded for WMS data sources!"));

    // Get the data source connection info based on form data
    std::string dsInfo = getConnectionInfo();

    // Perform connection
    std::unique_ptr<te::da::DataSource> ds(te::da::DataSourceFactory::make("WMS2", dsInfo));

    if(ds.get() == 0)
      throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("Could not open WMS server!"));

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

void te::ws::ogc::wms::qt::WMSConnectorDialog::helpPushButtonPressed()
{
  QMessageBox::warning(this,
                       tr("TerraLib Qt Components"),
                       tr("Not implemented yet!\nWe will provide it soon!"));
}

void te::ws::ogc::wms::qt::WMSConnectorDialog::authCheckBoxStateChanged(const int &state)
{
  Qt::CheckState checkState = (Qt::CheckState) state;

  if(checkState == Qt::Checked)
  {
    m_ui->m_authUsernameLineEdit->setEnabled(true);
    m_ui->m_authPasswordLineEdit->setEnabled(true);
  }
  else
  {
    m_ui->m_authUsernameLineEdit->setEnabled(false);
    m_ui->m_authPasswordLineEdit->setEnabled(false);
    m_ui->m_authUsernameLineEdit->clear();
    m_ui->m_authPasswordLineEdit->clear();
  }
}

const std::string te::ws::ogc::wms::qt::WMSConnectorDialog::getConnectionInfo() const
{
  std::string serviceURL; // Auxiliary string used to hold temporary data

  std::string strURI("wms://");

  // Get the server URL
  serviceURL = m_ui->m_serverLineEdit->text().trimmed().toUtf8().constData();

  if (serviceURL.empty())
    throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("Please define the server address first!"));

  if(m_ui->m_authCheckBox->checkState() == Qt::Checked)
  {

    if(m_ui->m_authUsernameLineEdit->text().isEmpty() || m_ui->m_authPasswordLineEdit->text().isEmpty())
    {
      throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("Username or password is not defined!"));
    }

    std::string username = m_ui->m_authUsernameLineEdit->text().toUtf8().constData();
    std::string password = m_ui->m_authPasswordLineEdit->text().toUtf8().constData();

    strURI = strURI + username + std::string(":") + password + std::string("@wms");
  }

  std::string encodedURL = te::core::URIEncode(serviceURL);

  std::string usrDataDir = te::qt::af::AppCtrlSingleton::getInstance().getUserDataDir().toUtf8().data();

  strURI = strURI + "?URI="+ encodedURL + "&VERSION=1.3.0" + "&USERDATADIR=" + usrDataDir;

  return strURI;
}
