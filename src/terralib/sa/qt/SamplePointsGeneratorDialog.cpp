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
  \file terralib/sa/qt/SamplePointsGeneratorDialog.cpp

  \brief A dialog to generate sample points inside an area of a dataset.
*/

// TerraLib
#include "../../common/Logger.h"
#include "../../common/progress/ProgressManager.h"
#include "../../common/Translator.h"
#include "../../common/STLUtils.h"
#include "../../dataaccess/datasource/DataSource.h"
#include "../../dataaccess/datasource/DataSourceInfo.h"
#include "../../dataaccess/datasource/DataSourceInfoManager.h"
#include "../../dataaccess/datasource/DataSourceManager.h"
#include "../../dataaccess/utils/Utils.h"
#include "../../geometry/GeometryProperty.h"
#include "../../maptools/DataSetLayer.h"
#include "../../qt/widgets/datasource/selector/DataSourceSelectorDialog.h"
#include "../core/SamplePointsGeneratorRandom.h"
#include "../core/SamplePointsGeneratorStratified.h"
#include "../Enums.h"
#include "../Exception.h"
#include "SamplePointsGeneratorDialog.h"
#include "ui_SamplePointsGeneratorDialogForm.h"

// Qt
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QValidator>

// STL
#include <memory>

// Boost
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

Q_DECLARE_METATYPE(te::map::AbstractLayerPtr);

te::sa::SamplePointsGeneratorDialog::SamplePointsGeneratorDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f),
    m_ui(new Ui::SamplePointsGeneratorDialogForm)
{
// add controls
  m_ui->setupUi(this);

  fillGeneratorTypes();

  m_ui->m_nPointsRandomLineEdit->setValidator(new QIntValidator(this));
  m_ui->m_nPointsStratifiedLineEdit->setValidator(new QIntValidator(this));

// add icons
  m_ui->m_imgLabel->setPixmap(QIcon::fromTheme("sa-samplepoint-hint").pixmap(112,48));
  m_ui->m_targetDatasourceToolButton->setIcon(QIcon::fromTheme("datasource"));

// connectors
  connect(m_ui->m_inputLayerComboBox, SIGNAL(activated(int)), this, SLOT(onInputLayerComboBoxActivated(int)));

  connect(m_ui->m_okPushButton, SIGNAL(clicked()), this, SLOT(onOkPushButtonClicked()));
  connect(m_ui->m_targetDatasourceToolButton, SIGNAL(pressed()), this, SLOT(onTargetDatasourceToolButtonPressed()));
  connect(m_ui->m_targetFileToolButton, SIGNAL(pressed()), this,  SLOT(onTargetFileToolButtonPressed()));

// help info
  m_ui->m_helpPushButton->setNameSpace("dpi.inpe.br.plugins"); 
  m_ui->m_helpPushButton->setPageReference("plugins/sa/sa_samplepointsgenerator.html");
}

te::sa::SamplePointsGeneratorDialog::~SamplePointsGeneratorDialog()
{
}

void te::sa::SamplePointsGeneratorDialog::setLayers(std::list<te::map::AbstractLayerPtr> layers)
{
  std::list<te::map::AbstractLayerPtr>::iterator it = layers.begin();

  while(it != layers.end())
  {
    te::map::AbstractLayerPtr l = *it;

    std::auto_ptr<te::da::DataSetType> dsType = l->getSchema();

    if(dsType->hasGeom())
      m_ui->m_inputLayerComboBox->addItem(it->get()->getTitle().c_str(), QVariant::fromValue(l));

    ++it;
  }

// fill attributes combo
  if(m_ui->m_inputLayerComboBox->count() > 0)
    onInputLayerComboBoxActivated(0);
}

void te::sa::SamplePointsGeneratorDialog::onInputLayerComboBoxActivated(int index)
{
  QVariant varLayer = m_ui->m_inputLayerComboBox->itemData(index, Qt::UserRole);
  
  te::map::AbstractLayerPtr l = varLayer.value<te::map::AbstractLayerPtr>();

  std::auto_ptr<te::da::DataSetType> dsType = l->getSchema();

  std::vector<te::dt::Property*> propVec = dsType->getProperties();

  m_ui->m_attrStratifiedComboBox->clear();

  for(std::size_t t = 0; t < propVec.size(); ++t)
  {
    int dataType = propVec[t]->getType();

    if(dataType == te::dt::STRING_TYPE)
      m_ui->m_attrStratifiedComboBox->addItem(propVec[t]->getName().c_str(), dataType);
  }
}

