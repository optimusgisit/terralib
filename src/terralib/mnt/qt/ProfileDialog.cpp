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
  \file terralib/mnt/qt/ProfileDialog.cpp

  \brief .
*/
//TerraLib

#include "../../edit/qt/Renderer.h"
#include "../../edit/qt/tools/CreateLineTool.h"
#include "../../edit/qt/tools/DeleteGeometryTool.h"
#include "../../edit/qt/tools/VertexTool.h"
#include "../../edit/Feature.h"
#include "../../edit/Repository.h"
#include "../../edit/RepositoryManager.h"
#include "../../qt/af/events/MapEvents.h"
#include "../../qt/widgets/Utils.h"
#include "../../dataaccess/datasource/DataSourceFactory.h"
#include "../../dataaccess/datasource/DataSourceInfoManager.h"
#include "../../dataaccess/datasource/DataSourceManager.h"
#include "../../dataaccess/utils/Utils.h"
#include "../../geometry/GeometryProperty.h"
#include "../../mnt/core/Profile.h"
#include "../../qt/af/ApplicationController.h"
#include "../../qt/af/BaseApplication.h"
#include "../../qt/af/connectors/MapDisplay.h"
#include "../../qt/widgets/canvas/Canvas.h"
#include "../../qt/widgets/datasource/selector/DataSourceSelectorDialog.h"
#include "../../qt/widgets/layer/utils/DataSet2Layer.h"
#include "../../qt/widgets/canvas/MapDisplay.h"
#include "../../raster.h"
#include "../../statistics/core/Utils.h"

#include "ProfileDialog.h"
#include "ProfileResultDialog.h"
#include "ui_ProfileDialogForm.h"

// Qt
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QValidator>

// STL
#include <memory>

// Boost
#include <boost/filesystem.hpp>



Q_DECLARE_METATYPE(te::map::AbstractLayerPtr)

te::mnt::ProfileDialog::ProfileDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f),
    m_ui(new Ui::ProfileDialogForm),
    m_tooltype(ToolNone),
    m_tool(0),
    m_app(0)
{
// add controls
  m_ui->setupUi(this);

// add icons
  m_ui->m_addPointMousePushButton->setIcon(QIcon::fromTheme("mnt-profile-line create"));
  m_ui->m_changePointPushButton->setIcon(QIcon::fromTheme("mnt-profile-vertex move"));
  m_ui->m_addPointPushButton->setIcon(QIcon::fromTheme("mnt-profile-vertex create"));
  m_ui->m_deletePointPushButton->setIcon(QIcon::fromTheme("mnt-profile-vertex delete"));
  m_ui->m_deletePathPushButton->setIcon(QIcon::fromTheme("mnt-profile-delete"));
  m_ui->m_invertPushButton->setIcon(QIcon::fromTheme("mnt-profile-invert"));

// connectors
  connect(m_ui->m_inputLayersComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onInputComboBoxChanged(int)));
  
  connect(m_ui->m_dummycheckBox, SIGNAL(toggled(bool)), m_ui->m_dummylineEdit, SLOT(setEnabled(bool)));

  connect(m_ui->m_editionradioButton, SIGNAL(toggled(bool)), this, SLOT(oneditionEnabled(bool)));
  connect(m_ui->m_selectionradioButton, SIGNAL(toggled(bool)), this, SLOT(onselectionEnabled(bool)));

  connect(m_ui->m_vectorlayersComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onVectorInputComboBoxChanged(int)));

  connect(m_ui->m_okPushButton, SIGNAL(clicked()), this, SLOT(onOkPushButtonClicked()));

  connect(m_ui->m_addPointMousePushButton, SIGNAL(toggled(bool)), SLOT(onaddPointMouseToggled(bool)));
  connect(m_ui->m_changePointPushButton, SIGNAL(toggled(bool)), SLOT(onchangePointToggled(bool)));
  connect(m_ui->m_addPointPushButton, SIGNAL(toggled(bool)), SLOT(onaddPointToggled(bool)));
  connect(m_ui->m_deletePointPushButton, SIGNAL(toggled(bool)), SLOT(ondeletePointToggled(bool)));
  connect(m_ui->m_deletePathPushButton, SIGNAL(toggled(bool)), SLOT(ondeletePathToggled(bool)));
  connect(m_ui->m_invertPushButton, SIGNAL(toggled(bool)), SLOT(oninvertToggled(bool)));

  m_color.push_back(te::color::RGBAColor(255, 0, 0, 255));
  m_color.push_back(te::color::RGBAColor(255, 0, 255, 255));
  m_color.push_back(te::color::RGBAColor(0, 255, 255, 255));
  m_color.push_back(te::color::RGBAColor(37, 127, 0, 255));
  m_color.push_back(te::color::RGBAColor(0, 0, 255, 255));

  m_vertex = true;

  setDefaultInterface();
}

