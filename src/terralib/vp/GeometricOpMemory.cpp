/*  Copyright (C) 2008-2013 National Institute For Space Research (INPE) - Brazil.

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
  \file GeographicOpMemory.h

  \brief Geographic Vector Processing functions.
*/

//Terralib

#include "../common/progress/TaskProgress.h"
#include "../common/Logger.h"
#include "../common/Translator.h"

#include "../dataaccess/dataset/DataSet.h"
#include "../dataaccess/utils/Utils.h"
#include "../datatype/Property.h"
#include "../datatype/SimpleProperty.h"
#include "../datatype/StringProperty.h"

#include "../geometry/Geometry.h"
#include "../geometry/GeometryProperty.h"
#include "../geometry/GeometryCollection.h"
#include "../geometry/LineString.h"
#include "../geometry/MultiLineString.h"
#include "../geometry/MultiPolygon.h"
#include "../geometry/Point.h"
#include "../geometry/Polygon.h"
#include "../geometry/Utils.h"

#include "../memory/DataSet.h"
#include "../memory/DataSetItem.h"

#include "GeometricOpMemory.h"
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

te::vp::GeometricOpMemory::GeometricOpMemory()
{}

te::vp::GeometricOpMemory::~GeometricOpMemory()
{}


bool te::vp::GeometricOpMemory::run()
{
  // get the geometric operation and/or tabular operation. 
  std::vector<int> opGeom;
  std::vector<int> opTab;
  std::vector<te::da::DataSetType*> dsTypeVec;

  for(std::size_t i = 0; i < m_operations.size(); ++i)
  {
    switch(m_operations[i])
    {
      case te::vp::CENTROID:
        opGeom.push_back(te::vp::CENTROID);
        break;
      case te::vp::CONVEX_HULL:
        opGeom.push_back(te::vp::CONVEX_HULL);
        break;
      case te::vp::MBR:
        opGeom.push_back(te::vp::MBR);
        break;
      case te::vp::AREA:
        opTab.push_back(te::vp::AREA);
        break;
      case te::vp::LINE:
        opTab.push_back(te::vp::LINE);
        break;
      case te::vp::PERIMETER:
        opTab.push_back(te::vp::PERIMETER);
        break;
    }
  }

  if(m_outputLayer)
  {
    bool hasMultiGeomColumns = false;
    bool result = false;

    switch(m_objStrategy)
    {
      case te::vp::ALL_OBJ:
        {
          if(hasMultiGeomColumns) //Condi��o se o DataSource suporta mais de uma coluna geometrica...
          {
            dsTypeVec[0] = te::vp::GeometricOp::GetDataSetType(te::vp::ALL_OBJ, true);
          }
          else
          {
            if(opGeom.size() > 0)
            {
              for(std::size_t i = 0; i < opGeom.size(); ++i)
                dsTypeVec.push_back(te::vp::GeometricOp::GetDataSetType(te::vp::ALL_OBJ, false, opGeom[i]));
            }
            else
            {
              dsTypeVec.push_back(te::vp::GeometricOp::GetDataSetType(te::vp::ALL_OBJ, false));
            }
          }

          for(std::size_t dsTypePos = 0; dsTypePos < dsTypeVec.size(); ++dsTypePos)
          {
            std::auto_ptr<te::da::DataSetType> outDataSetType(dsTypeVec[dsTypePos]);
            std::auto_ptr<te::mem::DataSet> outDataSet(SetAllObjects(dsTypeVec[dsTypePos], opTab, opGeom));
            result = save(outDataSet, outDataSetType);
            if(!result)
              return result;
          }
          
        }
        break;
      case te::vp::AGGREG_OBJ:
        {
          if(hasMultiGeomColumns) //Condi��o se o DataSource suporta mais de uma coluna geometrica...
          {
            dsTypeVec[0] = te::vp::GeometricOp::GetDataSetType(te::vp::AGGREG_OBJ, true);
          }
          else
          {
            for(std::size_t i = 0; i < opGeom.size(); ++i)
              dsTypeVec.push_back(te::vp::GeometricOp::GetDataSetType(te::vp::AGGREG_OBJ, false, opGeom[i]));
          }
        }
        break;
      case te::vp::AGGREG_BY_ATTRIBUTE:
        {
          if(hasMultiGeomColumns) //Condi��o se o DataSource suporta mais de uma coluna geometrica...
          {
            dsTypeVec[0] = te::vp::GeometricOp::GetDataSetType(te::vp::AGGREG_BY_ATTRIBUTE, true);
          }
          else
          {
            for(std::size_t i = 0; i < opGeom.size(); ++i)
              dsTypeVec.push_back(te::vp::GeometricOp::GetDataSetType(te::vp::AGGREG_BY_ATTRIBUTE, false, opGeom[i]));
          }
        }
        break;
    }

    return result;
  }
  else
  {
    //Descobrir se o DataSource suporta adi��o de mais de uma coluna geometrica.
    return false;
  }
}


