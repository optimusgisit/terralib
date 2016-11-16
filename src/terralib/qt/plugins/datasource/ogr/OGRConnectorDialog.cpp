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
  \file terralib/qt/plugins/datasource/ogr/OGRConnectorDialog.cpp

  \brief A dialog window for showing the OGR connector widget.
*/

// TerraLib
#include "../../../../core/translator/Translator.h"
#include "../../../../core/uri/URI.h"
#include "../../../../core/uri/Utils.h"
#include "../../../../dataaccess/datasource/DataSource.h"
#include "../../../../dataaccess/datasource/DataSourceFactory.h"
#include "../../../../dataaccess/datasource/DataSourceInfo.h"
#include "../../../../dataaccess/datasource/DataSourceManager.h"
#include "../../../af/Utils.h"
#include "../../../widgets/Exception.h"
#include "../../../widgets/Utils.h"
#include "OGRConnectorDialog.h"
#include "ui_OGRConnectorDialogForm.h"
#include "Utils.h"

// Boost
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

// Qt
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>

// STL
#include <cassert>

te::qt::plugins::ogr::OGRConnectorDialog::OGRConnectorDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f),
    m_ui(new Ui::OGRConnectorDialogForm)
{
// add controls
  m_ui->setupUi(this);

// connect signal and slots
  connect(m_ui->m_openPushButton, SIGNAL(pressed()), this, SLOT(openPushButtonPressed()));
  connect(m_ui->m_testPushButton, SIGNAL(pressed()), this, SLOT(testPushButtonPressed()));
  connect(m_ui->m_searchFeatureToolButton, SIGNAL(pressed()), this, SLOT(searchFeatureToolButtonPressed()));

  m_ui->m_helpPushButton->setNameSpace("dpi.inpe.br.plugins");
  m_ui->m_helpPushButton->setPageReference("plugins/ogr/ogr.html");
}

te::qt::plugins::ogr::OGRConnectorDialog::~OGRConnectorDialog()
{
}

const te::da::DataSourceInfoPtr& te::qt::plugins::ogr::OGRConnectorDialog::getDataSource() const
{
  return m_datasource;
}

const te::da::DataSourcePtr& te::qt::plugins::ogr::OGRConnectorDialog::getDriver() const
{
  return m_driver;
}

void te::qt::plugins::ogr::OGRConnectorDialog::set(const te::da::DataSourceInfoPtr& ds)
{
  m_datasource = ds;

  if(m_datasource.get() != 0)
  {
    setConnectionInfo(m_datasource->getConnInfoAsString());

    m_ui->m_datasourceTitleLineEdit->setText(QString::fromUtf8(m_datasource->getTitle().c_str()));

    m_ui->m_datasourceDescriptionTextEdit->setText(QString::fromUtf8(m_datasource->getDescription().c_str()));
  }
}