te::mnt::ProfileDialog::~ProfileDialog()
{
}

te::map::AbstractLayerPtr te::mnt::ProfileDialog::getLayer()
{
  return m_outputLayer;
}

void te::mnt::ProfileDialog::setLayers(std::list<te::map::AbstractLayerPtr> layers)
{
  m_layers = layers;
  m_ui->m_vectorlayersComboBox->clear();
  m_ui->m_inputLayersComboBox->clear();

  m_ui->m_vectorlayersComboBox->addItem(QString(""), QVariant(""));

  std::list<te::map::AbstractLayerPtr>::iterator it = m_layers.begin();

  while (it != m_layers.end())
  {
    if (it->get())
    {
      if (it->get()->isValid())
      {
        std::auto_ptr<te::da::DataSetType> dsType = it->get()->getSchema();
        if (dsType.get())
        {
          if (dsType->hasGeom())
          {
            std::auto_ptr<te::gm::GeometryProperty>geomProp(te::da::GetFirstGeomProperty(dsType.get()));
            te::gm::GeomType gmType = geomProp->getGeometryType();
            if (gmType == te::gm::LineStringType || gmType == te::gm::MultiLineStringType ||
              gmType == te::gm::LineStringZType || gmType == te::gm::MultiLineStringZType ||
              gmType == te::gm::LineStringMType || gmType == te::gm::MultiLineStringMType ||
              gmType == te::gm::LineStringZMType || gmType == te::gm::MultiLineStringZMType)
            {
              m_ui->m_vectorlayersComboBox->addItem(QString(it->get()->getTitle().c_str()), QVariant(it->get()->getId().c_str()));
            }

            geomProp.release();
          }
          m_ui->m_inputLayersComboBox->addItem(QString(it->get()->getTitle().c_str()), QVariant(it->get()->getId().c_str()));
          dsType.release();
        }
      }
    }
    ++it;
  }

  setDefaultInterface();
}

void te::mnt::ProfileDialog::setDefaultInterface()
{
  m_ui->m_selectionradioButton->setChecked(true);
  m_ui->m_trajectorystackedWidget->setCurrentIndex(1);
}

void te::mnt::ProfileDialog::release()
{
  m_visadas.clear();
  DrawSelected(m_visadas, 1, false);
  if (m_tool)
  {
    m_app->getMapDisplay()->removeEventFilter(m_tool);
    delete m_tool;
    m_tool = 0;
  }
  m_tooltype = ToolNone;

  if (m_inputLayer)
  {
    te::edit::Repository* repository = te::edit::RepositoryManager::getInstance().getRepository(m_inputLayer->getId());
    if (repository)
      repository->clear();
  }

  te::qt::af::BaseApplication* window = dynamic_cast<te::qt::af::BaseApplication*>(te::qt::af::AppCtrlSingleton::getInstance().getMainWindow());
  window->onDrawTriggered();
 
  setDefaultInterface();

}

void te::mnt::ProfileDialog::onInputComboBoxChanged(int index)
{
  m_inputLayer = 0;
  std::list<te::map::AbstractLayerPtr>::iterator it = m_layers.begin();
  std::string layerID = m_ui->m_inputLayersComboBox->itemData(index, Qt::UserRole).toString().toStdString();
  while (it != m_layers.end())
  {
    if (layerID == it->get()->getId().c_str())
    {
      m_inputLayer = it->get();
      std::auto_ptr<te::da::DataSetType> dsType = m_inputLayer->getSchema();
      te::map::DataSetLayer* indsLayer = dynamic_cast<te::map::DataSetLayer*>(m_inputLayer.get());
      te::da::DataSourcePtr inDataSource = te::da::GetDataSource(indsLayer->getDataSourceId(), true);
      std::auto_ptr<te::da::DataSet> dsRaster = inDataSource->getDataSet(indsLayer->getDataSetName());
      if (dsType->hasRaster()) //GRID
      {
        te::rst::RasterProperty* rasterProp = te::da::GetFirstRasterProperty(dsType.get());
        std::auto_ptr<te::rst::Raster> in_raster = dsRaster->getRaster(rasterProp->getName());
        m_ui->m_dummylineEdit->setText(QString::number(in_raster->getBand(0)->getProperty()->m_noDataValue));
        m_tol = in_raster->getResolutionX() * 3;
        in_raster.release();
        dsRaster.release();
      }
      m_srid = m_inputLayer->getSRID();
      dsType.release();
    }
    ++it;
  }

}

