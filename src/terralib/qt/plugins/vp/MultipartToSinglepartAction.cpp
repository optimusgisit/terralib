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
  \file terralib/qt/plugins/vp/MultipartToSinglepartAction.cpp

  \brief This file defines the Multipart to Singlepart class
*/

// Terralib
#include "../../../vp/qt/MultipartToSinglepartDialog.h"
#include "../../af/ApplicationController.h"
#include "../../af/events/LayerEvents.h"
#include "MultipartToSinglepartAction.h"

// Qt
#include <QObject>
#include <QMessageBox>

// STL
#include <memory>

te::qt::plugins::vp::MultipartToSinglepartAction::MultipartToSinglepartAction(QMenu* menu)
  : te::qt::plugins::vp::AbstractAction(menu)
{
  createAction(tr("Multipart To Singlepart...").toUtf8().data(), "vp-multiparttosinglepart-icon");
  m_action->setObjectName("Processing.Vector Processing.Multipart to Singlepart");
}

te::qt::plugins::vp::MultipartToSinglepartAction::~MultipartToSinglepartAction()
{
}

void te::qt::plugins::vp::MultipartToSinglepartAction::onActionActivated(bool)
{
  QWidget* parent = te::qt::af::AppCtrlSingleton::getInstance().getMainWindow();
  te::vp::MultipartToSinglepartDialog dlg(parent);

  dlg.setLayers(getLayers());

  if(dlg.exec() != QDialog::Accepted)
    return;

  te::map::AbstractLayerPtr layer = dlg.getLayer();

  if(!layer)
    return;

  int reply = QMessageBox::question(0, tr("Result"), tr("The operation was concluded successfully. Would you like to add the layer to the project?"), QMessageBox::No, QMessageBox::Yes);

  if(reply == QMessageBox::Yes)
    addNewLayer(layer);
}
