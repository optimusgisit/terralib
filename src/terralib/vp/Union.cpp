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
\file Union.cpp
*/

#include "../core/translator/Translator.h"
#include "../core/logger/Logger.h"
#include "../dataaccess/utils/Utils.h"
#include "../datatype/SimpleData.h"
#include "../geometry/Geometry.h"
#include "../geometry/GeometryProperty.h"
#include "../geometry/MultiLineString.h"
#include "../geometry/MultiPoint.h"
#include "../geometry/MultiPolygon.h"
#include "../geometry/Utils.h"
#include "ComplexData.h"
#include "Union.h"
#include "Utils.h"

te::vp::Union::Union()
{
}

bool te::vp::Union::executeMemory(te::vp::AlgorithmParams* mainParams)
{
  te::vp::ValidateAlgorithmParams(mainParams, te::vp::MEMORY);

  std::vector<te::vp::InputParams> inputParams =  mainParams->getInputParams();

  te::da::DataSetType* firstDt = inputParams[0].m_inputDataSetType;
  te::da::DataSet* firstDs = inputParams[0].m_inputDataSet;
  te::gm::GeometryProperty* firstGeomProp =
      dynamic_cast<te::gm::GeometryProperty*>(
          te::da::GetFirstSpatialProperty(firstDt));

  te::da::DataSetType* secondDt = inputParams[1].m_inputDataSetType;
  te::da::DataSet* secondDs = inputParams[1].m_inputDataSet;
  te::gm::GeometryProperty* secondGeomProp =
      dynamic_cast<te::gm::GeometryProperty*>(
          te::da::GetFirstSpatialProperty(secondDt));

  std::map<std::string, te::dt::AbstractData*> specificParams =
      mainParams->getSpecificParams();

  te::sam::rtree::Index<std::size_t, 8>* rtreeSecond =
      te::vp::GetRtree(secondDs);

  std::unique_ptr<te::da::DataSetType> outputDataSetType(
      getOutputDataSetType(mainParams));

  std::unique_ptr<te::mem::DataSet> outputDataSet(
      new te::mem::DataSet(outputDataSetType.get()));

  te::gm::GeometryProperty* outputGeometryProperty =
      te::da::GetFirstGeomProperty(outputDataSetType.get());

  te::da::DataSourcePtr outputSource = mainParams->getOutputDataSource();

  bool isCollection = this->isCollection(specificParams);

  firstDs->moveBeforeFirst();

  while (firstDs->moveNext())
  {
    std::auto_ptr<te::gm::Geometry> firstCurrGeom =
        firstDs->getGeometry(firstGeomProp->getName());

    std::vector<std::size_t> report;
    rtreeSecond->search(*firstCurrGeom->getMBR(), report);

    for (std::size_t i = 0; i < report.size(); ++i)
    {
      secondDs->move(report[i]);

      std::unique_ptr<te::gm::Geometry> secondCurrGeom =
          std::move(secondDs->getGeometry(secondGeomProp->getName()));

      if(firstCurrGeom->intersects(secondCurrGeom.get()) &&
         !firstCurrGeom->touches(secondCurrGeom.get()))
      {
        te::gm::Geometry* geomResult = 0;

        for (std::size_t o = 0; o < 2; ++o)
        {
          if (o == 0)
            geomResult = firstCurrGeom->difference(secondCurrGeom.get());
          else
            geomResult = firstCurrGeom->intersection(secondCurrGeom.get());

          if (!geomResult || !geomResult->isValid())
          {
#ifdef TERRALIB_LOGGER_ENABLED
            //te::core::Logger::logDebug("Intersection - Invalid geometry found");
#endif //TERRALIB_LOGGER_ENABLED
            continue;
          }

          std::unique_ptr<te::mem::DataSetItem> item(
              new te::mem::DataSetItem(outputDataSet.get()));

          for(std::map<std::string, std::string>::iterator it =
                  m_firstAttrNameMap.begin();
              it != m_firstAttrNameMap.end(); ++it)
          {
            if (!firstDs->isNull(it->second))
              item->setValue(it->first, firstDs->getValue(it->second).release());
          }

          if (o == 1)
          {
            for(std::map<std::string, std::string>::iterator it =
                    m_secondAttrNameMap.begin();
                it != m_secondAttrNameMap.end(); ++it)
            {
              if (!secondDs->isNull(it->second))
                item->setValue(it->first,
                               secondDs->getValue(it->second).release());
            }
          }

          if (!geomResult->isEmpty())
          {
            std::vector<te::gm::Geometry*> geomVec;

            if (isCollection)
              geomVec.push_back(geomResult);
            else
              te::gm::Multi2Single(geomResult, geomVec);

            for (std::size_t g = 0; g < geomVec.size(); ++g)
            {
              if(!te::vp::IsMultiType(geomVec[g]->getGeomTypeId()) &&
                 isCollection)
                geomVec[g] = te::vp::SetGeomAsMulti(geomVec[g]);

              std::unique_ptr<te::mem::DataSetItem> currentItem = item->clone();

              if(geomVec[g]->getGeomTypeId() !=
                 outputGeometryProperty->getGeometryType())
                 continue;

              currentItem->setGeometry(outputGeometryProperty->getName(),
                                       geomVec[g]);

              outputDataSet->add(currentItem.release());
            }
          }
        }
      }
    }
  }

  te::sam::rtree::Index<std::size_t, 8>* rtreeFirst = te::vp::GetRtree(firstDs);

  secondDs->moveBeforeFirst();
  while (secondDs->moveNext())
  {
    std::unique_ptr<te::gm::Geometry> secondCurrGeom =
        std::move(secondDs->getGeometry(secondGeomProp->getName()));

    std::vector<std::size_t> report;
    rtreeFirst->search(*secondCurrGeom->getMBR(), report);

    for (std::size_t i = 0; i < report.size(); ++i)
    {
      firstDs->move(report[i]);

      std::unique_ptr<te::gm::Geometry> firstCurrGeom =
          std::move(firstDs->getGeometry(firstGeomProp->getName()));

      if(secondCurrGeom->intersects(firstCurrGeom.get()) &&
         !secondCurrGeom->touches(firstCurrGeom.get()))
      {
        te::gm::Geometry* geomResult = 0;

        geomResult = secondCurrGeom->difference(firstCurrGeom.get());

        if (!geomResult || !geomResult->isValid())
        {
#ifdef TERRALIB_LOGGER_ENABLED
          //te::core::Logger::logDebug("Intersection - Invalid geometry found");
#endif //TERRALIB_LOGGER_ENABLED
          continue;
        }

        std::unique_ptr<te::mem::DataSetItem> item(
            new te::mem::DataSetItem(outputDataSet.get()));

        for(std::map<std::string, std::string>::iterator it =
                m_secondAttrNameMap.begin();
            it != m_secondAttrNameMap.end(); ++it)
        {
          if (!secondDs->isNull(it->second))
            item->setValue(it->first, secondDs->getValue(it->second).release());
        }

        if (!geomResult->isEmpty())
        {
          std::vector<te::gm::Geometry*> geomVec;

          if (isCollection)
            geomVec.push_back(geomResult);
          else
            te::gm::Multi2Single(geomResult, geomVec);

          for (std::size_t g = 0; g < geomVec.size(); ++g)
          {
            if(!te::vp::IsMultiType(geomVec[g]->getGeomTypeId()) &&
               isCollection)
              geomVec[g] = te::vp::SetGeomAsMulti(geomVec[g]);

            std::unique_ptr<te::mem::DataSetItem> currentItem = item->clone();
            currentItem->setGeometry(outputGeometryProperty->getName(),
                                     geomVec[g]);

            outputDataSet->add(currentItem.release());
          }
        }
      }
    }
  }

  outputDataSet->moveBeforeFirst();

  std::unique_ptr<te::da::DataSet> dataSetPrepared =
      std::move(PrepareAdd(outputDataSet.release(), outputDataSetType.get()));

  if (!dataSetPrepared.get())
    throw te::common::Exception(
        TE_TR("Output DataSet was not prepared to save."));

  if (dataSetPrepared->size() == 0)
    throw te::common::Exception(TE_TR("The resultant layer is empty!"));

  te::vp::Save(outputSource.get(), dataSetPrepared.get(),
               outputDataSetType.get());

  return true;
}