void te::qt::plugins::ogr::OGRConnectorDialog::openPushButtonPressed()
{
  try
  {
    // check if driver is loaded
    if(te::da::DataSourceFactory::find("OGR") == 0)
      throw te::qt::widgets::Exception(TE_TR("Sorry! No data access driver loaded for OGR data sources!"));

    // get data source connection info based on form data
   const std::string dsInfo = getConnectionInfo();

// perform connection
    std::unique_ptr<te::da::DataSource> ds = te::da::DataSourceFactory::make("OGR", dsInfo);
    ds->open();
    m_driver.reset(ds.release());

    if(m_driver.get() == 0)
      throw te::qt::widgets::Exception(TE_TR("Could not open dataset via OGR due to an unknown error!"));

    QString title = m_ui->m_datasourceTitleLineEdit->text().trimmed();

    if(title.isEmpty())
      title = m_ui->m_featureRepoLineEdit->text();

    if(m_datasource.get() == 0)
    {
// create a new data source based on form data
      m_datasource.reset(new te::da::DataSourceInfo);

      m_datasource->setConnInfo(dsInfo);

      boost::uuids::basic_random_generator<boost::mt19937> gen;
      boost::uuids::uuid u = gen();
      std::string dsId = boost::uuids::to_string(u);

      m_datasource->setId(dsId);
      m_driver->setId(dsId);
      m_datasource->setTitle(title.toUtf8().data());
      m_datasource->setDescription(m_ui->m_datasourceDescriptionTextEdit->toPlainText().toUtf8().data());
      m_datasource->setAccessDriver("OGR");
      m_datasource->setType("OGR");
    }
    else
    {
      m_driver->setId(m_datasource->getId());
      m_datasource->setConnInfo(dsInfo);
      m_datasource->setTitle(title.toUtf8().data());
      m_datasource->setDescription(m_ui->m_datasourceDescriptionTextEdit->toPlainText().trimmed().toUtf8().data());
    }

    if(m_ui->m_fileRadioButton->isChecked())
    {
      QString path = m_ui->m_featureRepoLineEdit->text().trimmed();
      if(IsShapeFile(path) && !HasShapeFileSpatialIndex(path))
      {
        if(QMessageBox::question(this, windowTitle(), tr("Do you want create spatial index to the selected ESRI ShapeFile?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        {
          std::vector<std::string> datasetNames = m_driver->getDataSetNames();
          assert(!datasetNames.empty());

          std::string command = "CREATE SPATIAL INDEX ON " + datasetNames[0];

          QApplication::setOverrideCursor(Qt::WaitCursor);

          m_driver->execute(command);

          QApplication::restoreOverrideCursor();

          QMessageBox::information(this, windowTitle(), "Spatial index created with successfully!");
        }
      }
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
                         tr("Unknown error while opening feature via OGR!"));
    return;
  }

  accept();
}

void te::qt::plugins::ogr::OGRConnectorDialog::testPushButtonPressed()
{
  try
  {
    // check if driver is loaded
    if(te::da::DataSourceFactory::find("OGR") == 0)
      throw te::qt::widgets::Exception(TE_TR("Sorry! No data access driver loaded for OGR data sources!"));

    // perform connection
    std::unique_ptr<te::da::DataSource> ds(te::da::DataSourceFactory::make("OGR", getConnectionInfo()));
    ds->open();

    if(ds.get() == 0)
      throw te::qt::widgets::Exception(TE_TR("Could not open feature repository via OGR!"));
    
    if (m_ui->m_dirRadioButton->isChecked() && ds->getNumberOfDataSets() <= 0)
      QMessageBox::information(this,
                               tr("TerraLib Qt Components"),
                               tr("Directory does not contain datasets!"));
    else
      QMessageBox::information(this,
                       tr("TerraLib Qt Components"),
                       tr("Data source is ok!"));
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
                         tr("Unknown error while testing OGR data source!"));
  }
}

void te::qt::plugins::ogr::OGRConnectorDialog::searchFeatureToolButtonPressed()
{
  if(m_ui->m_fileRadioButton->isChecked())
  {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Vector File"), te::qt::widgets::GetFilePathFromSettings("vector"), tr("Esri Shapefile (*.shp *.SHP);; Mapinfo File (*.mif *.MIF);; GeoJSON (*.geojson *.GeoJSON);; GML (*.gml *.GML);; KML (*.kml *.KML);; All Files (*.*)"), 
      0, QFileDialog::ReadOnly);
    
    //  Web Feature Service - WFS (*.xml *.XML *.wfs *.WFS);; => not stable yet!

    if(fileName.isEmpty())
      return;

    QFileInfo info(fileName);

    te::qt::widgets::AddFilePathToSettings(info.absolutePath(), "vector");

    m_ui->m_featureRepoLineEdit->setText(fileName);
  }
  else if(m_ui->m_dirRadioButton->isChecked())
  {
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Select a directory with shape files"), te::qt::widgets::GetFilePathFromSettings("vector"), QFileDialog::ShowDirsOnly);

    if(dirName.isEmpty())
      return;

    te::qt::widgets::AddFilePathToSettings(dirName, "vector");

    m_ui->m_featureRepoLineEdit->setText(dirName);
  }
  else
  {
    QMessageBox::warning(this,
                         tr("TerraLib Qt Components"),
                         tr("Sorry, network files are not implemented yet!\nWe will provide it soon!"));
  }
}

const std::string te::qt::plugins::ogr::OGRConnectorDialog::getConnectionInfo() const
{
  QString qstr; // Auxiliary string used to hold temporary data

  std::string strURI = "file://"; // The base of the URI

  qstr = m_ui->m_featureRepoLineEdit->text().trimmed();
  
  if(qstr.isEmpty())
    throw te::qt::widgets::Exception(TE_TR("Please select a feature file first!"));
  
  strURI += qstr.toUtf8().data();

  return strURI;
}

void te::qt::plugins::ogr::OGRConnectorDialog::setConnectionInfo(const std::string& connInfo)
{
  const te::core::URI uri(connInfo);
  std::string path = uri.host() + uri.path();

  if(!path.empty())
  {
    m_ui->m_featureRepoLineEdit->setText(QString::fromUtf8(path.c_str()));

    if(boost::filesystem::is_directory(path))
      m_ui->m_dirRadioButton->setChecked(true);
    else if(boost::filesystem::is_regular_file(path))
      m_ui->m_fileRadioButton->setChecked(true);
  }
}

