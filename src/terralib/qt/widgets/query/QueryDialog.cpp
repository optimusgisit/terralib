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
  \file terralib/qt/widgets/query/QueryDialog.h

  \brief This file defines a class for a Query Dialog Dialog.
*/

// TerraLib
#include "../../../dataaccess/dataset/DataSet.h"
#include "../../../dataaccess/dataset/DataSetType.h"
#include "../../../dataaccess/dataset/ObjectIdSet.h"
#include "../../../dataaccess/datasource/DataSourceCapabilities.h"
#include "../../../dataaccess/query/FunctionNames.h"
#include "../../../dataaccess/query/QueryCapabilities.h"
#include "../../../dataaccess/utils/Utils.h"
#include "../../../maptools/DataSetLayer.h"
#include "../utils/ColorPickerToolButton.h"
#include "QueryDialog.h"
#include "WhereClauseWidget.h"
#include "ui_QueryDialogForm.h"

// Qt
#include <QGridLayout>
#include <QMessageBox>

// STL
#include <cassert>
#include <memory>

Q_DECLARE_METATYPE(te::map::AbstractLayerPtr);

te::qt::widgets::QueryDialog::QueryDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f),
    m_ui(new Ui::QueryDialogForm)
{
  m_ui->setupUi(this);

  // Build form
  QGridLayout* layout = new QGridLayout(m_ui->m_widget);
  m_whereClauseWidget.reset( new te::qt::widgets::WhereClauseWidget(m_ui->m_widget));
  layout->addWidget(m_whereClauseWidget.get());
  layout->setContentsMargins(0, 0, 0, 0);

  // Color Picker
  m_colorPicker = new te::qt::widgets::ColorPickerToolButton(this);
  m_colorPicker->setFixedSize(70, 24);
  m_colorPicker->setColor(QColor(255, 255, 0, 128));

  // Adjusting...
  QGridLayout* colorPickerLayout = new QGridLayout(m_ui->m_colorPickerFrame);
  colorPickerLayout->setContentsMargins(0, 0, 0, 0);
  colorPickerLayout->addWidget(m_colorPicker);

  // Signals¨& slots
  connect(m_ui->m_inputLayerComboBox, SIGNAL(activated(QString)), this, SLOT(onInputLayerActivated(QString)));
  connect(m_ui->m_applyPushButton, SIGNAL(clicked()), this, SLOT(onApplyPushButtonClicked()));

  //
  m_ui->m_helpPushButton->setPageReference("widgets/query/query_dialog.html");
}

te::qt::widgets::QueryDialog::~QueryDialog()
{
}

void te::qt::widgets::QueryDialog::setLayerList(std::list<te::map::AbstractLayerPtr>& layerList)
{
  m_whereClauseWidget->setLayerList(layerList);

  m_ui->m_inputLayerComboBox->clear();

  std::list<te::map::AbstractLayerPtr>::iterator it = layerList.begin();

  while(it != layerList.end())
  {
    te::map::AbstractLayerPtr l = *it;

    std::auto_ptr<te::da::DataSetType> dsType = l->getSchema();

    te::map::DataSetLayer* dsLayer = dynamic_cast<te::map::DataSetLayer*>(l.get());

    if(dsLayer && dsType->hasGeom())
      m_ui->m_inputLayerComboBox->addItem(l->getTitle().c_str(), QVariant::fromValue(l));

    ++it;
  }

  if(m_ui->m_inputLayerComboBox->count() > 0)
  {
    QString s = m_ui->m_inputLayerComboBox->currentText();

    onInputLayerActivated(s);
  }
}

void te::qt::widgets::QueryDialog::setCurrentLayer(te::map::AbstractLayerPtr layer)
{
  for(int i = 0; i < m_ui->m_inputLayerComboBox->count(); ++i)
  {
    QVariant varLayer = m_ui->m_inputLayerComboBox->itemData(i, Qt::UserRole);
    te::map::AbstractLayerPtr l = varLayer.value<te::map::AbstractLayerPtr>();

    if(layer == l)
    {
      m_ui->m_inputLayerComboBox->setCurrentIndex(i);
      break;
    }
  }

  if(m_ui->m_inputLayerComboBox->count() > 0)
  {
    QString s = m_ui->m_inputLayerComboBox->currentText();

    onInputLayerActivated(s);
  }
}

std::string te::qt::widgets::QueryDialog::setAliasName(std::string value)
{
  if(value.empty() == false)
  {
    std::string dataSetName = value;
    std::string aliasName = value;

    int pos = dataSetName.find(".");
    if(pos != std::string::npos)
    {
      aliasName = dataSetName.substr(pos + 1, dataSetName.size() - 1);
    }

    return aliasName;
  }
  return "";
}