void  te::mnt::ProfileDialog::oneditionEnabled(bool checked)
{
  if (!checked)
    return;

  m_ui->m_trajectorystackedWidget->setCurrentIndex(0);

  m_app = dynamic_cast<te::qt::af::BaseApplication*>(te::qt::af::AppCtrlSingleton::getInstance().getMainWindow());
  m_srid = m_app->getMapDisplay()->getSRID();

  m_ui->m_addPointMousePushButton->setChecked(true);
  m_ui->m_addPointMousePushButton->toggled(true);

  m_ui->m_vectorlayersComboBox->setCurrentIndex(0);
  m_visadas.clear();
}

void  te::mnt::ProfileDialog::onselectionEnabled(bool checked)
{
  if (!checked)
    return;
  
  release();

  m_ui->m_trajectorystackedWidget->setCurrentIndex(1);
  m_ui->m_vectorlayersComboBox->setCurrentIndex(0);
}


void te::mnt::ProfileDialog::onVectorInputComboBoxChanged(int index)
{
  m_trajectoryLayer = 0;
  
  std::list<te::map::AbstractLayerPtr>::iterator it = m_layers.begin();
  std::string layerID = m_ui->m_vectorlayersComboBox->itemData(index, Qt::UserRole).toString().toStdString();
  while (it != m_layers.end())
  {
    if (layerID == it->get()->getId().c_str())
    {
      m_trajectoryLayer = it->get();
      if (m_trajectoryLayer == m_inputLayer)
      {
        m_ui->m_vectorlayersComboBox->setCurrentIndex(0);
        m_trajectoryLayer = 0;
        throw te::common::Exception(TE_TR("Trajectory layer must be different from input layer!"));
      }
      //std::auto_ptr<te::da::DataSetType> dsType = m_trajectoryLayer->getSchema();
      //std::vector<te::dt::Property*> props = dsType->getProperties();
      //dsType.release();
    }
    ++it;
  }
}

void te::mnt::ProfileDialog::onaddPointMouseToggled(bool checked)
{
  if (!checked)
    return;

  if (!m_app)
    return;

  if (!m_inputLayer)
    return;

  if (m_tooltype != CreateLine)
  {
    m_tooltype = CreateLine;
    if (m_tool)
      delete m_tool;

    m_tool = new te::edit::CreateLineTool(m_app->getMapDisplay(), m_inputLayer, Qt::ArrowCursor, 0);
    m_app->getMapDisplay()->installEventFilter(m_tool);

    connect(m_tool, SIGNAL(geometriesEdited()), SLOT(onGeometriesChanged()));

    m_vertex = true;
  }

  DrawSelected(m_visadas, 1, m_vertex);
}

void te::mnt::ProfileDialog::onchangePointToggled(bool checked)
{
  if (!checked)
    return;

  if (!m_inputLayer)
    return;

  setVertexEdition();

  ((te::mnt::ProfileTools*)m_tool)->setType(VERTEX_MOVE);

  m_vertex = true;
  DrawSelected(m_visadas, 1, m_vertex);

}

void te::mnt::ProfileDialog::onaddPointToggled(bool checked)
{
  if (!checked)
    return;

  if (!m_inputLayer)
    return;

  setVertexEdition();

  ((te::mnt::ProfileTools*)m_tool)->setType(VERTEX_ADD);

  m_vertex = true;
  DrawSelected(m_visadas, 1, m_vertex);
}

void te::mnt::ProfileDialog::ondeletePointToggled(bool checked)
{
  if (!checked)
    return;

  if (!m_inputLayer)
    return;

  setVertexEdition();

  ((te::mnt::ProfileTools*)m_tool)->setType(VERTEX_DELETE);

  m_vertex = true;
  DrawSelected(m_visadas, 1, m_vertex);
}