bool te::vp::Union::isCollection(
    const std::map<std::string, te::dt::AbstractData*>& specificParams)
{
  bool isCollection = false;

  for(std::map<std::string, te::dt::AbstractData*>::const_iterator it =
          specificParams.begin();
      it != specificParams.end(); ++it)
  {
    if(it->first != "IS_COLLECTION")
    {
      continue;
    }

    te::dt::SimpleData<bool, te::dt::BOOLEAN_TYPE>* sd =
        dynamic_cast<te::dt::SimpleData<bool, te::dt::BOOLEAN_TYPE>*>(
            it->second);

    if(sd)
      isCollection = sd->getValue();
  }

  return isCollection;
}

std::vector<std::pair<std::string, std::string> > te::vp::Union::getProperties(
    const std::map<std::string, te::dt::AbstractData*>& specificParams)
{
  std::map<std::string, te::dt::AbstractData*>::const_iterator it =
      specificParams.begin();

  std::vector<std::pair<std::string, std::string> > propNames;

  while(it != specificParams.end())
  {
    if(it->first != "ATTRIBUTES")
    {
      ++it;
      continue;
    }

    te::dt::AbstractData* abData = it->second;

    te::vp::ComplexData<std::vector<std::pair<std::string, std::string> > >*
        cd = dynamic_cast<te::vp::ComplexData<
            std::vector<std::pair<std::string, std::string> > >*>(abData);

    if(cd)
      propNames = cd->getValue();

    ++it;
  }

  return propNames;
}

