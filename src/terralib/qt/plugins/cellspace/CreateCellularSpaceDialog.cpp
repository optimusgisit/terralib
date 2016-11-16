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
  \file terralib/qt/widgets/connector/cellspace/CreateCellularSpaceDialog.cpp

  \brief ....
*/

// TerraLib
#include "../../../cellspace/CellSpaceOperations.h"
#include "../../../common/Exception.h"
#include "../../../common/progress/ProgressManager.h"
#include "../../../common/StringUtils.h"
#include "../../../core/translator/Translator.h"
#include "../../../core/uri/URI.h"
#include "../../../core/uri/Utils.h"
#include "../../../common/UnitsOfMeasureManager.h"
#include "../../../dataaccess/datasource/DataSource.h"
#include "../../../dataaccess/datasource/DataSourceFactory.h"
#include "../../../dataaccess/datasource/DataSourceInfoManager.h"
#include "../../../dataaccess/datasource/DataSourceTransactor.h"
#include "../../../dataaccess/datasource/DataSourceManager.h"
#include "../../../dataaccess/utils/Utils.h"
#include "../../../qt/widgets/layer/utils/DataSet2Layer.h"
#include "../../../qt/widgets/help/HelpPushButton.h"
#include "../../../qt/widgets/progress/ProgressViewerDialog.h"
#include "../../../qt/widgets/srs/SRSManagerDialog.h"
#include "../../../qt/widgets/utils/ScopedCursor.h"
#include "../../../srs/SpatialReferenceSystemManager.h"
#include "../../../srs/Converter.h"
#include "../../widgets/datasource/selector/DataSourceSelectorDialog.h"
#include "ui_CreateCellularSpaceDialogForm.h"
#include "CreateCellularSpaceDialog.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>
#include <QToolButton>

// Boost
#include <boost/filesystem.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

Q_DECLARE_METATYPE(te::map::AbstractLayerPtr);
Q_DECLARE_METATYPE(te::common::UnitOfMeasurePtr);

te::qt::plugins::cellspace::CreateCellularSpaceDialog::CreateCellularSpaceDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f),
    m_bbSRID(4618),
    m_isFile(false),
    m_outputDataSetName(""),
    m_ui(new Ui::CreateCellularSpaceDialogForm)
{
// add controls
  m_ui->setupUi(this);

// Set icons
  QSize iconSize(32, 32);

  m_ui->m_imgLabel->setPixmap(QIcon::fromTheme("cellspace-no-mask-hint").pixmap(48, 48));
  m_ui->m_maskRadioButton->setIconSize(iconSize);
  m_ui->m_maskRadioButton->setIcon(QIcon::fromTheme("cellspace-mask-hint"));
  m_ui->m_noMaskRadioButton->setIconSize(iconSize);
  m_ui->m_noMaskRadioButton->setIcon(QIcon::fromTheme("cellspace-no-mask-hint"));
  m_ui->m_polygonsRadioButton->setIconSize(iconSize);
  m_ui->m_polygonsRadioButton->setIcon(QIcon::fromTheme("cellspace-polygons-hint"));
  m_ui->m_pointsRadioButton->setIconSize(iconSize);
  m_ui->m_pointsRadioButton->setIcon(QIcon::fromTheme("cellspace-points-hint"));
  m_ui->m_targetDatasourceToolButton->setIcon(QIcon::fromTheme("datasource"));
  m_ui->m_srsToolButton->setIcon(QIcon::fromTheme("srs"));

// Set validators
  m_ui->m_llxLineEdit->setValidator(new QDoubleValidator(this));
  m_ui->m_llyLineEdit->setValidator(new QDoubleValidator(this));
  m_ui->m_urxLineEdit->setValidator(new QDoubleValidator(this));
  m_ui->m_uryLineEdit->setValidator(new QDoubleValidator(this));
  m_ui->m_resXLineEdit->setValidator(new QDoubleValidator(0, 99999999, 8, this));
  m_ui->m_resYLineEdit->setValidator(new QDoubleValidator(0, 99999999, 8, this));

  initUnitsOfMeasure();

  connect(m_ui->m_layersComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onLayersComboBoxChanged(int)));
  connect(m_ui->m_unitComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onUnitComboBoxChanged(int)));

  connect(m_ui->m_resXLineEdit, SIGNAL(editingFinished()), this, SLOT(onResXLineEditEditingFinished()));
  connect(m_ui->m_resYLineEdit, SIGNAL(editingFinished()), this, SLOT(onResYLineEditEditingFinished()));
  connect(m_ui->m_llxLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onEnvelopeChanged(const QString &)));
  connect(m_ui->m_llyLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onEnvelopeChanged(const QString &)));
  connect(m_ui->m_urxLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onEnvelopeChanged(const QString &)));
  connect(m_ui->m_uryLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onEnvelopeChanged(const QString &)));

  connect(m_ui->m_targetFileToolButton, SIGNAL(clicked()), this, SLOT(onTargetFileToolButtonClicked()));
  connect(m_ui->m_targetDatasourceToolButton, SIGNAL(clicked()), this, SLOT(onTargetDatasourceToolButtonClicked()));
  connect(m_ui->m_createPushButton, SIGNAL(clicked()), this, SLOT(onCreatePushButtonClicked()));
  connect(m_ui->m_srsToolButton, SIGNAL(clicked()), this, SLOT(onSrsToolButtonClicked()));

  m_ui->m_helpPushButton->setNameSpace("dpi.inpe.br.plugins");
  m_ui->m_helpPushButton->setPageReference("plugins/cellspace/cellspace.html");
}

