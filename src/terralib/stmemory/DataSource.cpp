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
  \file terralib/stmemory/DataSource.cpp

  \brief Implements a DataSource that contains st memory DataSets indexed by space and time. 
*/

// TerraLib
#include "../common/STLUtils.h"
#include "../common/StringUtils.h"
#include "../core/translator/Translator.h"
#include "../core/uri/URI.h"
#include "../core/uri/Utils.h"
#include "../dataaccess/dataset/DataSet.h"
#include "../dataaccess/dataset/DataSetType.h"
#include "../dataaccess/dataset/CheckConstraint.h"
#include "../dataaccess/dataset/ForeignKey.h"
#include "../dataaccess/dataset/Index.h"
#include "../dataaccess/dataset/PrimaryKey.h"
#include "../dataaccess/dataset/Sequence.h"
#include "../dataaccess/dataset/UniqueKey.h"
#include "../datatype/DateTimePeriod.h"

#include "DataSet.h"
#include "DataSource.h"
#include "Transactor.h"
#include "Exception.h"
#include "Globals.h"

// Boost
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

te::da::DataSourceCapabilities te::stmem::DataSource::sm_capabilities;
const te::da::SQLDialect te::stmem::DataSource::sm_dialect;

te::stmem::DataSource::DataSource(const std::string& connInfo)
  : te::da::DataSource(connInfo), 
    m_datasets(),
    m_schemas(),
    m_maxdatasets(TE_STMEMORY_DRIVER_MAX_DATASETS),
    m_isOpened(false),
    m_deepCopy(false),
    m_transactor(0)
{
}

te::stmem::DataSource::DataSource(const te::core::URI& uri)
  : te::da::DataSource(uri),
  m_datasets(),
  m_schemas(),
  m_maxdatasets(TE_STMEMORY_DRIVER_MAX_DATASETS),
  m_isOpened(false),
  m_deepCopy(false),
  m_transactor(0)
{
}


te::stmem::DataSource::~DataSource()
{
  //delete the pointers
  std::map<std::string, DataSet* >::iterator it = m_datasets.begin();    
  while(it!=m_datasets.end())
  {
    delete(it->second);
    ++it;
  }
  m_datasets.clear();

  //delete the pointers
  std::map<std::string, te::da::DataSetType* >::iterator it2 = m_schemas.begin();    
  while(it2!=m_schemas.end())
  {
    delete(it2->second);
    ++it2;
  }
  m_schemas.clear();
}

std::string te::stmem::DataSource::getType() const 
{
  return Globals::sm_driverIdentifier;
}

std::auto_ptr<te::da::DataSourceTransactor> te::stmem::DataSource::getTransactor()
{
  return std::auto_ptr<te::da::DataSourceTransactor>(new Transactor(this));
}

void te::stmem::DataSource::open() 
{
// assure we are in a closed state
  close();

  m_transactor.reset(new Transactor(this));

  std::map<std::string, std::string> kvp = te::core::Expand(m_uri.query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  std::map<std::string, std::string>::const_iterator itend = kvp.end();

// check if it is required a different dataset limit
  it = kvp.find("MAX_DATASETS");
  if(it != itend && !it->second.empty())
    m_maxdatasets = boost::lexical_cast<std::size_t>(it->second);

// check operation mode
  it = kvp.find("OPERATION_MODE");
  if((it != itend && !it->second.empty()) && (te::common::Convert2UCase(it->second) == "NON-SHARED"))
    m_deepCopy = true;
 
  m_isOpened = true;
}

void te::stmem::DataSource::close() 
{
  if(!m_isOpened)
    return;

  //delete the pointers
  std::map<std::string, DataSet* >::iterator it = m_datasets.begin();    
  while(it!=m_datasets.end())
  {
    delete(it->second);
    ++it;
  }
  m_datasets.clear();

  //delete the pointers
  std::map<std::string, te::da::DataSetType* >::iterator it2 = m_schemas.begin();    
  while(it2!=m_schemas.end())
  {
    delete(it2->second);
    ++it2;
  }
  m_schemas.clear();

  m_maxdatasets = TE_STMEMORY_DRIVER_MAX_DATASETS;

  m_isOpened = false;

  m_deepCopy = false;

  m_transactor.reset(0);
}

bool te::stmem::DataSource::isOpened() const 
{
  return m_isOpened;
}

bool te::stmem::DataSource::isValid() const 
{
  return m_isOpened;
}

const te::da::DataSourceCapabilities& te::stmem::DataSource::getCapabilities() const 
{
  return sm_capabilities;
}

const te::da::SQLDialect* te::stmem::DataSource::getDialect() const 
{
  return &sm_dialect;
}

void te::stmem::DataSource::add(const std::string& name, te::da::DataSetType* t, DataSet* d)
{
  m_transactor->add(name, t, d);
}

std::auto_ptr<te::da::DataSet> te::stmem::DataSource::getDataSet(const std::string& name, const te::dt::DateTime* dt, te::dt::TemporalRelation r,
                                          te::common::TraverseType travType, bool connected,
                                          const te::common::AccessPolicy accessPolicy)
{
  return m_transactor->getDataSet(name, dt, r, travType, connected, accessPolicy);
}

std::auto_ptr<te::da::DataSet> te::stmem::DataSource::getDataSet(const std::string& name, 
                                                  const te::gm::Geometry* geom, te::gm::SpatialRelation sr,
                                                  const te::dt::DateTime* dt, te::dt::TemporalRelation tr,
                                                  te::common::TraverseType travType, bool connected,
                                                  const te::common::AccessPolicy accessPolicy)
{
  return m_transactor->getDataSet(name, geom, sr, dt, tr, travType, connected, accessPolicy);
}

std::auto_ptr<te::da::DataSet> te::stmem::DataSource::getDataSet(const std::string& name, 
                                                  const te::gm::Envelope* e, te::gm::SpatialRelation sr,
                                                  const te::dt::DateTime* dt, te::dt::TemporalRelation tr,
                                                  te::common::TraverseType travType, bool connected,
                                                  const te::common::AccessPolicy accessPolicy)
{
  return m_transactor->getDataSet(name, e, sr, dt, tr, travType, connected, accessPolicy);
}

std::auto_ptr<te::dt::DateTimePeriod> 
te::stmem::DataSource::getTemporalExtent(const std::string& name)
{
  return m_transactor->getTemporalExtent(name);
}

///protected Methods
void te::stmem::DataSource::create(const std::string& connInfo /*dsInfo*/)
{
  return;
}

void te::stmem::DataSource::drop(const std::string& connInfo /*dsInfo*/)
{
  return;
}

bool te::stmem::DataSource::exists(const std::string& connInfo /*dsInfo*/)
{
  return false;
}

std::vector<std::string> te::stmem::DataSource::getDataSourceNames(const std::string& connInfo /*dsInfo*/)
{
  return std::vector<std::string>();
}