void te::mnt::ProfileDialog::ondeletePathToggled(bool checked)
{
  if (!checked)
    return;

  if (!m_inputLayer)
    return;

  ((te::mnt::ProfileTools*)m_tool)->setType(LINE_DELETE);

  if (m_tooltype != DeleteLine)
  {
    m_tooltype = DeleteLine;
    if (m_tool)
      delete m_tool;

    m_tool = new te::edit::DeleteGeometryTool(m_app->getMapDisplay(), m_inputLayer, 0);
    m_app->getMapDisplay()->installEventFilter(m_tool);
    te::edit::Repository *rep = te::edit::RepositoryManager::getInstance().getRepository(m_inputLayer->getId());
    if (rep)
    {
      rep->clear();
      for (size_t i = 0; i < m_visadas.size(); i++)
      {
        rep->add(m_visadas[i], te::edit::GEOMETRY_DELETE);
      }
    }

    connect(m_tool, SIGNAL(geometriesEdited()), SLOT(onGeometriesChanged()));
  }

  m_vertex = false;
  DrawSelected(m_visadas, 1, m_vertex);

}

void te::mnt::ProfileDialog::oninvertToggled(bool checked)
{
  if (!checked)
    return;
 
  if (!m_inputLayer)
    return;

  setVertexEdition();
  
  ((te::mnt::ProfileTools*)m_tool)->setType(LINE_INVERT);
  connect(m_tool, SIGNAL(geometriesEdited()), SLOT(onGeometriesChanged()));

  m_vertex = false;
  DrawSelected(m_visadas, 1, m_vertex);
}

void te::mnt::ProfileDialog::onGeometriesChanged()
{
  if (!m_inputLayer)
    return;

  if (m_tooltype == CreateLine)
  {
    te::edit::CreateLineTool *ct = dynamic_cast<te::edit::CreateLineTool*>(m_tool);
    te::gm::LineString *ls = dynamic_cast<te::gm::LineString*>(ct->buildLine());

    m_visadas.push_back(ls);
  }
  else if (m_tooltype == DeleteLine)
  {
    std::vector<te::edit::Feature*> features = te::edit::RepositoryManager::getInstance().getFeatures(m_inputLayer->getId(), m_inputLayer->getExtent(), m_inputLayer->getSRID());
    for (size_t i = 0; i < features.size(); i++)
    { 
      if (features[i]->getOperationType() == te::edit::GEOMETRY_UPDATE)
      {
        std::vector<te::gm::LineString*> ld;
        ld.push_back(dynamic_cast<te::gm::LineString*>(features[i]->getGeometry()));
        DrawSelected(ld, 3, false);
        if (QMessageBox::question(0, tr("Profile"), tr("Remove this trajectory?")) == QMessageBox::Yes)
        {
          for (size_t j = 0; j < m_visadas.size(); j++)
          {
            if (m_visadas[j]->equals(ld[0]))
            {
              m_visadas.erase(m_visadas.begin() + j);
              break;
            }
          }
        }

        te::edit::Repository* repository = te::edit::RepositoryManager::getInstance().getRepository(m_inputLayer->getId());
        if (repository)
          repository->clear();
        for (size_t j = 0; j < m_visadas.size(); j++)
        {
          repository->add(m_visadas[j], te::edit::GEOMETRY_DELETE);
        }

        break;
      }
    }
  }
  else
  {
    if (((te::mnt::ProfileTools*)m_tool)->getType() != te::mnt::VERTEX_ADD)
    {
      te::edit::Repository* repository = te::edit::RepositoryManager::getInstance().getRepository(m_inputLayer->getId());
      if (repository)
        repository->clear();
      ((te::mnt::ProfileTools*)m_tool)->setLines(m_visadas);
    }
  }

  testGeometries();

  DrawSelected(m_visadas, 1, m_vertex);

}

