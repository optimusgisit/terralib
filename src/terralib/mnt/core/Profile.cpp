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
  \file terralib/mnt/core/Profile.cpp

  \brief This file contains a class that represents the profile.

 */

//TerraLib

#include "../../core/translator/Translator.h"
#include "../../common/progress/TaskProgress.h"
#include "../../core/logger/Logger.h"
#include "../../common/UnitsOfMeasureManager.h"

#include "../../dataaccess/dataset/DataSetTypeConverter.h"
#include "../../dataaccess/dataset/DataSetTypeCapabilities.h"
#include "../../dataaccess/datasource/DataSource.h"
#include "../../dataaccess/datasource/DataSourceFactory.h"
#include "../../dataaccess/datasource/DataSourceInfo.h"
#include "../../dataaccess/datasource/DataSourceManager.h"
#include "../../dataaccess/query/EqualTo.h"

#include "../../datatype/Property.h"
#include "../../datatype/SimpleProperty.h"
#include "../../datatype/StringProperty.h"

#include "../../geometry/Geometry.h"
#include "../../geometry/GeometryProperty.h"
#include "../../geometry/Line.h"
#include "../../geometry/MultiLineString.h"
#include "../../geometry/MultiPolygon.h"
#include "../../geometry/PointZ.h"
#include "../../geometry/Polygon.h"

#include "../../maptools/AbstractLayer.h"

#include "../../memory/DataSet.h"
#include "../../memory/DataSetItem.h"

#include "../../raster/RasterProperty.h"
#include "../../raster/RasterFactory.h"
#include "../../raster/Utils.h"
#include "../../raster/BandProperty.h"

#include "../../srs/SpatialReferenceSystemManager.h"
#include "../../statistics/core/Enums.h"

#include "Profile.h"
#include "Utils.h"

//STL
#include <cassert>

//Boost
#include <boost/lexical_cast.hpp>

te::mnt::Profile::Profile()
{
}

te::mnt::Profile::~Profile()
{
}

void te::mnt::Profile::setInput(te::da::DataSourcePtr inDsrc, std::string inName, std::auto_ptr<te::da::DataSetType> inDsetType, double dummy, std::string zattr)
{
  m_inDsrc = inDsrc;
  m_inName = inName;
  m_inDsType = inDsetType;
  m_dummy = dummy;
  m_attrZ = zattr;
}


std::auto_ptr<te::rst::Raster> te::mnt::Profile::getPrepareRaster()
{
  std::auto_ptr<te::da::DataSetType>dsTypeRaster = m_inDsrc->getDataSetType(m_inName);
  //prepare raster
  te::rst::RasterProperty* inProp = te::da::GetFirstRasterProperty(dsTypeRaster.get());
  std::auto_ptr<te::da::DataSet> ds = m_inDsrc->getDataSet(m_inName);
  std::auto_ptr<te::rst::Raster> raster = ds->getRaster(inProp->getName());
  return raster;
}


