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
  \file terralib/dataaccess/datasource/DataSourceInfoManager.cpp

  \brief A singleton to keep all the registered data sources.
*/

// TerraLib
#include "../../common/STLUtils.h"
#include "../../core/uri/URI.h"
#include "../../core/translator/Translator.h"
#include "../Exception.h"
#include "DataSourceInfo.h"
#include "DataSourceInfoManager.h"

// STL
#include <cassert>

// Boost
#include <boost/foreach.hpp>
#include <boost/format.hpp>

te::da::DataSourceInfoPtr te::da::DataSourceInfoManager::get(const std::string& id) const
{
  std::map<std::string, DataSourceInfoPtr>::const_iterator it = m_datasources.find(id);

  return (it != m_datasources.end()) ? it->second : DataSourceInfoPtr();
}

bool te::da::DataSourceInfoManager::add(const DataSourceInfoPtr& ds)
{
  if (ds.get() == 0)
    throw Exception(TE_TR("Can not insert a NULL data source into the manager!"));

  if (m_datasources.find(ds->getId()) == m_datasources.end())
  {
    std::string conInfo = ds->getConnInfoAsString();

    std::map<std::string, DataSourceInfoPtr>::const_iterator dsInfoIt = m_datasources.begin();
    while (dsInfoIt != m_datasources.end())
    {
      std::string dsConInfo = dsInfoIt->second->getConnInfoAsString();

      if (conInfo == dsConInfo)
      {
        ds->setId(dsInfoIt->second->getId());
        return false;
      }
      ++dsInfoIt;
    }
    m_datasources[ds->getId()] = ds;
  }
  return true;
}

void te::da::DataSourceInfoManager::remove(const std::string& id)
{
  std::map<std::string, DataSourceInfoPtr>::iterator it = m_datasources.find(id);

  if(it == m_datasources.end())
    throw Exception((boost::format(TE_TR("There isn't a data source with the given id (%1%) in data source manager!")) % id).str());

  m_datasources.erase(it);
}

void te::da::DataSourceInfoManager::removeByType(const std::string& dsTypeName)
{
  std::map<std::string, DataSourceInfoPtr>::iterator it = m_datasources.begin();

  while(it!=m_datasources.end())
    if(it->second->getType() == dsTypeName)
      m_datasources.erase(it++);
    else
      ++it;
}


void te::da::DataSourceInfoManager::getByType(const std::string& dsTypeName, std::vector<DataSourceInfoPtr>& datasources) const
{
  std::map<std::string, DataSourceInfoPtr>::const_iterator it = m_datasources.begin();
  std::map<std::string, DataSourceInfoPtr>::const_iterator itend = m_datasources.end();

  while(it != itend)
  {
    if(it->second->getType() == dsTypeName)
      datasources.push_back(it->second);

    ++it;
  }
}

te::da::DataSourceInfoPtr te::da::DataSourceInfoManager::getByConnInfo(const std::string& connInfo)
{
  std::map<std::string, DataSourceInfoPtr>::const_iterator dsInfoIt = m_datasources.begin();
  while (dsInfoIt != m_datasources.end())
  {
    std::string dsConInfo = dsInfoIt->second->getConnInfoAsString();

    if (connInfo == dsConInfo)
      return dsInfoIt->second;

    ++dsInfoIt;
  }
  return  DataSourceInfoPtr();
}

std::size_t te::da::DataSourceInfoManager::size() const
{
  return m_datasources.size();
}

te::da::DataSourceInfoManager::const_iterator te::da::DataSourceInfoManager::begin() const
{
  return m_datasources.begin();
}

te::da::DataSourceInfoManager::iterator te::da::DataSourceInfoManager::begin()
{
  return m_datasources.begin();
}

te::da::DataSourceInfoManager::const_iterator te::da::DataSourceInfoManager::end() const
{
  return m_datasources.end();
}

te::da::DataSourceInfoManager::iterator te::da::DataSourceInfoManager::end()
{
  return m_datasources.end();
}

te::da::DataSourceInfoManager::DataSourceInfoManager()
{
}

te::da::DataSourceInfoManager::~DataSourceInfoManager()
{
}