bool te::vp::Union::executeQuery(te::vp::AlgorithmParams* mainParams)
{
  return false;
}

te::da::DataSetType* te::vp::Union::getOutputDataSetType(
    te::vp::AlgorithmParams* mainParams)
{
  te::vp::InputParams firstInputParams = mainParams->getInputParams()[0];
  te::vp::InputParams secondInputParams = mainParams->getInputParams()[1];

  std::string outputDataSetName = mainParams->getOutputDataSetName();

  const std::map<std::string, te::dt::AbstractData*> specificParams =
      mainParams->getSpecificParams();

  bool isCollection = this->isCollection(specificParams);

  te::da::DataSetType* firstDataSetType = firstInputParams.m_inputDataSetType;

  te::gm::GeometryProperty* firstGeometryProperty =
      te::da::GetFirstGeomProperty(firstDataSetType);

  te::da::DataSetType* secondDataSetType = secondInputParams.m_inputDataSetType;

  te::gm::GeometryProperty* secondGeometryProperty =
      te::da::GetFirstGeomProperty(secondDataSetType);

  std::vector<te::dt::Property*> firstSelectedProperties =
      getFirstSelectedProperties(firstDataSetType, specificParams);

  std::vector<te::dt::Property*> secondSelectedProperties =
      getSecondSelectedProperties(secondDataSetType, specificParams);

  te::da::DataSetType* outputDataSetType =
      new te::da::DataSetType(outputDataSetName);

  std::vector<std::string> attrNamesAux;

  if(mainParams->getOutputDataSource()->getType() != "OGR")
  {
    te::dt::SimpleProperty* pkProperty = new te::dt::SimpleProperty(
        outputDataSetName + "_id", te::dt::INT32_TYPE);
    pkProperty->setAutoNumber(true);
    outputDataSetType->add(pkProperty);

    te::da::PrimaryKey* pk =
        new te::da::PrimaryKey(outputDataSetName + "_pk", outputDataSetType);
    pk->add(pkProperty);
    outputDataSetType->setPrimaryKey(pk);
  }

  for(std::size_t i = 0; i < firstSelectedProperties.size(); ++i)
  {
    te::dt::Property* clonedProperty = firstSelectedProperties[i]->clone();

    te::dt::SimpleProperty* spAux =
        dynamic_cast<te::dt::SimpleProperty*>(clonedProperty);

    if(spAux)
    {
      spAux->setRequired(false);
    }

    std::string pName = clonedProperty->getName();

    if(mainParams->getOutputDataSource()->getType() == "OGR")
      pName = GetDistinctName(pName, attrNamesAux, 10);
    else
      pName = GetDistinctName(pName, attrNamesAux);

    m_firstAttrNameMap[pName] = clonedProperty->getName();

    clonedProperty->setName(pName);

    outputDataSetType->add(clonedProperty);

    attrNamesAux.push_back(clonedProperty->getName());
  }

  for(std::size_t i = 0; i < secondSelectedProperties.size(); ++i)
  {
    te::dt::Property* clonedProperty = secondSelectedProperties[i]->clone();

    te::dt::SimpleProperty* spAux =
        dynamic_cast<te::dt::SimpleProperty*>(clonedProperty);

    if(spAux)
    {
      spAux->setRequired(false);
    }

    std::string pName = clonedProperty->getName();

    if(mainParams->getOutputDataSource()->getType() == "OGR")
      pName = GetDistinctName(pName, attrNamesAux, 10);
    else
      pName = GetDistinctName(pName, attrNamesAux);

    m_secondAttrNameMap[pName] = clonedProperty->getName();

    clonedProperty->setName(pName);

    outputDataSetType->add(clonedProperty);

    attrNamesAux.push_back(clonedProperty->getName());
  }

  te::gm::GeometryProperty* newGeometryProperty =
      new te::gm::GeometryProperty("geom");

  te::gm::GeomType type = getGeomResultType(
      firstGeometryProperty->getGeometryType(),
      secondGeometryProperty->getGeometryType(), isCollection);
  newGeometryProperty->setGeometryType(type);

  newGeometryProperty->setSRID(firstGeometryProperty->getSRID());

  outputDataSetType->add(newGeometryProperty);

  return outputDataSetType;
}

