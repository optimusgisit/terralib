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
  \file terralib/wms/qt/WMSLayerSelector.cpp

  \brief A simple widget that allows the selection of datasets from a given WMS server.
*/

// TerraLib
#include "../../core/translator/Translator.h"
#include "../../dataaccess/datasource/DataSource.h"
#include "../../dataaccess/datasource/DataSourceManager.h"
#include "../../maptools/DataSetLayer.h"
#include "../../qt/widgets/dataset/selector/DataSetSelectorDialog.h"
#include "../../qt/widgets/layer/utils/DataSet2Layer.h"
#include "../Exception.h"
#include "../WMS2Layer.h"
#include "WMSLayerSelector.h"

// STL
#include <algorithm>
#include <iterator>
#include <memory>

te::wms::WMSLayerSelector::WMSLayerSelector(QWidget* parent, Qt::WindowFlags f)
  : te::qt::widgets::AbstractLayerSelector(parent, f)
{
}

te::wms::WMSLayerSelector::~WMSLayerSelector()
{
}

void te::wms::WMSLayerSelector::set(const std::list<te::da::DataSourceInfoPtr>& datasources)
{
  m_datasources = datasources;
}

std::list<te::map::AbstractLayerPtr> te::wms::WMSLayerSelector::getLayers()
{
  std::list<te::map::AbstractLayerPtr> layers;

  for(std::list<te::da::DataSourceInfoPtr>::iterator it = m_datasources.begin(); it != m_datasources.end(); ++it)
  {
    te::da::DataSourcePtr datasource = te::da::DataSourceManager::getInstance().find((*it)->getId());

    if(datasource.get() == 0)
    {
      datasource = te::da::DataSourceManager::getInstance().get((*it)->getId(), (*it)->getAccessDriver(), (*it)->getConnInfo());

      if(datasource.get() == 0)
        throw Exception(TE_TR("Could not retrieve the data source instance!"));
    }

    if(!datasource->isOpened())
      datasource->open();

    std::auto_ptr<te::qt::widgets::DataSetSelectorDialog> ldialog(new te::qt::widgets::DataSetSelectorDialog(static_cast<QWidget*>(parent())));

    ldialog->set(*it, true);

    int retval = ldialog->exec();

    if(retval == QDialog::Rejected)
      continue;

    std::list<te::da::DataSetTypePtr> datasets = ldialog->getCheckedDataSets();

    std::transform(datasets.begin(), datasets.end(), std::back_inserter(layers), WMS2Layer((*it)->getId()));
  }

  return layers;
}
