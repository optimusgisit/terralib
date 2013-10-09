/*  Copyright (C) 2008-2011 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/vp/qt/Utils.cpp
   
  \brief Utility functions for Vector Processing.
*/

// TerraLib
#include "../dataaccess/datasource/DataSourceInfo.h"
#include "../dataaccess/datasource/DataSourceManager.h"
#include "../dataaccess/utils/Utils.h"
#include "../geometry/Geometry.h"
#include "../geometry/GeometryCollection.h"
#include "../geometry/GeometryProperty.h"
#include "../geometry/MultiPoint.h"
#include "../geometry/Point.h"
#include "../memory/DataSet.h"
#include "Utils.h"

// Qt
#include <QtGui/QTreeView>

//STL
#include <vector>
#include <memory>

// Boost
#include <boost/algorithm/string.hpp>

te::gm::Geometry* te::vp::GetGeometryUnion(const std::vector<te::mem::DataSetItem*>& items, size_t geomIdx, te::gm::GeomType outGeoType)
{
  te::gm::Geometry* resultGeometry = 0; 

  te::gm::Geometry* seedGeometry = items[0]->getGeometry(geomIdx);
  
  if(items.size() < 2)
  {
      resultGeometry = seedGeometry;
  }
  if(items.size() == 2)
  {
    te::gm::Geometry* teGeom = items[1]->getGeometry(geomIdx);

    if(items[1]->getGeometry(geomIdx)->isValid())
      resultGeometry = seedGeometry->Union(teGeom);
    else
      resultGeometry = seedGeometry;
  }
  if(items.size() > 2)
  {
    te::gm::GeometryCollection* teGeomColl = new te::gm::GeometryCollection(0, te::gm::GeometryCollectionType, seedGeometry->getSRID());

    for(std::size_t i = 1; i < items.size(); ++i)
    {
      if(items[i]->getGeometry(geomIdx)->isValid())
        teGeomColl->add(items[i]->getGeometry(geomIdx));
    }

    resultGeometry = seedGeometry->Union(teGeomColl);
  }

  if (resultGeometry->getGeomTypeId() != outGeoType)
  {
    te::gm::GeometryCollection* gc = new te::gm::GeometryCollection(1,outGeoType,resultGeometry->getSRID());
    gc->setGeometryN(0,resultGeometry);
    return gc;
  }
  else
    return resultGeometry;
}

std::string te::vp::GetSimpleTableName(std::string fullName)
{
  std::string key (".");
  std::size_t found = fullName.rfind(key);
  
  if(found >= std::string::npos)
    return fullName;

  std::string tableName = fullName.substr(found + 1);

  return tableName;
}

te::gm::GeomType te::vp::GeomOpResultType(te::gm::GeomType firstGeom, te::gm::GeomType secondGeom)
{
  if( (firstGeom == te::gm::PolygonType && secondGeom == te::gm::PolygonType) ||
      (firstGeom == te::gm::MultiPolygonType &&  secondGeom == te::gm::MultiPolygonType) ||
      (firstGeom == te::gm::PolygonType && secondGeom == te::gm::MultiPolygonType) ||
      (firstGeom == te::gm::MultiPolygonType && secondGeom == te::gm::PolygonType))
    return te::gm::MultiPolygonType;
  
  if((firstGeom == te::gm::PolygonType && secondGeom == te::gm::LineStringType)||
          (firstGeom == te::gm::PolygonType && secondGeom == te::gm::MultiLineStringType)||
          (firstGeom == te::gm::MultiPolygonType && secondGeom == te::gm::LineStringType)||
          (firstGeom == te::gm::MultiPolygonType && secondGeom == te::gm::MultiLineStringType)||

          (firstGeom == te::gm::LineStringType && secondGeom == te::gm::PolygonType)||
          (firstGeom == te::gm::LineStringType && secondGeom == te::gm::MultiPolygonType)||
          (firstGeom == te::gm::MultiLineStringType && secondGeom == te::gm::PolygonType)||
          (firstGeom == te::gm::MultiLineStringType && secondGeom == te::gm::MultiPolygonType)||

          (firstGeom == te::gm::LineStringType && secondGeom == te::gm::LineStringType) ||
          (firstGeom == te::gm::MultiLineStringType && secondGeom == te::gm::MultiLineStringType) ||
          (firstGeom == te::gm::LineStringType && secondGeom == te::gm::MultiLineStringType) ||
          (firstGeom == te::gm::MultiLineStringType && secondGeom == te::gm::LineStringType))
    return te::gm::MultiLineStringType;

  return te::gm::MultiPointType;
}

te::gm::GeomType te::vp::GeomOpResultType(te::gm::GeomType firstGeom)
{
  if (firstGeom == te::gm::PolygonType)
    return te::gm::MultiPolygonType;
  
  if (firstGeom == te::gm::LineStringType)
    return te::gm::MultiLineStringType;
  
  if (firstGeom == te::gm::PointType)
    return te::gm::MultiPointType;
  
  return firstGeom;
}