te::qt::plugins::cellspace::CreateCellularSpaceDialog::~CreateCellularSpaceDialog()
{
}

void te::qt::plugins::cellspace::CreateCellularSpaceDialog::setLayers(std::list<te::map::AbstractLayerPtr> layers)
{
  if (layers.empty())
  {
    m_ui->m_noMaskRadioButton->setChecked(true);
    m_ui->m_maskRadioButton->setEnabled(false);
    m_ui->m_resSRIDLabel->setText("No SRS defined");
    m_ui->m_layerSRIDLabel->setText("No SRS defined");
    m_ui->m_unitComboBox->setCurrentIndex(0);
    m_bbSRID = 0;
    return;
  }
  
  std::list<te::map::AbstractLayerPtr>::iterator it = layers.begin();
  m_bbSRID = it->get()->getSRID();
  while(it != layers.end())
  {
    m_ui->m_layersComboBox->addItem(QString(it->get()->getTitle().c_str()), QVariant::fromValue(*it));
    ++it;
  }
  m_ui->m_layersComboBox->setCurrentIndex(0);
}

te::map::AbstractLayerPtr te::qt::plugins::cellspace::CreateCellularSpaceDialog::getLayer()
{
  if(m_isFile)
  {
    // let's include the new datasource in the managers
    boost::uuids::basic_random_generator<boost::mt19937> gen;
    boost::uuids::uuid u = gen();
    std::string id_ds = boost::uuids::to_string(u);
    
    boost::filesystem::path uri(m_outDataSourceInfo->getConnInfo().host() + m_outDataSourceInfo->getConnInfo().path());
    
    te::da::DataSourceInfoPtr ds(new te::da::DataSourceInfo);
    ds->setConnInfo(m_outDataSourceInfo->getConnInfo());
    ds->setTitle(uri.stem().string());
    ds->setAccessDriver("OGR");
    ds->setType("OGR");
    ds->setDescription(uri.string());
    ds->setId(id_ds);
    
    te::da::DataSourcePtr newds = te::da::DataSourceManager::getInstance().get(id_ds, "OGR", ds->getConnInfo());
    newds->open();
    te::da::DataSourceInfoManager::getInstance().add(ds);
    m_outDataSourceInfo = ds;
    
    m_outputDataSetName = uri.stem().string();
  }
  
  te::da::DataSourcePtr outDataSource = te::da::GetDataSource(m_outDataSourceInfo->getId());
  outDataSource->open();
  
  te::qt::widgets::DataSet2Layer converter(m_outDataSourceInfo->getId());
  
  te::da::DataSetTypePtr dt(outDataSource->getDataSetType(m_outputDataSetName).release());
  
  te::map::AbstractLayerPtr layer = converter(dt);
  
  layer->setSRID(m_bbSRID);
  
  return layer;
}

