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
  \file terralib/memory/DataSource.cpp

  \brief Implementation of a random-access dataset class for the TerraLib In-Memory Data Access driver.
*/

// TerraLib
#include "../core/translator/Translator.h"
#include "../core/uri/URI.h"
#include "../core/uri/Utils.h"
#include "../dataaccess/dataset/DataSetType.h"
#include "../dataaccess/datasource/DataSourceCatalog.h"
#include "../dataaccess/datasource/DataSourceCapabilities.h"
#include "../dataaccess/datasource/DataSourceTransactor.h"
#include "../dataaccess/query/SQLDialect.h"
#include "../dataaccess/utils/Utils.h"
#include "../geometry/Envelope.h"
#include "DataSet.h"
#include "DataSource.h"
#include "Exception.h"

// Boost
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

// STL
#include <cassert>


te::da::DataSourceCapabilities te::mem::DataSource::sm_capabilities;

const te::da::SQLDialect te::mem::DataSource::sm_dialect;

te::mem::DataSource::DataSource(const std::string& connInfo)
  : te::da::DataSource(connInfo), 
    m_numDatasets(0),
    m_maxNumDatasets(TE_MEMORY_MAX_DATASETS),
    m_isOpened(false),
    m_deepCopy(false)
{
}

te::mem::DataSource::DataSource(const te::core::URI& uri)
  : te::da::DataSource(uri), 
  m_numDatasets(0),
  m_maxNumDatasets(TE_MEMORY_MAX_DATASETS),
  m_isOpened(false),
  m_deepCopy(false)
{
}

te::mem::DataSource::~DataSource()
{
  close();
}

const std::map<std::string, te::da::DataSetPtr>& te::mem::DataSource::getDataSets() const
{
  return m_datasets;
}

const std::map<std::string, te::da::DataSetTypePtr> te::mem::DataSource::getSchemas() const
{
  return m_schemas;
}

std::string te::mem::DataSource::getType() const
{
  return TE_MEMORY_DRIVER_IDENTIFIER;
}

std::auto_ptr<te::da::DataSourceTransactor> te::mem::DataSource::getTransactor()
{
  //return std::auto_ptr<te::da::DataSourceTransactor>(new te::mem::DataSourceTransactor(this));
  return std::auto_ptr<te::da::DataSourceTransactor>(0);
}

void te::mem::DataSource::open()
{
  // Assure we are in a closed state
  close();

  if (!m_uri.isValid())
    throw Exception(TE_TR("There is no valid information about the data source"));

  std::map<std::string, std::string> kvp = te::core::Expand(m_uri.query());
  std::map<std::string, std::string>::const_iterator it;

  // Check if it is required a different dataset limit
   it = kvp.find("MAX_DATASETS");

  if(it != kvp.end())
    m_maxNumDatasets = boost::lexical_cast<std::size_t>(it->second);

  // Check operation mode
  it = kvp.find("OPERATION_MODE");

  if((it != kvp.end()) && (boost::to_upper_copy(it->second) == "NON-SHARED"))
    m_deepCopy = true;

  m_isOpened = true;
}

void te::mem::DataSource::close()
{
 if(!m_isOpened)
    return;

  m_datasets.clear();
  m_schemas.clear();

  m_numDatasets = 0;
  m_maxNumDatasets = TE_MEMORY_MAX_DATASETS;

  m_isOpened = false;
  m_deepCopy = false;
}

bool te::mem::DataSource::isOpened() const
{
  return m_isOpened;
}

bool te::mem::DataSource::isValid() const
{
  return m_isOpened;
}

const te::da::DataSourceCapabilities& te::mem::DataSource::getCapabilities() const
{
  return sm_capabilities;
}

const te::da::SQLDialect* te::mem::DataSource::getDialect() const
{
  return &sm_dialect;
}

std::auto_ptr<te::da::DataSet> te::mem::DataSource::getDataSet(const std::string& name,
                                                               te::common::TraverseType /*travType*/,
                                                               const te::common::AccessPolicy /*accessPolicy*/ )
{
  if(!dataSetExists(name))
    throw Exception((boost::format(TE_TR("There is no dataset with this name: \"%1%\"!")) % name).str());

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  std::map<std::string, te::da::DataSetPtr>::const_iterator it = m_datasets.find(name);

  return std::auto_ptr<te::da::DataSet>(new DataSet(*(it->second), m_deepCopy));
}

