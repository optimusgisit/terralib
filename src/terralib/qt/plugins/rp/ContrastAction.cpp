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
  \file terralib/qt/plugins/rp/ContrastAction.cpp

  \brief This file defines the Contrast class
*/

// Terralib
#include "../../af/events/LayerEvents.h"
#include "../../af/ApplicationController.h"
#include "../../af/BaseApplication.h"
#include "ContrastAction.h"

// Qt
#include <QObject>
#include <QMessageBox>
#include <QApplication>

// STL
#include <memory>

te::qt::plugins::rp::ContrastAction::ContrastAction(QMenu* menu, QMenu* popupMenu):te::qt::plugins::rp::AbstractAction(menu, popupMenu)
{
  m_contrastDlg = 0;

  createAction(tr("Contrast...").toUtf8().data(), "histogram");

  m_action->setObjectName("Processing.Raster Processing.Contrast");
}

te::qt::plugins::rp::ContrastAction::~ContrastAction()
{
  if (m_contrastDlg)
  {
    delete m_contrastDlg;
    m_contrastDlg = 0;
  }
}

void te::qt::plugins::rp::ContrastAction::onActionActivated(bool checked)
{
	te::map::AbstractLayerPtr layer = getCurrentLayer();

	if (layer.get())
	{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (layer->getVisibility() != te::map::VISIBLE)
    {
      QApplication::restoreOverrideCursor();
      QMessageBox::warning(te::qt::af::AppCtrlSingleton::getInstance().getMainWindow(), tr("Warning"), tr("The layer is not checked!"));
      return;
    }

    std::auto_ptr<te::da::DataSetType> dsType = layer->getSchema();
    //Checking if the layer contains a raster property
    if (!dsType->hasRaster())
    {
      QApplication::restoreOverrideCursor();
      QMessageBox::warning(te::qt::af::AppCtrlSingleton::getInstance().getMainWindow(), tr("Warning"), tr("There is no selected raster layer."));
      return;
    }

    // Register the application framework listener
    te::qt::af::AppCtrlSingleton::getInstance().addListener(this);

    if (m_contrastDlg)
    {
      delete m_contrastDlg;
      m_contrastDlg = 0;
    }

		m_contrastDlg = new te::qt::widgets::ContrastDialogForm(te::qt::af::AppCtrlSingleton::getInstance().getMainWindow());

    connect(m_contrastDlg, SIGNAL(addLayer(te::map::AbstractLayerPtr)), this, SLOT(addLayer(te::map::AbstractLayerPtr)));
    connect(m_contrastDlg, SIGNAL(closeTool()), this, SLOT(closeTool()));

    te::qt::af::BaseApplication* ba = dynamic_cast<te::qt::af::BaseApplication*>(te::qt::af::AppCtrlSingleton::getInstance().getMainWindow());

    m_contrastDlg->setModal(false);
    m_contrastDlg->setMapDisplay(ba->getMapDisplay());
    m_contrastDlg->set(layer);
    m_contrastDlg->setConfigurations();

    QApplication::restoreOverrideCursor();

    m_contrastDlg->show();
    m_contrastDlg->exec();
	}
  else
    QMessageBox::warning(te::qt::af::AppCtrlSingleton::getInstance().getMainWindow(), tr("Warning"), tr("There is no selected layer."));
}

void te::qt::plugins::rp::ContrastAction::onPopUpActionActivated(bool checked)
{
  te::map::AbstractLayerPtr layer = getCurrentLayer();

  if(layer.get())
  {
    te::qt::widgets::ContrastDialogForm dlg(te::qt::af::AppCtrlSingleton::getInstance().getMainWindow());

    dlg.set(layer);

    dlg.exec();
  }
  else
  {
    QMessageBox::warning(te::qt::af::AppCtrlSingleton::getInstance().getMainWindow(), tr("Warning"), tr("The layer selected is invalid or does not have an raster representation."));
  }
}

void te::qt::plugins::rp::ContrastAction::onApplicationTriggered(te::qt::af::evt::Event* e)
{
  switch (e->m_id)
  {
    case te::qt::af::evt::LAYER_SELECTED:
    {
      te::qt::af::evt::LayerSelected* evt = static_cast<te::qt::af::evt::LayerSelected*>(e);

      te::map::AbstractLayerPtr layer = evt->m_layer;
      assert(layer);

      if (layer == 0)
      {
        return;
      }

      if (layer->getVisibility() != te::map::VISIBLE)
      {
        QMessageBox::warning(te::qt::af::AppCtrlSingleton::getInstance().getMainWindow(), tr("Warning"), tr("The layer is not checked!"));
        return ;
      }

      if (m_contrastDlg)
      {
        m_contrastDlg->resetWindow(layer);
      }
    }
    break;

    default:
      break;
  }
}

void te::qt::plugins::rp::ContrastAction::addLayer(te::map::AbstractLayerPtr outputLayer)
{
  //add new layer
  addNewLayer(outputLayer);
}

void te::qt::plugins::rp::ContrastAction::closeTool()
{
  if (m_contrastDlg)
  {
    delete m_contrastDlg;
    m_contrastDlg = 0;
  }
}
