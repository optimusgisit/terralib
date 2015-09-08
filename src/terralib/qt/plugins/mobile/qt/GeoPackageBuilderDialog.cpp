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
\file src/terraMobilePlugin/qt/GeoPackageBuilderDialog.cpp

\brief This interface is used to get the input parameters for GeoPackage Builder operation.
*/

// TerraLib
#include "../../../../common/progress/ProgressManager.h"
#include "../../../../common/progress/TaskProgress.h"
#include "../../../../common/STLUtils.h"
#include "../../../../common/StringUtils.h"
#include "../../../../dataaccess/utils/Utils.h"
#include "../../../../datatype/Utils.h"
#include "../../../../geometry/GeometryProperty.h"
#include "../core/form/AbstractFormItem.h"
#include "../core/form/BoolFormItem.h"
#include "../core/form/DateFormItem.h"
#include "../core/form/DoubleFormItem.h"
#include "../core/form/IntFormItem.h"
#include "../core/form/LabelFormItem.h"
#include "../core/form/PictureFormItem.h"
#include "../core/form/StringComboFormItem.h"
#include "../core/form/StringFormItem.h"
#include "../core/form/TimeFormItem.h"
#include "../core/form/Form.h"
#include "GeoPackageBuilderDialog.h"
#include "ui_GeoPackageBuilderDialogForm.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>

#define FORM_TREE_ITEM      0
#define PROPERTY_TREE_ITEM  1

Q_DECLARE_METATYPE(te::map::AbstractLayerPtr);

te::qt::plugins::terramobile::GeoPackageBuilderDialog::GeoPackageBuilderDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f),
  m_ui(new Ui::GeoPackageBuilderDialogForm),
  m_section(0),
  m_curForm(0),
  m_curFormItem(0),
  m_itemTreeType(-1)
{
  // add controls
  m_ui->setupUi(this);
  m_ui->m_imgLabel->setPixmap(QIcon::fromTheme("terraMobile-hint").pixmap(100, 50));

  m_ui->m_addFormToolButton->setIcon(QIcon::fromTheme("list-add"));

  m_ui->m_saveFormItemToolButton->setIcon(QIcon::fromTheme("check"));
  m_ui->m_stringComboAddToolButton->setIcon(QIcon::fromTheme("list-add"));
  

  //build form
  QGridLayout* ilGridLayout = new QGridLayout(m_ui->m_tabWidget->widget(0));
  ilGridLayout->setContentsMargins(0, 0, 0, 0);
  m_inputLayers = new te::qt::widgets::DoubleListWidget(m_ui->m_tabWidget->widget(0));
  ilGridLayout->addWidget(m_inputLayers);

  m_inputLayers->setLeftLabel("Input Layers");
  m_inputLayers->setRightLabel("GeoPackage Layers");

  QGridLayout* glGridLayout = new QGridLayout(m_ui->m_tabWidget->widget(1));
  glGridLayout->setContentsMargins(0, 0, 0, 0);
  m_gatheringLayers = new te::qt::widgets::DoubleListWidget(m_ui->m_tabWidget->widget(1));
  glGridLayout->addWidget(m_gatheringLayers);

  m_gatheringLayers->setLeftLabel("Point Layers");
  m_gatheringLayers->setRightLabel("Gathering Layers");

  m_ui->m_formTableWidget->resizeColumnsToContents();
  m_ui->m_formTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  //empty page
  m_ui->m_stackedWidget->setCurrentIndex(9);

  //connects
  connect(m_ui->m_dirToolButton, SIGNAL(pressed()), this, SLOT(onDirToolButtonPressed()));
  connect(m_ui->m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabWidgetChanged(int)));
  connect(m_ui->m_formLayerComboBox, SIGNAL(activated(int)), this, SLOT(onFormLayerActivated(int)));
  connect(m_ui->m_addFormToolButton, SIGNAL(pressed()), this, SLOT(onAddFormToolButtonPressed()));
  connect(m_ui->m_treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onTreeItemClicked(QTreeWidgetItem*, int)));
  connect(m_ui->m_stringComboAddToolButton, SIGNAL(pressed()), this, SLOT(onStringComboAddToolButtonPressed()));
  connect(m_ui->m_saveFormItemToolButton, SIGNAL(pressed()), this, SLOT(onSaveFormItemToolButton()));
}

