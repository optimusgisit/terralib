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
\file terralib/qt/widgets/layer/utils/CreateLayerWidget.cpp

\brief This interface is used to create new layer operation.
*/

// TerraLib
#include "../../../../common/STLUtils.h"
#include "../../../../dataaccess/datasource/DataSourceFactory.h"
#include "../../../../dataaccess/datasource/DataSourceInfoManager.h"
#include "../../../../dataaccess/datasource/DataSourceManager.h"
#include "../../../../dataaccess/utils/Utils.h"
#include "../../../../datatype/Utils.h"
#include "../../../../qt/widgets/datasource/selector/DataSourceSelectorDialog.h"
#include "../../../../qt/widgets/property/NewPropertyWidget.h"
#include "../../layer/utils/DataSet2Layer.h"
#include "CreateLayerWidget.h"
#include "ui_CreateLayerWidgetForm.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>

// Boost
#include <boost/filesystem.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

te::qt::widgets::CreateLayerWidget::CreateLayerWidget(QWidget* parent, Qt::WindowFlags f) 
  : QWidget(parent, f),
  m_ui(new Ui::CreateLayerWidgetForm)
{
  // add controls
  m_ui->setupUi(this);
  m_ui->m_targetDatasourceToolButton->setIcon(QIcon::fromTheme("datasource"));
  m_ui->m_addToolButton->setIcon(QIcon::fromTheme("list-add"));

  //add new property widget
  QGridLayout* layout = new QGridLayout(m_ui->m_widget);
  layout->setContentsMargins(0, 0, 0, 0);

  m_newPropWidget = new te::qt::widgets::NewPropertyWidget(m_ui->m_widget);

  layout->addWidget(m_newPropWidget);

  //mapper initializer
  m_removeMapper = new QSignalMapper(this);

  //connects
  connect(m_ui->m_addToolButton, SIGNAL(pressed()), this, SLOT(onAddPushButtonClicked()));
  connect(m_ui->m_targetDatasourceToolButton, SIGNAL(pressed()), this, SLOT(onTargetDatasourceToolButtonPressed()));
  connect(m_ui->m_targetFileToolButton, SIGNAL(pressed()), this, SLOT(onTargetFileToolButtonPressed()));

  connect(m_removeMapper, SIGNAL(mapped(int)), this, SLOT(onRemoveAttrExecuted(int)));
}

te::qt::widgets::CreateLayerWidget::~CreateLayerWidget()
{
  clear();
}

void te::qt::widgets::CreateLayerWidget::setDataSource(te::da::DataSourceInfoPtr dsInfo)
{
  m_outputDatasource = dsInfo;

  m_newPropWidget->setDataSourceId(m_outputDatasource->getId());

  m_ui->m_repositoryLineEdit->setText(m_outputDatasource->getTitle().c_str());

  m_toFile = false;

  m_ui->m_targetFileToolButton->setEnabled(false);
  m_ui->m_targetDatasourceToolButton->setEnabled(false);

  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_newLayerNameLineEdit->setEnabled(true);
}

