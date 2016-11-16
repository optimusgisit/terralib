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
  \file terralib/vp/qt/UnionDialog.cpp

  \brief Union operation dialog.
*/

// TerraLib
#include "../../core/filesystem/FileSystem.h"
#include "../../common/StringUtils.h"
#include "../../dataaccess/dataset/DataSetAdapter.h"
#include "../../dataaccess/dataset/DataSetTypeConverter.h"
#include "../../dataaccess/dataset/ObjectIdSet.h"
#include "../../dataaccess/datasource/DataSourceInfoManager.h"
#include "../../dataaccess/datasource/DataSourceManager.h"
#include "../../dataaccess/datasource/DataSourceFactory.h"
#include "../../dataaccess/query/And.h"
#include "../../dataaccess/query/DataSetName.h"
#include "../../dataaccess/query/Expression.h"
#include "../../dataaccess/query/Field.h"
#include "../../dataaccess/query/Fields.h"
#include "../../dataaccess/query/From.h"
#include "../../dataaccess/query/FromItem.h"
#include "../../dataaccess/query/LiteralInt32.h"
#include "../../dataaccess/query/PropertyName.h"
#include "../../dataaccess/query/ST_SetSRID.h"
#include "../../dataaccess/query/ST_Transform.h"
#include "../../dataaccess/query/Where.h"
#include "../../dataaccess/utils/Utils.h"
#include "../../datatype/SimpleData.h"
#include "../../geometry/GeometryProperty.h"
#include "../../maptools/QueryLayer.h"
#include "../../qt/widgets/datasource/selector/DataSourceSelectorDialog.h"
#include "../../qt/widgets/layer/utils/DataSet2Layer.h"
#include "../../qt/widgets/utils/DoubleListWidget.h"

#include "../ComplexData.h"
#include "../Union.h"

#include "UnionDialog.h"
#include "ui_UnionDialogForm.h"

// Qt
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>

// BOOST
#include <boost/filesystem.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

Q_DECLARE_METATYPE(te::map::AbstractLayerPtr)

te::vp::UnionDialog::UnionDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f),
    m_ui(new Ui::UnionDialogForm),
    m_layers(std::list<te::map::AbstractLayerPtr>())
{
// add controls
  m_ui->setupUi(this);

  m_ui->m_imgLabel->setPixmap(
      QIcon::fromTheme("vp-union-hint").pixmap(112, 48));

  m_ui->m_targetDatasourceToolButton->setIcon(QIcon::fromTheme("datasource"));

  //add double list widget to this form
  m_doubleListWidget.reset(new te::qt::widgets::DoubleListWidget(
      m_ui->m_specificParamsTabWidget->widget(0)));
  m_doubleListWidget->setLeftLabel("");
  m_doubleListWidget->setRightLabel("");

  QGridLayout* layout =
      new QGridLayout(m_ui->m_specificParamsTabWidget->widget(0));
  layout->addWidget(m_doubleListWidget.get());
  layout->setContentsMargins(0, 0, 0, 0);

  QSize iconSize(96, 48);

  m_ui->m_singleRadioButton->setIconSize(iconSize);
  m_ui->m_singleRadioButton->setIcon(QIcon::fromTheme("vp-single-objects"));

  m_ui->m_multiRadioButton->setIconSize(iconSize);
  m_ui->m_multiRadioButton->setIcon(QIcon::fromTheme("vp-multi-objects"));

  connect(m_ui->m_firstLayerComboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onInputLayerComboBoxChanged(int)));

  connect(m_ui->m_secondLayerComboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onOverlayLayerComboBoxChanged(int)));

  connect(m_ui->m_targetDatasourceToolButton, SIGNAL(pressed()), this,
          SLOT(onTargetDatasourceToolButtonPressed()));

  connect(m_ui->m_targetFileToolButton, SIGNAL(pressed()), this,
          SLOT(onTargetFileToolButtonPressed()));

  connect(m_ui->m_okPushButton, SIGNAL(clicked()), this,
          SLOT(onOkPushButtonClicked()));

  connect(m_ui->m_cancelPushButton, SIGNAL(clicked()), this,
          SLOT(onCancelPushButtonClicked()));

  m_ui->m_helpPushButton->setNameSpace("dpi.inpe.br.plugins"); 
  m_ui->m_helpPushButton->setPageReference("plugins/vp/vp_union.html");
}