te::qt::plugins::terramobile::GeoPackageBuilderDialog::~GeoPackageBuilderDialog()
{
  delete m_section;
}

void te::qt::plugins::terramobile::GeoPackageBuilderDialog::setLayerList(std::list<te::map::AbstractLayerPtr> list)
{
  m_list = list;

  //clear lists
  m_inputLayers->clearInputValues();
  m_inputLayers->clearOutputValues();

  m_gatheringLayers->clearInputValues();
  m_gatheringLayers->clearOutputValues();

  std::vector<std::string> inputLayerNames;
  std::vector<std::string> gatheringLayerNames;


  //fill combos
  std::list<te::map::AbstractLayerPtr>::iterator it = list.begin();

  while (it != list.end())
  {
    te::map::AbstractLayerPtr l = *it;

    if (l->isValid())
    {
      std::auto_ptr<te::da::DataSetType> dsType = l->getSchema();

      if (dsType->hasGeom())
      {
        inputLayerNames.push_back(l->getTitle());

        //if this layer has point as geom type, add this layer to gatherer list
        te::gm::GeometryProperty* geomProp = te::da::GetFirstGeomProperty(dsType.get());

        if (geomProp && (geomProp->getGeometryType() == te::gm::PointType || geomProp->getGeometryType() == te::gm::MultiPointType))
        {
          gatheringLayerNames.push_back(l->getTitle());
        }
      }
      else if (dsType->hasRaster())
      {
        inputLayerNames.push_back(l->getTitle());
      }
    }
    ++it;
  }

  m_inputLayers->setInputValues(inputLayerNames);
  m_gatheringLayers->setInputValues(gatheringLayerNames);
}

void te::qt::plugins::terramobile::GeoPackageBuilderDialog::onDirToolButtonPressed()
{
  //QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."), QString(), tr("Geopackage (*.gpkg *.GPKG);;"), 0, QFileDialog::DontConfirmOverwrite);
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."), QString(), tr("JSON FILE (*.txt *.TXT);;"), 0, QFileDialog::DontConfirmOverwrite);

  if (fileName.isEmpty())
    return;

  m_ui->m_geopackageFileNameLineEdit->setText(fileName);
}

void te::qt::plugins::terramobile::GeoPackageBuilderDialog::onTabWidgetChanged(int index)
{
  if (index == 2)
  {
    std::list<te::map::AbstractLayerPtr> layers = getGatheringLayers();

    m_ui->m_formLayerComboBox->clear();

    std::list<te::map::AbstractLayerPtr>::iterator it;

    for (it = layers.begin(); it != layers.end(); ++it)
    {
      m_ui->m_formLayerComboBox->addItem(it->get()->getTitle().c_str(), QVariant::fromValue(*it));
    }

    if (!layers.empty())
    {
      onFormLayerActivated(0);
    }
  }
  else if (index == 3)
  {
    if (!m_section)
      return;

    m_ui->m_treeWidget->clear();

    for (std::size_t t = 0; t < m_section->getForms().size(); ++t)
    {
      te::qt::plugins::terramobile::Form* form = m_section->getForms()[t];

      QTreeWidgetItem* item = new QTreeWidgetItem(m_ui->m_treeWidget, FORM_TREE_ITEM);
      item->setText(0, form->getName().c_str());
      item->setIcon(0, QIcon::fromTheme("document-info"));

      m_ui->m_treeWidget->addTopLevelItem(item);

      for (std::size_t p = 0; p < form->getItems().size(); ++p)
      {
        te::qt::plugins::terramobile::AbstractFormItem* formItem = form->getItems()[p];

        QTreeWidgetItem* subItem = new QTreeWidgetItem(item, PROPERTY_TREE_ITEM);
        subItem->setText(0, formItem->getLabel().c_str());
        subItem->setText(1, formItem->getType().c_str());
        subItem->setIcon(0, QIcon::fromTheme("layer-info"));
        item->addChild(subItem);
        item->setExpanded(true);
      }
    }

    m_ui->m_treeWidget->resizeColumnToContents(0);
    m_ui->m_treeWidget->resizeColumnToContents(1);
  }
}