std::vector<std::string> te::mem::DataSource::getDataSetNames()
{
  std::vector<std::string> datasetNames;

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  std::map<std::string, te::da::DataSetTypePtr>::const_iterator it;
  for(it = m_schemas.begin(); it != m_schemas.end(); ++it)
    datasetNames.push_back(it->first);

  return datasetNames;
}

std::size_t te::mem::DataSource::getNumberOfDataSets()
{
  return m_numDatasets;
}

std::auto_ptr<te::da::DataSetType> te::mem::DataSource::getDataSetType(const std::string& datasetName)
{
  if(!dataSetExists(datasetName))
    throw Exception((boost::format(TE_TR("There is no dataset with this name: \"%1%\"!")) % datasetName).str());

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  std::map<std::string, te::da::DataSetTypePtr>::const_iterator it = m_schemas.find(datasetName);

  te::da::DataSetType* dt = static_cast<te::da::DataSetType*>(it->second.get()->clone());

  return std::auto_ptr<te::da::DataSetType>(dt);
}

boost::ptr_vector<te::dt::Property> te::mem::DataSource::getProperties(const std::string& datasetName)
{
  boost::ptr_vector<te::dt::Property> properties;

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  std::map<std::string, te::da::DataSetTypePtr>::const_iterator it = m_schemas.find(datasetName);

  if(it == m_schemas.end())
    return properties;

  const te::da::DataSetTypePtr& dt = it->second;

  const std::size_t np = dt->size();

  for(std::size_t i = 0; i < np; ++i)
    properties.push_back(dt->getProperty(i)->clone());

  return properties;
}

std::auto_ptr<te::dt::Property> te::mem::DataSource:: getProperty(const std::string& datasetName, const std::string& name)
{
  if(!propertyExists(datasetName,name))
    throw Exception((boost::format(TE_TR("The dataset \"%1%\" has no property with this name: \"%2%\"!")) % datasetName % name).str());

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  std::auto_ptr<te::da::DataSetType> dt = getDataSetType(datasetName);

  te::dt::Property* p = dt->getProperty(name);

  return std::auto_ptr<te::dt::Property>(p->clone());
}

std::auto_ptr<te::dt::Property> te::mem::DataSource::getProperty(const std::string& datasetName, std::size_t propertyPos)
{
  if(!dataSetExists(datasetName))
    throw Exception((boost::format(TE_TR("There is no dataset with this name: \"%1%\"!")) % datasetName).str());

  assert(propertyPos < getNumberOfProperties(datasetName));

  std::auto_ptr<te::da::DataSetType> dt = getDataSetType(datasetName);

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  te::dt::Property* p = dt->getProperty(propertyPos);

  return std::auto_ptr<te::dt::Property>(p->clone());
}

std::vector<std::string> te::mem::DataSource::getPropertyNames(const std::string& datasetName)
{
  if(!dataSetExists(datasetName))
    throw Exception((boost::format(TE_TR("There is no dataset with this name: \"%1%\"!")) % datasetName).str());

  std::vector<std::string> pNames;

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  std::map<std::string, te::da::DataSetTypePtr>::const_iterator it = m_schemas.find(datasetName);

  const te::da::DataSetTypePtr& dt = it->second;

  std::vector<te::dt::Property*>& properties = dt->getProperties();
  for(std::size_t i = 0; i < properties.size(); ++i)
    pNames.push_back(properties[i]->getName());

  return pNames;
}

std::size_t te::mem::DataSource::getNumberOfProperties(const std::string& datasetName)
{
  return getPropertyNames(datasetName).size();
}

bool te::mem::DataSource::propertyExists(const std::string& datasetName, const std::string& name)
{
  std::vector<std::string> pNames = getPropertyNames(datasetName);

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  if(std::find(pNames.begin(), pNames.end(), name) != pNames.end())
    return true;

  return false;
}