te::gm::GeomType te::vp::Union::getGeomResultType(
    const te::gm::GeomType& firstGeom, const te::gm::GeomType& secondGeom,
    const bool& isCollection)
{
  te::gm::GeomType geomType;

  if ((firstGeom == te::gm::PointType) ||
    (firstGeom == te::gm::PointZType) ||
    (firstGeom == te::gm::PointMType) ||
    (firstGeom == te::gm::PointZMType) ||
    (firstGeom == te::gm::PointKdType) ||

    (secondGeom == te::gm::PointType) ||
    (secondGeom == te::gm::PointZType) ||
    (secondGeom == te::gm::PointMType) ||
    (secondGeom == te::gm::PointZMType) ||
    (secondGeom == te::gm::PointKdType) ||

    (firstGeom == te::gm::MultiPointType) ||
    (firstGeom == te::gm::MultiPointZType) ||
    (firstGeom == te::gm::MultiPointMType) ||
    (firstGeom == te::gm::MultiPointZMType) ||

    (secondGeom == te::gm::MultiPointType) ||
    (secondGeom == te::gm::MultiPointZType) ||
    (secondGeom == te::gm::MultiPointMType) ||
    (secondGeom == te::gm::MultiPointZMType))
  {
    geomType = te::gm::MultiPointType;
  }
  else if ((firstGeom == te::gm::LineStringType) ||
    (firstGeom == te::gm::LineStringZType) ||
    (firstGeom == te::gm::LineStringMType) ||
    (firstGeom == te::gm::LineStringZMType) ||

    (secondGeom == te::gm::LineStringType) ||
    (secondGeom == te::gm::LineStringZType) ||
    (secondGeom == te::gm::LineStringMType) ||
    (secondGeom == te::gm::LineStringZMType) ||

    (firstGeom == te::gm::MultiLineStringType) ||
    (firstGeom == te::gm::MultiLineStringZType) ||
    (firstGeom == te::gm::MultiLineStringMType) ||
    (firstGeom == te::gm::MultiLineStringZMType) ||

    (secondGeom == te::gm::MultiLineStringType) ||
    (secondGeom == te::gm::MultiLineStringZType) ||
    (secondGeom == te::gm::MultiLineStringMType) ||
    (secondGeom == te::gm::MultiLineStringZMType))
  {
    geomType = te::gm::MultiLineStringType;
  }
  else
    geomType = te::gm::MultiPolygonType;

  if (isCollection)
  {
    if (te::vp::IsMultiType(geomType))
      return geomType;
    else
      return te::vp::GetMultiType(geomType);
  }
  else
  {
    if (te::vp::IsMultiType(geomType))
      return te::vp::GetSimpleType(geomType);
    else
      return geomType;
  }
}