void te::qt::plugins::terramobile::GeoPackageBuilderDialog::onFormLayerActivated(int index)
{
  if (index == -1)
    return;

  QVariant varLayer = m_ui->m_formLayerComboBox->itemData(index, Qt::UserRole);
  te::map::AbstractLayerPtr layer = varLayer.value<te::map::AbstractLayerPtr>();

  std::auto_ptr<te::da::DataSetType> dsType = layer->getSchema();

  std::vector < te::dt::Property*> props = dsType->getProperties();

  m_ui->m_formNameLineEdit->setText(layer->getTitle().c_str());

  //fill table
  m_ui->m_formTableWidget->setRowCount(0);

  for (std::size_t t = 0; t < props.size(); ++t)
  {
    int newrow = m_ui->m_formTableWidget->rowCount();
    m_ui->m_formTableWidget->insertRow(newrow);

    //check
    {
      QCheckBox* checkBox = new QCheckBox(m_ui->m_formTableWidget);
      m_ui->m_formTableWidget->setCellWidget(newrow, 0, checkBox);
    }

    //name
    {
      QTableWidgetItem* item = new QTableWidgetItem(props[t]->getName().c_str());
      item->setFlags(Qt::ItemIsEnabled);
      m_ui->m_formTableWidget->setItem(newrow, 1, item);
    }

    //type
    {
      QTableWidgetItem* item = new QTableWidgetItem(te::dt::ConvertDataTypeToString(props[t]->getType()).c_str());
      item->setFlags(Qt::ItemIsEnabled);
      m_ui->m_formTableWidget->setItem(newrow, 2, item);
    }

    //key
    {
      QTableWidgetItem* item = new QTableWidgetItem(props[t]->getName().c_str());
      item->setFlags(Qt::ItemIsEnabled);
      m_ui->m_formTableWidget->setItem(newrow, 3, item);
    }

    //label
    {
      QTableWidgetItem* item = new QTableWidgetItem(props[t]->getName().c_str());
      item->setFlags(Qt::ItemIsEnabled);
      m_ui->m_formTableWidget->setItem(newrow, 4, item);
    }

    //type combo
    {
      QComboBox* cmbProperty = new QComboBox(m_ui->m_formTableWidget);

      cmbProperty->addItem(BOOLFORMITEMTYPE);
      cmbProperty->addItem(DATEFORMITEMTYPE);
      cmbProperty->addItem(DOUBLEFORMITEMTYPE);
      cmbProperty->addItem(INTFORMITEMTYPE);
      cmbProperty->addItem(LABELFORMITEMTYPE);
      cmbProperty->addItem(PICTUREFORMITEMTYPE);
      cmbProperty->addItem(STRINGCOMBOFORMITEMTYPE);
      cmbProperty->addItem(STRINGFORMITEMTYPE);
      cmbProperty->addItem(TIMEFORMITEMTYPE);

      //suggest value

      if (props[t]->getType() == te::dt::INT16_TYPE || props[t]->getType() == te::dt::INT32_TYPE || props[t]->getType() == te::dt::INT64_TYPE)
      {
        cmbProperty->setCurrentText(INTFORMITEMTYPE);
      }
      else if (props[t]->getType() == te::dt::DOUBLE_TYPE || props[t]->getType() == te::dt::FLOAT_TYPE)
      {
        cmbProperty->setCurrentText(DOUBLEFORMITEMTYPE);
      }
      else if (props[t]->getType() == te::dt::BOOLEAN_TYPE)
      {
        cmbProperty->setCurrentText(BOOLFORMITEMTYPE);
      }
      else if (props[t]->getType() == te::dt::STRING_TYPE)
      {
        cmbProperty->setCurrentText(STRINGFORMITEMTYPE);
      }
      
      m_ui->m_formTableWidget->setCellWidget(newrow, 5, cmbProperty);
    }
  }

}