// whenever the reference layer changes the parameters of the new cellular space will bem modified accordingly
void te::qt::plugins::cellspace::CreateCellularSpaceDialog::onLayersComboBoxChanged(int)
{
  te::map::AbstractLayerPtr layer = getReferenceLayer();
  if(!layer)
    return;

  m_bbSRID = layer->getSRID();
  int precision = 2;
  if (m_bbSRID>0)
  {
    te::common::UnitOfMeasurePtr unit = te::srs::SpatialReferenceSystemManager::getInstance().getUnit(m_bbSRID);
    if (unit->getId() == te::common::UOM_Degree ||
        unit->getId() == te::common::UOM_Radian)
      precision = 6;
    m_ui->m_resSRIDLabel->setText(te::srs::SpatialReferenceSystemManager::getInstance().getName(m_bbSRID).c_str());
    m_ui->m_layerSRIDLabel->setText(te::srs::SpatialReferenceSystemManager::getInstance().getName(m_bbSRID).c_str());
    setResolutionUnit(unit);
  }
  else
  {
    m_ui->m_resSRIDLabel->setText("No SRS defined");
    m_ui->m_layerSRIDLabel->setText("No SRS defined");
    m_ui->m_unitComboBox->setCurrentIndex(0);
    precision = 6;
  }
  showEnvelope(layer->getExtent(),precision);

  if(layer->getSchema()->hasRaster())
  {
    m_ui->m_maskRadioButton->setEnabled(false);
    m_ui->m_noMaskRadioButton->setChecked(true);
  }
  else
  {
    m_ui->m_maskRadioButton->setEnabled(true);
    m_ui->m_maskRadioButton->setChecked(true);
  }
  
}


void te::qt::plugins::cellspace::CreateCellularSpaceDialog::onEnvelopeChanged(const QString &)
{
  clearResolution();
}

void te::qt::plugins::cellspace::CreateCellularSpaceDialog::onSrsToolButtonClicked()
{
  te::qt::widgets::SRSManagerDialog srsDialog(this);
  srsDialog.setWindowTitle(tr("Choose the SRS"));
  
  if(srsDialog.exec() == QDialog::Rejected)
    return;
  
  int newSRID =  srsDialog.getSelectedSRS().first;
  
  if (m_ui->m_maskRadioButton->isChecked())
  {
    te::map::AbstractLayerPtr refLayer = getReferenceLayer();
    if (refLayer &&
        ((newSRID <= 0 && refLayer->getSRID()  > 0) ||
         (newSRID  > 0 && refLayer->getSRID() <= 0) ))
      m_ui->m_noMaskRadioButton->setChecked(true);
  }
  
  if (newSRID <= 0)
  {
    m_ui->m_unitComboBox->setCurrentIndex(0);
    m_ui->m_resSRIDLabel->setText("No SRS defined");
  }
  else
  {
    std::string name = te::srs::SpatialReferenceSystemManager::getInstance().getName(newSRID);
    m_ui->m_resSRIDLabel->setText(name.c_str());
    if (m_bbSRID > 0)
    {
      te::gm::Envelope env = getEnvelope();
      clearEnvelope();
      if (env.isValid())
      {
        int precision = 2;
        te::common::UnitOfMeasurePtr unit = te::srs::SpatialReferenceSystemManager::getInstance().getUnit(newSRID);
        if (unit->getId() == te::common::UOM_Degree ||
            unit->getId() == te::common::UOM_Radian)
          precision = 6;
        
        env.transform(m_bbSRID,newSRID);
        showEnvelope(env, precision);
      }
    }
  }
  m_bbSRID = newSRID;
  te::common::UnitOfMeasurePtr unit = te::srs::SpatialReferenceSystemManager::getInstance().getUnit(m_bbSRID);
  setResolutionUnit(unit);
  clearResolution();
}