void te::mnt::ProfileDialog::onOkPushButtonClicked()
{
  te::map::Visibility visibility;

  try
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    //raster
    if (!m_tool)
    {
      if (!m_inputLayer)
        throw te::common::Exception(TE_TR("Select an input layer!"));

      if (m_trajectoryLayer)
        if (m_inputLayer->getSRID() != m_trajectoryLayer->getSRID())
            throw te::common::Exception(TE_TR("Can not execute this operation with different SRIDs geometries!"));
    }

    m_dummy = m_ui->m_dummylineEdit->text().toDouble();

    te::map::DataSetLayer* indsLayer = dynamic_cast<te::map::DataSetLayer*>(m_inputLayer.get());
    if (!indsLayer)
      throw te::common::Exception(TE_TR("Can not execute this operation on this type of layer!"));

    te::da::DataSourcePtr inDataSource = te::da::GetDataSource(indsLayer->getDataSourceId(), true);
    if (!inDataSource.get())
      throw te::common::Exception(TE_TR("The selected input data source can not be accessed!"));

    std::string inDsetName = indsLayer->getDataSetName();
    std::auto_ptr<te::da::DataSetType> inDsetType(inDataSource->getDataSetType(inDsetName));
    // end raster

    Profile* profile = new Profile();
    profile->setInput(inDataSource, inDsetName, inDsetType, m_dummy);
    profile->setSRID(m_inputLayer->getSRID());

    std::string geostype;

    //vector
    if (m_ui->m_selectionradioButton->isChecked())
    {
      if (!m_trajectoryLayer)
        throw te::common::Exception(TE_TR("Select an vector layer!"));
      std::auto_ptr<te::da::DataSet> inDset;

      m_srid = m_trajectoryLayer->getSRID();

      if (m_ui->m_selectCheckBox->isChecked())
      {
        const te::da::ObjectIdSet* objSet = m_trajectoryLayer->getSelected();
        inDset = m_trajectoryLayer->getData(objSet);
      }
      else
        inDset = te::da::GetDataSource(m_trajectoryLayer->getDataSourceId(), true)->getDataSet(m_trajectoryLayer->getDataSetName());

      std::size_t geo_pos = te::da::GetFirstPropertyPos(inDset.get(), te::dt::GEOMETRY_TYPE);

      inDset->moveBeforeFirst();

      m_visadas.clear();

      while (inDset->moveNext())
      {
        std::auto_ptr<te::gm::Geometry> gin = inDset->getGeometry(geo_pos);
        gin->setSRID(m_srid);
        geostype = gin.get()->getGeometryType();

        if (geostype == "LineString")
        {
          te::gm::LineString *l = dynamic_cast<te::gm::LineString*>(gin.get()->clone());
          m_visadas.push_back(l);
        }
        if (geostype == "MultiLineString")
        {
          te::gm::MultiLineString *g = dynamic_cast<te::gm::MultiLineString*>(gin.get());
          std::size_t np = g->getNumGeometries();
          for (std::size_t i = 0; i < np; ++i)
          {
            te::gm::LineString *l = dynamic_cast<te::gm::LineString*>(g->getGeometryN(i)->clone());
            m_visadas.push_back(l);
          }
        }
      }

      visibility = m_trajectoryLayer->getVisibility();
      if (visibility == te::map::VISIBLE)
        m_trajectoryLayer->setVisibility(te::map::NOT_VISIBLE);
    }
    else
    {
      if (!m_visadas.size())
        throw te::common::Exception(TE_TR("No trajectory selected!"));
    }
    //end vector

    // Principal function calling
    std::vector<te::gm::LineString*> profileSet;
    std::auto_ptr<te::da::DataSetType> dsType = m_inputLayer->getSchema();
    if (dsType->hasRaster())
    {
      profile->runRasterProfile(m_visadas, profileSet);
    }
    if (dsType->hasGeom())
    {
      std::auto_ptr<te::gm::GeometryProperty>geomProp(te::da::GetFirstGeomProperty(dsType.get()));
      te::gm::GeomType gmType = geomProp->getGeometryType();
      if (gmType == te::gm::LineStringType || gmType == te::gm::MultiLineStringType ||
        gmType == te::gm::LineStringZType || gmType == te::gm::MultiLineStringZType ||
        gmType == te::gm::LineStringMType || gmType == te::gm::MultiLineStringMType ||
        gmType == te::gm::LineStringZMType || gmType == te::gm::MultiLineStringZMType)
        {
          profile->runIsolinesProfile(m_visadas, profileSet);
        }
      else if (gmType == te::gm::TINType || gmType == te::gm::MultiPolygonType || gmType == te::gm::PolyhedralSurfaceType || gmType == te::gm::PolygonType ||
        gmType == te::gm::TINZType || gmType == te::gm::MultiPolygonZType || gmType == te::gm::PolyhedralSurfaceZType || gmType == te::gm::PolygonZType ||
        gmType == te::gm::GeometryType)//TIN
        {
        }
      else
        throw te::common::Exception(TE_TR("Input layer type is invalid!"));

    }

    DrawSelected(m_visadas, 2, false);

    dsType.release();

    te::mnt::ProfileResultDialog result(m_ui->m_titleLineEdit->text(), m_ui->m_yAxisLineEdit->text(), profileSet, m_color, this->parentWidget());

    QApplication::restoreOverrideCursor();
    if (result.exec() != QDialog::Accepted)
    {
      if (m_trajectoryLayer)
      {
        m_trajectoryLayer->setVisibility(visibility);
        m_visadas.clear();
      }

      DrawSelected(m_visadas, 1, false);

      return;
    }
  }
  catch (const std::exception& e)
  {
    QApplication::restoreOverrideCursor();
    QMessageBox::information(this, tr("Profile "), e.what());

    if (m_trajectoryLayer)
      m_trajectoryLayer->setVisibility(visibility);
    release();

    return;
  }

  if (m_trajectoryLayer)
    m_trajectoryLayer->setVisibility(visibility);
  release();

  if (m_tool)
    delete m_tool;
  m_tool = 0;

  QApplication::restoreOverrideCursor();
  accept();

}