bool te::mnt::Profile::runRasterProfile(std::vector<te::gm::LineString*> visadas, std::vector<te::gm::LineString*>& profileSet)
{
  double distbase = 0;
  int ind_pf = -1;
  int ind_pfant = -1;

  double xmin, xmax, ymin, ymax, zval, pt1X, pt1Y, pt2X, pt2Y;
  unsigned int col, row;

  te::gm::Coord2D collin;

  std::map<double, double> line;

  std::auto_ptr<te::rst::Raster> raster = getPrepareRaster();
  if (!raster.get())
  {
    return false;
  }

  double boxLowerLeft = raster->getExtent()->getLowerLeftX(); //x1 = lowerLeftX
  double boxUpperRigth = raster->getExtent()->getUpperRightY(); //y2 = UpperRigthY

  double resX = raster->getResolutionX();
  double resY = raster->getResolutionY();
  bool isll = false;

  m_srid = raster->getSRID();

  if (m_srid)
  {
    te::common::UnitOfMeasurePtr unitin = te::srs::SpatialReferenceSystemManager::getInstance().getUnit((unsigned int)m_srid);
    if (unitin && unitin->getId() != te::common::UOM_Metre)
      isll = true;
  }
  te::common::UnitOfMeasurePtr unitout = te::common::UnitsOfMeasureManager::getInstance().find("metre");

  for (std::size_t v = 0; v < visadas.size(); ++v)
  {
    te::gm::LineString *l = visadas[v];
    for (std::size_t i = 0; i < l->getNPoints()-1; i++)
    {
      pt1X = l->getX(i);
      pt1Y = l->getY(i);

      collin = raster->getGrid()->geoToGrid(pt1X, pt1Y);
      col = (unsigned int)collin.getX();
      row = (unsigned int)collin.getY();

      if (col >= raster->getNumberOfColumns() || row >= raster->getNumberOfRows())
        continue;
      raster->getValue(col, row, zval);
      if (zval == m_dummy)
        continue;

      if (!line.size()) //first element to insert
        line.insert(std::map<double, double>::value_type(0.0, zval));

      ind_pf++;

      pt2X = l->getX(i+1);
      pt2Y = l->getY(i+1);

      if (pt1X > pt2X)
      {
        xmin = pt2X;
        xmax = pt1X;
      }
      else
      {
        xmin = pt1X;
        xmax = pt2X;
      }

      if (pt1Y > pt2Y)
      {
        ymin = pt2Y;
        ymax = pt1Y;
      }
      else
      {
        ymin = pt1Y;
        ymax = pt2Y;
      }

      int initcol = (int)((xmin - boxLowerLeft) / resX);
      int finalcol = (int)((xmax - boxLowerLeft) / resX + .9999);
      int initline = (int)((boxUpperRigth - ymax) / resY);
      int finalline = (int)((boxUpperRigth - ymin) / resY + .9999);

      double dx = pt2X - pt1X;
      double dy = pt2Y - pt1Y;

      //Calculate intersections of the segment with the columns
      if (((finalcol - initcol) > 1) && (fabs((double)dx) > 1.0e-6))
      {
        for (int c = (initcol + 1); c < finalcol; c++)
        {
          double x = (boxLowerLeft + c * resX);
          double y = (pt1Y + (dy / dx) * (x - pt1X));

          collin = raster->getGrid()->geoToGrid(x, y);
          col = (unsigned int)collin.getX();
          row = (unsigned int)collin.getY();

          if (col >= raster->getNumberOfColumns() || row >= raster->getNumberOfRows())
            continue;

          raster->getValue(col, row, zval);
          if (zval != m_dummy)
          {
            double dist = sqrt(((double)(x - pt1X)*(double)(x - pt1X)) + ((double)(y - pt1Y)*(double)(y - pt1Y))) + distbase;
            if (isll)
              convertAngleToPlanar(dist, unitout, 0);
            line.insert(std::map<double, double>::value_type(dist, zval));
            ind_pf++;
          }
        }
      }

      //Calculate intersections of the segment with the lines
      if (((finalline - initline) > 1) && (fabs((double)dy) > 1.0e-6))
      {
        for (int l = (initline + 1); l < finalline; l++)
        {
          //Calculate intersections of the segment with the bottom line
          double y = boxUpperRigth - (l * resY);
          double x = pt1X + (dx / dy) * (y - pt1Y);

          collin = raster->getGrid()->geoToGrid(x, y);
          col = (unsigned int)collin.getX();
          row = (unsigned int)collin.getY();

          if (col >= raster->getNumberOfColumns() || row >= raster->getNumberOfRows())
            continue;
          
          raster->getValue(col, row, zval);
          if (zval != m_dummy)
          {
            //Calculates the distance of the current point with point x1, y1
            double dist = sqrt(((double)(x - pt1X)*(double)(x - pt1X)) + ((double)(y - pt1Y)*(double)(y - pt1Y))) + distbase;
            if (isll)
              convertAngleToPlanar(dist, unitout, 0);
            line.insert(std::map<double, double>::value_type(dist, zval));
            ind_pf++;
          }
        }
      }

      //Stores second end (pt2) in the profile structure
      collin = raster->getGrid()->geoToGrid(pt2X, pt2Y);
      col = (unsigned int)collin.getX();
      row = (unsigned int)collin.getY();

      if (col >= raster->getNumberOfColumns() || row >= raster->getNumberOfRows())
        continue;
      raster->getValue(col, row, zval);
      if (zval != m_dummy)
      {
        double dist = sqrt(((double)dx * (double)dx) + ((double)dy* (double)dy)) + distbase;

        if (ind_pf != ind_pfant)
          distbase = dist;

        ind_pfant = ind_pf;
        pt1X = pt2X;
        pt1Y = pt2Y;
        if (isll)
          convertAngleToPlanar(dist, unitout, 0);
        line.insert(std::map<double, double>::value_type(dist, zval));
       }
    }

    te::gm::LineString* profile = new te::gm::LineString(0, te::gm::LineStringType);
    std::map<double, double>::iterator it = line.begin();

    while (it != line.end())
    {
      profile->setNumCoordinates(profile->size() + 1);
      profile->setPoint(profile->size() - 1, it->first, it->second);
      it++;
    }

    profileSet.push_back(profile);
    line.clear();
    distbase = 0;
  }

  return true;
}

