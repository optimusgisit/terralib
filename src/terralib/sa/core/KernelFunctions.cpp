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
  \file terralib/sa/core/KernelFunctions.cpp

  \brief This file contains a functions used by the kernel operation.

  \reference Adapted from TerraLib4.
*/

#define M_PI       3.14159265358979323846

//TerraLib
#include "../../geometry/MultiPolygon.h"
#include "../../geometry/Point.h"
#include "../../geometry/Polygon.h"
#include "../../raster/Grid.h"
#include "../../raster/Raster.h"
#include "KernelFunctions.h"
#include "StatisticsFunctions.h"

void te::sa::GridStatRadiusKernel(te::sa::KernelParams* params, te::sa::KernelTree& kTree, te::sa::KernelMap& kMap, te::rst::Raster* raster, double radius)
{
  assert(params);
  assert(raster);

  double totKernel = 0.;

  //fill raster
  for(unsigned int i = 0; i < raster->getNumberOfRows(); ++i)
  {
    for(unsigned int j = 0; j < raster->getNumberOfColumns(); ++j)
    {
      te::gm::Coord2D coord = raster->getGrid()->gridToGeo(j, i);

      //calculate box to search
      te::gm::Envelope ext(coord.x, coord.y, coord.x, coord.y);

      ext.m_llx -= radius;
      ext.m_lly -= radius;
      ext.m_urx += radius;
      ext.m_ury += radius;

      //get all elements
      if(params->m_functionType == te::sa::Normal)
      {
        ext = kTree.getMBR();
      }

      //search
      std::vector<int> results;
      kTree.search(ext, results);

      //calculate kernel value
      double val = KernelValue(params, kMap, radius, coord, results);

      totKernel += val;

      //set value
      raster->setValue(j, i, val, 0);
    }
  }

  //normalize output raster
  KernelNormalize(params, kMap, raster, totKernel);
}

void te::sa::GridAdaptRadiusKernel(te::sa::KernelParams* params, te::sa::KernelTree& kTree, te::sa::KernelMap& kMap, te::rst::Raster* raster)
{
  assert(params);
  assert(raster);
}

double te::sa::KernelValue(te::sa::KernelParams* params, te::sa::KernelMap& kMap, double radius, te::gm::Coord2D& coord, std::vector<int> idxVec)
{
  double kernelValue = 0.;

  for(std::size_t t = 0; t < idxVec.size(); ++t)
  {
    int id = idxVec[t];

    te::gm::Geometry* g = kMap[id].first;

    double intensity = kMap[id].second;

    double distance = CalculateDistance(g, coord);

    //calculate kernel value for this element
    double localK = 0.;

    switch(params->m_functionType) 
    {
      case te::sa::Quartic:
        localK = KernelQuartic(radius, distance, intensity);
        break;
      case te::sa::Normal:
        localK = KernelNormal(radius, distance, intensity);
        break;
      case te::sa::Triangular:
        localK = KernelTriangular(radius, distance, intensity);
        break;
      case te::sa::Negative_Exp:
        localK = KernelNegExponential(radius, distance, intensity);
      break;
        case te::sa::Uniform:
        localK = KernelUniform(radius, distance, intensity);
      break;
    }

    kernelValue += localK; 
  }

  return kernelValue;
}

double te::sa::CalculateDistance(te::gm::Geometry* geom, te::gm::Coord2D& coord)
{
  double distance = std::numeric_limits<double>::max();

  std::auto_ptr<te::gm::Point> point(new te::gm::Point(coord.x, coord.y));

  point->setSRID(geom->getSRID());

  if(geom->getGeomTypeId() == te::gm::PointType)
  {
    te::gm::Point* p = ((te::gm::Point*)geom);

    distance = p->distance(point.get());
  }
  else if(geom->getGeomTypeId() == te::gm::PolygonType)
  {
    te::gm::Point* p = ((te::gm::Polygon*)geom)->getCentroid();

    distance = p->distance(point.get());

    delete p;
  }
  else if(geom->getGeomTypeId() == te::gm::MultiPolygonType)
  {
    te::gm::Polygon* poly = (te::gm::Polygon*)((te::gm::MultiPolygon*)geom)->getGeometryN(0);

    te::gm::Point* p = poly->getCentroid();

    distance = p->distance(point.get());

    delete p;
  }

  return distance;
}

double te::sa::KernelQuartic(double tau, double distance, double intensity) 
{
  if (distance > tau)
    return 0.0;

  return intensity * (3.0 / (tau * tau * M_PI)) *
  pow(1 - ((distance * distance)/ (tau * tau)),2.0);
}

double te::sa::KernelNormal(double tau, double distance, double intensity)
{
  return intensity * (1.0 / (tau * tau * 2 * M_PI)) *
  exp(-1.0 * (distance * distance)/ (2 * tau * tau));
}

double te::sa::KernelUniform(double tau, double distance, double intensity)
{
  if (distance > tau)
    return 0.0;

  return intensity;
}

double te::sa::KernelTriangular(double tau, double distance, double intensity)
{
  if (distance > tau)
    return 0.0;

  return intensity * (1.0 - 1.0/tau) * distance;
}

double te::sa::KernelNegExponential(double tau, double distance, double intensity)
{
  if (distance > tau)
    return 0.0;

  return intensity * exp(-3.0 * distance);
}

void te::sa::KernelNormalize(te::sa::KernelParams* params, te::sa::KernelMap& kMap, te::rst::Raster* raster, double totKernel)
{
  assert(params);
  assert(raster);

  te::sa::KernelEstimationType type = params->m_estimationType;

  double normFactor = te::sa::Sum(kMap);

  double area = raster->getResolutionX() * raster->getResolutionY();

  for(unsigned int i = 0; i < raster->getNumberOfRows(); ++i)
  {
    for(unsigned int j = 0; j < raster->getNumberOfColumns(); ++j)
    {
      double kernel = 0.;
      
      raster->getValue(j, i, kernel);

      double normKernel = 0.;

      switch(type)
      {
        case te::sa::Spatial_Moving_Average:
          normKernel = (kernel * normFactor) / totKernel;
          break;

        case te::sa::Density:
          normKernel = ((kernel * normFactor) / totKernel) / area;
          break;

        case te::sa::Probability:
          normKernel = kernel / totKernel;
          break;
      }

      raster->setValue(j, i, normKernel);
    }
  }
}
