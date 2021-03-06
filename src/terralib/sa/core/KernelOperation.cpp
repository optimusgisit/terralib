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
  \file terralib/sa/core/KernelOperation.cpp

  \brief This file contains a class that represents the kernel operation.

  \reference Adapted from TerraLib4.
*/

//TerraLib
#include "../../dataaccess/datasource/DataSource.h"
#include "../../dataaccess/datasource/DataSourceManager.h"
#include "../../dataaccess/utils/Utils.h"
#include "../../datatype/SimpleProperty.h"
#include "../../geometry/Geometry.h"
#include "../../geometry/GeometryProperty.h"
#include "../../memory/DataSet.h"
#include "../../memory/DataSetItem.h"
#include "../../raster/BandProperty.h"
#include "../../raster/Grid.h"
#include "../../raster/Raster.h"
#include "../../raster/RasterFactory.h"
#include "KernelOperation.h"
#include "Utils.h"

//STL
#include <cassert>

// Boost
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

te::sa::KernelOperation::KernelOperation()
{
}

te::sa::KernelOperation::~KernelOperation()
{
}

void te::sa::KernelOperation::setOutputParameters(te::sa::KernelOutputParams* outParams)
{
  m_outputParams.reset(outParams);
}

void te::sa::KernelOperation::runRasterKernel(te::sa::KernelInputParams* inputParams, te::sa::KernelTree& kTree, te::sa::KernelMap& kMap, te::rst::Raster* raster)
{
  if(!raster)
    throw;

  //check if use adaptative radius or not
  if(inputParams->m_useAdaptativeRadius)
  {
    te::sa::GridAdaptRadiusKernel(inputParams, kTree, kMap, raster);
  }
  else
  {
    double val = std::max<double>(raster->getGrid()->getExtent()->getWidth(), raster->getGrid()->getExtent()->getHeight());
    double radius = (val * inputParams->m_radiusPercentValue) / 100.;

    te::sa::GridStatRadiusKernel(inputParams, kTree, kMap, raster, radius);
  }
}

std::auto_ptr<te::mem::DataSet> te::sa::KernelOperation::runDataSetKernel(te::sa::KernelInputParams* inputParams, te::sa::KernelTree& kTree, te::sa::KernelMap& kMap, te::da::DataSetType* dsType)
{
  if(!dsType)
    throw;

  //create dataset in memory
  std::auto_ptr<te::mem::DataSet> dataSet = createDataSet(inputParams->m_ds.get(), dsType);

  //get kernel attr index
  std::size_t kernelIdx = dsType->getPropertyPosition(m_outputParams->m_outputAttrName);

  //get geom attr index
  std::size_t geomIdx = te::da::GetFirstPropertyPos(dataSet.get(), te::dt::GEOMETRY_TYPE);

  //check if use adaptative radius or not
  if(inputParams->m_useAdaptativeRadius)
  {
    te::sa::DataSetAdaptRadiusKernel(inputParams, kTree, kMap, dataSet.get(), kernelIdx, geomIdx);
  }
  else
  {
    double val = std::max<double>(kTree.getMBR().getWidth(), kTree.getMBR().getHeight());
    double radius = (val * inputParams->m_radiusPercentValue) / 100.;

    te::sa::DataSetStatRadiusKernel(inputParams, kTree, kMap, dataSet.get(), kernelIdx, geomIdx, radius);
  }

  return dataSet;
}

std::auto_ptr<te::rst::Raster> te::sa::KernelOperation::buildRaster(te::sa::KernelInputParams* inputParams, te::sa::KernelTree& kTree, std::string driver)
{
  //get extent of input data
  te::gm::Envelope* env = new te::gm::Envelope(kTree.getMBR());

  //get srid
  int srid = TE_UNKNOWN_SRS;
  
  te::gm::GeometryProperty* gmProp = te::da::GetFirstGeomProperty(inputParams->m_dsType.get());

  if(gmProp)
    srid = gmProp->getSRID();

  //create grid
  double val = std::max<double>(env->getWidth(), env->getHeight());

  double res = val / m_outputParams->m_nCols;

  te::rst::Grid* grid = new te::rst::Grid(res, res, env, srid);

  //create bands
  te::rst::BandProperty* bProp = new te::rst::BandProperty(0, te::dt::DOUBLE_TYPE);

  bProp->m_noDataValue = 0.;

  std::vector<te::rst::BandProperty*> vecBandProp;
  vecBandProp.push_back(bProp);

  //create raster info
  std::string fileName = m_outputParams->m_outputPath + "/" + m_outputParams->m_outputDataSetName + ".tif";

  std::map<std::string, std::string> rInfo;
  rInfo["URI"] = fileName;

  //create raster
  std::auto_ptr<te::rst::Raster> rst(te::rst::RasterFactory::make(driver, grid, vecBandProp, rInfo));

  return rst;
}

void te::sa::KernelOperation::saveDataSet(te::da::DataSet* dataSet, te::da::DataSetType* dsType)
{
  //create dataset info
  std::string fileName = m_outputParams->m_outputPath + "/" + m_outputParams->m_outputDataSetName + ".shp";

  //data source id
  boost::uuids::basic_random_generator<boost::mt19937> gen;
  boost::uuids::uuid u = gen();
  std::string id_ds = boost::uuids::to_string(u);

  //create data source
  te::da::DataSourcePtr ds = te::da::DataSourceManager::getInstance().get(id_ds, "OGR", ("file://" + fileName));

  //save dataset
  dataSet->moveBeforeFirst();

  std::map<std::string, std::string> options;

  ds->createDataSet(dsType, options);

  ds->add(m_outputParams->m_outputDataSetName, dataSet, options);
}

std::auto_ptr<te::da::DataSetType> te::sa::KernelOperation::createDataSetType(te::da::DataSetType* dsType)
{
  std::auto_ptr<te::da::DataSetType> dataSetType(new te::da::DataSetType(m_outputParams->m_outputDataSetName));

  //create all input dataset properties
  std::vector<te::dt::Property*> propertyVec = dsType->getProperties();

  for(std::size_t t = 0; t < propertyVec.size(); ++t)
  {
    te::dt::Property* prop = propertyVec[t];

    te::dt::Property* newProp = prop->clone();
    newProp->setId(0);
    newProp->setParent(0);

    dataSetType->add(newProp);
  }

  //create kernel property
  te::dt::SimpleProperty* kernelProperty = new te::dt::SimpleProperty(m_outputParams->m_outputAttrName, te::dt::DOUBLE_TYPE);
  dataSetType->add(kernelProperty);

  return dataSetType;
}

std::auto_ptr<te::mem::DataSet> te::sa::KernelOperation::createDataSet(te::da::DataSet* inputDataSet, te::da::DataSetType* dsType)
{
  std::auto_ptr<te::mem::DataSet> outDataset(new te::mem::DataSet(dsType));

  std::size_t nProp = inputDataSet->getNumProperties();

  inputDataSet->moveBeforeFirst();

  while(inputDataSet->moveNext())
  {
    //create dataset item
    te::mem::DataSetItem* outDSetItem = new te::mem::DataSetItem(outDataset.get());

    for(std::size_t t = 0; t < nProp; ++t)
    {
      te::dt::AbstractData* ad = inputDataSet->getValue(t).release();

      outDSetItem->setValue(t, ad);
    }

    //set kernel default value
    outDSetItem->setDouble(m_outputParams->m_outputAttrName, 0.);

    //add item into dataset
    outDataset->add(outDSetItem);
  }

  return outDataset;
}