bool te::qt::widgets::CreateLayerWidget::createLayer(std::string& errorMessage)
{
  if (!m_outputDatasource.get())
  {
    errorMessage = "Define the data source first.";
    return false;
  }

  if (m_props.empty())
  {
    errorMessage = "Define the properties first.";
    return false;
  }

  if (m_ui->m_newLayerNameLineEdit->text().isEmpty())
  {
    errorMessage = "Layer name not defined.";
    return false;
  }

  std::string dsTypeName = m_ui->m_newLayerNameLineEdit->text().toUtf8().data();

  std::auto_ptr<te::da::DataSetType> dsType(new te::da::DataSetType(dsTypeName));

  for (std::size_t t = 0; t < m_props.size(); ++t)
  {
    dsType->add(m_props[t]->clone());
  }

  //create
  std::map<std::string, std::string> nopt;

  if (m_toFile)
  {
    boost::filesystem::path uri(m_ui->m_repositoryLineEdit->text().toUtf8().data());

    const std::string connInfo("file://" + uri.string());
    te::da::DataSourcePtr dsOGR(te::da::DataSourceFactory::make("OGR", connInfo).release());
    dsOGR->open();

    try
    {
      dsOGR->createDataSet(dsType.get(), nopt);
    }
    catch (const std::exception& ex)
    {
      errorMessage = ex.what();
      return false;
    }
    catch (...)
    {
      errorMessage = "Error creating layer.";
      return false;
    }
  }
  else
  {
    te::da::DataSourcePtr outputDataSource = te::da::GetDataSource(m_outputDatasource->getId());

    try
    {
      outputDataSource->createDataSet(dsType.get(), nopt);
    }
    catch (const std::exception& ex)
    {
      errorMessage = ex.what();
      return false;
    }
    catch (...)
    {
      errorMessage = "Error creating layer.";
      return false;
    }
  }

  return true;
}

te::map::AbstractLayerPtr te::qt::widgets::CreateLayerWidget::getLayer()
{
  //std::string driverName = m_outputDatasource->getType();
  //std::map<std::string, std::string> connInfo = m_outputDatasource->getConnInfo();
  std::string dataSetName = m_ui->m_newLayerNameLineEdit->text().toUtf8().data();

  std::size_t idx = dataSetName.find(".");
  if (idx != std::string::npos)
    dataSetName = dataSetName.substr(0, idx);

  te::map::AbstractLayerPtr layer;

  //static boost::uuids::basic_random_generator<boost::mt19937> gen;

  //boost::uuids::uuid valU = gen();
  //std::string id = boost::uuids::to_string(valU);

  //std::auto_ptr<te::da::DataSource> ds(te::da::DataSourceFactory::make(driverName));
  //ds->setConnectionInfo(connInfo);
  //ds->open();

  ////add ds info
  //te::da::DataSourceInfoPtr dsInfoPtr(new te::da::DataSourceInfo);
  //dsInfoPtr->setConnInfo(connInfo);
  //dsInfoPtr->setId(id);
  //dsInfoPtr->setTitle(dataSetName);
  //dsInfoPtr->setAccessDriver(driverName);
  //dsInfoPtr->setType(driverName);

  //te::da::DataSourceInfoManager::getInstance().add(dsInfoPtr);

  //add ds
  te::da::DataSourcePtr dsPtr = te::da::GetDataSource(m_outputDatasource->getId());
  //dsPtr->setId(id);

  //te::da::DataSourceManager::getInstance().insert(dsPtr);

  //create layer
  te::da::DataSetTypePtr dsType(dsPtr->getDataSetType(dataSetName).release());

  te::qt::widgets::DataSet2Layer ds2l(dsPtr->getId());

  layer = ds2l(dsType);

  return layer;
}

void te::qt::widgets::CreateLayerWidget::clear()
{
  te::common::FreeContents(m_props);
  m_props.clear();

  listProperties();
}

void te::qt::widgets::CreateLayerWidget::onAddPushButtonClicked()
{
  if (m_newPropWidget->buildProperty())
  {
    //get property
    te::dt::SimpleProperty* sp = m_newPropWidget->getProperty();

    m_props.push_back(sp);

    if (m_outputDatasource.get())
      m_newPropWidget->setDataSourceId(m_outputDatasource->getId());

    listProperties();
  }
}

void te::qt::widgets::CreateLayerWidget::onTargetDatasourceToolButtonPressed()
{
  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_newLayerNameLineEdit->setEnabled(true);

  te::qt::widgets::DataSourceSelectorDialog dlg(this);
  dlg.exec();

  std::list<te::da::DataSourceInfoPtr> dsPtrList = dlg.getSelecteds();

  if (dsPtrList.size() <= 0)
    return;

  std::list<te::da::DataSourceInfoPtr>::iterator it = dsPtrList.begin();

  m_ui->m_repositoryLineEdit->setText(QString(it->get()->getTitle().c_str()));

  m_outputDatasource = *it;

  te::da::DataSourcePtr outputDataSource = te::da::DataSourceManager::getInstance().get(m_outputDatasource->getId(), m_outputDatasource->getType(), m_outputDatasource->getConnInfo());

  outputDataSource->open();

  m_newPropWidget->setDataSourceId(m_outputDatasource->getId());

  m_toFile = false;
}

