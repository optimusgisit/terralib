/*  Copyright (C) 2001-2009 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/maptools/QueryLayer.cpp

  \brief A layer resulting from a query.
*/

// TerraLib
#include "../common/StringUtils.h"
#include "../dataaccess2/dataset/DataSet.h"
#include "../dataaccess2/dataset/DataSetType.h"
#include "../dataaccess2/dataset/ObjectIdSet.h"
#include "../dataaccess2/datasource/DataSource.h"
//#include "../dataaccess/datasource/DataSourceCatalogLoader.h"
//#include "../dataaccess/datasource/DataSourceTransactor.h"
#include "../dataaccess2/query/And.h"
#include "../dataaccess2/query/DataSetName.h"
#include "../dataaccess2/query/Expression.h"
#include "../dataaccess2/query/Field.h"
#include "../dataaccess2/query/Function.h"
#include "../dataaccess2/query/FromItem.h"
#include "../dataaccess2/query/LiteralEnvelope.h"
#include "../dataaccess2/query/PropertyName.h"
#include "../dataaccess2/query/Select.h"
#include "../dataaccess2/query/ST_Intersects.h"
#include "../dataaccess2/query/Where.h"
#include "../dataaccess2/utils/Utils.h"
#include "../datatype/Property.h"
#include "../geometry/GeometryProperty.h"
#include "../memory2/DataSet.h"
#include "../memory2/DataSetItem.h"
#include "Exception.h"
#include "QueryLayer.h"
#include "RendererFactory.h"
#include "Utils.h"

// Boost
#include <boost/format.hpp>

// STL
#include <memory>

const std::string te::map::QueryLayer::sm_type("QUERYLAYER");

te::map::QueryLayer::QueryLayer(AbstractLayer* parent)
  : AbstractLayer(parent),
    m_query(0)
{
}

te::map::QueryLayer::QueryLayer(const std::string& id, AbstractLayer* parent)
  : AbstractLayer(id, parent),
    m_query(0)
{
}

te::map::QueryLayer::QueryLayer(const std::string& id,
                                const std::string& title,
                                AbstractLayer* parent)
  : AbstractLayer(id, title, parent),
    m_query(0)
{
}

te::map::QueryLayer::~QueryLayer()
{
  delete m_query;
}

const te::map::LayerSchema* te::map::QueryLayer::getSchema(const bool /*full*/) const
{
  // TODO: Can we store this schema?

  std::auto_ptr<te::map::LayerSchema> output(new te::map::LayerSchema(getTitle()));

  te::da::DataSourcePtr ds = te::da::GetDataSource(m_datasourceId);

  const te::da::Fields* fields = m_query->getFields();

  for(size_t i = 0; i < fields->size(); ++i)
  {
    te::da::Field field = fields->at(i);
    te::da::Expression* exp = field.getExpression();

    te::da::PropertyName* pName = dynamic_cast<te::da::PropertyName*>(exp);

    std::vector<std::string> tokens;
    te::common::Tokenize(pName->getName(), tokens, ".");

    assert(tokens.size() == 2);

    std::auto_ptr<te::da::DataSetType> dt = ds->getDataSetType(tokens[0]);

    te::dt::Property* pRef = dt->getProperty(tokens[1]);
    assert(pRef);

    std::auto_ptr<te::dt::Property> p(pRef->clone());
    p->setName(pName->getName());

    output->add(p.release());
  }

  return output.release();
}

te::da::DataSet* te::map::QueryLayer::getData(te::common::TraverseType travType, 
                                              te::common::AccessPolicy rwRole) const
{
  return getData(m_query, travType, rwRole);
}

te::da::DataSet* te::map::QueryLayer::getData(const te::gm::Envelope& e,
                                              te::gm::SpatialRelation r,
                                              te::common::TraverseType travType,
                                              te::common::AccessPolicy rwRole) const
{
  te::gm::GeometryProperty* geometryProperty = te::da::GetFirstGeomProperty(getSchema());

  assert(geometryProperty);

  return getData(*geometryProperty, e, r, travType, rwRole);
}

te::da::DataSet* te::map::QueryLayer::getData(const te::dt::Property& p,
                                              const te::gm::Envelope& e,
                                              te::gm::SpatialRelation r,
                                              te::common::TraverseType travType,
                                              te::common::AccessPolicy rwRole) const
{
  te::da::LiteralEnvelope* lenv = new te::da::LiteralEnvelope(e, m_srid);

  te::da::PropertyName* pname = new te::da::PropertyName(p.getName());

  // TODO: switch that verifies the given te::gm::SpatialRelation and build the query object (ST_Intersects. ST_Touches, etc).
  te::da::ST_Intersects* intersects = new te::da::ST_Intersects(pname, lenv);

  // The final select
  std::auto_ptr<te::da::Select> select(static_cast<te::da::Select*>(m_query->clone()));

  // Original Where
  te::da::Where* wh = select->getWhere();

  // Original restriction expression
  te::da::Expression* exp = wh->getExp()->clone();

  // The final restriction: original restriction expression + extent restriction
  te::da::And* andop = new te::da::And(exp, intersects);
  wh->setExp(andop);

  return getData(select.get(), travType, rwRole);
}