void te::mem::DataSource::addProperty(const std::string& datasetName, te::dt::Property* p)
{
  std::string pName = p->getName();

  if(propertyExists(datasetName, p->getName()))
    throw Exception((boost::format(TE_TR("The dataset already \"%1%\" has a property with this name \"%2%\"!")) % datasetName % pName).str());

  std::auto_ptr<te::da::DataSetType> dt = getDataSetType(datasetName);

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  dt->add(p);

  // Adjust the dataset properties
  std::auto_ptr<te::da::DataSet> dataset = getDataSet(datasetName);
  te::mem::DataSet* datasetp = static_cast<te::mem::DataSet*>(dataset.release());

  datasetp->add(pName, p->getType());
}

void te::mem::DataSource::dropProperty(const std::string& datasetName, const std::string& propertyName)
{
  if(!propertyExists(datasetName, propertyName))
    throw Exception((boost::format(TE_TR("The dataset \"%1%\" has no property with this name \"%2%\"!")) % datasetName % propertyName).str());

  std::auto_ptr<te::da::DataSet> dataset = getDataSet(datasetName);

  std::auto_ptr<te::da::DataSetType> dt = getDataSetType(datasetName);

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  te::mem::DataSet* datasetp = static_cast<te::mem::DataSet*>(dataset.release());
  std::size_t pos = te::da::GetPropertyPos(datasetp, propertyName);

  datasetp->drop(pos);

  te::dt::Property* p = dt->getProperty(propertyName);

  dt->remove(p);
}

void te::mem::DataSource::renameProperty(const std::string& datasetName,
                                         const std::string& name,
                                         const std::string& newName)
{
  if(!propertyExists(datasetName, name))
    throw Exception((boost::format(TE_TR("The dataset \"%1%\" has no property with this name \"%2%\"!")) % datasetName % name).str());

  if(!isPropertyNameValid(newName))
    throw Exception((boost::format(TE_TR("The new property name \"%1%\" is not valid!")) % newName).str());

  std::auto_ptr<te::da::DataSetType> dt = getDataSetType(datasetName);

  // Update the property name in the dataset
  std::auto_ptr<te::da::DataSet> dataset = getDataSet(datasetName);

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  te::mem::DataSet* datasetp = static_cast<te::mem::DataSet*>(dataset.release());

  std::size_t pos = te::da::GetPropertyPos(datasetp, name);
  datasetp->setPropertyName(newName, pos);

  // Update the property name in the schema
  dt->getProperty(pos)->setName(newName);
}

std::size_t te::mem::DataSource::getNumberOfItems(const std::string& datasetName)
{
  if(!dataSetExists(datasetName))
    throw Exception((boost::format(TE_TR("There is no dataset with this name: \"%1%\"!")) % datasetName).str());

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  te::da::DataSetPtr& dataset = m_datasets.find(datasetName)->second;

  return dataset.get()->size();
}

bool te::mem::DataSource::hasDataSets()
{
  return (m_numDatasets == 0) ? false : true;
}

bool te::mem::DataSource::dataSetExists(const std::string& name)
{
  std::vector<std::string> datasetNames = getDataSetNames();

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  if(std::find(datasetNames.begin(), datasetNames.end(), name) != datasetNames.end())
     return true;

  return false;
}

void te::mem::DataSource::createDataSet(te::da::DataSetType* dt,
                                        const std::map<std::string, std::string>& /*options*/)
{
  std::string datasetName = dt->getName();

  if(m_schemas.find(datasetName) != m_schemas.end())
    throw Exception((boost::format(TE_TR("A dataset with the same name (%1%) already exists!")) % datasetName).str());

  if((m_numDatasets + 1) > m_maxNumDatasets)
    throw Exception((boost::format(TE_TR("The maximum number of datasets was exceeded!"))).str());

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  te::da::DataSetTypePtr dtp(dt);
  m_schemas[datasetName] = dtp;

  DataSetPtr dataset(new DataSet(dt));

  m_datasets[datasetName] = dataset;

  ++m_numDatasets;
}