te::gm::LineString* te::mnt::Profile::calculateProfile(std::vector<te::gm::Geometry*> &isolines, te::gm::LineString &trajectory)
{
  //std::auto_ptr<te::gm::Geometry> resultGeom;
  te::gm::LineString* iso;
  te::gm::Line seg(te::gm::LineStringType, m_srid);
  te::gm::Line seg1(te::gm::LineStringType, m_srid);
  double dist = 0.;
  te::gm::LineString* profile = new te::gm::LineString(0, te::gm::LineStringType);
  std::map<double, double> line;
  te::gm::PointZ ptinter;

  for (std::size_t t = 0; t < trajectory.size() - 1; t++)
  {
    seg.setCoord(0, trajectory.getX(t), trajectory.getY(t));
    seg.setPoint(1, trajectory.getX(t + 1), trajectory.getY(t + 1));
    for (std::size_t i = 0; i < isolines.size(); i++)
    {
      iso = dynamic_cast<te::gm::LineString*>(isolines[i]);
      for (std::size_t j = 0; j < iso->getNPoints() - 1; j++)
      {
        seg1.setCoord(0, iso->getX(j), iso->getY(j));
        seg1.setPoint(1, iso->getX(j + 1), iso->getY(j + 1));
        if (seg.intersection(seg1, ptinter))
        {
          double dist0 = Distance(trajectory.getX(t), trajectory.getY(t), ptinter.getX(), ptinter.getY());
          line.insert(std::map<double, double>::value_type(dist + dist0, *iso->getZ()));
        }
      }
    }
    dist += Distance(trajectory.getX(t), trajectory.getY(t), trajectory.getX(t + 1), trajectory.getY(t + 1));
  }

    std::map<double, double>::iterator it = line.begin();

    while (it != line.end())
    {
      profile->setNumCoordinates(profile->size() + 1);
      profile->setPoint(profile->size() - 1, it->first, it->second);
      it++;
    }
    line.clear();
    return profile;
}

bool te::mnt::Profile::runIsolinesProfile(std::vector<te::gm::LineString*> visadas, std::vector<te::gm::LineString*>& profileSet)
{
  std::auto_ptr<te::da::DataSetType>dsType = m_inDsrc->getDataSetType(m_inName);

  bool isll = false;
  if (m_srid)
  {
    te::common::UnitOfMeasurePtr unitin = te::srs::SpatialReferenceSystemManager::getInstance().getUnit((unsigned int)m_srid);
    if (unitin && unitin->getId() != te::common::UOM_Metre)
      isll = true;
  }
  te::common::UnitOfMeasurePtr unitout = te::common::UnitsOfMeasureManager::getInstance().find("metre");

  te::gm::MultiPoint mpt(0, te::gm::MultiPointZType, m_srid);
  te::gm::MultiLineString isolines(0, te::gm::MultiLineStringZType, m_srid);
  
  std::string geostype;
  te::gm::Envelope env;
  
  if (ReadSamples(m_inName, m_inDsrc, m_attrZ, 0, 0, None, mpt, isolines, geostype, env, m_srid) == 0)
    return true;

  te::sam::rtree::Index<te::gm::Geometry*> linetree;
  for (size_t i = 0; i < isolines.getNumGeometries(); ++i)
  {
    linetree.insert(*isolines.getGeometryN(i)->getMBR(), dynamic_cast<te::gm::Geometry*>(isolines.getGeometryN(i)));
  }

  std::vector<te::gm::Geometry*> reportline;
  for (std::size_t v = 0; v < visadas.size(); ++v)
  {
    reportline.clear();
    linetree.search(*visadas[v]->getMBR(), reportline);
    if (reportline.size())
    {
      te::gm::LineString* profile = calculateProfile(reportline, *visadas[v]);
      if (profile)
        profileSet.push_back(profile);
    }
  }
  return true;
}

