/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled
   applications.

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
  \file terralib/vp/qt/GeometricOpWizardPage.cpp

  \brief This file defines a class for a Geometric Operation Wizard Page.
*/

// TerraLib
#include "../../common/StringUtils.h"
#include "../../dataaccess/dataset/DataSet.h"
#include "../../dataaccess/dataset/DataSetType.h"
#include "../../dataaccess/utils/Utils.h"
#include "../../maptools/AbstractLayer.h"
#include "../../qt/widgets/datasource/selector/DataSourceSelectorDialog.h"
#include "../../qt/widgets/Utils.h"
#include "../../qt/widgets/utils/DoubleListWidget.h"

#include "../../geometry/Geometry.h"

#include "GeometricOpOutputWizardPage.h"
#include "ui_GeometricOpOutputWizardPageForm.h"

// Qt
#include <QFileDialog>
#include <QIntValidator>
#include <QMessageBox>

// BOOST
#include <boost/filesystem.hpp>

// STL
#include <memory>

te::vp::GeometricOpOutputWizardPage::GeometricOpOutputWizardPage(
    QWidget* parent)
    : QWizardPage(parent),
      m_ui(new Ui::GeometricOpOutputWizardPageForm),
      m_toFile(false)
{
  // setup controls
  m_ui->setupUi(this);

  // configure page
  this->setTitle(tr("Output Layer Attributes"));
  this->setSubTitle(
      tr("Choose the output parameters that compose the output layer."));

  // icons
  QSize iconSize(32, 32);

  m_ui->m_convexHullCheckBox->setIconSize(iconSize);
  m_ui->m_convexHullCheckBox->setIcon(QIcon::fromTheme("vp-convex-hull-hint"));

  m_ui->m_centroidCheckBox->setIconSize(iconSize);
  m_ui->m_centroidCheckBox->setIcon(QIcon::fromTheme("vp-centroid-hint"));

  m_ui->m_mbrCheckBox->setIconSize(iconSize);
  m_ui->m_mbrCheckBox->setIcon(QIcon::fromTheme("vp-mbr-hint"));

  m_ui->m_areaCheckBox->setIconSize(iconSize);
  m_ui->m_areaCheckBox->setIcon(QIcon::fromTheme("vp-area-hint"));

  m_ui->m_lineCheckBox->setIconSize(iconSize);
  m_ui->m_lineCheckBox->setIcon(QIcon::fromTheme("vp-line-length-hint"));

  m_ui->m_perimeterCheckBox->setIconSize(iconSize);
  m_ui->m_perimeterCheckBox->setIcon(QIcon::fromTheme("vp-perimeter-hint"));

  m_ui->m_targetDatasourceToolButton->setIcon(QIcon::fromTheme("datasource"));

  connect(m_ui->m_attributesComboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onAttributeComboBoxChanged(int)));
  connect(m_ui->m_byObjectRadioButton, SIGNAL(toggled(bool)), this,
          SLOT(onAllObjectsToggled()));
  connect(m_ui->m_allLayerRadioButton, SIGNAL(toggled(bool)), this,
          SLOT(onAllLayerOperationToggled()));
  connect(m_ui->m_byAttributeRadioButton, SIGNAL(toggled(bool)), this,
          SLOT(onAttributeOperationToggled()));
  connect(m_ui->m_targetDatasourceToolButton, SIGNAL(pressed()), this,
          SLOT(onTargetDatasourceToolButtonPressed()));
  connect(m_ui->m_targetFileToolButton, SIGNAL(pressed()), this,
          SLOT(onTargetFileToolButtonPressed()));

  onAllLayerOperationToggled();
}

te::vp::GeometricOpOutputWizardPage::~GeometricOpOutputWizardPage()
{
}

bool te::vp::GeometricOpOutputWizardPage::hasConvexHull()
{
  return m_ui->m_convexHullCheckBox->isChecked();
}

bool te::vp::GeometricOpOutputWizardPage::hasCentroid()
{
  return m_ui->m_centroidCheckBox->isChecked();
}

bool te::vp::GeometricOpOutputWizardPage::hasMBR()
{
  return m_ui->m_mbrCheckBox->isChecked();
}

bool te::vp::GeometricOpOutputWizardPage::hasArea()
{
  return m_ui->m_areaCheckBox->isChecked();
}

bool te::vp::GeometricOpOutputWizardPage::hasLine()
{
  return m_ui->m_lineCheckBox->isChecked();
}

bool te::vp::GeometricOpOutputWizardPage::hasPerimeter()
{
  return m_ui->m_perimeterCheckBox->isChecked();
}

std::string te::vp::GeometricOpOutputWizardPage::getAttribute()
{
  return m_attribute;
}