te::da::Where* te::qt::widgets::QueryDialog::getWhere()
{
  return m_whereClauseWidget->getWhere();
}

void te::qt::widgets::QueryDialog::layerSelected(te::map::AbstractLayerPtr layer)
{
  //setCurrentLayer(layer);
}

void te::qt::widgets::QueryDialog::layerAdded(te::map::AbstractLayerPtr layer)
{
  //add layer to input layer combo box
  te::map::DataSetLayer* dsLayer = dynamic_cast<te::map::DataSetLayer*>(layer.get());

  if(dsLayer)
    m_ui->m_inputLayerComboBox->addItem(layer->getTitle().c_str(), QVariant::fromValue(layer));

  int curIdx = m_ui->m_inputLayerComboBox->currentIndex();

  QVariant varLayer = m_ui->m_inputLayerComboBox->itemData(curIdx, Qt::UserRole);
  te::map::AbstractLayerPtr l = varLayer.value<te::map::AbstractLayerPtr>();

  if(l == layer)
  {
    QString s = m_ui->m_inputLayerComboBox->currentText();

    onInputLayerActivated(s);
  }

  //add layer to spatial layer combo box
  if (dsLayer)
    m_whereClauseWidget->addLayer(layer);
}

void te::qt::widgets::QueryDialog::layerRemoved(te::map::AbstractLayerPtr layer)
{
  //remove layer from input layer combo box
  int curIdx = m_ui->m_inputLayerComboBox->currentIndex();

  for(int i = 0; i < m_ui->m_inputLayerComboBox->count(); ++i)
  {
    QVariant varLayer = m_ui->m_inputLayerComboBox->itemData(i, Qt::UserRole);
    te::map::AbstractLayerPtr l = varLayer.value<te::map::AbstractLayerPtr>();

    if(l == layer)
    {
      m_ui->m_inputLayerComboBox->removeItem(i);

      if(i == curIdx)
      {
        if(m_ui->m_inputLayerComboBox->count() != 0)
        {
          m_ui->m_inputLayerComboBox->setCurrentIndex(0);
        
          QString s = m_ui->m_inputLayerComboBox->currentText();

          onInputLayerActivated(s);
        }
        else
        {
          m_whereClauseWidget->resetInterface();
        }
      }
    }
  }

  //remove layer from spatial layer combo box
  m_whereClauseWidget->removeLayer(layer);
}