te::mem::DataSet* te::vp::GeometricOpMemory::SetAllObjects( te::da::DataSetType* dsType,
                                                            std::vector<int> tabVec,
                                                            std::vector<int> geoVec)
{
  std::auto_ptr<te::mem::DataSet> outDSet(new te::mem::DataSet(dsType));

  int pk = 0;
  m_inDset->moveBeforeFirst();
  std::size_t geom_pos = te::da::GetFirstSpatialPropertyPos(m_inDset.get());

  while(m_inDset->moveNext())
  {
    te::mem::DataSetItem* item = new te::mem::DataSetItem(outDSet.get());
     
    item->setInt32(0, pk);

    if(m_selectedProps.size() > 0)
    {
      for(std::size_t prop_pos = 0; prop_pos < m_selectedProps.size(); ++prop_pos)
      {
        item->setValue(m_selectedProps[prop_pos], m_inDset->getValue(m_selectedProps[prop_pos]).get());
      }
    }

    std::auto_ptr<te::gm::Geometry> in_geom = m_inDset->getGeometry(geom_pos);

    if(tabVec.size() > 0)
    {
      for(std::size_t tabPos = 0; tabPos < tabVec.size(); ++tabPos)
      {
        switch(tabVec[tabPos])
        {
          case te::vp::AREA:
            {
              double area = 0;
              area = CalculateTabularOp(tabVec[tabPos], in_geom.get());
              item->setDouble("area", area);
            }
            break;
          case te::vp::LINE:
            {
              double line = 0;
              line = CalculateTabularOp(tabVec[tabPos], in_geom.get());
              item->setDouble("line_length", line);
            }
            break;
          case te::vp::PERIMETER:
            {
              double perimeter = 0;
              perimeter = CalculateTabularOp(tabVec[tabPos], in_geom.get());
              item->setDouble("perimeter", perimeter);
            }
            break;
        }
      }
    }

    if(geoVec.size() > 0)
    {
      for(std::size_t geoPos = 0; geoPos < geoVec.size(); ++geoPos)
      {
        switch(geoVec[geoPos])
        {
          case te::vp::CONVEX_HULL:
            {
              std::size_t pos = te::da::GetPropertyPos(dsType, "convex_hull");
              if(pos < dsType->size())
                item->setGeometry("convex_hull", in_geom->convexHull());
            }
            break;
          case te::vp::CENTROID:
            {
              std::size_t pos = te::da::GetPropertyPos(dsType, "centroid");
              if(pos < dsType->size())
              {
                const te::gm::Envelope* env = in_geom->getMBR();
                te::gm::Coord2D center = env->getCenter();
                te::gm::Point* point = new te::gm::Point(center.x, center.y, in_geom->getSRID());
                item->setGeometry("centroid", point);
              }
            }
            break;
          case te::vp::MBR:
            {
              std::size_t pos = te::da::GetPropertyPos(dsType, "mbr");
              if(pos < dsType->size())
                item->setGeometry("mbr", in_geom->getEnvelope());
            }
            break;
        }
      }
    }
    else
    {
      std::auto_ptr<te::gm::Geometry> g(in_geom.release());
      te::gm::GeometryCollection* teGeomColl = new te::gm::GeometryCollection(0, te::gm::GeometryCollectionType, g->getSRID());

      switch(g->getGeomTypeId())
      {
        case te::gm::PointType:
          {
            if(g->isValid())
              teGeomColl->add(g.release());
          }
          break;
        case te::gm::LineStringType:
          {
            if(g->isValid())
              teGeomColl->add(g.release());
          }
          break;
        case te::gm::PolygonType:
          {
            if(g->isValid())
              teGeomColl->add(g.release());
          }
          break;
      }
      if(teGeomColl->getNumGeometries() != 0)
        item->setGeometry("geom", teGeomColl);
      else
        item->setGeometry("geom", g.release());
    }

    outDSet->add(item);

    ++pk;
  }

  return outDSet.release();
}


double te::vp::GeometricOpMemory::CalculateTabularOp(int tabOperation, 
                                                    te::gm::Geometry* geom)
{
  double value = 0;
  te::gm::GeomType geomType = geom->getGeomTypeId();

  switch(tabOperation)
  {
    case te::vp::AREA:
      {
        if(geomType == te::gm::PolygonType)
        {
          te::gm::Polygon* pol = dynamic_cast<te::gm::Polygon*>(geom);
          if(pol)
            value = pol->getArea();
        }
        if(geomType == te::gm::MultiPolygonType)
        {
          te::gm::MultiPolygon* m_pol = dynamic_cast<te::gm::MultiPolygon*>(geom);
          if(m_pol)
            value = m_pol->getArea();
        }
        return value;
      }
      break;
    case te::vp::LINE:
      {
        if(geomType == te::gm::LineStringType)
        {
          te::gm::LineString* lineString = dynamic_cast<te::gm::LineString*>(geom);
          if(lineString)
            value = lineString->getLength();
        }
        if(geomType == te::gm::MultiLineStringType)
        {
          te::gm::MultiLineString* m_lineString = dynamic_cast<te::gm::MultiLineString*>(geom);
          if(m_lineString)
            value = m_lineString->getLength();
        }
        return value;
      }
      break;
    case te::vp::PERIMETER:
      {
        if(geomType == te::gm::PolygonType)
        {
          te::gm::Polygon* pol = dynamic_cast<te::gm::Polygon*>(geom);
          if(pol)
            value = pol->getPerimeter();
        }
        if(geomType == te::gm::MultiPolygonType)
        {
          te::gm::MultiPolygon* m_pol = dynamic_cast<te::gm::MultiPolygon*>(geom);
          if(m_pol)
            value = m_pol->getPerimeter();
        }
        return value;
      }
      break;
  }
}