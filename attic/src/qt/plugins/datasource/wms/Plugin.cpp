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
  \file terralib/qt/plugins/datasource/wms/Plugin.cpp

  \brief Plugin implementation for the OGC Web Map Service (WMS) data source widget.
*/
#include "Plugin.h"

// TerraLib
#include "../../../../common/Config.h"
#include "../../../../core/translator/Translator.h"
#include "../../../../core/logger/Logger.h"
#include "../../../../dataaccess/datasource/DataSourceInfoManager.h"
#include "../../../af/ApplicationController.h"
#include "../../../af/events/ApplicationEvents.h"
#include "../../../af/events/LayerEvents.h"
#include "../../../widgets/Utils.h"
#include "../../../widgets/datasource/core/DataSourceTypeManager.h"
#include "../../../widgets/layer/explorer/LayerItem.h"
#include "../../../widgets/layer/explorer/LayerItemView.h"
#include "WMSItemDelegate.h"
#include "WMSType.h"

// Boost
#include <boost/functional/factory.hpp>
#include <boost/bind.hpp>


void GetAllWMSLayers(te::qt::widgets::LayerItemView* view, const QModelIndex& parent,
                        QModelIndexList& layersIdx)
{
  QAbstractItemModel* model = view->model();
  int cS = model->rowCount(parent);

  for(int i = 0; i < cS; i++)
  {
    QModelIndex idx = model->index(i, 0, parent);

    te::qt::widgets::TreeItem* child =
        static_cast<te::qt::widgets::TreeItem*>(idx.internalPointer());

    if(child->getType() == "FOLDER")
      GetAllWMSLayers(view, idx, layersIdx);
    else if(child->getType() == "LAYER")
    {
      if(((te::qt::widgets::LayerItem*)child)->getLayer()->getType() == "WMSLAYER")
        layersIdx.push_back(idx);
    }
  }
}

std::list<te::map::AbstractLayerPtr> GetLayers(const QModelIndexList& lst)
{
  std::list<te::map::AbstractLayerPtr> res;

  for(QModelIndexList::const_iterator it = lst.begin(); it != lst.end(); ++it)
  {
    QModelIndex idx = *it;
    te::qt::widgets::TreeItem* item = static_cast<te::qt::widgets::TreeItem*>(idx.internalPointer());
    res.push_back(((te::qt::widgets::LayerItem*)item)->getLayer());
  }

  return res;
}

te::qt::plugins::wms::Plugin::Plugin(const te::core::PluginInfo& pluginInfo)
  : QObject(),
    te::core::CppPlugin(pluginInfo),
    m_delegate(0)
{
}

te::qt::plugins::wms::Plugin::~Plugin() 
{
}

void te::qt::plugins::wms::Plugin::startup()
{
  if(m_initialized)
    return;

  te::qt::widgets::DataSourceTypeManager::getInstance().add(new WMSType);

  TE_LOG_TRACE(TE_TR("TerraLib Qt OGC Web Map Service (WMS) widget startup!"));

  m_initialized = true;

  te::qt::af::AppCtrlSingleton::getInstance().addListener(this, te::qt::af::SENDER);

  updateDelegate(true);
}

void te::qt::plugins::wms::Plugin::shutdown()
{
  if(!m_initialized)
    return;

  te::da::DataSourceInfoManager::getInstance().removeByType("WMS");
  te::qt::widgets::DataSourceTypeManager::getInstance().remove("WMS");

  TE_LOG_TRACE(TE_TR("TerraLib Qt OGC Web Map Service (WMS) widget shutdown!"));

  m_initialized = false;

  updateDelegate(false);

  QModelIndexList wls;

  te::qt::af::evt::GetLayerExplorer e;

  emit triggered(&e);

  if(e.m_layerExplorer == 0)
    return;

  GetAllWMSLayers(e.m_layerExplorer, QModelIndex(), wls);

  if(!wls.isEmpty())
  {
    std::list<te::map::AbstractLayerPtr> lst = GetLayers(wls);

    e.m_layerExplorer->removeItems(wls);

    te::qt::af::evt::LayerRemoved evt(lst);

    emit triggered(&evt);
  }

  te::qt::af::AppCtrlSingleton::getInstance().removeListener(this);
}

void te::qt::plugins::wms::Plugin::updateDelegate(const bool& add)
{
  te::qt::af::evt::GetLayerExplorer e;

  emit triggered(&e);

  te::qt::widgets::LayerItemView* view = e.m_layerExplorer;

  if(view == 0)
    return;

  if(add)
  {
    m_delegate = new WMSItemDelegate((QStyledItemDelegate*)view->itemDelegate(), this);
    view->setItemDelegate(m_delegate);
  }
  else
  {
    view->removeDelegate(m_delegate);
    delete m_delegate;
    m_delegate = 0;
  }
}


TERRALIB_PLUGIN_CALL_BACK_IMPL(te::qt::plugins::wms::Plugin)