void te::qt::widgets::QueryDialog::onInputLayerActivated(QString value)
{
  m_whereClauseWidget->resetInterface();

  // Gets the input layer
  int idxLayer = m_ui->m_inputLayerComboBox->currentIndex();

  QVariant varLayer = m_ui->m_inputLayerComboBox->itemData(idxLayer, Qt::UserRole);

  te::map::AbstractLayerPtr layer = varLayer.value<te::map::AbstractLayerPtr>();

  te::map::DataSetLayer* dsLayer = dynamic_cast<te::map::DataSetLayer*>(layer.get());

  // Sets the DataSource
  std::string dataSourceName = dsLayer->getDataSourceId();

  te::da::DataSourcePtr dsPtr = te::da::GetDataSource(dataSourceName);

  if(!dsPtr->isOpened())
      dsPtr->open();

  m_whereClauseWidget->setDataSource(dsPtr);

  // Gets the capabilities
  std::vector<std::string> vecOperators;
  std::vector<std::string> vecSpatialOperators;
  std::vector<std::string> vecConnectors;

  te::da::DataSourceCapabilities dsCap = dsPtr->getCapabilities();
  te::da::QueryCapabilities queryCap = dsCap.getQueryCapabilities();

  std::set<std::string>::iterator it;

  it = queryCap.getArithmeticOperators().begin();
  while(it != queryCap.getArithmeticOperators().end())
  {
    vecOperators.push_back(*it);
    ++it;
  }

  it = queryCap.getComparsionOperators().begin();
  while(it != queryCap.getComparsionOperators().end())
  {
    vecOperators.push_back(*it);
    ++it;
  }

  m_whereClauseWidget->setOperatorsList(vecOperators);

  it = queryCap.getSpatialTopologicOperators().begin();
  while(it != queryCap.getSpatialTopologicOperators().end())
  {
    vecSpatialOperators.push_back(*it);
    ++it;
  }

  // For while! TODO: Revision!!!
  if(vecSpatialOperators.size() == 1 && vecSpatialOperators[0] == te::da::FunctionNames::sm_ST_EnvelopeIntersects)
  {
    vecSpatialOperators.clear();
    vecSpatialOperators.push_back(te::da::FunctionNames::sm_ST_Intersects);
    vecSpatialOperators.push_back(te::da::FunctionNames::sm_ST_Disjoint);
    vecSpatialOperators.push_back(te::da::FunctionNames::sm_ST_Touches);
    vecSpatialOperators.push_back(te::da::FunctionNames::sm_ST_Overlaps);
    vecSpatialOperators.push_back(te::da::FunctionNames::sm_ST_Crosses);
    vecSpatialOperators.push_back(te::da::FunctionNames::sm_ST_Within);
    vecSpatialOperators.push_back(te::da::FunctionNames::sm_ST_Contains);
    vecSpatialOperators.push_back(te::da::FunctionNames::sm_ST_Equals);
  }

  m_whereClauseWidget->setSpatialOperatorsList(vecSpatialOperators);

  it = queryCap.getLogicalOperators().begin();
  while(it != queryCap.getLogicalOperators().end())
  {
    vecConnectors.push_back(*it);
    ++it;
  }

  m_whereClauseWidget->setConnectorsList(vecConnectors);

  // Gets data set information
  std::auto_ptr<te::da::DataSetType> dsType = dsLayer->getSchema();
  
  std::string dsName = dsType->getName();
  std::string aliasName = setAliasName(dsName);

  std::vector<std::pair<std::string, std::string> > list;
  list.push_back(std::pair<std::string, std::string>(dsName, aliasName));

  m_whereClauseWidget->setFromItems(list);

  // Get properties
  std::vector<std::string> inputProperties;
  std::vector<std::string> geomProperties;

  for(size_t i = 0; i < dsType->size(); ++i)
  {
    std::string propName = dsType->getProperty(i)->getName();

    if(dsType->getProperty(i)->getType() == te::dt::GEOMETRY_TYPE)
      geomProperties.push_back(propName);
    else
      inputProperties.push_back(propName);
  }

  m_whereClauseWidget->setAttributeList(inputProperties);
  m_whereClauseWidget->setGeomAttributeList(geomProperties, dsLayer->getSRID());
}

void te::qt::widgets::QueryDialog::onApplyPushButtonClicked()
{
  // Gets the defined restriction
  te::da::Where* wh = getWhere();
  if(wh == 0 || wh->getExp() == 0)
  {
    QMessageBox::information(this, tr("Query"), tr("Add a restriction expression first."));
    return;
  }

  // Gets the selected layer
  int index = m_ui->m_inputLayerComboBox->currentIndex();

  QVariant varLayer = m_ui->m_inputLayerComboBox->itemData(index, Qt::UserRole);

  te::map::AbstractLayerPtr layer = varLayer.value<te::map::AbstractLayerPtr>();

  assert(layer.get());

  // Let's execute the query
  try
  {
    setCursor(Qt::WaitCursor);

    // The filter expression
    te::da::Expression* e = wh->getExp()->clone();

    // Gets the layer schema
    std::auto_ptr<const te::map::LayerSchema> schema(layer->getSchema());

    // Gets the dataset
    std::auto_ptr<te::da::DataSet> dataset = layer->getData(e);
    assert(dataset.get());

    if(m_ui->m_newSelRadioButton->isChecked())
    {
      // Generates the oids
      dataset->moveBeforeFirst();
      te::da::ObjectIdSet* oids = te::da::GenerateOIDSet(dataset.get(), schema.get());

      oids->setExpression(wh->getExp()->clone(), false);

      layer->clearSelected();
      layer->select(oids);

      emit layerSelectedObjectsChanged(layer);
    }
    else if(m_ui->m_addSelRadioButton->isChecked())
    {
      // Generates the oids
      dataset->moveBeforeFirst();
      te::da::ObjectIdSet* oids = te::da::GenerateOIDSet(dataset.get(), schema.get());

      oids->setExpression(wh->getExp()->clone(), false);

      layer->select(oids);

      emit layerSelectedObjectsChanged(layer);
    }

    emit highlightLayerObjects(layer, dataset.get(), m_colorPicker->getColor());

    setCursor(Qt::ArrowCursor);

    QMessageBox::information(this, tr("Query"), tr("Query executed with successfully."));
  }
  catch(std::exception& e)
  {
    setCursor(Qt::ArrowCursor);
    QMessageBox::information(this, tr("Query"), e.what());
  }
}