void te::mnt::ProfileDialog::DrawSelected(const std::vector<te::gm::LineString*> visadas, int width, bool vertex)
{
  te::qt::af::BaseApplication* window = dynamic_cast<te::qt::af::BaseApplication*>(te::qt::af::AppCtrlSingleton::getInstance().getMainWindow());
  window->onDrawTriggered();

  const te::gm::Envelope& displayExtent = window->getMapDisplay()->getExtent();
  te::qt::widgets::Canvas canvas(window->getMapDisplay()->getDisplayPixmap());
  canvas.setWindow(displayExtent.m_llx, displayExtent.m_lly, displayExtent.m_urx, displayExtent.m_ury);
  canvas.setLineWidth(width);
  canvas.setLineDashStyle(te::map::SolidLine);
  te::qt::widgets::Config2DrawPoints(&canvas, "circle", 8, Qt::red, Qt::red, 1);

  bool needRemap = false;

  if ((m_srid != TE_UNKNOWN_SRS) && (window->getMapDisplay()->getSRID() != TE_UNKNOWN_SRS) && (m_srid != window->getMapDisplay()->getSRID()))
    needRemap = true;

  for (unsigned int v = 0; v < visadas.size(); ++v)
  {
    canvas.setLineColor(m_color[v%m_color.size()]);
    canvas.setPointColor(m_color[v%m_color.size()]);
    if (needRemap || visadas[v]->getSRID() != m_srid)
    {
      visadas[v]->setSRID(m_srid);
      visadas[v]->transform(window->getMapDisplay()->getSRID());
    }
    canvas.draw(visadas[v]);
    if (vertex)
    {
      for (size_t p = 0; p < visadas[v]->getNPoints(); p++)
      {
        std::auto_ptr<te::gm::Point> point(visadas[v]->getPointN(p));
        canvas.draw(point.get());
      }
    }

  }
  window->getMapDisplay()->repaint();
}

void  te::mnt::ProfileDialog::testGeometries()
{
  bool changed = false;
  for (size_t i = 0; i < m_visadas.size(); i++)
  {
    if (m_visadas[i]->getNPoints() < 2)
    {
      m_visadas.erase(m_visadas.begin() + i);
      i--;
      changed = true;
    }
  }
  if (changed)
    ((te::mnt::ProfileTools*)m_tool)->setLines(m_visadas);

}

void te::mnt::ProfileDialog::setVertexEdition()
{
  if (m_tooltype != EditVertex)
  {
    m_tooltype = EditVertex;

    if (m_tool)
      delete m_tool;

    m_tool = new te::mnt::ProfileTools(m_app->getMapDisplay(), m_inputLayer);
    connect(m_tool, SIGNAL(geometriesEdited()), SLOT(onGeometriesChanged()));

    m_app->getMapDisplay()->installEventFilter(m_tool);
  }

  ((te::mnt::ProfileTools*)m_tool)->setLines(m_visadas);
}