void te::qt::widgets::CreateLayerWidget::onTargetFileToolButtonPressed()
{
  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_repositoryLineEdit->clear();

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."), QString(), tr("Shapefile (*.shp *.SHP);;"), 0, QFileDialog::DontConfirmOverwrite);

  if (fileName.isEmpty())
    return;

  boost::filesystem::path outfile(fileName.toUtf8().data());

  m_ui->m_repositoryLineEdit->setText(outfile.string().c_str());

  m_ui->m_newLayerNameLineEdit->setText(outfile.leaf().string().c_str());

  m_ui->m_newLayerNameLineEdit->setEnabled(false);

  m_toFile = true;

  //create new data source
  boost::filesystem::path uri(m_ui->m_repositoryLineEdit->text().toUtf8().data());

  const std::string connInfo("file://" + uri.string());
  std::unique_ptr<te::da::DataSource> ds = te::da::DataSourceFactory::make("OGR", connInfo);
  ds->open();

  boost::uuids::basic_random_generator<boost::mt19937> gen;
  boost::uuids::uuid u = gen();
  std::string id_ds = boost::uuids::to_string(u);

  te::da::DataSourceInfoPtr dsInfoPtr(new te::da::DataSourceInfo);
  dsInfoPtr->setConnInfo(connInfo);
  dsInfoPtr->setTitle(uri.stem().string());
  dsInfoPtr->setAccessDriver("OGR");
  dsInfoPtr->setType("OGR");
  dsInfoPtr->setDescription(uri.string());
  dsInfoPtr->setId(id_ds);

  te::da::DataSourceInfoManager::getInstance().add(dsInfoPtr);

  m_outputDatasource = dsInfoPtr;

  te::da::DataSourcePtr outputDataSource = te::da::DataSourceManager::getInstance().get(m_outputDatasource->getId(), m_outputDatasource->getType(), m_outputDatasource->getConnInfo());

  outputDataSource->open();

  m_newPropWidget->setDataSourceId(m_outputDatasource->getId());
}

void te::qt::widgets::CreateLayerWidget::onRemoveAttrExecuted(int row)
{
  m_props.erase(m_props.begin() + row);

  listProperties();
}

void te::qt::widgets::CreateLayerWidget::listProperties()
{
  m_ui->m_tableWidget->setRowCount(0);

  for (std::size_t t = 0; t < m_props.size(); ++t)
  {
    te::dt::Property* p = m_props[t];

    //set property info into table widget
    int newrow = m_ui->m_tableWidget->rowCount();
    m_ui->m_tableWidget->insertRow(newrow);

    //name
    QTableWidgetItem* itemName = new QTableWidgetItem(p->getName().c_str());
    m_ui->m_tableWidget->setItem(newrow, 2, itemName);

    //type
    std::string propType = te::dt::ConvertDataTypeToString(p->getType());
    QTableWidgetItem* itemType = new QTableWidgetItem(propType.c_str());
    m_ui->m_tableWidget->setItem(newrow, 1, itemType);

    //remove button
    QToolButton* removeToolButton = new QToolButton(m_ui->m_tableWidget);
    removeToolButton->setIcon(QIcon::fromTheme("list-remove"));
    m_ui->m_tableWidget->setCellWidget(newrow, 0, removeToolButton);

    m_removeMapper->setMapping(removeToolButton, newrow);
    connect(removeToolButton, SIGNAL(pressed()), m_removeMapper, SLOT(map()));
  }

  m_ui->m_tableWidget->resizeColumnsToContents();
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
  m_ui->m_tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
  #endif
}