void te::vp::GeometricOpOutputWizardPage::setInputGeometryType(
    te::gm::GeomType type)
{
  switch(type)
  {
    case te::gm::PointType:
    case te::gm::PointMType :
    case te::gm::PointZType :
    case te::gm::PointZMType :
    case te::gm::PointKdType :
    case te::gm::MultiPointType:
    case te::gm::MultiPointMType :
    case te::gm::MultiPointZType :
    case te::gm::MultiPointZMType :
      m_ui->m_areaCheckBox->setEnabled(false);
      m_ui->m_lineCheckBox->setEnabled(false);
      m_ui->m_perimeterCheckBox->setEnabled(false);
      m_ui->m_byObjectRadioButton->setEnabled(false);
      m_ui->m_byAttributeRadioButton->setEnabled(false);
      m_ui->m_byAttributeRadioButton->setChecked(true);
      break;
    case te::gm::LineStringType:
    case te::gm::LineStringMType:
    case te::gm::LineStringZType:
    case te::gm::LineStringZMType:
    case te::gm::MultiLineStringType:
    case te::gm::MultiLineStringMType:
    case te::gm::MultiLineStringZType:
    case te::gm::MultiLineStringZMType:
      m_ui->m_areaCheckBox->setEnabled(false);
      m_ui->m_lineCheckBox->setEnabled(true);
      m_ui->m_perimeterCheckBox->setEnabled(false);
      m_ui->m_byObjectRadioButton->setEnabled(true);
      m_ui->m_byAttributeRadioButton->setEnabled(true);
      m_ui->m_byObjectRadioButton->setChecked(true);
      break;
    default:
      m_ui->m_areaCheckBox->setEnabled(true);
      m_ui->m_lineCheckBox->setEnabled(false);
      m_ui->m_perimeterCheckBox->setEnabled(true);
      m_ui->m_byObjectRadioButton->setEnabled(true);
      m_ui->m_byAttributeRadioButton->setEnabled(true);
      m_ui->m_byObjectRadioButton->setChecked(true);
      break;
  }
}

void te::vp::GeometricOpOutputWizardPage::setAttributes(
    std::vector<std::string> attributes)
{
  m_ui->m_attributesComboBox->clear();

  for(std::size_t i = 0; i < attributes.size(); ++i)
  {
    m_ui->m_attributesComboBox->addItem(
        QString::fromUtf8(attributes[i].c_str()));
  }
}

te::vp::GeometricOpObjStrategy
te::vp::GeometricOpOutputWizardPage::getObjectStrategy()
{
  if(m_ui->m_byObjectRadioButton->isChecked())
    return te::vp::ALL_OBJ;
  if(m_ui->m_allLayerRadioButton->isChecked())
    return te::vp::AGGREG_OBJ;
  if(m_ui->m_byAttributeRadioButton->isChecked())
    return te::vp::AGGREG_BY_ATTRIBUTE;

  return te::vp::ALL_OBJ;
}

std::string te::vp::GeometricOpOutputWizardPage::getOutDsName()
{
  return m_ui->m_newLayerNameLineEdit->text().toUtf8().data();
}

bool te::vp::GeometricOpOutputWizardPage::getToFile()
{
  return m_toFile;
}

te::da::DataSourceInfoPtr te::vp::GeometricOpOutputWizardPage::getDsInfoPtr()
{
  return m_outputDatasource;
}

std::string te::vp::GeometricOpOutputWizardPage::getPath()
{
  return m_ui->m_repositoryLineEdit->text().toUtf8().data();
}

void te::vp::GeometricOpOutputWizardPage::onAttributeComboBoxChanged(int index)
{
  if(m_ui->m_byAttributeRadioButton->isChecked())
    m_attribute = m_ui->m_attributesComboBox->itemText(index).toUtf8().data();
  else
    m_attribute = "";
}

void te::vp::GeometricOpOutputWizardPage::onAllObjectsToggled()
{
  m_ui->m_attributesComboBox->setEnabled(false);
  onAttributeComboBoxChanged(0);
}

void te::vp::GeometricOpOutputWizardPage::onAllLayerOperationToggled()
{
  m_ui->m_attributesComboBox->setEnabled(false);
  onAttributeComboBoxChanged(0);
}

void te::vp::GeometricOpOutputWizardPage::onAttributeOperationToggled()
{
  m_ui->m_attributesComboBox->setEnabled(true);
}

void te::vp::GeometricOpOutputWizardPage::onTargetDatasourceToolButtonPressed()
{
  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_newLayerNameLineEdit->setEnabled(true);
  te::qt::widgets::DataSourceSelectorDialog dlg(this);
  dlg.exec();

  std::list<te::da::DataSourceInfoPtr> dsPtrList = dlg.getSelecteds();

  if(dsPtrList.empty())
    return;

  std::list<te::da::DataSourceInfoPtr>::iterator it = dsPtrList.begin();

  m_ui->m_repositoryLineEdit->setText(QString(it->get()->getTitle().c_str()));

  m_outputDatasource = *it;

  m_toFile = false;
}

void te::vp::GeometricOpOutputWizardPage::onTargetFileToolButtonPressed()
{
  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_repositoryLineEdit->clear();

  QString dir = QFileDialog::getExistingDirectory(
      this, tr("Open Directory"),
      te::qt::widgets::GetFilePathFromSettings("vp_geomOp"),
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if(dir.isEmpty() == false)
  {
    m_path = dir.replace(QRegExp("\\\\"), "/").toUtf8().data();

    m_ui->m_repositoryLineEdit->setText(m_path.c_str());

    te::qt::widgets::AddFilePathToSettings(m_path.c_str(), "vp_geomOp");

    m_toFile = true;
  }
}
