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
  \file LineToPolygonMemory.h

  \brief Line to Polygon Vector Processing functions.
*/

//Terralib

#include "../common/progress/TaskProgress.h"
#include "../core/logger/Logger.h"
#include "../core/translator/Translator.h"

#include "../dataaccess/dataset/DataSet.h"
#include "../dataaccess/dataset/DataSetAdapter.h"
#include "../dataaccess/utils/Utils.h"

#include "../geometry/GeometryProperty.h"
#include "../geometry/Utils.h"

#include "../memory/DataSet.h"
#include "../memory/DataSetItem.h"

#include "LineToPolygonMemory.h"
#include "Utils.h"

// STL
#include <iostream>
#include <string>

te::vp::LineToPolygonMemory::LineToPolygonMemory()
{}

te::vp::LineToPolygonMemory::~LineToPolygonMemory()
{}

bool te::vp::LineToPolygonMemory::run() throw (te::common::Exception)
{
  std::auto_ptr<te::da::DataSetType> outDsType = buildOutDataSetType();

  te::gm::GeometryProperty* geomProp = te::da::GetFirstGeomProperty(m_converter->getResult());

  std::string geomName = geomProp->getName();
  std::size_t geomPos = te::da::GetPropertyPos(m_converter->getResult(), geomName);

  std::auto_ptr<te::da::DataSet> inDsetSrc;

  if(m_oidSet == 0)
    inDsetSrc = m_inDsrc->getDataSet(m_inDsetName);
  else
    inDsetSrc = m_inDsrc->getDataSet(m_inDsetName, m_oidSet);

  std::auto_ptr<te::da::DataSetAdapter> inDset(te::da::CreateAdapter(inDsetSrc.get(), m_converter.get()));

  std::auto_ptr<te::mem::DataSet> outDSet(new te::mem::DataSet(outDsType.get()));

  te::common::TaskProgress task("Processing...");
  task.setTotalSteps((int)inDset->size());
  task.useTimer(true);

  inDset->moveBeforeFirst();
  while(inDset->moveNext())
  {
    te::mem::DataSetItem* outDsItem = new te::mem::DataSetItem(outDSet.get());
    bool geomState = true;

    for(size_t i = 0; i < outDsItem->getNumProperties(); ++i)
    {
      if(outDsItem->getPropertyDataType(i) != te::dt::GEOMETRY_TYPE)
      {
        outDsItem->setValue(i, inDset->getValue(outDsItem->getPropertyName(i)).get()->clone());
      }
      else
      {
        std::auto_ptr<te::gm::Geometry> geom = inDset->getGeometry(geomPos);
        if(!geom->isValid())
        {
          geomState = false;
          continue;
        }

        std::auto_ptr<te::gm::MultiPolygon> polygonResult = line2Polygon(geom.get());
        if(!polygonResult->isValid() || polygonResult->isEmpty())
          geomState = false;

        outDsItem->setGeometry(i, polygonResult.release());
      }
    }
    if(!geomState)
      continue;

    outDSet->add(outDsItem);

    if (task.isActive() == false)
      throw te::vp::Exception(TE_TR("Operation canceled!"));
  
    task.pulse();
  }

  te::vp::Save(m_outDsrc.get(), outDSet.get(), outDsType.get());
  return true;
}

std::auto_ptr<te::gm::MultiPolygon> te::vp::LineToPolygonMemory::line2Polygon(te::gm::Geometry* geom)
{
  std::vector<te::gm::Polygon*> polygons;
  std::auto_ptr<te::gm::MultiPolygon> polygonResult(new te::gm::MultiPolygon(0, te::gm::MultiPolygonType, geom->getSRID()));

  getPolygons(geom, polygons);

  for(std::size_t i = 0; i < polygons.size(); ++i)
    polygonResult->add(polygons[i]);

  return polygonResult;
}

void te::vp::LineToPolygonMemory::getPolygons(te::gm::Geometry* geom, std::vector<te::gm::Polygon*>& polygons)
{
  if(geom == 0)
    return;

  switch(geom->getGeomTypeId())
  {
    case te::gm::MultiLineStringType:
      getPolygons(dynamic_cast<te::gm::GeometryCollection*>(geom), polygons);
    break;

    case te::gm::LineStringType:
      getPolygons(dynamic_cast<te::gm::LineString*>(geom), polygons);
    break;

    case te::gm::LineStringZType:
      getPolygons(dynamic_cast<te::gm::LineString*>(geom), polygons);
    break;

    case te::gm::LineStringMType:
      getPolygons(dynamic_cast<te::gm::LineString*>(geom), polygons);
    break;

    case te::gm::LineStringZMType:
      getPolygons(dynamic_cast<te::gm::LineString*>(geom), polygons);
    break;

    default:
      break;
  }
}

void te::vp::LineToPolygonMemory::getPolygons(te::gm::GeometryCollection* gc, std::vector<te::gm::Polygon*>& polygons)
{
  assert(gc);

  for(std::size_t i = 0; i < gc->getNumGeometries(); ++i)
    getPolygons(gc->getGeometryN(i), polygons);
}

void te::vp::LineToPolygonMemory::getPolygons(te::gm::LineString* l, std::vector<te::gm::Polygon*>& polygons)
{
  assert(l);

  te::gm::Polygon* p = new te::gm::Polygon(0, te::gm::PolygonType, l->getSRID());

  te::gm::LinearRing* ring = 0;

  if(l->isClosed())
  {
    ring = new te::gm::LinearRing(l->getNPoints(), te::gm::LineStringType);
  }
//  else
//  {
//    std::string wkt = l->asText();
//    std::unique_ptr<te::gm::Geometry> geomTemp(SnapToSelf(l, 100, true));
//    wkt = geomTemp->asText();
//    l = dynamic_cast<te::gm::LineString*>(geomTemp.release());
//    wkt = l->asText();

//    if(!l->isClosed())
//      return;
//  }

  for(std::size_t i = 0; i < l->getNPoints(); ++i)
    ring->setPoint(i, l->getX(i), l->getY(i));

  p->add(ring);

  getPolygons(p, polygons);
}

void te::vp::LineToPolygonMemory::getPolygons(te::gm::Polygon* p, std::vector<te::gm::Polygon*>& polygons)
{
  assert(p);
  polygons.push_back(p);
}