std::vector<te::dt::Property*> te::vp::Union::getFirstSelectedProperties(
    const te::da::DataSetType* dataSetType,
    const std::map<std::string, te::dt::AbstractData*>& specificParams)
{
  std::vector<te::dt::Property*> result;

  std::vector<std::pair<std::string, std::string> > selectedProperties =
      getProperties(specificParams);

  for(std::size_t i = 0; i < selectedProperties.size(); ++i)
  {
    if(selectedProperties[i].first == "FIRST")
    {
      result.push_back(dataSetType->getProperty(selectedProperties[i].second));
    }
  }

  return result;
}

std::vector<te::dt::Property*> te::vp::Union::getSecondSelectedProperties(
    const te::da::DataSetType* dataSetType,
    const std::map<std::string, te::dt::AbstractData*>& specificParams)
{
  std::vector<te::dt::Property*> result;

  std::vector<std::pair<std::string, std::string> > selectedProperties =
      getProperties(specificParams);

  for(std::size_t i = 0; i < selectedProperties.size(); ++i)
  {
    if(selectedProperties[i].first == "SECOND")
    {
      result.push_back(dataSetType->getProperty(selectedProperties[i].second));
    }
  }

  return result;
}

te::gm::GeomType te::vp::Union::setGeomResultType(
    const te::gm::GeomType& geomType, const bool& isCollection)
{
  if(isCollection)
  {
    if(te::vp::IsMultiType(geomType))
      return geomType;
    else
      return te::vp::GetMultiType(geomType);
  }
  else
  {
    if(te::vp::IsMultiType(geomType))
      return te::vp::GetSimpleType(geomType);
    else
      return geomType;
  }
}

te::gm::Geometry* te::vp::Union::setGeomAsMulti(te::gm::Geometry* geom)
{
  // note that this has effect only in Points, Lines and Polygons
  switch(geom->getGeomTypeId())
  {
    case te::gm::PointType:
    {
      te::gm::MultiPoint* geomColl =
          new te::gm::MultiPoint(0, te::gm::MultiPointType, geom->getSRID());
      geomColl->add(geom);

      return geomColl;
    }
    case te::gm::LineStringType:
    {
      te::gm::MultiLineString* geomColl = new te::gm::MultiLineString(
          0, te::gm::MultiLineStringType, geom->getSRID());
      geomColl->add(geom);

      return geomColl;
    }
    case te::gm::PolygonType:
    {
      te::gm::MultiPolygon* geomColl = new te::gm::MultiPolygon(
          0, te::gm::MultiPolygonType, geom->getSRID());
      geomColl->add(geom);

      return geomColl;
    }
    default:
      return geom;
  }
}