void te::sa::SamplePointsGeneratorDialog::onOkPushButtonClicked()
{
  // check input parameters
  if(m_ui->m_repositoryLineEdit->text().isEmpty())
  {
    QMessageBox::information(this, tr("Warning"), tr("Define a repository for the result."));
    return;
  }
       
  if(m_ui->m_newLayerNameLineEdit->text().isEmpty())
  {
    QMessageBox::information(this, tr("Warning"), tr("Define a name for the resulting layer."));
    return;
  }

  //set operation parameters
  te::sa::SamplePointsGeneratorType spgt = (te::sa::SamplePointsGeneratorType)m_ui->m_generatorTypeComboBox->itemData(m_ui->m_generatorTypeComboBox->currentIndex()).toInt();

  if(spgt == te::sa::Random)
  {
    if(m_ui->m_nPointsRandomLineEdit->text().isEmpty())
    {
      QMessageBox::information(this, tr("Warning"), tr("Number of points not defined."));
      return;
    }

    int nPoints = m_ui->m_nPointsRandomLineEdit->text().toInt();
  }
  else if(spgt == te::sa::Stratified)
  {
    if(m_ui->m_nPointsStratifiedLineEdit->text().isEmpty())
    {
      QMessageBox::information(this, tr("Warning"), tr("Number of points not defined."));
      return;
    }

    int nPoints = m_ui->m_nPointsStratifiedLineEdit->text().toInt();
  }

  //get selected layer
  QVariant varLayer = m_ui->m_inputLayerComboBox->itemData(m_ui->m_inputLayerComboBox->currentIndex(), Qt::UserRole);
  te::map::AbstractLayerPtr l = varLayer.value<te::map::AbstractLayerPtr>();
  
  std::auto_ptr<te::da::DataSetType> dsType = l->getSchema();

  //get srid
  te::gm::GeometryProperty* gmProp =  te::da::GetFirstGeomProperty(dsType.get());

  if(!gmProp)
    throw;

  int srid = gmProp->getSRID();

  te::gm::Envelope env = l->getExtent();

  //create datasource to save the output information
  te::da::DataSourcePtr outputDataSource;

  if(m_toFile)
  {
    //create new data source
    boost::filesystem::path uri(m_ui->m_repositoryLineEdit->text().toStdString());

    std::map<std::string, std::string> dsInfo;
    dsInfo["URI"] = uri.string();

    boost::uuids::basic_random_generator<boost::mt19937> gen;
    boost::uuids::uuid u = gen();
    std::string id_ds = boost::uuids::to_string(u);

    te::da::DataSourceInfoPtr dsInfoPtr(new te::da::DataSourceInfo);
    dsInfoPtr->setConnInfo(dsInfo);
    dsInfoPtr->setTitle(uri.stem().string());
    dsInfoPtr->setAccessDriver("OGR");
    dsInfoPtr->setType("OGR");
    dsInfoPtr->setDescription(uri.string());
    dsInfoPtr->setId(id_ds);

    te::da::DataSourceInfoManager::getInstance().add(dsInfoPtr);

    outputDataSource = te::da::DataSourceManager::getInstance().get(id_ds, "OGR", dsInfoPtr->getConnInfo());
  }
  else
  {
    outputDataSource = te::da::GetDataSource(m_outputDatasource->getId());
  }

  //get output dataset name
  std::string dataSetName = m_ui->m_newLayerNameLineEdit->text().toStdString();

  std::size_t idx = dataSetName.find(".");
  if (idx != std::string::npos)
        dataSetName=dataSetName.substr(0,idx);

  try
  {
    te::sa::SamplePointsGeneratorAbstract* spga;

    if(spgt == te::sa::Random)
    {
      spga = new te::sa::SamplePointsGeneratorRandom();

      spga->setOutputDataSetName(dataSetName);
      spga->setOutputDataSource(outputDataSource);
      spga->setEnvelope(env);
      spga->setSRID(srid);

      ((te::sa::SamplePointsGeneratorRandom*)spga)->setNumberOfPoints(m_ui->m_nPointsRandomLineEdit->text().toInt());
    }
    else if(spgt == te::sa::Stratified)
    {
      spga = new te::sa::SamplePointsGeneratorStratified();

      spga->setOutputDataSetName(dataSetName);
      spga->setOutputDataSource(outputDataSource);
      spga->setEnvelope(env);
      spga->setSRID(srid);

      ((te::sa::SamplePointsGeneratorStratified*)spga)->setNumberOfPoints(m_ui->m_nPointsStratifiedLineEdit->text().toInt());
      ((te::sa::SamplePointsGeneratorStratified*)spga)->setInputDataSet(l->getData());
      ((te::sa::SamplePointsGeneratorStratified*)spga)->setInputAttributeName(m_ui->m_attrStratifiedComboBox->currentText().toStdString());
      ((te::sa::SamplePointsGeneratorStratified*)spga)->isProportionalToArea(m_ui->m_propStratifiedCheckBox->isChecked());
    }

    assert(spga);

    spga->execute();

    delete spga;
  }
  catch(...)
  {
    QMessageBox::warning(this, tr("Warning"), tr("Internal error. Sample Points not generated."));
    return;
  }

  accept();
}

void te::sa::SamplePointsGeneratorDialog::onTargetDatasourceToolButtonPressed()
{
  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_newLayerNameLineEdit->setEnabled(true);

  te::qt::widgets::DataSourceSelectorDialog dlg(this);
  dlg.exec();

  std::list<te::da::DataSourceInfoPtr> dsPtrList = dlg.getSelecteds();

  if(dsPtrList.size() <= 0)
    return;

  std::list<te::da::DataSourceInfoPtr>::iterator it = dsPtrList.begin();

  m_ui->m_repositoryLineEdit->setText(QString(it->get()->getTitle().c_str()));

  m_outputDatasource = *it;
  
  m_toFile = false;
}

void te::sa::SamplePointsGeneratorDialog::onTargetFileToolButtonPressed()
{
  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_repositoryLineEdit->clear();
  
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."), QString(), tr("Shapefile (*.shp *.SHP);;"),0, QFileDialog::DontConfirmOverwrite);
  
  if (fileName.isEmpty())
    return;
  
  boost::filesystem::path outfile(fileName.toStdString());

  m_ui->m_repositoryLineEdit->setText(outfile.string().c_str());

  m_ui->m_newLayerNameLineEdit->setText(outfile.leaf().string().c_str());

  m_ui->m_newLayerNameLineEdit->setEnabled(false);
  
  m_toFile = true;
}

void te::sa::SamplePointsGeneratorDialog::fillGeneratorTypes()
{
  m_ui->m_generatorTypeComboBox->clear();

  m_ui->m_generatorTypeComboBox->addItem("Random", QVariant(te::sa::Random));
  m_ui->m_generatorTypeComboBox->addItem("Stratified", QVariant(te::sa::Stratified));
}