// if user change the unit it will have to re-enter resolution
void te::qt::plugins::cellspace::CreateCellularSpaceDialog::onUnitComboBoxChanged(int index)
{
  if (m_bbSRID <= 0 && index != 0)
  {
    QMessageBox::warning(this, tr("Cellular Spaces"), tr("There is no no spatial reference defined for the new cellular space. It is not possible to select a unit for the resolution."));
    m_ui->m_unitComboBox->setCurrentIndex(0);
    return;
  }
  
  if (index == 0)
    return;
  
  clearResolution();
  
  te::common::UnitOfMeasurePtr resUnit = getResolutionUnit();
  te::common::UnitOfMeasurePtr bbUnit = te::srs::SpatialReferenceSystemManager::getInstance().getUnit(m_bbSRID);
  
  if (!resUnit || !bbUnit)
    return;
  
  if (resUnit == bbUnit)
    return;
  
  if ((te::common::UnitsOfMeasureManager::getInstance().areConvertible(bbUnit->getName(), resUnit->getName()) ||  // units are convertible
       // accepting an exception to deal with the most common case: bounding box in geographic coordinates
       // and resolution in meters, kilometers or foot
       (te::srs::SpatialReferenceSystemManager::getInstance().isGeographic(m_bbSRID) &&
        (resUnit->getId() == te::common::UOM_Metre ||
         resUnit->getId() == te::common::UOM_Foot  ||
         resUnit->getId() == te::common::UOM_Kilometre)) ||
       (!te::srs::SpatialReferenceSystemManager::getInstance().isGeographic(m_bbSRID) &&
         (resUnit->getId() == te::common::UOM_Radian ||
         resUnit->getId() == te::common::UOM_Degree ))))
    return;
  
  QMessageBox::warning(this, tr("Cellular Spaces"), tr("Unable to convert between the selected unit for the resolution and the unit of new cell layer SRS"));
  m_ui->m_unitComboBox->setItemText(m_ui->m_unitComboBox->currentIndex(), QString(bbUnit->getName().c_str()));
}

void te::qt::plugins::cellspace::CreateCellularSpaceDialog::onResXLineEditEditingFinished()
{
  te::gm::Envelope env = getEnvelope();
  if(!env.isValid())
  {
    QMessageBox::warning(this, tr("Cellular Spaces"), tr("Invalid bounding box."));
    return;
  }
  
  double resX = m_ui->m_resXLineEdit->text().toDouble();
  unitConvertion(resX, getResolutionUnit(), te::srs::SpatialReferenceSystemManager::getInstance().getUnit(m_bbSRID));
  
  int maxCols = (int)ceil((env.m_urx - env.m_llx)/resX);
  m_ui->m_colsLineEdit->setText(QString::number(maxCols));
}

void te::qt::plugins::cellspace::CreateCellularSpaceDialog::onResYLineEditEditingFinished()
{
  te::gm::Envelope env = getEnvelope();
  if(!env.isValid())
  {
    QMessageBox::warning(this, tr("Cellular Spaces"), tr("Invalid bounding box."));
    return;
  }

  double resY = m_ui->m_resXLineEdit->text().toDouble();
  unitConvertion(resY, getResolutionUnit(), te::srs::SpatialReferenceSystemManager::getInstance().getUnit(m_bbSRID));
  
  int maxRows = (int)ceil((env.m_ury - env.m_lly)/resY);
  m_ui->m_rowsLineEdit->setText(QString::number(maxRows));
}

void te::qt::plugins::cellspace::CreateCellularSpaceDialog::onTargetDatasourceToolButtonClicked()
{
  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_repositoryLineEdit->clear();
  
  te::qt::widgets::DataSourceSelectorDialog dlg(this);
  dlg.exec();
  
  std::list<te::da::DataSourceInfoPtr> dsPtrList = dlg.getSelecteds();
  
  if(dsPtrList.empty())
    return;
  
  std::list<te::da::DataSourceInfoPtr>::iterator it = dsPtrList.begin();
  
  m_outDataSourceInfo = *it;
  
  m_ui->m_repositoryLineEdit->setText(QString(it->get()->getTitle().c_str()));
  
  m_isFile = false;
  
  m_ui->m_newLayerNameLineEdit->setEnabled(true);
}

