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
  \file terralib/wcs/Transactor.cpp

  \brief Implementation of the transactor for the WCS driver.
*/

// TerraLib
#include "../core/translator/Translator.h"
#include "../gdal/DataSet.h"
#include "../gdal/Utils.h"
#include "../geometry/Envelope.h"
#include "../raster/Grid.h"
#include "Exception.h"
#include "Transactor.h"
#include "Utils.h"

// GDAL
#include <gdal_priv.h>

// STL
#include <vector>

te::wcs::Transactor::Transactor(const std::string& serviceURL, const std::string& coverageName)
  : m_serviceURL(serviceURL),
    m_coverageName(coverageName)
{
}

te::wcs::Transactor::~Transactor()
{
}

te::da::DataSource* te::wcs::Transactor::getDataSource() const
{
  return 0;
}

std::auto_ptr<te::da::DataSet> te::wcs::Transactor::getDataSet(const std::string& name,
                                                               te::common::TraverseType /*travType*/,
                                                               bool /*connected*/,
                                                               const te::common::AccessPolicy /*accessPolicy*/)
{
  if(name != m_coverageName)
    throw Exception(TE_TR("The informed data set could not be found in the data source!!"));

  // Retrieves the data set type
  std::auto_ptr<te::da::DataSetType> type = getDataSetType(name);

  // Build the GDAL WCS request
  std::string request = BuildRequest(m_serviceURL, m_coverageName);

  return std::auto_ptr<te::da::DataSet>(new te::gdal::DataSet(type, te::common::RAccess, request));
}

std::auto_ptr<te::da::DataSet> te::wcs::Transactor::getDataSet(const std::string& name,
                                                               const std::string& /*propertyName*/,
                                                               const te::gm::Envelope* e,
                                                               te::gm::SpatialRelation /*r*/,
                                                               te::common::TraverseType /*travType*/,
                                                               bool /*connected*/,
                                                               const te::common::AccessPolicy /*accessPolicy*/)
{
  if(name != m_coverageName)
    throw Exception(TE_TR("The informed data set could not be found in the data source!"));

  // Retrieves the data set type
  std::auto_ptr<te::da::DataSetType> type = getDataSetType(name);

  // Build the GDAL WCS request with extent restriction
  std::string request = BuildRequest(m_serviceURL, m_coverageName, e);

  return std::auto_ptr<te::da::DataSet>(new te::gdal::DataSet(type, te::common::RAccess, request));
}

std::auto_ptr<te::da::DataSet> te::wcs::Transactor::getDataSet(const std::string& name,
                                                               const std::string& propertyName,
                                                               const te::gm::Geometry* g,
                                                               te::gm::SpatialRelation r,
                                                               te::common::TraverseType travType,
                                                               bool connected,
                                                               const te::common::AccessPolicy accessPolicy)
{
  return getDataSet(name, propertyName, g->getMBR(), r, travType, connected, accessPolicy);
}

std::auto_ptr<te::da::DataSet> te::wcs::Transactor::getDataSet(const std::string& name,
                                                               const te::da::ObjectIdSet* oids,
                                                               te::common::TraverseType travType,
                                                               bool connected,
                                                               const te::common::AccessPolicy accessPolicy)
{
  throw Exception(TE_TR("The ObjectIdSet concept is not supported by the WCS driver!"));
}

std::auto_ptr<te::da::DataSet> te::wcs::Transactor::query(const te::da::Select& q,
                                                          te::common::TraverseType travType,
                                                          bool connected,
                                                          const te::common::AccessPolicy accessPolicy)
{
  throw Exception(TE_TR("Query operations is not supported by the WCS driver!"));
}

std::auto_ptr<te::da::DataSet> te::wcs::Transactor::query(const std::string& query,
                                                          te::common::TraverseType travType,
                                                          bool connected,
                                                          const te::common::AccessPolicy accessPolicy)
{
  throw Exception(TE_TR("Query operations is not supported by the WCS driver!"));
}

std::vector<std::string> te::wcs::Transactor::getDataSetNames()
{
  std::vector<std::string> names;
  names.push_back(m_coverageName);

  return names;
}

std::size_t te::wcs::Transactor::getNumberOfDataSets()
{
  return 1;
}

