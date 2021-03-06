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
  \file BufferMemory.h

  \brief Buffer Vector Processing functions.
*/

//Terralib

#include "../BuildConfig.h"
#include "../common/progress/TaskProgress.h"
#include "../core/logger/Logger.h"
#include "../core/translator/Translator.h"

#include "../dataaccess/dataset/DataSet.h"
#include "../dataaccess/dataset/DataSetAdapter.h"
#include "../dataaccess/utils/Utils.h"
#include "../datatype/Property.h"
#include "../datatype/SimpleProperty.h"
#include "../datatype/StringProperty.h"

#include "../geometry/Geometry.h"
#include "../geometry/GeometryCollection.h"
#include "../geometry/GeometryProperty.h"
#include "../geometry/Utils.h"

#include "../memory/DataSet.h"
#include "../memory/DataSetItem.h"

#include "../sam/rtree.h"

#include "BufferMemory.h"
#include "Config.h"
#include "Exception.h"
#include "Utils.h"

// STL
#include <map>
#include <string>
#include <vector>

// BOOST
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

te::vp::BufferMemory::BufferMemory()
{}

te::vp::BufferMemory::~BufferMemory()
{}

bool te::vp::BufferMemory::run() throw(te::common::Exception)
{
  std::unique_ptr<te::da::DataSetType> outDSType(GetDataSetType());
  std::unique_ptr<te::mem::DataSet> outDSet(new te::mem::DataSet(outDSType.get()));
  te::gm::GeometryProperty* geomProp = te::da::GetFirstGeomProperty(outDSType.get());

  int type;
  int pk = 0;

  std::unique_ptr<te::da::DataSet> inDsetSrc;
  
  if(m_oidSet == 0)
    inDsetSrc = m_inDsrc->getDataSet(m_inDsetName);
  else
    inDsetSrc = m_inDsrc->getDataSet(m_inDsetName, m_oidSet);
  
  std::unique_ptr<te::da::DataSetAdapter> inDset(te::da::CreateAdapter(inDsetSrc.get(), m_converter.get()));

  inDset->moveBeforeFirst();

  while(inDset->moveNext())
  {
    te::gm::Geometry* auxGeom = 0;

    for(int level = 1; level <= m_levels; ++level)
    {
      te::mem::DataSetItem* dataSetItem = new te::mem::DataSetItem(outDSet.get());

      for(std::size_t property = 0; property < inDset->getNumProperties(); ++property)
      {
        if(inDset->isNull(property))
          continue;

        type = inDset->getPropertyDataType(property);
        if(m_copyInputColumns)
        {
          switch (type)
          {
            case te::dt::INT32_TYPE:
              if(inDset->getPropertyName(property) != "FID")
                dataSetItem->setInt32(property+2, inDset->getInt32(property));
              break;
            case te::dt::INT64_TYPE:
              dataSetItem->setInt64(property+2, inDset->getInt64(property));
              break;
            case te::dt::DOUBLE_TYPE:
              dataSetItem->setDouble(property+2, inDset->getDouble(property));
              break;
            case te::dt::STRING_TYPE:
              dataSetItem->setString(property+2, inDset->getString(property));
              break;
            case te::dt::GEOMETRY_TYPE:
              {
                std::unique_ptr<te::gm::Geometry> currentGeom = inDset->getGeometry(property);
                std::unique_ptr<te::gm::Geometry> outGeom;

                if (!currentGeom->isValid())
                {
#ifdef TERRALIB_LOGGER_ENABLED
                    TE_CORE_LOG_DEBUG("vp", "Buffer - Invalid geometry found");
#endif //TERRALIB_LOGGER_ENABLED

                  continue;
                }

                dataSetItem->setInt32(0, pk); //pk
                dataSetItem->setInt32(1, level); //level

                if (m_attributePosition >= 0 && m_attributePosition < (int)inDset->getNumProperties())
                {
                  double distance = getDistanceByAttribute(*inDset.get(), m_attributePosition);
                  dataSetItem->setDouble(2, distance*(level)); //distance

                  if(distance != 0) //The distance can be negative.
                    outGeom.reset(setBuffer(currentGeom.get(), m_distance, level, auxGeom));

                  //TO DO: EMITIR MSG PARA USUARIO AVISANDO QUE HÁ VALORES NULOS OU ZERADOS.
                }
                else
                {
                  dataSetItem->setDouble(2, m_distance*(level)); //distance
                  outGeom.reset(setBuffer(currentGeom.get(), m_distance, level, auxGeom));
                }

                if(outGeom.get() && outGeom->isValid())
                {
                  outGeom->setSRID(geomProp->getSRID());

                  if(outGeom->getGeomTypeId() == te::gm::MultiPolygonType)
                  {
                    dataSetItem->setGeometry(property+2, outGeom.release());
                  }
                  else
                  {
                    std::unique_ptr<te::gm::GeometryCollection> mPolygon(new te::gm::GeometryCollection(0, te::gm::MultiPolygonType, outGeom->getSRID()));
                    mPolygon->add(outGeom.release());
                    dataSetItem->setGeometry(property+2, mPolygon.release());
                  }

                  outDSet->add(dataSetItem);
                  ++pk;
                }
              }
              break;
            default:
              {
#ifdef TERRALIB_LOGGER_ENABLED
                TE_CORE_LOG_DEBUG("vp", "Buffer - The type of input layer attribute (" + inDset->getPropertyName(property) +") was not found.");
#endif //TERRALIB_LOGGER_ENABLED
              }
          }
        }
        else
        {
          if(type == te::dt::GEOMETRY_TYPE)
          {
            std::unique_ptr<te::gm::Geometry> currentGeom = inDset->getGeometry(property);
            std::unique_ptr<te::gm::Geometry> outGeom;

            if (!currentGeom->isValid())
            {
#ifdef TERRALIB_LOGGER_ENABLED
                TE_CORE_LOG_DEBUG("vp", "Buffer - Invalid geometry found");
#endif //TERRALIB_LOGGER_ENABLED

              continue;
            }

            dataSetItem->setInt32(0, pk); //pk
            dataSetItem->setInt32(1, level); //level

            if (m_attributePosition >= 0 && m_attributePosition < (int)inDset->getNumProperties())
            {
              double distance = getDistanceByAttribute(*inDset.get(), m_attributePosition);
              dataSetItem->setDouble(2, distance*(level)); //distance

              if(distance != 0) //The distance can be negative.
                outGeom.reset(setBuffer(currentGeom.get(), distance, level, auxGeom));

              //TO DO: EMITIR MSG PARA USUARIO AVISANDO QUE HÁ VALORES NULOS OU ZERADOS.
            }
            else
            {
              dataSetItem->setDouble(2, m_distance*(level)); //distance
              outGeom.reset(setBuffer(currentGeom.get(), m_distance, level, auxGeom));
            }


            if(outGeom.get() && outGeom->isValid())
            {
              outGeom->setSRID(geomProp->getSRID());

              if(outGeom->getGeomTypeId() == te::gm::MultiPolygonType)
              {
                dataSetItem->setGeometry(3, outGeom.release());
              }
              else
              {
                std::unique_ptr<te::gm::GeometryCollection> mPolygon(new te::gm::GeometryCollection(0, te::gm::MultiPolygonType, outGeom->getSRID()));
                mPolygon->add(outGeom.release());
                dataSetItem->setGeometry(3, mPolygon.release());
              }

              outDSet->add(dataSetItem);
              ++pk;
            }
          }
        }
      }
    }
  }
  
  if(m_bufferBoundariesRule == te::vp::DISSOLVE)
  {
    dissolveMemory(outDSet.get(), m_levels);
  }

  te::vp::Save(m_outDsrc.get(), outDSet.get(), outDSType.get());
  return true;
}