void te::qt::plugins::terramobile::GeoPackageBuilderDialog::onAddFormToolButtonPressed()
{
  //create section
  if (!m_section)
  {
    if (m_ui->m_sectionNameLineEdit->text().isEmpty())
    {
      QMessageBox::warning(this, tr("Warning"), tr("Section name not defined."));
      return;
    }

    std::string sectionName = m_ui->m_sectionNameLineEdit->text().toStdString();
    std::string sectionDesc = "";

    if (!m_ui->m_sectionDescLineEdit->text().isEmpty())
    {
      sectionDesc = m_ui->m_sectionDescLineEdit->text().toStdString();
    }

    m_section = new te::qt::plugins::terramobile::Section();

    m_section->setName(sectionName);
    m_section->setDescription(sectionDesc);

    m_ui->m_sectionNameLineEdit->setEnabled(false);
    m_ui->m_sectionDescLineEdit->setEnabled(false);
  }

  //create form
  if (m_ui->m_formNameLineEdit->text().isEmpty())
  {
    QMessageBox::warning(this, tr("Warning"), tr("Form name not defined."));
    return;
  }

  std::string formName = m_ui->m_formNameLineEdit->text().toStdString();

  te::qt::plugins::terramobile::Form* form = new te::qt::plugins::terramobile::Form();

  form->setName(formName);

  //add properties to form
  int nRows = m_ui->m_formTableWidget->rowCount();
  for (int i = 0; i < nRows; ++i)
  {
    //check if property was selected
    QCheckBox* checkBox = dynamic_cast<QCheckBox*>(m_ui->m_formTableWidget->cellWidget(i, 0));

    if (checkBox && checkBox->isChecked())
    {
      QComboBox* cmbBox = dynamic_cast<QComboBox*>(m_ui->m_formTableWidget->cellWidget(i, 5));

      if (cmbBox)
      {
        std::string propType = cmbBox->currentText().toStdString();

        std::string key = m_ui->m_formTableWidget->item(i, 3)->text().toStdString();
        std::string label = m_ui->m_formTableWidget->item(i, 4)->text().toStdString();

        te::qt::plugins::terramobile::AbstractFormItem* item;

        if (propType == BOOLFORMITEMTYPE)
        {
          item = new te::qt::plugins::terramobile::BoolFormItem();
        }
        else if (propType == DATEFORMITEMTYPE)
        {
          item = new te::qt::plugins::terramobile::DateFormItem();
        }
        else if (propType == DOUBLEFORMITEMTYPE)
        {
          item = new te::qt::plugins::terramobile::DoubleFormItem();
        }
        else if (propType == INTFORMITEMTYPE)
        {
          item = new te::qt::plugins::terramobile::IntFormItem();
        }
        else if (propType == LABELFORMITEMTYPE)
        {
          item = new te::qt::plugins::terramobile::LabelFormItem();
        }
        else if (propType == PICTUREFORMITEMTYPE)
        {
          item = new te::qt::plugins::terramobile::PictureFormItem();
        }
        else if (propType == STRINGCOMBOFORMITEMTYPE)
        {
          item = new te::qt::plugins::terramobile::StringComboFormItem();
        }
        else if (propType == STRINGFORMITEMTYPE)
        {
          item = new te::qt::plugins::terramobile::StringFormItem();
        }
        else if (propType == TIMEFORMITEMTYPE)
        {
          item = new te::qt::plugins::terramobile::TimeFormItem();
        }

        item->setKey(key);
        item->setLabel(label);

        form->getItems().push_back(item);
      }
    }
  }

  //add form to section
  m_section->getForms().push_back(form);
}