te::vp::UnionDialog::~UnionDialog()
{
}

void te::vp::UnionDialog::setLayers(std::list<te::map::AbstractLayerPtr> layers)
{
  std::list<te::map::AbstractLayerPtr>::iterator it = layers.begin();

  while (it != layers.end())
  {
    std::unique_ptr<te::da::DataSetType> dsType =
        std::move(it->get()->getSchema());

    if (dsType->hasGeom())
    {
      m_layers.push_back(*it);
    }

    ++it;
  }

  updateFirstLayerComboBox();

  updateSecondLayerComboBox();

  updateDoubleListWidget();
}

te::map::AbstractLayerPtr te::vp::UnionDialog::getLayer()
{
  return m_outputLayer;
}

te::da::Select* te::vp::UnionDialog::getSelectQueryFromLayer(
    te::map::AbstractLayerPtr layer, bool onlySelectedObjects, int srid)
{
// Do a Cast from AbstractLayerPtr to DataSetLayer or QueryLayer.
  te::map::DataSetLayer* dataSetLayer = 0;
  te::map::QueryLayer* queryLayer = 0;

  if (layer->getType() == "DATASETLAYER")
  {
    dataSetLayer = dynamic_cast<te::map::DataSetLayer*>(layer.get());
    if (!dataSetLayer)
    {
      QMessageBox::information(
          this, tr("Error"),
          tr("Can not execute this operation on this type of layer."));
      return 0;
    }
  }
  else if (layer->getType() == "QUERYLAYER")
  {
    queryLayer = dynamic_cast<te::map::QueryLayer*>(layer.get());
    if (!queryLayer)
    {
      QMessageBox::information(
          this, tr("Error"),
          tr("Can not execute this operation on this type of layer."));
      return 0;
    }
  }
  else
  {
    QMessageBox::information(
        this, tr("Error"),
        tr("Can not execute this operation on this type of layer."));
    return 0;
  }

// Select query
  te::da::Select* select = new te::da::Select();

// From dataSetLayer
  if (dataSetLayer)
  {
    te::da::Fields* fields = new te::da::Fields;

    if (srid == 0)
    {
      te::da::Field* f_all = new te::da::Field("*");
      fields->push_back(f_all);
    }
    else
    {
      std::unique_ptr<te::da::DataSetType> dsType =
          std::move(dataSetLayer->getSchema());

      std::vector<te::dt::Property*> properties = dsType.get()->getProperties();

      for (std::size_t i = 0; i < properties.size(); ++i)
      {
        if (properties[i]->getType() != te::dt::GEOMETRY_TYPE)
        {
          te::da::Field* currentField =
              new te::da::Field(properties[i]->getName());
          fields->push_back(currentField);
        }
        else
        {
          te::da::LiteralInt32* litSRID = new te::da::LiteralInt32(srid);

          te::da::Expression* eSetSRID = new te::da::ST_SetSRID(
              new te::da::PropertyName(properties[i]->getName()), litSRID);

          te::da::Expression* eTransform =
              new te::da::ST_Transform(eSetSRID, srid);

          te::da::Field* geomField =
              new te::da::Field(*eTransform, properties[i]->getName());
          fields->push_back(geomField);
        }
      }
    }

    select->setFields(fields);

    te::da::From* from = new te::da::From;
    te::da::FromItem* fromItem =
        new te::da::DataSetName(dataSetLayer->getDataSetName());
    from->push_back(fromItem);
    select->setFrom(from);

    if (onlySelectedObjects)
    {
      const te::da::ObjectIdSet* oidSet = layer->getSelected();
      if (!oidSet)
      {
        QMessageBox::information(
            this, tr("Union"),
            tr("Select the layer objects to perform the operation."));
        return 0;
      }

      te::da::Where* w_oid = new te::da::Where(oidSet->getExpression());
      select->setWhere(w_oid);
    }
  }
// From queryLayer
  else
  {
    select = queryLayer->getQuery();

    if (srid != 0)
    {
      std::unique_ptr<te::da::DataSetType> dsType =
          std::move(queryLayer->getSchema());

      std::vector<te::dt::Property*> properties = dsType.get()->getProperties();

      te::da::Fields* fields = new te::da::Fields;

      for (std::size_t i = 0; i < properties.size(); ++i)
      {
        if (properties[i]->getType() != te::dt::GEOMETRY_TYPE)
        {
          te::da::Field* currentField =
              new te::da::Field(properties[i]->getName());
          fields->push_back(currentField);
        }
        else
        {
          te::da::LiteralInt32* litSRID = new te::da::LiteralInt32(srid);

          te::da::Expression* eSetSRID = new te::da::ST_SetSRID(
              new te::da::PropertyName(properties[i]->getName()), litSRID);

          te::da::Expression* eTransform =
              new te::da::ST_Transform(eSetSRID, srid);

          te::da::Field* geomField =
              new te::da::Field(*eTransform, properties[i]->getName());
          fields->push_back(geomField);
        }
      }

      select->setFields(fields);

    }

    if (onlySelectedObjects)
    {
      const te::da::ObjectIdSet* oidSet = layer->getSelected();
      if (!oidSet)
      {
        QMessageBox::information(
            this, tr("Union"),
            tr("Select the layer objects to perform the operation."));
        return 0;
      }

      te::da::Where* w = select->getWhere();

      te::da::Expression* e_where =  w->getExp()->clone();
      te::da::Expression* e_oidWhere = oidSet->getExpression();

      te::da::And* andPtr = new te::da::And(e_where, e_oidWhere);

      te::da::Where* newWhere = new te::da::Where(andPtr);

      select->setWhere(newWhere);
    }
  }

  return select;
}