bool te::mnt::Profile::runTINProfile(std::vector<te::gm::LineString*> visadas, std::vector<te::gm::LineString*>& profileSet)
{
  std::auto_ptr<te::da::DataSetType>dsType = m_inDsrc->getDataSetType(m_inName);

  bool isll = false;
  if (m_srid)
  {
    te::common::UnitOfMeasurePtr unitin = te::srs::SpatialReferenceSystemManager::getInstance().getUnit((unsigned int)m_srid);
    if (unitin && unitin->getId() != te::common::UOM_Metre)
      isll = true;
  }
  te::common::UnitOfMeasurePtr unitout = te::common::UnitsOfMeasureManager::getInstance().find("metre");

  te::gm::MultiPoint mpt(0, te::gm::MultiPointZType, m_srid);

  std::auto_ptr<te::da::DataSet> inDset = m_inDsrc->getDataSet(m_inName);
  std::size_t geo_pos = te::da::GetFirstPropertyPos(inDset.get(), te::dt::GEOMETRY_TYPE);
  std::auto_ptr<te::gm::GeometryProperty>geomProp(te::da::GetFirstGeomProperty(dsType.get()));

  inDset->moveBeforeFirst();

  std::vector<te::gm::Polygon *> vp;
  te::gm::LineString *edge;
  te::sam::rtree::Index<te::gm::Geometry*> linetree;
  std::vector<te::gm::Geometry*> reportline;

  try
  {
    while (inDset->moveNext())
    {
      std::auto_ptr<te::gm::Geometry> gin = inDset->getGeometry(geo_pos);

      if ((gin->getGeomTypeId()) % 1000 == te::gm::MultiPolygonType)
      {
        te::gm::MultiPolygon *mg = dynamic_cast<te::gm::MultiPolygon*>(gin.get()->clone());
        if (!mg)
          throw te::common::Exception(TE_TR("Isn't possible to read data!"));

        std::size_t np = mg->getNumGeometries();
        for (std::size_t i = 0; i < np; i++)
          vp.push_back(dynamic_cast<te::gm::Polygon*>(mg->getGeometryN(i)));
      }
      if ((gin->getGeomTypeId()) % 1000 == te::gm::PolygonType)
      {
        te::gm::Polygon *pol = dynamic_cast<te::gm::Polygon*>(gin.get()->clone());
        if (!pol)
          throw te::common::Exception(TE_TR("Isn't possible to read data!"));
        vp.push_back(pol);
      }
      for (std::size_t i = 0; i < vp.size(); ++i)
      {
        te::gm::Polygon *pol = vp[i];
        te::gm::Curve* c = pol->getRingN(0);
        te::gm::LinearRing* lr = dynamic_cast<te::gm::LinearRing*>(c);
        for (std::size_t p = 0; p < 3; p++)
        {
          edge = new te::gm::LineString(2, te::gm::LineStringZType);
          edge->setPointZ(0, lr->getPointN(p)->getX(), lr->getPointN(p)->getY(), lr->getPointN(p)->getZ());
          edge->setPointZ(1, lr->getPointN(p + 1)->getX(), lr->getPointN(p + 1)->getY(), lr->getPointN(p)->getZ());
          reportline.clear();
          linetree.search(*edge->getMBR(), reportline);
          if (reportline.size())
            continue;
          linetree.insert(*edge->getMBR(), dynamic_cast<te::gm::Geometry*>(edge));
        }
      }
      vp.clear();
    }

    for (std::size_t v = 0; v < visadas.size(); ++v)
    {
      reportline.clear();
      linetree.search(*visadas[v]->getMBR(), reportline);
      if (reportline.size())
      {
        te::gm::LineString* profile = calculateProfile(reportline, *visadas[v]);
        if (profile)
          profileSet.push_back(profile);
      }
    }

    reportline.clear();
    linetree.clear();
  }
  catch (te::common::Exception& e)
  {
#ifdef TERRALIB_LOGGER_ENABLED
    TE_CORE_LOG_DEBUG("mnt", e.what());
#endif
    dsType.release();
    throw te::common::Exception(e.what());
  }

  dsType.release();

  return true;
}