std::auto_ptr<te::da::DataSetType> te::wcs::Transactor::getDataSetType(const std::string& name)
{
  if(name != m_coverageName)
    throw Exception(TE_TR("The informed data set could not be found in the data source."));

   // Build the GDAL WCS request
  std::string request = BuildRequest(m_serviceURL, m_coverageName);

  GDALDataset* gds = static_cast<GDALDataset*>(GDALOpen(request.c_str(), GA_ReadOnly));
  if(gds == 0)
    throw Exception(TE_TR("The data set type could not be retrieved from data source."));
  
  te::da::DataSetType* type = new te::da::DataSetType(m_coverageName, 0);
  type->setTitle(m_coverageName);
  
  te::rst::Grid* grid = te::gdal::GetGrid(gds);

  std::vector<te::rst::BandProperty*> bandProperties;
  te::gdal::GetBandProperties(gds, bandProperties);

  te::rst::RasterProperty* rp = new te::rst::RasterProperty("raster");
  rp->set(grid);

  for(std::size_t i = 0; i < bandProperties.size(); ++i)
    rp->add(bandProperties[i]);

  type->add(rp);

  GDALClose(gds);

  return std::auto_ptr<te::da::DataSetType>(type);
}

boost::ptr_vector<te::dt::Property> te::wcs::Transactor::getProperties(const std::string& datasetName)
{
  boost::ptr_vector<te::dt::Property> properties;

  std::auto_ptr<te::da::DataSetType> type = getDataSetType(datasetName);

  const std::vector<te::dt::Property*>& props = type->getProperties();
  for(std::size_t i = 0; i < props.size(); ++i)
      properties.push_back(props[i]->clone());
  
  return properties;
}

std::auto_ptr<te::dt::Property> te::wcs::Transactor::getProperty(const std::string& datasetName, const std::string& name)
{
  std::auto_ptr<te::da::DataSetType> type = getDataSetType(datasetName);

  const std::vector<te::dt::Property*>& props = type->getProperties();
  for(std::size_t i = 0; i < props.size(); ++i)
  {
    if(props[i]->getName() == name)
      return std::auto_ptr<te::dt::Property>(props[i]->clone()); 
  }

  throw Exception(TE_TR("The informed property name could not be found in the data set."));
}

std::auto_ptr<te::dt::Property> te::wcs::Transactor::getProperty(const std::string& datasetName, std::size_t propertyPos)
{
  std::auto_ptr<te::da::DataSetType> type = getDataSetType(datasetName);

  const std::vector<te::dt::Property*>& props = type->getProperties();

  if(propertyPos >= props.size())
    throw Exception(TE_TR("The informed property position could not be found in the data set."));

  return std::auto_ptr<te::dt::Property>(props[propertyPos]->clone());
}

std::vector<std::string> te::wcs::Transactor::getPropertyNames(const std::string& datasetName)
{
  std::vector<std::string> propertyNames;

  std::auto_ptr<te::da::DataSetType> type = getDataSetType(datasetName);

  const std::vector<te::dt::Property*>& props = type->getProperties();
  for(std::size_t i = 0; i < props.size(); ++i)
    propertyNames.push_back(props[i]->getName());

  return propertyNames;
}

std::size_t te::wcs::Transactor::getNumberOfProperties(const std::string& datasetName)
{
  return getPropertyNames(datasetName).size();
}

bool te::wcs::Transactor::propertyExists(const std::string& datasetName, const std::string& name)
{
  std::vector<std::string> propertyNames = getPropertyNames(datasetName);

  for(std::size_t i = 0; i < propertyNames.size(); ++i)
    if(propertyNames[i] == name)
      return true;

  return false;
}

std::auto_ptr<te::gm::Envelope> te::wcs::Transactor::getExtent(const std::string& datasetName,
                                                               const std::string& propertyName)
{
  std::auto_ptr<te::da::DataSetType> type = getDataSetType(datasetName);

  std::auto_ptr<te::dt::Property> prop = getProperty(datasetName, propertyName);

  te::rst::RasterProperty* rasterProperty = static_cast<te::rst::RasterProperty*>(prop.get());
  te::gm::Envelope* e = rasterProperty->getGrid()->getExtent();
  
  return std::auto_ptr<te::gm::Envelope>(new te::gm::Envelope(e->getLowerLeftX(), e->getLowerLeftY(),
                                                              e->getUpperRightX(), e->getUpperRightY()));
}

std::auto_ptr<te::gm::Envelope> te::wcs::Transactor::getExtent(const std::string& datasetName,
                                                               std::size_t propertyPos)
{
  std::auto_ptr<te::da::DataSetType> type = getDataSetType(datasetName);

  std::auto_ptr<te::dt::Property> prop = getProperty(datasetName, propertyPos);

  te::rst::RasterProperty* rasterProperty = static_cast<te::rst::RasterProperty*>(prop.get());
  te::gm::Envelope* e = rasterProperty->getGrid()->getExtent();

  return std::auto_ptr<te::gm::Envelope>(new te::gm::Envelope(e->getLowerLeftX(), e->getLowerLeftY(),
                                                              e->getUpperRightX(), e->getUpperRightY()));
}