te::vp::UnionDialog::DataStruct te::vp::UnionDialog::getDataStructFromLayer(
    te::map::AbstractLayerPtr layer, bool onlySelectedObjects, int srid)
{
  DataStruct data;

  std::string sourceId = layer->getDataSourceId();

  te::da::DataSourcePtr dataSource =
      te::da::DataSourceManager::getInstance().find(sourceId);

  std::unique_ptr<te::da::DataSetType> dataSetType =
      std::move(layer->getSchema());

  std::unique_ptr<te::da::DataSet> dataSet =
      std::move(dataSource->getDataSet(dataSetType->getName()));

  if (dataSetType.get() && dataSet.get())
  {
    if (onlySelectedObjects)
    {
      const te::da::ObjectIdSet* oidSet = layer->getSelected();
      if (!oidSet)
      {
        QMessageBox::information(
            this, tr("Union"),
            tr("Select the layer objects to perform the operation."));
        return data;
      }

      dataSet = dataSource->getDataSet(dataSetType->getName(), oidSet);

      if (!dataSet.get())
        return data;
    }

    if (srid != 0)
    {
      std::unique_ptr<te::da::DataSetTypeConverter> converter(
          new te::da::DataSetTypeConverter(dataSetType.get(),
                                           dataSource->getCapabilities(),
                                           dataSource->getEncoding()));

      te::da::AssociateDataSetTypeConverterSRID(converter.get(), srid);

      dataSetType.reset(new te::da::DataSetType(*converter->getResult()));

      te::da::DataSetAdapter* dataSetAdapter =
          te::da::CreateAdapter(dataSet.release(), converter.get());

      if (!dataSetAdapter)
        return data;

      dataSet.reset(dataSetAdapter);
    }

    data.m_dataSetType = dataSetType.release();
    data.m_dataSet = dataSet.release();
  }

  return data;
}

std::vector<std::pair<std::string, std::string> >
te::vp::UnionDialog::getSelectedProperties()
{
  std::vector<std::string> outVec = m_doubleListWidget->getOutputValues();
  std::vector<std::pair<std::string, std::string> > result;

  for (std::size_t i = 0; i < outVec.size(); ++i)
  {
    std::vector<std::string> tok;
    te::common::Tokenize(outVec[i], tok, ": ");

    std::pair<std::string, std::string> p;

    if (tok[0] == m_firstSelectedLayer->getTitle())
    {
      p.first = "FIRST";
    }
    else
    {
      p.first = "SECOND";
    }

    p.second = tok[1];

    result.push_back(p);
  }

  return result;
}