void te::qt::plugins::cellspace::CreateCellularSpaceDialog::onTargetFileToolButtonClicked()
{
  m_ui->m_newLayerNameLineEdit->clear();
  m_ui->m_repositoryLineEdit->clear();
  
  if (m_ui->m_colsLineEdit->text().isEmpty())
    onResXLineEditEditingFinished();
  
  if (m_ui->m_rowsLineEdit->text().isEmpty())
    onResYLineEditEditingFinished();
  
  QString extension;
  
  std::string accessDriver;
  
  extension = tr("Shapefile (*.shp *.SHP);;");
  accessDriver = "OGR";
  
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                                  QString(), extension, 0, QFileDialog::DontConfirmOverwrite);
  
  if (fileName.isEmpty())
    return;
  
  boost::filesystem::path outfile(fileName.toUtf8().data());
  std::string aux = outfile.leaf().string();
  m_ui->m_newLayerNameLineEdit->setText(aux.c_str());
  aux = outfile.string();
  m_ui->m_repositoryLineEdit->setText(aux.c_str());
  
  m_isFile = true;
  
  m_ui->m_newLayerNameLineEdit->setEnabled(false);
  
  std::string connInfo("file://");
  connInfo += m_ui->m_repositoryLineEdit->text().toUtf8().data();
  
  m_outDataSourceInfo.reset(new te::da::DataSourceInfo);
  
  m_outDataSourceInfo->setAccessDriver(accessDriver);
  m_outDataSourceInfo->setConnInfo(connInfo);
}

void te::qt::plugins::cellspace::CreateCellularSpaceDialog::onCreatePushButtonClicked()
{
  std::string errors;
  if(!checkList(errors))
  {
    QMessageBox::warning(this, tr("Cellular Spaces"), errors.c_str());
    return;
  }

  std::auto_ptr<te::cellspace::CellularSpacesOperations> cellSpaceOp(new te::cellspace::CellularSpacesOperations());

  te::cellspace::CellularSpacesOperations::CellSpaceType type;

  if(m_ui->m_polygonsRadioButton->isChecked())
  {
    type = te::cellspace::CellularSpacesOperations::CELLSPACE_POLYGONS;
  }
  else
  {
    type = te::cellspace::CellularSpacesOperations::CELLSPACE_POINTS;
  }

  te::map::AbstractLayerPtr referenceLayer;
  if (m_ui->m_maskRadioButton->isChecked())
    referenceLayer = getReferenceLayer();
  
  m_outputDataSetName = m_ui->m_newLayerNameLineEdit->text().toUtf8().data();
  
  
  //progress
  te::qt::widgets::ProgressViewerDialog v(this);
  int id = te::common::ProgressManager::getInstance().addViewer(&v);

  try
  {
    te::qt::widgets::ScopedCursor cursor(Qt::WaitCursor);
    cellSpaceOp->createCellSpace(m_outDataSourceInfo, m_outputDataSetName,
                                   getResX(), getResY(), getEnvelope(), m_bbSRID,
                                   type,
                                   referenceLayer);
  }
  catch(te::common::Exception& e)
  {
    te::common::ProgressManager::getInstance().removeViewer(id);
    this->setCursor(Qt::ArrowCursor);
    QMessageBox::warning(this, tr("Cellular Spaces"), e.what());
    reject();
    return;
  }

  te::common::ProgressManager::getInstance().removeViewer(id);
  this->setCursor(Qt::ArrowCursor);

  accept();
}

void te::qt::plugins::cellspace::CreateCellularSpaceDialog::initUnitsOfMeasure()
{
  m_ui->m_unitComboBox->addItem("UNIT");
  
  te::common::UnitsOfMeasureManager::const_iterator it = te::common::UnitsOfMeasureManager::getInstance().begin();
  te::common::UnitsOfMeasureManager::const_iterator itend = te::common::UnitsOfMeasureManager::getInstance().end();
  while (it != itend)
  {
    te::common::UnitOfMeasurePtr uptr = it->second;
    if (it->second->getType() == te::common::Length ||
        it->second->getType() == te::common::Angle)
      m_ui->m_unitComboBox->addItem(uptr->getName().c_str(), QVariant::fromValue(uptr));
    ++it;
  }

  if(m_bbSRID > 0)
  {
    te::common::UnitOfMeasurePtr unit = te::srs::SpatialReferenceSystemManager::getInstance().getUnit(m_bbSRID);
    if (unit.get())
      setResolutionUnit(unit);
    return;
  }
  m_ui->m_unitComboBox->setCurrentIndex(0);
}