void te::mem::DataSource::cloneDataSet(const std::string& name,
                                       const std::string& cloneName,
                                       const std::map<std::string, std::string>& /*options*/)
{
  if(!dataSetExists(name))
    throw Exception((boost::format(TE_TR("There is no dataset with this name: \"%1%\"!")) % name).str());

  if(!isDataSetNameValid(cloneName))
    throw Exception((boost::format(TE_TR("The dataset clone name \"%1%\" is not valid!")) % cloneName).str());

  if(!dataSetExists(cloneName))
    throw Exception((boost::format(TE_TR("There is already a dataset with this name: \"%1%\"!")) % cloneName).str());

  // Clone the schema
  const te::da::DataSetTypePtr& dtp = m_schemas[name];

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);
  
  te::da::DataSetType* clonedt = static_cast<te::da::DataSetType*>(dtp->clone());
  te::da::DataSetTypePtr clonedtp(clonedt);
  m_schemas[cloneName] = clonedtp;

  // Clone the dataset
  te::da::DataSetPtr datasetClone(getDataSet(name).release());

  m_datasets[cloneName] = datasetClone;

  ++m_numDatasets;
}

void te::mem::DataSource::dropDataSet(const std::string& name)
{
  if(!dataSetExists(name))
    throw Exception((boost::format(TE_TR("There is no dataset with this name: \"%1%\"!")) % name).str());

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  m_schemas.erase(m_schemas.find(name));
  m_datasets.erase(m_datasets.find(name));

  --m_numDatasets;
}

void te::mem::DataSource::renameDataSet(const std::string& name, const std::string& newName)
{
  if(!dataSetExists(name))
    throw Exception((boost::format(TE_TR("There is no dataset with this name: \"%1%\"!")) % name).str());

  if(!isDataSetNameValid(newName))
    throw Exception((boost::format(TE_TR("The new name \"%1%\" is not valid!")) % newName).str());

  std::auto_ptr<te::da::DataSetType> dt = getDataSetType(name);

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  dt->setName(newName);
  dt->setCompositeName(newName);

  m_schemas.erase(m_schemas.find(name));
  m_schemas[newName] = dt;

  te::da::DataSetPtr& dataset = m_datasets[name];
  m_datasets.erase(m_datasets.find(name));
  m_datasets[newName] = dataset;
}

void te::mem::DataSource::add(const std::string& datasetName,
                              te::da::DataSet* d,
                              const std::map<std::string, std::string>& /*options*/,
                              std::size_t limit)
{
  if(!dataSetExists(datasetName))
    throw Exception((boost::format(TE_TR("There is no dataset with this name: \"%1%\"!")) % datasetName).str());

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  te::mem::DataSet* datasetp = static_cast<te::mem::DataSet*>(m_datasets[datasetName].get());
  datasetp->copy(*d, limit);
}

void te::mem::DataSource::remove(const std::string& datasetName, const te::da::ObjectIdSet* oids)
{
  if(!dataSetExists(datasetName))
    throw Exception((boost::format(TE_TR("There is no dataset with this name: \"%1%\"!")) % datasetName).str());

  boost::lock_guard<boost::recursive_mutex> lock(m_mtx);

  std::auto_ptr<te::da::DataSet> datasetp = te::da::DataSource::getDataSet(datasetName, oids);

  te::mem::DataSet* dataset = static_cast<te::mem::DataSet*>(datasetp.get());

  while(dataset->moveNext())
    dataset->remove();
}

void te::mem::DataSource::update(const std::string& /*datasetName*/,
                                 te::da::DataSet* /*dataset*/,
                                 const std::vector<std::size_t>& /*properties*/,
                                 const te::da::ObjectIdSet* /*oids*/,
                                 const std::map<std::string, std::string>& /*options*/,
                                 std::size_t /*limit*/)
{
}

void te::mem::DataSource::setCapabilities(const te::da::DataSourceCapabilities& capabilities)
{
  sm_capabilities = capabilities;
}


void te::mem::DataSource::create(const std::string& /*connInfo*/)
{
}

void te::mem::DataSource::drop(const std::string& /*connInfo*/)
{
}

bool te::mem::DataSource::exists(const std::string& /*connInfo*/)
{
  return false;
}

std::vector<std::string> te::mem::DataSource::getDataSourceNames(const std::string& /*connInfo*/)
{
  return std::vector<std::string>();
}