void te::vp::UnionDialog::updateFirstLayerComboBox()
{
  std::list<te::map::AbstractLayerPtr>::iterator it = m_layers.begin();

  disconnect(m_ui->m_firstLayerComboBox, SIGNAL(currentIndexChanged(int)), this,
             SLOT(onFirstLayerComboBoxChanged(int)));

  while (it != m_layers.end())
  {
    m_ui->m_firstLayerComboBox->addItem(QString(it->get()->getTitle().c_str()),
                                        QVariant::fromValue(*it));
    ++it;
  }

  connect(m_ui->m_firstLayerComboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onFirstLayerComboBoxChanged(int)));

  QVariant varLayer = m_ui->m_firstLayerComboBox->itemData(
      m_ui->m_firstLayerComboBox->currentIndex(), Qt::UserRole);

  te::map::AbstractLayerPtr layer = varLayer.value<te::map::AbstractLayerPtr>();

  m_firstSelectedLayer = layer;
}

void te::vp::UnionDialog::updateSecondLayerComboBox()
{
  int currIndex = m_ui->m_firstLayerComboBox->currentIndex();

  std::list<te::map::AbstractLayerPtr>::iterator it = m_layers.begin();

  disconnect(m_ui->m_secondLayerComboBox, SIGNAL(currentIndexChanged(int)),
             this, SLOT(onSecondLayerComboBoxChanged(int)));

  while (it != m_layers.end())
  {
    m_ui->m_secondLayerComboBox->addItem(QString(it->get()->getTitle().c_str()),
                                         QVariant::fromValue(*it));
    ++it;
  }

  m_ui->m_secondLayerComboBox->removeItem(currIndex);

  connect(m_ui->m_secondLayerComboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onSecondLayerComboBoxChanged(int)));

  QVariant varLayer = m_ui->m_secondLayerComboBox->itemData(
      m_ui->m_secondLayerComboBox->currentIndex(), Qt::UserRole);

  te::map::AbstractLayerPtr layer = varLayer.value<te::map::AbstractLayerPtr>();

  m_secondSelectedLayer = layer;
}

void te::vp::UnionDialog::updateDoubleListWidget()
{
  std::vector<std::string> inputValues;

  std::unique_ptr<te::da::DataSetType> firstSchema;
  std::unique_ptr<te::da::DataSetType> secondSchema;

  if (m_firstSelectedLayer)
    firstSchema = std::move(m_firstSelectedLayer->getSchema());
  else
    return;

  if (m_secondSelectedLayer)
    secondSchema = std::move(m_secondSelectedLayer->getSchema());
  else
    return;

  std::vector<te::dt::Property*> firstProps = firstSchema->getProperties();
  for (std::size_t i = 0; i < firstProps.size(); ++i)
  {
    if (firstProps[i]->getType() != te::dt::GEOMETRY_TYPE)
    {
      std::string name = firstSchema->getTitle();

      if (name.empty())
        name = firstSchema->getName();

      inputValues.push_back(name + ": " + firstProps[i]->getName());
    }
  }

  std::vector<te::dt::Property*> secondProps = secondSchema->getProperties();
  for (std::size_t i = 0; i < secondProps.size(); ++i)
  {
    if (secondProps[i]->getType() != te::dt::GEOMETRY_TYPE)
    {
      std::string name = secondSchema->getTitle();

      if (name.empty())
        name = secondSchema->getName();

      inputValues.push_back(secondSchema->getTitle() + ": " +
                            secondProps[i]->getName());
    }
  }

  m_doubleListWidget->setInputValues(inputValues);
}

void te::vp::UnionDialog::onFirstLayerComboBoxChanged(int index)
{
  QVariant varLayer = m_ui->m_firstLayerComboBox->itemData(index, Qt::UserRole);
  te::map::AbstractLayerPtr layer = varLayer.value<te::map::AbstractLayerPtr>();

  m_ui->m_secondLayerComboBox->clear();

  m_firstSelectedLayer = layer;

  updateSecondLayerComboBox();

  updateDoubleListWidget();
}

void te::vp::UnionDialog::onSecondLayerComboBoxChanged(int index)
{
  QVariant varLayer =
      m_ui->m_secondLayerComboBox->itemData(index, Qt::UserRole);

  te::map::AbstractLayerPtr layer = varLayer.value<te::map::AbstractLayerPtr>();

  m_secondSelectedLayer = layer;

  updateDoubleListWidget();
}

void te::vp::UnionDialog::onTargetDatasourceToolButtonPressed()
{
  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_newLayerNameLineEdit->setEnabled(true);
  te::qt::widgets::DataSourceSelectorDialog dlg(this);
  dlg.exec();

  std::list<te::da::DataSourceInfoPtr> dsPtrList = dlg.getSelecteds();

  if (dsPtrList.empty())
    return;

  std::list<te::da::DataSourceInfoPtr>::iterator it = dsPtrList.begin();

  m_ui->m_repositoryLineEdit->setText(QString(it->get()->getTitle().c_str()));

  m_outputDatasource = *it;

  m_toFile = false;
}