te::gm::Geometry* te::vp::BufferMemory::setBuffer(te::gm::Geometry* geom,  
                                                  const double& distance, 
                                                  const int& level,
                                                  te::gm::Geometry*& auxGeom)
{
  te::gm::Geometry* geomResult = 0;
  te::gm::Geometry* geomTemp = 0;
  std::unique_ptr<te::gm::Geometry> outGeom;
  std::unique_ptr<te::gm::Geometry> inGeom;
  switch(m_bufferPolygonRule)
  {
    case (te::vp::INSIDE_OUTSIDE):
      {
        outGeom.reset(geom->buffer(distance * level, 16, te::gm::CapButtType));
        inGeom.reset(geom->buffer(-distance * level, 16, te::gm::CapButtType));
        geomResult = outGeom->difference(inGeom.get());

        geomTemp = (te::gm::Geometry*)geomResult->clone();
        if (auxGeom && auxGeom->isValid())
          geomResult = geomResult->difference(auxGeom);

        delete auxGeom;
        auxGeom = geomTemp;
      }
      break;
    
    case (te::vp::ONLY_OUTSIDE):
      {
        outGeom.reset(geom->buffer(distance * level, 16, te::gm::CapButtType));
        geomResult = outGeom->difference(geom);

        geomTemp = (te::gm::Geometry*)geomResult->clone();
        if (auxGeom && auxGeom->isValid())
          geomResult = geomResult->difference(auxGeom);

        delete auxGeom;
        auxGeom = geomTemp;
      }
      break;
    
    case (te::vp::ONLY_INSIDE):
      {
        inGeom.reset(geom->buffer(-distance * level, 16, te::gm::CapButtType));
        geomResult = geom->difference(inGeom.get());

        geomTemp = (te::gm::Geometry*)geomResult->clone();
        if (auxGeom && auxGeom->isValid())
          geomResult = geomResult->difference(auxGeom);

        delete auxGeom;
        auxGeom = geomTemp;
      }
      break;

    default:
    {
#ifdef TERRALIB_LOGGER_ENABLED
      TE_CORE_LOG_DEBUG("vp", "Buffer - Polygon rule not found.");
#endif //TERRALIB_LOGGER_ENABLED
    }
  }
  return geomResult;
}