void te::qt::plugins::terramobile::GeoPackageBuilderDialog::onTreeItemClicked(QTreeWidgetItem* item, int column)
{
  if (!item)
    return;

  if (item->type() == PROPERTY_TREE_ITEM)
  {
    std::string propertyName = item->text(0).toStdString();

    QTreeWidgetItem* parent = item->parent();

    std::string formName = parent->text(0).toStdString();

    //get form
    te::qt::plugins::terramobile::Form* form = m_section->getForm(formName);

    if (!form)
      return;

    //get property
    te::qt::plugins::terramobile::AbstractFormItem* formItem = form->getItem(propertyName);

    if (!formItem)
      return;

    //set the right stacked widget
    std::string propType = formItem->getType();

    if (propType == BOOLFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::BoolFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::BoolFormItem*>(formItem);

      m_ui->m_defaultValueBoolCheckBox->setChecked(fi->hasDefaultValue());

      if (fi->hasDefaultValue())
      {
        m_ui->m_boolComboBox->setCurrentText(te::common::Convert2UCase(fi->getStrDefaultValue()).c_str());
      }

      m_ui->m_stackedWidget->setCurrentIndex(0);
    }
    else if (propType == DATEFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::DateFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::DateFormItem*>(formItem);

      m_ui->m_stackedWidget->setCurrentIndex(1);
    }
    else if (propType == DOUBLEFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::DoubleFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::DoubleFormItem*>(formItem);

      m_ui->m_defaultValueDoubleCheckBox->setChecked(fi->hasDefaultValue());

      if (fi->hasDefaultValue())
      {
        m_ui->m_doubleLineEdit->setText(fi->getStrDefaultValue().c_str());
      }

      m_ui->m_stackedWidget->setCurrentIndex(2);
    }
    else if (propType == INTFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::IntFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::IntFormItem*>(formItem);

      m_ui->m_defaultValueIntCheckBox->setChecked(fi->hasDefaultValue());

      if (fi->hasDefaultValue())
      {
        m_ui->m_intLineEdit->setText(fi->getStrDefaultValue().c_str());
      }

      m_ui->m_stackedWidget->setCurrentIndex(3);
    }
    else if (propType == LABELFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::LabelFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::LabelFormItem*>(formItem);

      m_ui->m_defaultValueLabelCheckBox->setChecked(fi->hasDefaultValue());

      if (fi->hasDefaultValue())
      {
        m_ui->m_labelLineEdit->setText(fi->getStrDefaultValue().c_str());
      }

      m_ui->m_stackedWidget->setCurrentIndex(4);
    }
    else if (propType == PICTUREFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::PictureFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::PictureFormItem*>(formItem);

      m_ui->m_stackedWidget->setCurrentIndex(9);
    }
    else if (propType == STRINGCOMBOFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::StringComboFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::StringComboFormItem*>(formItem);

      m_ui->m_defaultValueStringComboCheckBox->setChecked(fi->hasDefaultValue());

      if (fi->hasDefaultValue())
      {
        m_ui->m_stringComboLineEdit->setText(fi->getStrDefaultValue().c_str());
      }

      m_ui->m_listWidget->clear();

      for (std::size_t t = 0; t < fi->getValues().size(); ++t)
      {
        m_ui->m_listWidget->addItem(fi->getValues()[t].c_str());
      }

      m_ui->m_stackedWidget->setCurrentIndex(5);
    }
    else if (propType == STRINGFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::StringFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::StringFormItem*>(formItem);

      m_ui->m_defaultValueStringCheckBox->setChecked(fi->hasDefaultValue());

      if (fi->hasDefaultValue())
      {
        m_ui->m_stringLineEdit->setText(fi->getStrDefaultValue().c_str());
      }

      m_ui->m_stackedWidget->setCurrentIndex(6);
    }
    else if (propType == TIMEFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::TimeFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::TimeFormItem*>(formItem);

      m_ui->m_stackedWidget->setCurrentIndex(7);
    }

    m_ui->m_formItemLineEdit->setText(propertyName.c_str());

    m_curFormItem = formItem;
  }
  else if (item->type() == FORM_TREE_ITEM)
  {
    m_ui->m_stackedWidget->setCurrentIndex(8);

    std::string formName = item->text(0).toStdString();

    //get form
    te::qt::plugins::terramobile::Form* form = m_section->getForm(formName);

    //set properties
    m_ui->m_formItemsListWidget->clear();

    for (std::size_t t = 0; t < form->getItems().size(); ++t)
    {
      m_ui->m_formItemsListWidget->addItem(form->getItems()[t]->getLabel().c_str());
    }

    m_ui->m_formItemLineEdit->setText(formName.c_str());

    m_curForm = form;
  }

  m_itemTreeType = item->type();
}

void te::qt::plugins::terramobile::GeoPackageBuilderDialog::onStringComboAddToolButtonPressed()
{
  if (m_ui->m_stringComboValueLineEdit->text().isEmpty())
  {
    QMessageBox::warning(this, tr("Warning"), tr("Value not defined."));
    return;
  }

  m_ui->m_listWidget->addItem(m_ui->m_stringComboValueLineEdit->text());
}