void te::qt::plugins::cellspace::CreateCellularSpaceDialog::showEnvelope(const te::gm::Envelope& env, int precision)
{
  m_ui->m_llxLineEdit->setText(QString::number(env.m_llx,'f',precision));
  m_ui->m_llyLineEdit->setText(QString::number(env.m_lly,'f',precision));
  m_ui->m_urxLineEdit->setText(QString::number(env.m_urx,'f',precision));
  m_ui->m_uryLineEdit->setText(QString::number(env.m_ury,'f',precision));
}

void te::qt::plugins::cellspace::CreateCellularSpaceDialog::setResolutionUnit(te::common::UnitOfMeasurePtr unit)
{
  if (!unit)
  {
    m_ui->m_unitComboBox->setCurrentIndex(0);
    return;
  }
  
  for(int i = 0; i < m_ui->m_unitComboBox->count(); ++i)
  {
    std::string name = m_ui->m_unitComboBox->itemText(i).toUtf8().data();
    
    if(unit->getName() == name)
    {
      m_ui->m_unitComboBox->setCurrentIndex(i);
      break;
    }
  }
}

te::common::UnitOfMeasurePtr te::qt::plugins::cellspace::CreateCellularSpaceDialog::getResolutionUnit()
{
  QVariant varUnit = m_ui->m_unitComboBox->itemData(m_ui->m_unitComboBox->currentIndex(), Qt::UserRole);
  return varUnit.value<te::common::UnitOfMeasurePtr>();
}

te::map::AbstractLayerPtr te::qt::plugins::cellspace::CreateCellularSpaceDialog::getReferenceLayer()
{
  QVariant varLayer = m_ui->m_layersComboBox->itemData(m_ui->m_layersComboBox->currentIndex(), Qt::UserRole);
  return varLayer.value<te::map::AbstractLayerPtr>();
}

te::gm::Envelope te::qt::plugins::cellspace::CreateCellularSpaceDialog::getEnvelope()
{
  te::gm::Envelope env;
  env.m_llx = m_ui->m_llxLineEdit->text().toDouble();
  env.m_lly = m_ui->m_llyLineEdit->text().toDouble();
  env.m_urx = m_ui->m_urxLineEdit->text().toDouble();
  env.m_ury = m_ui->m_uryLineEdit->text().toDouble();

  return env;
}

void te::qt::plugins::cellspace::CreateCellularSpaceDialog::clearEnvelope()
{
  m_ui->m_llxLineEdit->clear();
  m_ui->m_llyLineEdit->clear();
  m_ui->m_urxLineEdit->clear();
  m_ui->m_uryLineEdit->clear();
}


double te::qt::plugins::cellspace::CreateCellularSpaceDialog::getResX()
{
  double resX = m_ui->m_resXLineEdit->text().toDouble();
  if (m_bbSRID <=0) // if there is no output SRS defined
    return resX;         // no check to be done
  
  unitConvertion(resX, getResolutionUnit(), te::srs::SpatialReferenceSystemManager::getInstance().getUnit(m_bbSRID));
  
  return resX;
}

double te::qt::plugins::cellspace::CreateCellularSpaceDialog::getResY()
{
  double resY = m_ui->m_resYLineEdit->text().toDouble();
  if (m_bbSRID <=0) // if there is no output SRS defined
    return resY;         // no check to be done
  
  unitConvertion(resY, getResolutionUnit(), te::srs::SpatialReferenceSystemManager::getInstance().getUnit(m_bbSRID));
  
  return resY;
}


bool te::qt::plugins::cellspace::CreateCellularSpaceDialog::unitConvertion(double& res,
                                                                    te::common::UnitOfMeasurePtr from,
                                                                    te::common::UnitOfMeasurePtr to)
{
  // no need to convert
  if(from == to)
    return true;
  
  if (!from || !to)
    return false;
  
  // check to see if conversion is possible
  if (te::common::UnitsOfMeasureManager::getInstance().areConvertible(from->getName(), to->getName()))
  {
    try
    {
      double factor = te::common::UnitsOfMeasureManager::getInstance().getConversion(from->getName(),to->getName());
      res *= factor;
      return true;
    }
    catch(te::common::Exception& /*e*/)
    {
      return false;
    }
  }
  
  // try to handle the most common: degree <-> meters and its variations
  if (from->getId() == te::common::UOM_Degree)
    return (convertAngleToPlanar(res, to));
  
  if (to->getId() == te::common::UOM_Degree)
    return (convertPlanarToAngle(res, from));
  
  return false;
}