void te::vp::BufferMemory::dissolveMemory(te::mem::DataSet* outDSet,
                                          const int& levels)
{
  std::vector<std::vector<te::gm::Geometry*> > vecGeom;
    
  int levelPos = (int)te::da::GetPropertyPos(outDSet, "level");
  int geomPos  = (int)te::da::GetPropertyPos(outDSet, "geom");
  int level;

  //te::common::TaskProgress task1("Dissolving boundaries...");
  //task1.setTotalSteps(levels*outDSet->size());
  //task1.setCurrentStep(1);
  for(int i = 1; i <= levels; ++i)
  {
    te::sam::rtree::Index<te::gm::Geometry*, 4> rtree;

    outDSet->moveBeforeFirst();
    while(outDSet->moveNext())
    {
      level = outDSet->getInt32(levelPos);
      if(level == i)
      {
        te::gm::Geometry* geom = outDSet->getGeometry(geomPos).release();
          
        std::vector<te::gm::Geometry*> vec;
          
        rtree.search(*(geom->getMBR()), vec);
          
        if(!vec.empty())
        {
          for(std::size_t t = 0; t < vec.size(); ++t)
          {
            if(geom->intersects(vec[t]))
            {
              geom = geom->Union(vec[t]);
              rtree.remove(*(vec[t]->getMBR()), vec[t]);
            }
          }
        }
        rtree.insert(*(geom->getMBR()), geom);
      }
      //task1.pulse();
    }
      
    std::vector<te::gm::Geometry*> geomVec;
    std::unique_ptr<te::gm::Envelope> e = outDSet->getExtent(geomPos);
    rtree.search(*(e.get()), geomVec);

    vecGeom.push_back(geomVec);

    rtree.clear();
  }
    
  outDSet->clear();
  outDSet->moveBeforeFirst();

  
  int pk = 0;
  std::size_t vecSize = vecGeom.size();
  
  //task1.setTotalSteps(vecSize);
  //task1.setCurrentStep(1);
  if(levels > 1)
  {
    for(std::size_t i = vecSize - 1; i > 0; --i)
    {
      std::vector<te::gm::Geometry*> currentVec = vecGeom[i];
      std::size_t c_vecSize = currentVec.size();

      for(std::size_t j = 0; j < i; ++j)
      {
        std::vector<te::gm::Geometry*> innerVec = vecGeom[j];
        std::size_t i_vecSize = innerVec.size();

        for(std::size_t k = 0; k < c_vecSize; ++k)
        {
          for(std::size_t l = 0; l < i_vecSize; ++l)
          {
            te::gm::Geometry* k_geom = currentVec[k];
            te::gm::Geometry* l_geom = innerVec[l];

            if(k_geom->intersects(l_geom))
            {
              te::gm::Geometry* tGeom = k_geom->difference(l_geom);
              if(tGeom->isValid())
              {
                delete currentVec[k];
                currentVec[k] = tGeom;
                vecGeom[i] = currentVec;
              }
            }
          }
        }
      }
      //task1.pulse();
    }
  }

  //task1.setTotalSteps(vecSize);
  //task1.setCurrentStep(1);
  for (std::size_t i = 0; i < vecSize; ++i)
  {
    std::vector<te::gm::Geometry*> currentVec = vecGeom[i];
    int c_vecSize = (int)currentVec.size();

    for(int j = 0; j < c_vecSize; ++j)
    {
      te::mem::DataSetItem* dataSetItem = new te::mem::DataSetItem(outDSet);
      dataSetItem->setInt32(0, pk); //pk
      dataSetItem->setInt32(1, (int)i+1); //level

      te::da::DataSetType* dsType = m_converter->getConvertee();

      if(m_attributePosition >= 0 && m_attributePosition < (int)dsType->getProperties().size())
        dataSetItem->setDouble(2, 0); //the dissolve in buffer by attribute can use buffers with differents distances.
      else
        dataSetItem->setDouble(2, m_distance*((int)i+1)); //fixed distance.
        
      if(currentVec[j]->getGeomTypeId() == te::gm::MultiPolygonType)
      {
        dataSetItem->setGeometry(3, currentVec[j]);
      }
      else
      {
        std::unique_ptr<te::gm::GeometryCollection> mPolygon(new te::gm::GeometryCollection(0, te::gm::MultiPolygonType, currentVec[j]->getSRID()));
        te::gm::GeometryCollection* gcIn = dynamic_cast<te::gm::GeometryCollection*>(currentVec[j]);
        if(gcIn == 0)
          mPolygon->add(currentVec[j]);
        else
          te::vp::SplitGeometryCollection(gcIn, mPolygon.get());

        dataSetItem->setGeometry(3, mPolygon.release());
      }
        
      outDSet->add(dataSetItem);
      ++pk;
    }
    //task1.pulse();
  }
}

double te::vp::BufferMemory::getDistanceByAttribute(te::da::DataSet& dataSet,
                                                    const int& position)
{
  double distance = 0;

  if(dataSet.isNull(position))
    return distance;

  int type = dataSet.getPropertyDataType(position);

  switch (type) {
  case te::dt::INT16_TYPE:
  case te::dt::CINT16_TYPE:
  case te::dt::UINT16_TYPE:
    distance = (double)dataSet.getInt16(position);
    break;
  case te::dt::INT32_TYPE:
  case te::dt::CINT32_TYPE:
  case te::dt::UINT32_TYPE:
    distance = (double)dataSet.getInt32(position);
    break;
  case te::dt::INT64_TYPE:
  case te::dt::UINT64_TYPE:
    distance = (double)dataSet.getInt64(position);
    break;
  case te::dt::FLOAT_TYPE:
  case te::dt::CFLOAT_TYPE:
    distance = (double)dataSet.getFloat(position);
    break;
  case te::dt::DOUBLE_TYPE:
  case te::dt::CDOUBLE_TYPE:
    distance = dataSet.getDouble(position);
    break;
  default:
    te::common::Exception(TE_TR("The chosen attribute has not a valid type to execute the buffer operation!"));
    break;
  }
  return distance;
}
