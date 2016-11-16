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

#include "WMSLayerSelectorDialog.h"

//TerraLib
#include "ui_WMSLayerSelectorDialogForm.h"
#include "../../../../core/translator/Translator.h"
#include "../../../../core/uri/URI.h"
#include "../../../../core/uri/Utils.h"
#include "../../../core/CurlWrapper.h"
#include "../../../../dataaccess/datasource/DataSource.h"
#include "../dataaccess/Transactor.h"

//QT
#include <QHeaderView>
#include <QVBoxLayout>
#include <QMessageBox>

te::ws::ogc::wms::qt::WMSLayerSelectorDialog::WMSLayerSelectorDialog(QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f),
    m_ui(new Ui::WMSLayerSelectorDialogForm),
    m_currentLayerItem(0),
    m_refreshingProperties(false)
{
  m_ui->setupUi(this);

  m_mapPreview.reset(new WMSLayerDisplay(m_ui->m_layerPreviewGroupBox));
  QVBoxLayout* mapPreviewGroupBoxLayout = new QVBoxLayout(m_ui->m_layerPreviewGroupBox);
  mapPreviewGroupBoxLayout->addWidget(m_mapPreview.get());
  m_mapPreview->show();

  connect(m_ui->m_layersTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(currentLayerChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
  connect(m_ui->m_layersTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(checkLayerItemChanged(QTreeWidgetItem*,int)));
  connect(m_ui->m_okPushButton, SIGNAL(pressed()), this, SLOT(okPushButtonPressed()));
  connect(m_ui->m_imgFormatComboBox,  SIGNAL(currentIndexChanged(int)), this, SLOT(imgFormatIndexChanged(int)));
  connect(m_ui->m_timeDimComboBox,  SIGNAL(currentIndexChanged(int)), this, SLOT(timeDimIndexChanged(int)));
  connect(m_ui->m_crsComboBox,  SIGNAL(currentIndexChanged(int)), this, SLOT(crsIndexChanged(int)));
  connect(m_ui->m_styleComboBox,  SIGNAL(currentIndexChanged(int)), this, SLOT(styleIndexChanged(int)));
  connect(m_ui->m_transparencyCheckBox,  SIGNAL(stateChanged(int)), this, SLOT(transparencyStateChanged(int)));
}

te::ws::ogc::wms::qt::WMSLayerSelectorDialog::~WMSLayerSelectorDialog()
{

}

void te::ws::ogc::wms::qt::WMSLayerSelectorDialog::set(const te::da::DataSourceInfoPtr &ds, bool useCheckableItems)
{

  m_datasourceInfo = ds;

  if(m_datasourceInfo.get() == 0)
    return;

  QString title(tr("WMS Layer Selection - %1"));
  title = title.arg(QString::fromUtf8(m_datasourceInfo->getTitle().c_str()));

  setWindowTitle(title);

  m_ui->m_serviceTitleLabel->setText(title);

  const te::core::URI& connInfo = m_datasourceInfo->getConnInfo();
  std::map<std::string, std::string> kvp = te::core::Expand(connInfo.query());

  m_client.reset(new te::ws::ogc::WMSClient(kvp["USERDATADIR"], kvp["URI"], kvp["VERSION"]));
  m_client->updateCapabilities();

  m_rootLayer = m_client->getCapabilities().m_capability.m_layer;

  WMSLayerItem* rootItem = new WMSLayerItem(m_rootLayer, m_ui->m_layersTreeWidget);

  m_ui->m_layersTreeWidget->resizeColumnToContents(0);
  m_ui->m_layersTreeWidget->resizeColumnToContents(1);

  m_ui->m_layersTreeWidget->setCurrentItem(rootItem);
  m_ui->m_layersTreeWidget->expandAll();

  std::vector<std::string> imageFormats = m_client->getCapabilities().m_capability.m_request.m_getMap.m_formats;

  for(size_t i = 0; i < imageFormats.size(); i++)
  {
    std::string format = imageFormats[i];

    // image format only.
    if(format.find("image/") != std::string::npos)
    {
      m_ui->m_imgFormatComboBox->addItem(QString(format.c_str()));
    }
  }
}

std::vector<te::ws::ogc::wms::WMSGetMapRequest> te::ws::ogc::wms::qt::WMSLayerSelectorDialog::getCheckedRequests()
{
  std::vector<WMSLayerItem*> checkedLayers = this->getCheckedLayerItens();

  std::vector<te::ws::ogc::wms::WMSGetMapRequest> checkedRequests;

  for(size_t i = 0; i < checkedLayers.size(); i++)
  {
    te::ws::ogc::wms::WMSGetMapRequest request = checkedLayers[i]->getRequest();

    checkedRequests.push_back(request);
  }

  return checkedRequests;
}

void te::ws::ogc::wms::qt::WMSLayerSelectorDialog::currentLayerChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
  m_currentLayerItem = dynamic_cast<WMSLayerItem*>(current);

  if(m_currentLayerItem == 0)
  {
    return;
  }

  te::ws::ogc::wms::Layer currentLayer = m_currentLayerItem->getLayer();

  this->refreshLayerInfoTree(currentLayer);
  this->refreshLayerProperties();

  if(m_currentLayerItem->checkState(0) == Qt::Checked)
  {
    this->refreshMapPreview();
  }
  else
  {
    m_mapPreview->clear();
  }

}

void te::ws::ogc::wms::qt::WMSLayerSelectorDialog::refreshLayerInfoTree(const te::ws::ogc::wms::Layer &currentLayer)
{
  m_ui->m_layerInfoTreeWidget->clear();

  //queryable
  QTreeWidgetItem* queryableItem = new QTreeWidgetItem(m_ui->m_layerInfoTreeWidget);
  QString queryableStr = QString("Queryable: ");
  QString queryableValue = currentLayer.m_queryable ? QString("Is Queryable") : QString ("Is Not Queryable");

  queryableStr += queryableValue;
  queryableItem->setText(0, queryableStr);

  //opaque
  QTreeWidgetItem* opaqueItem = new QTreeWidgetItem(m_ui->m_layerInfoTreeWidget);
  QString opaqueStr = QString("Opaque: ");
  QString opaqueValue = currentLayer.m_opaque ? QString("true") : QString ("false");

  opaqueStr += opaqueValue;
  opaqueItem->setText(0, opaqueStr);
  
  // Minimum Scale
  QTreeWidgetItem* minScaleItem = new QTreeWidgetItem(m_ui->m_layerInfoTreeWidget);

  QString minScaleStr = QString("Min Scale Denominator: ");
  QString minScaleValue = QString::number(currentLayer.m_minScaleDenominator);

  minScaleStr += minScaleValue;
  minScaleItem->setText(0, minScaleStr);

  // Maximum Scale
  QTreeWidgetItem* maxScaleItem = new QTreeWidgetItem(m_ui->m_layerInfoTreeWidget);

  QString maxScaleStr = QString("Max Scale Denominator: ");
  QString maxScaleValue = QString::number(currentLayer.m_maxScaleDenominator);

  maxScaleStr += maxScaleValue;
  maxScaleItem->setText(0, maxScaleStr);

  // Fixed Width
  QTreeWidgetItem* fixedWidthItem = new QTreeWidgetItem(m_ui->m_layerInfoTreeWidget);

  QString fixedWidthStr = QString("Fixed Width: ");
  QString fixedWidthValue = QString::number(currentLayer.m_fixedWidth);

  fixedWidthStr += fixedWidthValue;
  fixedWidthItem->setText(0, fixedWidthStr);

  // Fixed Width
  QTreeWidgetItem* fixedHeightItem = new QTreeWidgetItem(m_ui->m_layerInfoTreeWidget);

  QString fixedHeightStr = QString("Fixed Width: ");
  QString fixedHeightValue = QString::number(currentLayer.m_fixedHeight);

  fixedHeightStr += fixedHeightValue;
  fixedHeightItem->setText(0, fixedHeightStr);

  if(currentLayer.m_keywordList.size() > 0)
  {
    QTreeWidgetItem* keywordList = new QTreeWidgetItem(m_ui->m_layerInfoTreeWidget);
    keywordList->setText(0, QString("KeywordList"));

    for(size_t i = 0; i < currentLayer.m_keywordList.size(); i++)
    {
      QTreeWidgetItem* keyword = new QTreeWidgetItem(keywordList);
      keyword->setText(0, QString(currentLayer.m_keywordList[i].c_str()));
    }
  }

  if(currentLayer.m_boundingBoxes.size() > 0)
  {
    QTreeWidgetItem* crsListItem = new QTreeWidgetItem(m_ui->m_layerInfoTreeWidget);
    crsListItem->setText(0, QString("CRS"));

    for(size_t i = 0; i < currentLayer.m_boundingBoxes.size(); i++)
    {
      te::ws::ogc::wms::BoundingBox bbox = currentLayer.m_boundingBoxes[i];

      if(!bbox.m_crs.empty())
      {
        QTreeWidgetItem* crsItem = new QTreeWidgetItem(crsListItem);
        crsItem->setText(0, QString(bbox.m_crs.c_str()));
      }

      QTreeWidgetItem* bBoxItem = new QTreeWidgetItem(m_ui->m_layerInfoTreeWidget);
      bBoxItem->setText(0, QString("BoundingBox"));
      
      QString minX = QString("minX: ")  + QString::number(bbox.m_minX);
      QString minY = QString("minY: ")  + QString::number(bbox.m_minY);
      QString maxX = QString("maxX: ")  + QString::number(bbox.m_maxX);
      QString maxY = QString("maxY: ")  + QString::number(bbox.m_maxY);

      QTreeWidgetItem* minXItem = new QTreeWidgetItem(bBoxItem);
      QTreeWidgetItem* minYItem = new QTreeWidgetItem(bBoxItem);
      QTreeWidgetItem* maxXItem = new QTreeWidgetItem(bBoxItem);
      QTreeWidgetItem* maxYItem = new QTreeWidgetItem(bBoxItem);

      minXItem->setText(0, minX);
      minYItem->setText(0, minY);
      maxXItem->setText(0, maxX);
      maxYItem->setText(0, maxY);

    }
  }

  if(currentLayer.m_dimensions.size() > 0)
  {
    for(size_t i = 0; i < currentLayer.m_dimensions.size(); i++)
    {
      te::ws::ogc::wms::Dimension dimension = currentLayer.m_dimensions[i];

      QTreeWidgetItem* dimensionItem = new QTreeWidgetItem(m_ui->m_layerInfoTreeWidget);
      dimensionItem->setText(0, QString("Dimension"));

      QTreeWidgetItem* dimNameItem = new QTreeWidgetItem(dimensionItem);
      dimNameItem->setText(0, QString(dimension.m_name.c_str()));

      if(!dimension.m_default.empty())
      {
        QTreeWidgetItem* dimDefaultItem = new QTreeWidgetItem(dimensionItem);
        QString defaultStr = QString("default: ");
        QString defaultValue = QString(dimension.m_default.c_str());

        defaultStr += defaultValue;
        dimDefaultItem->setText(0, defaultStr);
      }

      if(!dimension.m_units.empty())
      {
        QTreeWidgetItem* dimUnitsItem = new QTreeWidgetItem(dimensionItem);
        QString unitsStr = QString("units: ");
        QString unitsValue = QString(dimension.m_units.c_str());

        unitsStr += unitsValue;
        dimUnitsItem->setText(0, unitsStr);
      }

      if(dimension.m_allowedValues.size() > 0)
      {
        QTreeWidgetItem* dimAllowedValuesItem = new QTreeWidgetItem(dimensionItem);
        dimAllowedValuesItem->setText(0, QString("AllowedValues"));

        for(size_t i = 0; i < dimension.m_allowedValues.size(); i++)
        {
          QTreeWidgetItem* dimAllowedValueItem = new QTreeWidgetItem(dimAllowedValuesItem);
          dimAllowedValueItem->setText(0, QString(dimension.m_allowedValues[i].c_str()));
        }
      }
    }
  }

  if(currentLayer.m_styles.size() > 0)
  {
    for(size_t i = 0; i < currentLayer.m_styles.size(); i++)
    {
      te::ws::ogc::wms::Style style = currentLayer.m_styles[i];

      //Style
      QTreeWidgetItem* styleItem = new QTreeWidgetItem(m_ui->m_layerInfoTreeWidget);
      styleItem->setText(0, QString("Style"));

      QTreeWidgetItem* styleNameItem = new QTreeWidgetItem(styleItem);
      styleNameItem->setText(0, QString(style.m_name.c_str()));

      if(!style.m_title.empty())
      {
        QTreeWidgetItem* styleTitleItem = new QTreeWidgetItem(styleItem);
        styleTitleItem->setText(0, QString(style.m_title.c_str()));
      }

      if(!style.m_abstract.empty())
      {
        QTreeWidgetItem* styleAbstractItem = new QTreeWidgetItem(styleItem);
        styleAbstractItem->setText(0, QString(style.m_abstract.c_str()));
      }

      te::ws::ogc::wms::LegendURL legendURL = style.m_legendURL;

      // LegendURL
      if(legendURL.m_width > 0 && legendURL.m_height > 0)
      {
        QTreeWidgetItem* legendUrlItem = new QTreeWidgetItem(styleItem);
        legendUrlItem->setText(0, QString("LegendURL"));

        if(!legendURL.m_format.empty())
        {
          QTreeWidgetItem* legendFormatItem = new QTreeWidgetItem(legendUrlItem);
          QString legendFormatStr = QString("format: ");
          QString legendFormatValue = QString(legendURL.m_format.c_str());

          legendFormatStr += legendFormatValue;
          legendFormatItem->setText(0, legendFormatStr);
        }

        QTreeWidgetItem* legendWidthItem = new QTreeWidgetItem(legendUrlItem);
        QString legendWidthStr = QString("width: ");
        QString legendWidthValue = QString::number(legendURL.m_width);

        legendWidthStr += legendWidthValue;
        legendWidthItem->setText(0, legendWidthStr);

        QTreeWidgetItem* legendHeightItem = new QTreeWidgetItem(legendUrlItem);
        QString legendHeightStr = QString("height: ");
        QString legendHeightValue = QString::number(legendURL.m_height);

        legendHeightStr += legendHeightValue;
        legendHeightItem->setText(0, legendHeightStr);

        te::ws::ogc::wms::OnlineResource onlineResource = legendURL.m_onlineResource;

        if(!onlineResource.m_href.empty())
        {
          QTreeWidgetItem* onlineResourceItem = new QTreeWidgetItem(legendUrlItem);
          onlineResourceItem->setText(0, QString("OnlineResource"));

          QTreeWidgetItem* hRefItem = new QTreeWidgetItem(onlineResourceItem);
          hRefItem->setText(0, QString(onlineResource.m_href.c_str()));
        }
      }
    }
  }

}

void te::ws::ogc::wms::qt::WMSLayerSelectorDialog::refreshLayerProperties()
{

  if(m_currentLayerItem == 0)
  {
    return;
  }

  m_refreshingProperties = true;

  te::ws::ogc::wms::Layer currentLayer = m_currentLayerItem->getLayer();

  te::ws::ogc::wms::WMSGetMapRequest request = m_currentLayerItem->getRequest();

  m_ui->m_crsComboBox->clear();

  for(size_t i = 0; i < currentLayer.m_crs.size(); i++)
  {
    // EPSG projections only.
    if(currentLayer.m_crs[i].find("EPSG:") != std::string::npos)
    {
      m_ui->m_crsComboBox->addItem(QString(currentLayer.m_crs[i].c_str()));
    }
  }

  m_ui->m_crsComboBox->setCurrentIndex(m_ui->m_crsComboBox->findText(QString(request.m_srs.c_str())));
  
  m_ui->m_transparencyCheckBox->setChecked(request.m_transparent);

  m_ui->m_timeDimComboBox->clear();
  m_ui->m_timeDimComboBox->setEnabled(false);

  if(!currentLayer.m_dimensions.empty())
  {
    for(size_t i = 0; i < currentLayer.m_dimensions.size(); i++)
    {
      te::ws::ogc::wms::Dimension dimension = currentLayer.m_dimensions[i];
      
      if(dimension.m_name == "time")
      {
        m_ui->m_timeDimComboBox->setEnabled(true);
        std::vector<std::string> allowedValues = dimension.m_allowedValues;

        m_ui->m_timeDimComboBox->addItem(QString(TE_TR("Select a time...")));

        for(size_t j = 0; j < allowedValues.size(); j++)
        {
          m_ui->m_timeDimComboBox->addItem(QString(allowedValues[j].c_str()));
        }

        if(!request.m_time.empty())
        {
          m_ui->m_timeDimComboBox->setCurrentIndex(m_ui->m_timeDimComboBox->findText(QString(request.m_time.c_str())));
        }
      }
    }
  }

  m_ui->m_styleComboBox->clear();

  if(!currentLayer.m_styles.empty())
  {
    for(size_t i = 0; i < currentLayer.m_styles.size(); i++)
    {
      m_ui->m_styleComboBox->addItem(QString(currentLayer.m_styles[i].m_name.c_str()));
    }

    if(!request.m_styles.empty())
    {
      m_ui->m_styleComboBox->setCurrentIndex(m_ui->m_styleComboBox->findText(QString(request.m_styles[0].c_str())));
    }
  }


  m_ui->m_imgFormatComboBox->setCurrentIndex(m_ui->m_imgFormatComboBox->findText(QString(request.m_format.c_str())));

  m_refreshingProperties = false;
}

void te::ws::ogc::wms::qt::WMSLayerSelectorDialog::refreshMapPreview()
{
  if(m_currentLayerItem == 0 || m_client.get() == 0 || m_mapPreview.get() == 0)
  {
    return;
  }

  te::ws::ogc::wms::WMSGetMapRequest request = m_currentLayerItem->getRequest();

  request.m_width = m_mapPreview->width();
  request.m_height = m_mapPreview->height();

  te::ws::ogc::wms::WMSGetMapResponse response = m_client->getMap(request);

  m_mapPreview->clear();

  m_mapPreview->draw(response);
}

std::vector<te::ws::ogc::wms::qt::WMSLayerItem *> te::ws::ogc::wms::qt::WMSLayerSelectorDialog::getCheckedLayerItens()
{
  QTreeWidgetItemIterator it (m_ui->m_layersTreeWidget);

  std::vector<WMSLayerItem*> checkedItens;
  
  while(*it)
  {
    WMSLayerItem* layerItem = dynamic_cast<WMSLayerItem*>(*it);

    if(layerItem == 0)
    {
      it++;
      continue;
    }

    if(layerItem->checkState(0) == Qt::Checked)
    {
      checkedItens.push_back(layerItem);
    }

    it++;
  }

  return checkedItens;
}


void te::ws::ogc::wms::qt::WMSLayerSelectorDialog::imgFormatIndexChanged(const int &index)
{
  if(index == -1 || m_currentLayerItem == 0 || m_refreshingProperties)
  {
    return;
  }

  te::ws::ogc::wms::WMSGetMapRequest request = m_currentLayerItem->getRequest();

  request.m_format = m_ui->m_imgFormatComboBox->currentText().toUtf8().constData();

  m_currentLayerItem->setRequest(request);
}

void te::ws::ogc::wms::qt::WMSLayerSelectorDialog::timeDimIndexChanged(const int &index)
{
  if(index == -1 || m_currentLayerItem == 0 || m_refreshingProperties)
  {
    return;
  }



  te::ws::ogc::wms::WMSGetMapRequest request = m_currentLayerItem->getRequest();

  if(index == 0)
  {
    request.m_time = "";
  }
  else
  {
    request.m_time = m_ui->m_timeDimComboBox->currentText().toUtf8().constData();
  }

  m_currentLayerItem->setRequest(request);

  Qt::CheckState state = m_currentLayerItem->checkState(0);

  if(state == Qt::Checked)
  {
    this->refreshMapPreview();
  }
}

void te::ws::ogc::wms::qt::WMSLayerSelectorDialog::styleIndexChanged(const int &index)
{
  if(index == -1 || m_currentLayerItem == 0 || m_refreshingProperties)
  {
    return;
  }

  te::ws::ogc::wms::WMSGetMapRequest request = m_currentLayerItem->getRequest();

  request.m_styles.clear();

  request.m_styles.push_back(m_ui->m_styleComboBox->currentText().toUtf8().constData());

  m_currentLayerItem->setRequest(request);

  Qt::CheckState state = m_currentLayerItem->checkState(0);

  if(state == Qt::Checked)
  {
    this->refreshMapPreview();
  }
}

void te::ws::ogc::wms::qt::WMSLayerSelectorDialog::crsIndexChanged(const int &index)
{
  if(index == -1 || m_currentLayerItem == 0)
  {
    return;
  }

  te::ws::ogc::wms::WMSGetMapRequest request = m_currentLayerItem->getRequest();

  request.m_srs = m_ui->m_crsComboBox->currentText().toUtf8().constData();

  m_currentLayerItem->setRequest(request);
}

void te::ws::ogc::wms::qt::WMSLayerSelectorDialog::transparencyStateChanged(const int &state)
{
  if(m_currentLayerItem == 0)
  {
    return;
  }

  te::ws::ogc::wms::WMSGetMapRequest request = m_currentLayerItem->getRequest();

  Qt::CheckState checkState = (Qt::CheckState) state;

  if(checkState == Qt::Checked)
  {
    request.m_transparent = true;
  }
  else
  {
    request.m_transparent = false;
  }

  m_currentLayerItem->setRequest(request);
}

void te::ws::ogc::wms::qt::WMSLayerSelectorDialog::checkLayerItemChanged(QTreeWidgetItem *item, int column)
{
  if(column != 0)
  {
    return;
  }

  Qt::CheckState state = item->checkState(column);

  if(state == Qt::Checked)
  {
    if(!item->isSelected())
    {
      m_ui->m_layersTreeWidget->setCurrentItem(item);
    }
    else
    {
      this->refreshMapPreview();
    }
  }
  else
  {
    if(!item->isSelected())
    {
      m_ui->m_layersTreeWidget->setCurrentItem(item);
    }

    m_mapPreview->clear();
  }
}

void te::ws::ogc::wms::qt::WMSLayerSelectorDialog::okPushButtonPressed()
{

  std::vector<WMSLayerItem*> layerItens = this->getCheckedLayerItens();

  if(!layerItens.empty())
  {
    QString message (tr("The Following WMS Layers will be created:\n"));

    for(size_t i = 0; i < layerItens.size(); i++)
    {
      te::ws::ogc::wms::Layer layer = layerItens[i]->getLayer();

      std::string layerTitle = layer.m_title.empty() ? layer.m_name : layer.m_title;

      layerTitle += "\n";

      message += QString(layerTitle.c_str());
    }

    QMessageBox::information(this, tr("WMS Layer Creation"), message);
  }

  accept();
}
