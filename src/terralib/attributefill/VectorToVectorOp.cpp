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
 \file VectorToVectorOp.cpp
 */

#include "../core/logger/Logger.h"
#include "../dataaccess/dataset/DataSet.h"
#include "../dataaccess/dataset/DataSetAdapter.h"
#include "../dataaccess/dataset/DataSetType.h"
#include "../dataaccess/dataset/DataSetTypeConverter.h"
#include "../dataaccess/dataset/ObjectIdSet.h"
#include "../dataaccess/datasource/DataSource.h"
#include "../dataaccess/datasource/DataSourceCapabilities.h"
#include "../dataaccess/datasource/DataSourceTransactor.h"
#include "../dataaccess/utils/Utils.h"

#include "../datatype/Property.h"
#include "../datatype/StringProperty.h"

#include "../geometry/GeometryProperty.h"

#include "../statistics/core/Utils.h"

#include "VectorToVectorOp.h"

te::attributefill::VectorToVectorOp::VectorToVectorOp():
  m_hasErrors(false),
  m_outDset("")
{
}

te::attributefill::VectorToVectorOp::~VectorToVectorOp()
{
}

void te::attributefill::VectorToVectorOp::setInput(te::map::AbstractLayerPtr fromLayer,
                                                   te::map::AbstractLayerPtr toLayer)
{
  m_fromLayer = fromLayer;
  m_toLayer = toLayer;
}

void te::attributefill::VectorToVectorOp::setParams(const std::map<std::string, std::vector<te::attributefill::OperationType> >& options, std::vector<std::string> toLayerProps)
{
  m_options = options;
  m_toLayerProps = toLayerProps;
}

void te::attributefill::VectorToVectorOp::setOutput(te::da::DataSourcePtr outDsrc, std::string dsname)
{
  m_outDsrc = outDsrc;
  m_outDset = dsname;
}

bool te::attributefill::VectorToVectorOp::paramsAreValid()
{/*
  if (!m_inDsetType.get())
    return false;
  
  if (!m_inDsetType->hasGeom())
    return false;
  
  if (m_groupProps.empty())
    return false;

  if (m_outDset.empty() || !m_outDsrc.get())
    return false;
  */
  return true;
}

bool  te::attributefill::VectorToVectorOp::save(std::auto_ptr<te::mem::DataSet> result, std::auto_ptr<te::da::DataSetType> outDsType)
{
  std::auto_ptr<te::da::DataSourceTransactor> t;
  try
  {
    // do any adaptation necessary to persist the output dataset
    te::da::DataSetTypeConverter* converter = new te::da::DataSetTypeConverter(outDsType.get(), m_outDsrc->getCapabilities(), m_outDsrc->getEncoding());
    te::da::DataSetType* dsTypeResult = converter->getResult();
    std::auto_ptr<te::da::DataSetAdapter> dsAdapter(te::da::CreateAdapter(result.get(), converter));

    // create the primary key if it is possible
    if (m_outDsrc->getCapabilities().getDataSetTypeCapabilities().supportsPrimaryKey())
    {
      std::string pk_name = dsTypeResult->getName() + "_pkey";
      te::da::PrimaryKey* pk = new te::da::PrimaryKey(pk_name, dsTypeResult);
      pk->add(dsTypeResult->getProperty(0));
      outDsType->setPrimaryKey(pk);
    }

    t = m_outDsrc->getTransactor();

    t->begin();

    std::map<std::string, std::string> options;
    // create the dataset
    m_outDsrc->createDataSet(dsTypeResult, options);

    // copy from memory to output datasource
    result->moveBeforeFirst();
    m_outDsrc->add(dsTypeResult->getName(),result.get(), options);

    t->commit();
  }
  catch(te::common::Exception& e)
  {
#ifdef TERRALIB_LOGGER_ENABLED
    std::string ex = e.what();
    ex += " | Ref: Error while persisting in the DataSource";
    TE_CORE_LOG_DEBUG("attributefill", ex.c_str());
#endif //TERRALIB_LOGGER_ENABLED
    t->rollBack();
    m_hasErrors = true;
  }
  catch(std::exception& e)
  {
#ifdef TERRALIB_LOGGER_ENABLED
    std::string ex = e.what();
    ex += " | Ref: Error while persisting in the DataSource";
    TE_CORE_LOG_DEBUG("attributefill", ex.c_str());
#endif //TERRALIB_LOGGER_ENABLED
    
    t->rollBack();
    m_hasErrors = true;
  }

  return true;
}

bool te::attributefill::VectorToVectorOp::hasErrors()
{
  return m_hasErrors;
}