te::da::DataSet* te::map::QueryLayer::getData(const te::gm::Geometry& /*g*/,
                                              te::gm::SpatialRelation /*r*/,
                                              te::common::TraverseType /*travType*/, 
                                              te::common::AccessPolicy /*rwRole*/) const
{
  return 0; // TODO
}

te::da::DataSet* te::map::QueryLayer::getData(const te::dt::Property& /*p*/,
                                              const te::gm::Geometry& /*g*/,
                                              te::gm::SpatialRelation /*r*/,
                                              te::common::TraverseType /*travType*/,
                                              te::common::AccessPolicy /*rwRole*/) const
{
  return 0; // TODO
}

te::da::DataSet* te::map::QueryLayer::getData(te::da::Expression* restriction,
                                              te::common::TraverseType travType,
                                              te::common::AccessPolicy rwRole) const
{
  // The final select
  std::auto_ptr<te::da::Select> select(static_cast<te::da::Select*>(m_query->clone()));

  // Original Where
  te::da::Where* wh = select->getWhere();

  // Original restriction expression
  te::da::Expression* exp = wh->getExp()->clone();

  // The final restriction: original restriction expression + the given restriction expression
  te::da::And* andop = new te::da::And(exp, restriction);
  wh->setExp(andop);

  return getData(select.get(), travType, rwRole);
}

te::da::DataSet* te::map::QueryLayer::getData(const te::da::ObjectIdSet* oids,
                                              te::common::TraverseType travType,
                                              te::common::AccessPolicy rwRole) const
{
  // The final select
  std::auto_ptr<te::da::Select> select(static_cast<te::da::Select*>(m_query->clone()));

  // Original Where
  te::da::Where* wh = select->getWhere();

  // Original restriction expression
  te::da::Expression* exp = wh->getExp()->clone();

  // The final restriction: original restriction expression + the oids restriction
  te::da::And* andop = new te::da::And(exp, oids->getExpression());
  wh->setExp(andop);

  return getData(select.get(), travType, rwRole);
}

bool te::map::QueryLayer::isValid() const
{
  return true;
}

void te::map::QueryLayer::draw(Canvas* canvas, const te::gm::Envelope& bbox, int srid)
{
  if(m_rendererType.empty())
    throw Exception((boost::format(TR_MAP("Could not draw the query layer %1%. The renderer type is empty!")) % getTitle()).str());

  // Try get the defined renderer
  std::auto_ptr<AbstractRenderer> renderer(RendererFactory::make(m_rendererType));
  if(renderer.get() == 0)
    throw Exception((boost::format(TR_MAP("Could not draw the query layer %1%. The renderer %2% could not be created!")) % getTitle() % m_rendererType).str());

  renderer->draw(this, canvas, bbox, srid);
}

const std::string& te::map::QueryLayer::getType() const
{
  return sm_type;
}

te::da::Select* te::map::QueryLayer::getQuery() const
{
  return m_query;
}

void te::map::QueryLayer::setQuery(te::da::Select* s)
{
  delete m_query;

  m_query = s;
}

const std::string& te::map::QueryLayer::getDataSourceId() const
{
  return m_datasourceId;
}

void te::map::QueryLayer::setDataSourceId(const std::string& id)
{
  m_datasourceId = id;
}

const std::string& te::map::QueryLayer::getRendererType() const
{
  return m_rendererType;
}

void te::map::QueryLayer::setRendererType(const std::string& t)
{
  m_rendererType = t;
}

void te::map::QueryLayer::computeExtent()
{
  if(m_mbr.isValid())
    return;

  // Get the associated data source
  te::da::DataSourcePtr ds = te::da::GetDataSource(m_datasourceId);
    
  // Get the dataset
  std::auto_ptr<te::da::DataSet> dataset = ds->query(m_query);
  assert(dataset.get());

  // MBR
  m_mbr = *dataset->getExtent(te::da::GetFirstPropertyPos(dataset.get(), te::dt::GEOMETRY_TYPE));
}

te::da::DataSet* te::map::QueryLayer::getData(te::da::Select* query,
                                              te::common::TraverseType travType,
                                              te::common::AccessPolicy rwRole) const
{
  te::da::DataSourcePtr ds = te::da::GetDataSource(m_datasourceId);

  std::auto_ptr<const LayerSchema> schema (getSchema());

  std::auto_ptr<te::da::DataSet> dset = ds->query(query, travType);
  assert(dset.get());

  // Convert to memory
  te::mem::DataSet* newDs = new te::mem::DataSet(schema.get());

  while(dset->moveNext())
  {
    te::mem::DataSetItem* item = new te::mem::DataSetItem(newDs);
    for(std::size_t i = 0; i < newDs->getNumProperties(); ++i)
    {
      std::vector<std::string> tokens;
      te::common::Tokenize(newDs->getPropertyName(i), tokens, ".");

      item->setValue(newDs->getPropertyName(i), dset->getValue(tokens[1]));
    }
    newDs->add(item);
  }

  return newDs;
}