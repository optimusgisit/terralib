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
  \file terralib/dataaccess/utils/Utils.cpp

  \brief Utility functions for the data access module.
*/

// TerraLib
#include "../../geometry/Envelope.h"
#include "../../geometry/GeometryProperty.h"
#include "../dataset/DataSetType.h"
#include "../datasource/DataSource.h"
#include "../datasource/DataSourceManager.h"
#include "../datasource/DataSourceCatalogLoader.h"
#include "../datasource/DataSourceTransactor.h"
#include "Utils.h"

// STL
#include <cassert>
#include <memory>

void te::da::LoadFull(te::da::DataSetType* dataset, const std::string& datasourceId)
{
  assert(dataset);
  assert(!datasourceId.empty());

  DataSourcePtr datasource(te::da::DataSourceManager::getInstance().find(datasourceId));

  if(datasource.get() == 0)
    return;

  LoadFull(dataset, datasource.get());
}

void te::da::LoadFull(DataSetType* dataset, DataSource* datasource)
{
  assert(dataset);
  assert(datasource);

  std::auto_ptr<DataSourceTransactor> transactor(datasource->getTransactor());

  LoadFull(dataset, transactor.get());
}

void te::da::LoadFull(DataSetType* dataset, DataSourceTransactor* transactor)
{
  assert(dataset);
  assert(transactor);

  std::auto_ptr<DataSourceCatalogLoader> cloader(transactor->getCatalogLoader());

  LoadFull(dataset, cloader.get());
}

void te::da::LoadFull(DataSetType* dataset, DataSourceCatalogLoader* cloader)
{
  assert(dataset);
  assert(cloader);

  if(dataset->size() == 0)
    cloader->getProperties(dataset);

  cloader->getCheckConstraints(dataset);

  cloader->getIndexes(dataset);

  cloader->getUniqueKeys(dataset);

  cloader->getPrimaryKey(dataset);

  dataset->setFullLoaded(true);
}

void te::da::LoadProperties(te::da::DataSetType* dataset, const std::string& datasourceId)
{
  assert(dataset);
  assert(!datasourceId.empty());

  DataSourcePtr datasource(te::da::DataSourceManager::getInstance().find(datasourceId));

  if(datasource.get() == 0)
    return;

  LoadProperties(dataset, datasource.get());
}

void te::da::LoadProperties(te::da::DataSetType* dataset, te::da::DataSource* datasource)
{
  assert(dataset);
  assert(datasource);

  std::auto_ptr<DataSourceTransactor> transactor(datasource->getTransactor());

  LoadProperties(dataset, transactor.get());
}

void te::da::LoadProperties(te::da::DataSetType* dataset, te::da::DataSourceTransactor* transactor)
{
  assert(dataset);
  assert(transactor);

  std::auto_ptr<DataSourceCatalogLoader> cloader(transactor->getCatalogLoader());

  cloader->getProperties(dataset);
}

void te::da::LoadExtent(te::gm::GeometryProperty* gp, const std::string& datasourceId, bool refresh)
{
  assert(gp);
  assert(!datasourceId.empty());

  DataSourcePtr datasource(te::da::DataSourceManager::getInstance().find(datasourceId));

  if(datasource.get() == 0)
    return;

  LoadExtent(gp, datasource.get(), refresh);
}

void te::da::LoadExtent(te::gm::GeometryProperty* gp, te::da::DataSource* datasource, bool refresh)
{
  assert(gp);
  assert(datasource);

  std::auto_ptr<DataSourceTransactor> transactor(datasource->getTransactor());

  LoadExtent(gp, transactor.get(), refresh);
}

void te::da::LoadExtent(te::gm::GeometryProperty* gp, te::da::DataSourceTransactor* transactor, bool refresh)
{
  assert(gp);
  assert(transactor);

  if(gp->getExtent() && !refresh)
    return;

  std::auto_ptr<DataSourceCatalogLoader> cloader(transactor->getCatalogLoader());

  te::gm::Envelope* mbr = cloader->getExtent(gp);

  gp->setExtent(mbr);
}

void te::da::GetDataSets(boost::ptr_vector<std::string>& datasets, const std::string& datasourceId)
{
  assert(!datasourceId.empty());

  DataSourcePtr datasource(te::da::DataSourceManager::getInstance().find(datasourceId));

  if(datasource.get() == 0)
    return;

  GetDataSets(datasets, datasource.get());
}

void te::da::GetDataSets(boost::ptr_vector<std::string>& datasets, te::da::DataSource* datasource)
{
  assert(datasource);

  std::auto_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

  GetDataSets(datasets, transactor.get());
}

void te::da::GetDataSets(boost::ptr_vector<std::string>& datasets, te::da::DataSourceTransactor* transactor)
{
  assert(transactor);

  std::auto_ptr<te::da::DataSourceCatalogLoader> cloader(transactor->getCatalogLoader());

  cloader->getDataSets(datasets);
}

bool te::da::HasDataSet(const std::string& datasourceId)
{
  assert(!datasourceId.empty());

  DataSourcePtr datasource(te::da::DataSourceManager::getInstance().find(datasourceId));

  if(datasource.get() == 0)
    return false;

  return HasDataSet(datasource.get());
}

bool te::da::HasDataSet(te::da::DataSource* datasource)
{
  assert(datasource);

  std::auto_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

  return HasDataSet(transactor.get());
}

bool te::da::HasDataSet(te::da::DataSourceTransactor* transactor)
{
  assert(transactor);

  std::auto_ptr<te::da::DataSourceCatalogLoader> cloader(transactor->getCatalogLoader());

  return cloader->hasDataSets();
}

te::da::DataSet* te::da::GetDataSet(const std::string& name, const std::string& datasourceId)
{
  assert(!datasourceId.empty());

  DataSourcePtr datasource(te::da::DataSourceManager::getInstance().find(datasourceId));

  if(datasource.get() == 0)
    return false;

  return GetDataSet(name, datasource.get());
}

te::da::DataSet* te::da::GetDataSet(const std::string& name, te::da::DataSource* datasource)
{
  assert(datasource);

  std::auto_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

  return transactor->getDataSet(name);
}