bool te::qt::plugins::cellspace::CreateCellularSpaceDialog::convertAngleToPlanar(double& val, te::common::UnitOfMeasurePtr planar)
{
  switch (planar->getId())
  {
    case te::common::UOM_Metre:
      val *= 111000;            // 1 degree = 111.000 meters
      break;
    case te::common::UOM_Kilometre:
      val *= 111;               // 1 degree = 111 kilometers
      break;
    case te::common::UOM_Foot:
      val *= 364173.24;        //  1 feet  = 3.28084 meters
      break;
    default:
      return false;
  }
  return true;
}

bool te::qt::plugins::cellspace::CreateCellularSpaceDialog::convertPlanarToAngle(double& val, te::common::UnitOfMeasurePtr planar)
{
  switch (planar->getId())
  {
    case te::common::UOM_Metre:
      val /= 111000;            // 1 degree = 111.000 meters
      break;
    case te::common::UOM_Kilometre:
      val /= 111;               // 1 degree = 111 kilometers
      break;
    case te::common::UOM_Foot:
      val /= 364173.24;        //  1 feet  = 3.28084 meters
      break;
    default:
      return false;
  }
  return true;
}

void te::qt::plugins::cellspace::CreateCellularSpaceDialog::clearResolution()
{
  m_ui->m_resXLineEdit->clear();
  m_ui->m_resYLineEdit->clear();
  m_ui->m_colsLineEdit->clear();
  m_ui->m_rowsLineEdit->clear();
}

void te::qt::plugins::cellspace::CreateCellularSpaceDialog::showSRS()
{
  if (m_bbSRID > 0)
  {
    std::string name = te::srs::SpatialReferenceSystemManager::getInstance().getName(m_bbSRID);
    m_ui->m_resSRIDLabel->setText(name.c_str());
  }
  else
  {
    m_ui->m_resSRIDLabel->setText("No SRS defined");
  }
}

bool te::qt::plugins::cellspace::CreateCellularSpaceDialog::checkList(std::string& errors)
{
  bool noErrors = true;
  errors = tr("Consistency errors:").toUtf8().data();
  
  te::gm::Envelope env = getEnvelope();
  if(!env.isValid())
  {
    errors += "\n - " + std::string(tr("Invalid bounding box").toUtf8().data());
    noErrors = false;
  }
  
  double resX = getResX();
  if (resX < 0. || resX >= env.getWidth())
  {
    char txt[40];
    sprintf(txt,"%.8g",env.getWidth());
    errors += "\n - " + std::string(tr("X resolution must be greater than 0 and smaller than the bounding box Width ").toUtf8().data()) + txt;
    noErrors = false;
  }
  
  double resY = getResY();
  if (resY < 0. || resY >= env.getHeight())
  {
    char txt[40];
    sprintf(txt,"%.8g",env.getHeight());
    errors += "\n - " + std::string(tr("Y resolution must be greater than 0 and smaller than the bounding box Height ").toUtf8().data()) + txt;
    noErrors = false;
  }
  
  std::string name = m_ui->m_newLayerNameLineEdit->text().toUtf8().data();
  if (name.empty())
  {
    errors += "\n - " + std::string(tr("Output layer name is empty").toUtf8().data());
    noErrors = false;
  }
  
  if (m_ui->m_maskRadioButton->isChecked())
  {
    te::map::AbstractLayerPtr referenceLayer = getReferenceLayer();
    if (!((referenceLayer->getSRID() >  0 && m_bbSRID > 0) ||
          (referenceLayer->getSRID() <= 0 && m_bbSRID <= 0)))
    {
      errors += "\n - " + std::string(tr("Reference layer and output layer have incompatible SRSs. It is not possible to create cells using polygons as masks.").toUtf8().data());
      noErrors = false;
    }
  }
  return noErrors;
}