void te::qt::plugins::terramobile::GeoPackageBuilderDialog::onSaveFormItemToolButton()
{
  if (m_itemTreeType == PROPERTY_TREE_ITEM)
  {
    std::string propType = m_curFormItem->getType();

    if (propType == BOOLFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::BoolFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::BoolFormItem*>(m_curFormItem);

      if (m_ui->m_defaultValueBoolCheckBox->isChecked())
      {
        std::string value = m_ui->m_boolComboBox->currentText().toStdString();

        if (value == "TRUE")
          fi->setValue(true);
        else
          fi->setValue(false);
      }
    }
    else if (propType == DATEFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::DateFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::DateFormItem*>(m_curFormItem);
    }
    else if (propType == DOUBLEFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::DoubleFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::DoubleFormItem*>(m_curFormItem);

      if (m_ui->m_defaultValueDoubleCheckBox->isChecked())
      {
        fi->setValue(m_ui->m_doubleLineEdit->text().toDouble());
      }
    }
    else if (propType == INTFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::IntFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::IntFormItem*>(m_curFormItem);

      if (m_ui->m_defaultValueIntCheckBox->isChecked())
      {
        fi->setValue(m_ui->m_intLineEdit->text().toInt());
      }
    }
    else if (propType == LABELFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::LabelFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::LabelFormItem*>(m_curFormItem);

      if (m_ui->m_defaultValueLabelCheckBox->isChecked())
      {
        fi->setValue(m_ui->m_labelLineEdit->text().toStdString());
      }
    }
    else if (propType == PICTUREFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::PictureFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::PictureFormItem*>(m_curFormItem);
    }
    else if (propType == STRINGCOMBOFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::StringComboFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::StringComboFormItem*>(m_curFormItem);

      if (m_ui->m_defaultValueStringComboCheckBox->isChecked())
      {
        fi->setValue(m_ui->m_stringComboLineEdit->text().toStdString());
      }

      std::vector<std::string> values;

      for (int i = 0; i < m_ui->m_listWidget->count(); ++i)
      {
        values.push_back(m_ui->m_listWidget->item(i)->text().toStdString());
      }

      fi->setValues(values);
    }
    else if (propType == STRINGFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::StringFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::StringFormItem*>(m_curFormItem);

      if (m_ui->m_defaultValueStringCheckBox->isChecked())
      {
        fi->setValue(m_ui->m_stringLineEdit->text().toStdString());
      }
    }
    else if (propType == TIMEFORMITEMTYPE)
    {
      te::qt::plugins::terramobile::TimeFormItem* fi = dynamic_cast<te::qt::plugins::terramobile::TimeFormItem*>(m_curFormItem);
    }
  }
  else if (m_itemTreeType == FORM_TREE_ITEM)
  {
    std::vector<std::string> values;

    for (int i = 0; i < m_ui->m_formItemsListWidget->count(); ++i)
    {
      values.push_back(m_ui->m_formItemsListWidget->item(i)->text().toStdString());
    }

    std::vector<AbstractFormItem*> backupItems;

    for (std::size_t t = 0; t < m_curForm->getItems().size(); ++t)
    {
      backupItems.push_back(m_curForm->getItems()[t]);
    }

    m_curForm->getItems().clear();

    for (std::size_t t = 0; t < values.size(); ++t)
    {
      for (std::size_t p = 0; p < backupItems.size(); ++p)
      {
        if (values[t] == backupItems[p]->getLabel())
        {
          m_curForm->getItems().push_back(backupItems[p]);
        }
      }
    }

    backupItems.clear();
  }
}

void te::qt::plugins::terramobile::GeoPackageBuilderDialog::onOkPushButtonClicked()
{
  if (m_ui->m_geopackageFileNameLineEdit->text().isEmpty())
    return;

  std::string fileName = m_ui->m_geopackageFileNameLineEdit->text().toStdString();

  if (m_section)
    m_section->serialize(fileName);
}

std::list<te::map::AbstractLayerPtr> te::qt::plugins::terramobile::GeoPackageBuilderDialog::getGatheringLayers()
{
  std::list<te::map::AbstractLayerPtr> list;

  //get gathering layers names
  std::vector<std::string> names = m_gatheringLayers->getOutputValues();

  for (std::size_t t = 0; t < names.size(); ++t)
  {
    std::list<te::map::AbstractLayerPtr>::iterator it;

    for (it = m_list.begin(); it != m_list.end(); ++it)
    {
      if (names[t] == it->get()->getTitle())
      {
        list.push_back(it->get());
      }
    }
  }

  return list;
}
