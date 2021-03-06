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
  \file IntersectionMemory.h

  \brief Intersection Vector Processing functions.
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
#include "../geometry/MultiLineString.h"
#include "../geometry/MultiPoint.h"
#include "../geometry/MultiPolygon.h"
#include "../geometry/Utils.h"

#include "../memory/DataSet.h"
#include "../memory/DataSetItem.h"

#include "../sam.h"

#include "IntersectionMemory.h"
#include "Config.h"
#include "Exception.h"
#include "Utils.h"

// STL
#include <map>
#include <math.h>
#include <string>
#include <vector>

// BOOST
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>


te::vp::IntersectionMemory::IntersectionMemory()
{}

te::vp::IntersectionMemory::~IntersectionMemory()
{}


bool te::vp::IntersectionMemory::run() throw(te::common::Exception)
{
  std::vector<te::dt::Property*> firstProps = getTabularProps(m_firstConverter->getResult());

  IntersectionMember firstMember;
  firstMember.dt = m_firstConverter->getResult();

  if (m_firstOidSet == 0)
  {
    firstMember.ds = te::da::CreateAdapter(m_firstDs.release(), m_firstConverter.get());
  }
  else
  {
    if (m_isFistQuery)
      firstMember.ds = te::da::CreateAdapter(m_firstDs.release(), m_firstConverter.get());
    else
      firstMember.ds = te::da::CreateAdapter(m_inFirstDsrc->getDataSet(m_inFirstDsetName, m_firstOidSet).release(), m_firstConverter.get());
  }
  
  firstMember.props = firstProps;

  IntersectionMember secondMember;
  secondMember.dt = m_secondConverter->getResult();

  if (m_secondOidSet == 0)
  {
    secondMember.ds = te::da::CreateAdapter(m_secondDs.release(), m_secondConverter.get());
  }
  else
  {
    if (m_isSecondQuery)
      secondMember.ds = te::da::CreateAdapter(m_secondDs.release(), m_firstConverter.get());
    else
      secondMember.ds = te::da::CreateAdapter(m_inSecondDsrc->getDataSet(m_inSecondDsetName, m_secondOidSet).release(), m_secondConverter.get());
  }

  secondMember.props = getTabularProps(secondMember.dt);
  
  std::pair<te::da::DataSetType*, te::da::DataSet*> resultPair;
  resultPair = this->pairwiseIntersection(m_outDsetName, firstMember, secondMember);

  if(resultPair.second->size() < 1)
    throw te::common::Exception(TE_TR("The Layers do not intersect!"));

  std::auto_ptr<te::da::DataSet> outDataSet(resultPair.second);
  std::auto_ptr<te::da::DataSetType> outDataSetType(resultPair.first);

  te::vp::Save(m_outDsrc.get(), outDataSet.get(), outDataSetType.get());
  return true;

}