std::size_t te::wcs::Transactor::getNumberOfItems(const std::string& datasetName)
{
  return 1;
}

bool te::wcs::Transactor::hasDataSets()
{
  return !m_coverageName.empty();
}

bool te::wcs::Transactor::dataSetExists(const std::string& name)
{
  return name == m_coverageName;
}

/** NOT SUPPORTED METHODS */
//@{

void te::wcs::Transactor::begin()
{
  throw Exception(TE_TR("The method begin() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::commit()
{
  throw Exception(TE_TR("The method commit() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::rollBack()
{
  throw Exception(TE_TR("The method rollBack() is not supported by the WCS driver!"));
}

bool te::wcs::Transactor::isInTransaction() const
{
  throw Exception(TE_TR("The method isInTransaction() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::execute(const te::da::Query& /*command*/)
{
  throw Exception(TE_TR("The method execute() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::execute(const std::string& /*command*/)
{
  throw Exception(TE_TR("The method execute() is not supported by the WCS driver!"));
}

std::auto_ptr<te::da::PreparedQuery> te::wcs::Transactor::getPrepared(const std::string& /*qName*/)
{
  throw Exception(TE_TR("The method getPrepared() is not supported by the WCS driver!"));
}

std::auto_ptr<te::da::BatchExecutor> te::wcs::Transactor::getBatchExecutor()
{
  throw Exception(TE_TR("The method getBatchExecutor() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::cancel()
{
  throw Exception(TE_TR("The method cancel() is not supported by the WCS driver!"));
}

boost::int64_t te::wcs::Transactor::getLastGeneratedId()
{
  throw Exception(TE_TR("The method getLastGeneratedId() is not supported by the WCS driver!"));
}

std::string te::wcs::Transactor::escape(const std::string& /*value*/)
{
  throw Exception(TE_TR("The method escape() is not supported by the WCS driver!"));
}

bool te::wcs::Transactor::isDataSetNameValid(const std::string& /*datasetName*/)
{
  throw Exception(TE_TR("The method isDataSetNameValid() is not supported by the WCS driver!"));
}

bool te::wcs::Transactor::isPropertyNameValid(const std::string& /*propertyName*/)
{
  throw Exception(TE_TR("The method isPropertyNameValid() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::addProperty(const std::string& /*datasetName*/, te::dt::Property* /*p*/)
{
  throw Exception(TE_TR("The method addProperty() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::dropProperty(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  throw Exception(TE_TR("The method dropProperty() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::renameProperty(const std::string& /*datasetName*/, const std::string& /*propertyName*/, const std::string& /*newPropertyName*/)
{
  throw Exception(TE_TR("The method renameProperty() is not supported by the WCS driver!"));
}

std::auto_ptr<te::da::PrimaryKey> te::wcs::Transactor::getPrimaryKey(const std::string& /*datasetName*/)
{
  throw Exception(TE_TR("The method getPrimaryKey() is not supported by the WCS driver!"));
}

bool te::wcs::Transactor::primaryKeyExists(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  throw Exception(TE_TR("The method primaryKeyExists() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::addPrimaryKey(const std::string& /*datasetName*/, te::da::PrimaryKey* /*pk*/)
{
  throw Exception(TE_TR("The method addPrimaryKey() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::dropPrimaryKey(const std::string& /*datasetName*/)
{
  throw Exception(TE_TR("The method dropPrimaryKey() is not supported by the WCS driver!"));
}

std::auto_ptr<te::da::ForeignKey> te::wcs::Transactor::getForeignKey(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  throw Exception(TE_TR("The method getForeignKey() is not supported by the WCS driver!"));
}

std::vector<std::string> te::wcs::Transactor::getForeignKeyNames(const std::string& /*datasetName*/)
{
  throw Exception(TE_TR("The method getForeignKeyNames() is not supported by the WCS driver!"));
}

bool te::wcs::Transactor::foreignKeyExists(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  throw Exception(TE_TR("The method foreignKeyExists() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::addForeignKey(const std::string& /*datasetName*/, te::da::ForeignKey* /*fk*/)
{
  throw Exception(TE_TR("The method addForeignKey() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::dropForeignKey(const std::string& /*datasetName*/, const std::string& /*fkName*/)
{
  throw Exception(TE_TR("The method dropForeignKey() is not supported by the WCS driver!"));
}

std::auto_ptr<te::da::UniqueKey> te::wcs::Transactor::getUniqueKey(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  throw Exception(TE_TR("The method getUniqueKey() is not supported by the WCS driver!"));
}

std::vector<std::string> te::wcs::Transactor::getUniqueKeyNames(const std::string& /*datasetName*/)
{
  throw Exception(TE_TR("The method getUniqueKeyNames() is not supported by the WCS driver!"));
}

bool te::wcs::Transactor::uniqueKeyExists(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  throw Exception(TE_TR("The method uniqueKeyExists() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::addUniqueKey(const std::string& /*datasetName*/, te::da::UniqueKey* /*uk*/)
{
  throw Exception(TE_TR("The method addUniqueKey() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::dropUniqueKey(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  throw Exception(TE_TR("The method dropUniqueKey() is not supported by the WCS driver!"));
}

std::auto_ptr<te::da::CheckConstraint> te::wcs::Transactor::getCheckConstraint(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  throw Exception(TE_TR("The method getCheckConstraint() is not supported by the WCS driver!"));
}

std::vector<std::string> te::wcs::Transactor::getCheckConstraintNames(const std::string& /*datasetName*/)
{
  throw Exception(TE_TR("The method getCheckConstraintNames() is not supported by the WCS driver!"));
}

bool te::wcs::Transactor::checkConstraintExists(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  throw Exception(TE_TR("The method checkConstraintExists() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::addCheckConstraint(const std::string& /*datasetName*/, te::da::CheckConstraint* /*cc*/)
{
  throw Exception(TE_TR("The method addCheckConstraint() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::dropCheckConstraint(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  throw Exception(TE_TR("The method dropCheckConstraint() is not supported by the WCS driver!"));
}

std::auto_ptr<te::da::Index> te::wcs::Transactor::getIndex(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  throw Exception(TE_TR("The method getIndex() is not supported by the WCS driver!"));
}

std::vector<std::string> te::wcs::Transactor::getIndexNames(const std::string& /*datasetName*/)
{
  throw Exception(TE_TR("The method getIndexNames() is not supported by the WCS driver!"));
}

bool te::wcs::Transactor::indexExists(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  throw Exception(TE_TR("The method indexExists() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::addIndex(const std::string& /*datasetName*/, te::da::Index* /*idx*/, const std::map<std::string, std::string>& /*options*/)
{
  throw Exception(TE_TR("The method addIndex() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::dropIndex(const std::string& /*datasetName*/, const std::string& /*idxName*/)
{
  throw Exception(TE_TR("The method dropIndex() is not supported by the WCS driver!"));
}

std::auto_ptr<te::da::Sequence> te::wcs::Transactor::getSequence(const std::string& /*name*/)
{
  throw Exception(TE_TR("The method getSequence() is not supported by the WCS driver!"));
}

std::vector<std::string> te::wcs::Transactor::getSequenceNames()
{
  throw Exception(TE_TR("The method getSequenceNames() is not supported by the WCS driver!"));
}

bool te::wcs::Transactor::sequenceExists(const std::string& /*name*/)
{
  throw Exception(TE_TR("The method sequenceExists() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::addSequence(te::da::Sequence* /*sequence*/)
{
  throw Exception(TE_TR("The method addSequence() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::dropSequence(const std::string& /*name*/)
{
  throw Exception(TE_TR("The method dropSequence() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::createDataSet(te::da::DataSetType* /*dt*/,
                                        const std::map<std::string, std::string>& /*options*/)
{
  throw Exception(TE_TR("The method createDataSet() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::cloneDataSet(const std::string& /*name*/,
                                       const std::string& /*cloneName*/,
                                       const std::map<std::string, std::string>& /*options*/)
{
  throw Exception(TE_TR("The method cloneDataSet() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::dropDataSet(const std::string& /*name*/)
{
  throw Exception(TE_TR("The method dropDataSet() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::renameDataSet(const std::string& /*name*/,
                                        const std::string& /*newName*/)
{
  throw Exception(TE_TR("The method renameDataSet() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::add(const std::string& /*datasetName*/,
                              te::da::DataSet* /*d*/,
                              const std::map<std::string, std::string>& /*options*/,
                              std::size_t /*limit*/)
{
  throw Exception(TE_TR("The method add() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::remove(const std::string& /*datasetName*/, const te::da::ObjectIdSet* /*oids*/)
{
  throw Exception(TE_TR("The method remove() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::update(const std::string& /*datasetName*/,
                                 te::da::DataSet* /*dataset*/,
                                 const std::vector<std::size_t>& /*properties*/,
                                 const te::da::ObjectIdSet* /*oids*/,
                                 const std::map<std::string, std::string>& /*options*/,
                                 std::size_t /*limit*/)
{
  throw Exception(TE_TR("The method update() is not supported by the WCS driver!"));
}

void te::wcs::Transactor::optimize(const std::map<std::string, std::string>& /*opInfo*/)
{
  throw Exception(TE_TR("The method optimize() is not supported by the WCS driver!"));
}

//@}