void te::vp::UnionDialog::onTargetFileToolButtonPressed()
{
  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_repositoryLineEdit->clear();

  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save as..."), QString(), tr("Shapefile (*.shp *.SHP);;"), 0,
      QFileDialog::DontConfirmOverwrite);

  if (fileName.isEmpty())
    return;

  boost::filesystem::path outfile(fileName.toUtf8().data());
  std::string aux = outfile.leaf().string();
  m_ui->m_newLayerNameLineEdit->setText(aux.c_str());
  aux = outfile.string();
  m_ui->m_repositoryLineEdit->setText(aux.c_str());

  m_toFile = true;
  m_ui->m_newLayerNameLineEdit->setEnabled(false);
}

void te::vp::UnionDialog::onOkPushButtonClicked()
{
  if(m_ui->m_firstLayerComboBox->currentText().isEmpty() ||
     m_ui->m_secondLayerComboBox->currentText().isEmpty())
  {
    QMessageBox::information(
        this, tr("Merge"), tr("It is necessary at least two layer to operat!"));
    return;
  }

  // Checking consistency of output paramenters
  if (m_ui->m_repositoryLineEdit->text().isEmpty())
  {
    QMessageBox::information(
        this, tr("Merge"), tr("Select a repository for the resulting layer!"));

    return;
  }

  if (m_ui->m_newLayerNameLineEdit->text().isEmpty())
  {
    QMessageBox::information(this, tr("Merge"),
                             tr("Define a name for the resulting layer!"));
    return;
  }

  int firstSrid = m_firstSelectedLayer->getSRID();
  int secondSrid = m_secondSelectedLayer->getSRID();

  if (firstSrid <= 0 || secondSrid <= 0)
  {
    QMessageBox::information(this, tr("Merge"),
                             tr("All layers must have SRID!"));
    return;
  }

  std::string outputdataset =
      m_ui->m_newLayerNameLineEdit->text().toUtf8().data();

  std::string firstSourceId = m_firstSelectedLayer->getDataSourceId();
  std::string secondSourceId = m_secondSelectedLayer->getDataSourceId();

  te::da::DataSourcePtr firstSource =
      te::da::DataSourceManager::getInstance().find(firstSourceId);

  te::da::DataSourcePtr secondSource =
      te::da::DataSourceManager::getInstance().find(secondSourceId);

  std::unique_ptr<te::da::DataSetTypeConverter> firstConverter(
      new te::da::DataSetTypeConverter(m_firstSelectedLayer->getSchema().get(),
                                       firstSource->getCapabilities(),
                                       firstSource->getEncoding()));

  te::da::AssociateDataSetTypeConverterSRID(firstConverter.get(),
                                            m_firstSelectedLayer->getSRID());

  std::unique_ptr<te::da::DataSetTypeConverter> secondConverter(
      new te::da::DataSetTypeConverter(m_secondSelectedLayer->getSchema().get(),
                                       secondSource->getCapabilities(),
                                       secondSource->getEncoding()));

  te::da::AssociateDataSetTypeConverterSRID(secondConverter.get(),
                                            m_secondSelectedLayer->getSRID(),
                                            m_firstSelectedLayer->getSRID());

  te::da::DataSetType* firstDst(firstConverter->getResult());
  te::da::DataSetType* secondDst(secondConverter->getResult());

  te::da::DataSetAdapter* firstAdapter = te::da::CreateAdapter(
      m_firstSelectedLayer->getData().release(), firstConverter.get());

  te::da::DataSetAdapter* secondAdapter = te::da::CreateAdapter(
      m_secondSelectedLayer->getData().release(), secondConverter.get());

  std::unique_ptr<te::da::DataSet> firstDs(firstAdapter);
  std::unique_ptr<te::da::DataSet> secondDs(secondAdapter);

  try
  {
    te::vp::InputParams inputParam;
    inputParam.m_inputDataSet = firstDs.get();
    inputParam.m_inputDataSetType = firstDst;
    inputParam.m_inputDataSource = firstSource;

    te::vp::InputParams mergeParam;
    mergeParam.m_inputDataSet = secondDs.get();
    mergeParam.m_inputDataSetType = secondDst;
    mergeParam.m_inputDataSource = secondSource;

    std::vector<te::vp::InputParams> params;
    params.push_back(inputParam);
    params.push_back(mergeParam);

    te::vp::ComplexData<std::vector<std::pair<std::string, std::string> > >*
        attrs = new te::vp::ComplexData<
            std::vector<std::pair<std::string, std::string> > >(
            getSelectedProperties());

    std::map<std::string, te::dt::AbstractData*> specificParams;
    specificParams["ATTRIBUTES"] = attrs;

    specificParams["IS_COLLECTION"] =
        new te::dt::SimpleData<bool, te::dt::BOOLEAN_TYPE>(
            !m_ui->m_singleRadioButton->isChecked());

    te::vp::AlgorithmParams* aParams = new te::vp::AlgorithmParams();
    aParams->setInputParams(params);
    aParams->setOutputDataSetName(outputdataset);
    aParams->setSpecificParams(specificParams);

    te::da::DataSourcePtr auxSource;
    std::string ogrDsinfo("file://");
    boost::filesystem::path ogrUri;

    if (m_toFile)
    {
      ogrUri = m_ui->m_repositoryLineEdit->text().toUtf8().data();

      if (te::core::FileSystem::exists(ogrUri.string()))
      {
        QMessageBox::information(this, tr("Merge"),
                                 tr("Output file already exists. Remove it or "
                                    "select a new name and try again."));
        return;
      }

      std::size_t idx = outputdataset.find(".");
      if (idx != std::string::npos)
        outputdataset = outputdataset.substr(0, idx);

      aParams->setOutputDataSetName(outputdataset);

      ogrDsinfo += ogrUri.string();

      auxSource.reset(
          te::da::DataSourceFactory::make("OGR", ogrDsinfo).release());
      auxSource->open();

      if (auxSource->dataSetExists(outputdataset))
      {
        QMessageBox::information(
            this, tr("Merge"),
            tr("There is already a dataset with the requested name in the "
               "output data source. Remove it or select a new name and try "
               "again."));
        return;
      }
    }
    else
    {
      auxSource = te::da::GetDataSource(m_outputDatasource->getId());
      if (!auxSource)
      {
        QMessageBox::information(
            this, tr("Merge"),
            tr("The selected output datasource can not be accessed."));
        return;
      }
      if (auxSource->dataSetExists(outputdataset))
      {
        QMessageBox::information(this, tr("Merge"),
                                 tr("Dataset already exists. Remove it or "
                                    "select a new name and try again."));
        return;
      }
    }

    aParams->setOutputDataSource(auxSource);

    te::vp::Union uni;
    uni.executeMemory(aParams);

    delete aParams;

    if (m_toFile)
    {
      // let's include the new datasource in the managers
      boost::uuids::basic_random_generator<boost::mt19937> gen;
      boost::uuids::uuid u = gen();
      std::string id = boost::uuids::to_string(u);

      te::da::DataSourceInfoPtr ds(new te::da::DataSourceInfo);
      ds->setConnInfo(ogrDsinfo);
      ds->setTitle(ogrUri.stem().string());
      ds->setAccessDriver("OGR");
      ds->setType("OGR");
      ds->setDescription(ogrUri.string());
      ds->setId(id);

      te::da::DataSourcePtr newds =
          te::da::DataSourceManager::getInstance().get(id, "OGR",
                                                       ds->getConnInfo());
      newds->open();
      te::da::DataSourceInfoManager::getInstance().add(ds);
      m_outputDatasource = ds;
    }

    // creating a layer for the result
    te::da::DataSourcePtr outDataSource =
        te::da::GetDataSource(m_outputDatasource->getId());

    te::qt::widgets::DataSet2Layer converter(m_outputDatasource->getId());

    te::da::DataSetTypePtr dt(
        outDataSource->getDataSetType(outputdataset).release());
    m_outputLayer = converter(dt);
  }
  catch (const te::common::Exception& e)
  {
    QMessageBox::warning(this, tr("Union"), e.what());
    return;
  }
  catch (const std::exception& e)
  {
    QMessageBox::warning(this, tr("Union"), e.what());
    return;
  }

  accept();
}

void te::vp::UnionDialog::onCancelPushButtonClicked()
{
  reject();
}