std::pair<te::da::DataSetType*, te::da::DataSet*> te::vp::IntersectionMemory::pairwiseIntersection(std::string newName, 
                                                                                                  IntersectionMember firstMember, 
                                                                                                  IntersectionMember secondMember)
{

  //Creating the RTree with the secound layer geometries
  DataSetRTree rtree(new te::sam::rtree::Index<size_t, 8>);
  size_t secGeomPropPos = secondMember.dt->getPropertyPosition(secondMember.dt->findFirstPropertyOfType(te::dt::GEOMETRY_TYPE));
  te::gm::GeometryProperty* geomProp = te::da::GetFirstGeomProperty(secondMember.dt);

  size_t secondDsCount = 0;
  int sridSecond = -1;

  secondMember.ds->moveBeforeFirst();
  while(secondMember.ds->moveNext())
  {
    std::auto_ptr<te::gm::Geometry> g = secondMember.ds->getGeometry(secGeomPropPos);
    
    if(sridSecond == -1)
      sridSecond = geomProp->getSRID();

    rtree->insert(*g->getMBR(), secondDsCount);

    ++secondDsCount;
  }

  firstMember.ds->moveBeforeFirst();

  te::gm::GeometryProperty* fiGeomProp = te::da::GetFirstGeomProperty(firstMember.dt);
  size_t fiGeomPropPos = firstMember.dt->getPropertyPosition(fiGeomProp);

  // Create the DataSetType and DataSet
  te::da::DataSetType* outputDt = this->getOutputDsType();
  te::mem::DataSet* outputDs = new te::mem::DataSet(outputDt);

  std::pair<te::da::DataSetType*, te::da::DataSet*> resultPair;

  te::common::TaskProgress task("Processing intersection...");
  task.setTotalSteps((int)firstMember.ds->size());
  task.useTimer(true);

  int pk = 0;

  while(firstMember.ds->moveNext())
  {
    std::auto_ptr<te::gm::Geometry> currGeom = firstMember.ds->getGeometry(fiGeomPropPos);

    if(currGeom->getSRID() != sridSecond)
      currGeom->transform(sridSecond);

    std::vector<size_t> report;
    rtree->search(*currGeom->getMBR(), report);

    if(!report.empty())
      currGeom->transform(fiGeomProp->getSRID());

    for(size_t i = 0; i < report.size(); ++i)
    {
      secondMember.ds->move(report[i]);
      std::auto_ptr<te::gm::Geometry> secGeom = secondMember.ds->getGeometry(secGeomPropPos);
      secGeom->setSRID(sridSecond);

      if (secGeom->getSRID() != fiGeomProp->getSRID())
        secGeom->transform(fiGeomProp->getSRID());

      if(!currGeom->intersects(secGeom.get()))
        continue;

      te::mem::DataSetItem* item = new te::mem::DataSetItem(outputDs);
      std::auto_ptr<te::gm::Geometry> resultGeom;

      if (currGeom->isValid() && secGeom->isValid())
        resultGeom.reset(currGeom->intersection(secGeom.get()));
      
      if(resultGeom.get()!=0 && resultGeom->isValid())
      {
        te::gm::GeometryProperty* fiGeomProp = (te::gm::GeometryProperty*)outputDt->findFirstPropertyOfType(te::dt::GEOMETRY_TYPE);

        if(fiGeomProp->getGeometryType() == te::gm::MultiPolygonType)
        {
          if((resultGeom->getGeomTypeId() == te::gm::MultiPolygonType)
            || (resultGeom->getGeomTypeId() == te::gm::MultiPolygonMType)
            || (resultGeom->getGeomTypeId() == te::gm::MultiPolygonZMType)
            || (resultGeom->getGeomTypeId() == te::gm::MultiPolygonZType))
          {
            item->setGeometry("geom", resultGeom.release());
          }
          else if((resultGeom->getGeomTypeId() == te::gm::PolygonType)
                  || (resultGeom->getGeomTypeId() == te::gm::PolygonMType)
                  || (resultGeom->getGeomTypeId() == te::gm::PolygonZMType)
                  || (resultGeom->getGeomTypeId() == te::gm::PolygonZType))
          {
            te::gm::MultiPolygon* newGeom = new te::gm::MultiPolygon(0, te::gm::GeomType(resultGeom->getGeomTypeId()+3), resultGeom->getSRID());
            newGeom->add(resultGeom.release());
            item->setGeometry("geom", newGeom);
          }
        }
        else if(fiGeomProp->getGeometryType() == te::gm::MultiLineStringType)
        {
          if ((resultGeom->getGeomTypeId() == te::gm::MultiLineStringType)
            || (resultGeom->getGeomTypeId() == te::gm::MultiLineStringZType)
            || (resultGeom->getGeomTypeId() == te::gm::MultiLineStringMType)
            || (resultGeom->getGeomTypeId() == te::gm::MultiLineStringZMType))
          {
            item->setGeometry("geom", resultGeom.release());
          }
          else if ((resultGeom->getGeomTypeId() == te::gm::LineStringType)
                   || (resultGeom->getGeomTypeId() == te::gm::LineStringZType)
                   || (resultGeom->getGeomTypeId() == te::gm::LineStringMType)
                   || (resultGeom->getGeomTypeId() == te::gm::LineStringZMType))
          {
            te::gm::MultiLineString* newGeom = new te::gm::MultiLineString(0, te::gm::GeomType(resultGeom->getGeomTypeId()+3), resultGeom->getSRID());
            newGeom->add(resultGeom.release());
            item->setGeometry("geom", newGeom);
          }
        }
        else if(fiGeomProp->getGeometryType() == te::gm::MultiPointType)
        {
          if((resultGeom->getGeomTypeId() == te::gm::MultiPointType)
             || (resultGeom->getGeomTypeId() == te::gm::MultiPointMType)
             || (resultGeom->getGeomTypeId() == te::gm::MultiPointZMType)
             || (resultGeom->getGeomTypeId() == te::gm::MultiPointZType))
          {
            item->setGeometry("geom", resultGeom.release());
          }
          else if((resultGeom->getGeomTypeId() == te::gm::PointType)
                  || (resultGeom->getGeomTypeId() == te::gm::PointKdType)
                  || (resultGeom->getGeomTypeId() == te::gm::PointMType)
                  || (resultGeom->getGeomTypeId() == te::gm::PointZMType)
                  || (resultGeom->getGeomTypeId() == te::gm::PointZType))
          {
            te::gm::MultiPoint* newGeom = new te::gm::MultiPoint(0, te::gm::GeomType(resultGeom->getGeomTypeId()+3), resultGeom->getSRID());
            newGeom->add(resultGeom.release());
            item->setGeometry("geom", newGeom);
          }
        }
      }
      else
      {
#ifdef TERRALIB_LOGGER_ENABLED
        TE_CORE_LOG_DEBUG("vp", "Intersection - Invalid geometry found");
#endif //TERRALIB_LOGGER_ENABLED
        continue;
      }

      for(size_t j = 0; j < firstMember.props.size(); ++j)
      {
        std::string name = firstMember.props[j]->getName();

        std::size_t inputPropPos = firstMember.dt->getPropertyPosition(name);

        if (!m_inFirstDsetName.empty())
          name = te::vp::GetSimpleTableName(m_inFirstDsetName) + "_" + name;

        std::size_t outputPropPos = outputDt->getPropertyPosition(name);

        if (outputPropPos >= outputDt->size())
          continue;

        if (!firstMember.ds->isNull(inputPropPos))
        {
          te::dt::AbstractData* ad = firstMember.ds->getValue(firstMember.props[j]->getName()).release();
          item->setValue(name, ad);
        }
      }

      for(size_t j = 0; j < secondMember.props.size(); ++j)
      {
        std::string name = secondMember.props[j]->getName();

        std::size_t inputPropPos = secondMember.dt->getPropertyPosition(name);

        if (!m_inSecondDsetName.empty())
          name = te::vp::GetSimpleTableName(m_inSecondDsetName) + "_" + name;

        std::size_t outputPropPos = outputDt->getPropertyPosition(name);

        if (outputPropPos >= outputDt->size())
          continue;

        if (!secondMember.ds->isNull(inputPropPos))
        {
          te::dt::AbstractData* ad = secondMember.ds->getValue(secondMember.props[j]->getName()).release();
          item->setValue(name, ad);
        }
      }

      item->setInt32(newName + "_id", pk);
      ++pk;

      outputDs->moveNext();

      std::size_t aux = te::da::GetFirstSpatialPropertyPos(outputDs);

      if(!item->isNull(aux))
        outputDs->add(item);
    }

    if(task.isActive() == false)
    {
      delete outputDt;
      delete outputDs;

      throw te::common::Exception(TE_TR("Operation canceled!"));
    }

    task.pulse();
  }

  outputDs->moveBeforeFirst();

  resultPair.first = outputDt;
  resultPair.second = outputDs;
  return resultPair;
}
