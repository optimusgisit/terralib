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
  \file terralib/geometry/WKBReader.cpp

  \brief A class that deserializes a geometry from a valid WKB.
*/

// TerraLib
#include "../common/ByteSwapUtils.h"
#include "../common/Globals.h"
#include "../core/utils/HexUtils.h"
#include "../core/translator/Translator.h"
#include "Exception.h"
#include "GeometryCollection.h"
#include "LinearRing.h"
#include "MultiCurve.h"
#include "MultiLineString.h"
#include "MultiPoint.h"
#include "MultiPolygon.h"
#include "PointM.h"
#include "PointZ.h"
#include "PointZM.h"
#include "Polygon.h"
#include "PolyhedralSurface.h"
#include "TIN.h"
#include "Triangle.h"
#include "WKBReader.h"

// STL
#include <cstring>

te::gm::Geometry* te::gm::WKBReader::read(const char* wkb)
{
  const char* aux = wkb;

  return getGeometry(wkb, &aux); 
}

te::gm::Geometry* te::gm::WKBReader::readHex(const char* hwkb)
{
  char* wkb = te::core::Hex2Binary(hwkb);

  Geometry* g = te::gm::WKBReader::read(wkb);

  delete [] wkb;

  return g;
}

te::gm::Geometry* te::gm::WKBReader::getGeometry(const char* wkb, const char** endptr)
{
  assert(wkb && endptr);
  
  char byteOrder = 0;
  unsigned int gType = 0;

  memcpy(&byteOrder, wkb, 1);
  memcpy(&gType, wkb + 1, 4);

  //wkb += 5; -> not skipping now!

  if(te::common::Globals::sm_machineByteOrder != static_cast<te::common::MachineByteOrder>(byteOrder))
    te::common::SwapBytes(gType);
  
  switch(gType)
  {
    case te::gm::PointType:
      return getPoint(wkb, endptr);

    case te::gm::PointZType:
      return getPointZ(wkb, endptr);

    case te::gm::PointMType:
      return getPointM(wkb, endptr);

    case te::gm::PointZMType:
      return getPointZM(wkb, endptr);
      

    case te::gm::LineStringType:
    case te::gm::LineStringZType:
    case te::gm::LineStringMType:
    case te::gm::LineStringZMType:
      return getLineString(wkb, endptr);

    case te::gm::PolygonType:
    case te::gm::PolygonZType:
    case te::gm::PolygonMType:
    case te::gm::PolygonZMType:
    case te::gm::TriangleZType:
    case te::gm::TriangleMType:
    case te::gm::TriangleZMType:
       return getPolygon(wkb, endptr);

    case te::gm::MultiPointType:
    case te::gm::MultiPointZType:
    case te::gm::MultiPointMType:
    case te::gm::MultiPointZMType:
    case te::gm::MultiLineStringType:
    case te::gm::MultiLineStringZType:
    case te::gm::MultiLineStringMType:
    case te::gm::MultiLineStringZMType:
    case te::gm::MultiPolygonType:
    case te::gm::MultiPolygonZType:
    case te::gm::MultiPolygonMType:
    case te::gm::MultiPolygonZMType:
    case te::gm::MultiSurfaceType:
    case te::gm::MultiSurfaceZType:
    case te::gm::MultiSurfaceMType:
    case te::gm::MultiSurfaceZMType:
    case te::gm::GeometryCollectionType:
    case te::gm::GeometryCollectionZType:
    case te::gm::GeometryCollectionMType:
    case te::gm::GeometryCollectionZMType:
      return getGeometryCollection(wkb, endptr);

    case te::gm::TINType:
    case te::gm::TINZType:
    case te::gm::TINMType:
    case te::gm::TINZMType:
    case te::gm::PolyhedralSurfaceType:
    case te::gm::PolyhedralSurfaceZType:
    case te::gm::PolyhedralSurfaceMType:
    case te::gm::PolyhedralSurfaceZMType:
      return getPolyhedralSurface(wkb, endptr);

    default:
      throw Exception(TE_TR("Could not read WKB due to an invalid geometry type!"));
  }
}

te::gm::Point* te::gm::WKBReader::getPoint(const char* wkb, const char** endptr)
{
  assert(wkb && endptr);

  char byteOrder = 0;
//unsigned int gType = 0;
  double x = 0.0;
  double y = 0.0;

  memcpy(&byteOrder, wkb, 1);
//memcpy(&gType, wkb + 1, 4);

  memcpy(&x, wkb + 5, 8);        
  memcpy(&y, wkb + 13, 8);

  wkb += 21;

  if(te::common::Globals::sm_machineByteOrder != static_cast<te::common::MachineByteOrder>(byteOrder))
  {
//  te::common::SwapBytes(gType);
    te::common::SwapBytes(x);      
    te::common::SwapBytes(y);
  }

  *endptr = wkb;

  return new Point(x, y, 0, 0);
}

te::gm::PointZ* te::gm::WKBReader::getPointZ(const char* wkb, const char** endptr)
{
  assert(wkb && endptr);

  char byteOrder = 0;
//unsigned int gType = 0;
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  memcpy(&byteOrder, wkb, 1);
//memcpy(&gType, wkb + 1, 4);
  memcpy(&x, wkb + 5, 8);        
  memcpy(&y, wkb + 13, 8);
  memcpy(&z, wkb + 21, 8);

  *endptr = wkb + 29;

  if(te::common::Globals::sm_machineByteOrder != byteOrder)
  {
//  te::common::SwapBytes(gType);
    te::common::SwapBytes(x);
    te::common::SwapBytes(y);
    te::common::SwapBytes(z);
  }

  return new PointZ(x, y, z, 0, 0);
}

te::gm::PointM* te::gm::WKBReader::getPointM(const char* wkb, const char** endptr)
{
  assert(wkb && endptr);

  char byteOrder = 0;
//unsigned int gType = 0;
  double x = 0.0;
  double y = 0.0;
  double m = 0.0;

  memcpy(&byteOrder, wkb, 1);
//memcpy(&gType, wkb + 1, 4);
  memcpy(&x, wkb + 5, 8);
  memcpy(&y, wkb + 13, 8);
  memcpy(&m, wkb + 21, 8);

  *endptr = wkb + 29;

  if(te::common::Globals::sm_machineByteOrder != static_cast<te::common::MachineByteOrder>(byteOrder))
  {
//  te::common::SwapBytes(gType);
    te::common::SwapBytes(x);      
    te::common::SwapBytes(y);
    te::common::SwapBytes(m);
  }

  return new PointM(x, y, m, 0, 0);
}

te::gm::PointZM* te::gm::WKBReader::getPointZM(const char* wkb, const char** endptr)
{
  assert(wkb && endptr);

  char byteOrder = 0;
//unsigned int gType = 0;
//int srid = 0;
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  double m = 0.0;

  memcpy(&byteOrder, wkb, 1);
//memcpy(&gType, wkb + 1, 4);
  memcpy(&x, wkb + 5, 8);
  memcpy(&y, wkb + 13, 8);
  memcpy(&z, wkb + 21, 8);
  memcpy(&m, wkb + 29, 8);

  *endptr = wkb + 37;


  if(te::common::Globals::sm_machineByteOrder != static_cast<te::common::MachineByteOrder>(byteOrder))
  {
//  te::common::SwapBytes(gType);
    te::common::SwapBytes(x);      
    te::common::SwapBytes(y);
    te::common::SwapBytes(z);
    te::common::SwapBytes(m);
  }

  return new PointZM(x, y, z, m, 0, 0);
}

te::gm::LineString* te::gm::WKBReader::getLineString(const char* wkb, const char** endptr)
{
  assert(wkb && endptr);

  char byteOrder = 0;
  unsigned int gType = 0;
  int srid = 0;
  unsigned int nPts = 0;

  memcpy(&byteOrder, wkb, 1);
  memcpy(&gType, wkb + 1, 4);
  memcpy(&nPts, wkb + 5, 4);

  wkb += 9;

  if(te::common::Globals::sm_machineByteOrder != static_cast<te::common::MachineByteOrder>(byteOrder))
  {
    te::common::SwapBytes(gType);
    te::common::SwapBytes(nPts);
  }

  LineString* l = new LineString(nPts, static_cast<GeomType>(gType), srid, 0);

  Coord2D* coords = l->getCoordinates();

  double* zA = l->getZ();

  double* mA = l->getM();

  switch(gType)
  {
    case LineStringType :
      memcpy(coords, wkb, 16 * nPts);
      wkb += nPts * 16;
    break;
    
    case LineStringZType :
      for(unsigned int i = 0; i < nPts; ++i)
      {
        memcpy(&(coords[i]), wkb, 16);
        memcpy(&(zA[i]), wkb + 16, 8);
        wkb += 24;
      }
    break;

    case LineStringMType :
      for(unsigned int i = 0; i < nPts; ++i)
      {
        memcpy(&(coords[i]), wkb, 16);
        memcpy(&(mA[i]), wkb + 16, 8);
        wkb += 24;
      }
    break;

    case LineStringZMType :
      for(unsigned int i = 0; i < nPts; ++i)
      {
        memcpy(&(coords[i]), wkb, 16);
        memcpy(&(zA[i]), wkb + 16, 8);
        memcpy(&(mA[i]), wkb + 24, 8);
        wkb += 32;
      }
    break;

    default:
      throw Exception(TE_TR("Could not read WKB due to an invalid line string type!"));
  }
  
  if(te::common::Globals::sm_machineByteOrder != static_cast<te::common::MachineByteOrder>(byteOrder))
  {
    for(unsigned int i = 0; i < nPts; ++i)
    {
      te::common::SwapBytes(coords[i].x);
      te::common::SwapBytes(coords[i].y);
    }

    if(zA)
      for(unsigned int i = 0; i < nPts; ++i)
        te::common::SwapBytes(zA[i]);

    if(mA)
      for(unsigned int i = 0; i < nPts; ++i)
        te::common::SwapBytes(mA[i]);
  }

  *endptr = wkb;

  return l;
}

te::gm::LinearRing* te::gm::WKBReader::getLinearRing(const char* wkb, const char** endptr, te::common::MachineByteOrder byteOrder, GeomType gType)
{
  assert(wkb && endptr);

  unsigned int nPts = 0;

  memcpy(&nPts, wkb, 4);
  wkb += 4;

  if(te::common::Globals::sm_machineByteOrder != byteOrder)
    te::common::SwapBytes(nPts);

  LinearRing* r = new LinearRing(nPts, gType, 0, 0);

  Coord2D* coords = r->getCoordinates();

  double* zA = r->getZ();

  double* mA = r->getM();

  switch(gType)
  {
    case LineStringType :
      memcpy(coords, wkb, 16 * nPts);
      wkb += nPts * 16;
    break;
    
    case LineStringZType :
      for(unsigned int i = 0; i < nPts; ++i)
      {
        memcpy(&(coords[i]), wkb, 16);
        memcpy(&(zA[i]), wkb + 16, 8);
        wkb += 24;
      }
    break;

    case LineStringMType :
      for(unsigned int i = 0; i < nPts; ++i)
      {
        memcpy(&(coords[i]), wkb, 16);
        memcpy(&(mA[i]), wkb + 16, 8);
        wkb += 24;
      }
    break;

    case LineStringZMType :
      for(unsigned int i = 0; i < nPts; ++i)
      {
        memcpy(&(coords[i]), wkb, 16);
        memcpy(&(zA[i]), wkb + 16, 8);
        memcpy(&(mA[i]), wkb + 24, 8);
        wkb += 32;
      }
    break;

    default:
      throw Exception(TE_TR("Could not read WKB due to an invalid linear ring type!"));
  }
  
  if(te::common::Globals::sm_machineByteOrder != byteOrder)
  {
    for(unsigned int i = 0; i < nPts; ++i)
    {
      te::common::SwapBytes(coords[i].x);
      te::common::SwapBytes(coords[i].y);
    }

    if(zA)
      for(unsigned int i = 0; i < nPts; ++i)
        te::common::SwapBytes(zA[i]);

    if(mA)
      for(unsigned int i = 0; i < nPts; ++i)
        te::common::SwapBytes(mA[i]);
  }

  *endptr = wkb;

  return r;
}

te::gm::Polygon* te::gm::WKBReader::getPolygon(const char* wkb, const char** endptr)
{
  assert(wkb && endptr);

  te::common::MachineByteOrder byteOrder = te::common::XDR;
  unsigned int gType = 0;
  unsigned int nRings = 0;

  memcpy(&byteOrder, wkb, 1);
  memcpy(&gType, wkb + 1, 4);
  memcpy(&nRings, wkb + 5, 4);

  wkb += 9;  

  if(te::common::Globals::sm_machineByteOrder != byteOrder)
  {
    te::common::SwapBytes(gType);
    te::common::SwapBytes(nRings);
  }
  
  Polygon* p = 0;
  
  GeomType rType = UnknownGeometryType;

  switch(gType)
  {
    case PolygonType:
      rType = LineStringType;
      p = new Polygon(nRings, PolygonType, 0, 0);
    break;

    case PolygonZType:
      rType = LineStringZType;
      p = new Polygon(nRings, PolygonZType, 0, 0);
    break;

    case PolygonMType:
      rType = LineStringMType;
      p = new Polygon(nRings, PolygonMType, 0, 0);
    break;

    case PolygonZMType:
      rType = LineStringZMType;
      p = new Polygon(nRings, PolygonZMType, 0, 0);
    break;

    case TriangleType:
      rType = LineStringType;
      
      if(nRings != 1)
        return 0;

      p = new Triangle(TriangleType, 0, 0);
    break;

    case TriangleZType:
      rType = LineStringZType;
      
      if(nRings != 1)
        return 0;

      p = new Triangle(TriangleZType, 0, 0);
    break;

    case TriangleMType:
      rType = LineStringMType;

      if(nRings != 1)
        return 0;

      p = new Triangle(TriangleMType, 0, 0);
    break;

    case TriangleZMType:
      rType = LineStringZMType;

      if(nRings != 1)
        return 0;

      p = new Triangle(TriangleZMType, 0, 0);      
    break;

    default:
      throw Exception(TE_TR("Could not read WKB due to an invalid polygon type!"));
  }

  *endptr = wkb;

  if(p == 0)
    return 0;

  for(unsigned int i = 0; i < nRings; ++i)
  {
    LinearRing* r = getLinearRing(wkb, endptr, byteOrder, rType);
    wkb = *endptr;
    p->setRingN(i, r);
  }

  return p;
}

te::gm::GeometryCollection* te::gm::WKBReader::getGeometryCollection(const char* wkb, const char** endptr)
{
  assert(wkb && endptr);

  te::common::MachineByteOrder byteOrder = te::common::XDR;
  unsigned int gType = 0;
  unsigned int nGeoms = 0;

  memcpy(&byteOrder, wkb, 1);
  memcpy(&gType, wkb + 1, 4);
  memcpy(&nGeoms, wkb + 5, 4);

  if(te::common::Globals::sm_machineByteOrder != byteOrder)
  {
    te::common::SwapBytes(gType);
    te::common::SwapBytes(nGeoms);
  }
  
  wkb += 9;
  
  GeometryCollection* gc = 0;

  switch(gType)
  {
    case MultiPolygonType:
    case MultiPolygonZType:
    case MultiPolygonMType:
    case MultiPolygonZMType:
      gc = new MultiPolygon(nGeoms, static_cast<GeomType>(gType));
    break;

    case MultiPointType:
    case MultiPointZType:
    case MultiPointMType:
    case MultiPointZMType:
      gc = new MultiPoint(nGeoms, static_cast<GeomType>(gType));
    break;

    case MultiLineStringType:
    case MultiLineStringZType:
    case MultiLineStringMType:
    case MultiLineStringZMType:
      gc = new MultiLineString(nGeoms, static_cast<GeomType>(gType));
    break;

    case MultiSurfaceType:
    case MultiSurfaceZType:
    case MultiSurfaceMType:
    case MultiSurfaceZMType:
      gc = new MultiSurface(nGeoms, static_cast<GeomType>(gType));
      break;

    case GeometryCollectionType:
    case GeometryCollectionZType:
    case GeometryCollectionMType:
    case GeometryCollectionZMType:
      gc = new GeometryCollection(nGeoms, static_cast<GeomType>(gType));
    break;

    default:
      throw Exception(TE_TR("Could not read WKB due to an invalid geometry collection type!"));
  }

  *endptr = wkb;

  if(gc == 0)
    throw Exception(TE_TR("Could not create geometry collection from wkb!"));

  for(unsigned int i = 0; i < nGeoms; ++i)
  {
    Geometry* g = getGeometry(wkb, endptr);
    wkb = *endptr;
    gc->setGeometryN(i, g);
  }

  return gc;
}

te::gm::PolyhedralSurface* te::gm::WKBReader::getPolyhedralSurface(const char* wkb, const char** endptr)
{
  assert(wkb && endptr);

  te::common::MachineByteOrder byteOrder = te::common::XDR;
  unsigned int gType = 0;
  unsigned int nPols = 0;

  memcpy(&byteOrder, wkb, 1);
  memcpy(&gType, wkb + 1, 4);
  memcpy(&nPols, wkb + 5, 4);

  if(te::common::Globals::sm_machineByteOrder != byteOrder)
  {
    te::common::SwapBytes(gType);
    te::common::SwapBytes(nPols);
  }

  wkb += 9;
  
  PolyhedralSurface* ph = 0;

  switch(gType)
  {
    case PolyhedralSurfaceType:
    case PolyhedralSurfaceZType:
    case PolyhedralSurfaceMType:
    case PolyhedralSurfaceZMType:
      ph = new PolyhedralSurface(nPols, static_cast<GeomType>(gType), 0, 0);
    break;

    case TINType:
    case TINZType:
    case TINMType:
    case TINZMType:
      ph = new TIN(nPols, static_cast<GeomType>(gType), 0, 0);
    break;

    default:
      throw Exception(TE_TR("Could not read WKB due to an invalid polyhedral surface type!"));
  }

  *endptr = wkb;

  if(ph == 0)
    throw Exception(TE_TR("Could not create polyhedral surface from wkb!"));

  for(unsigned int i = 0; i < nPols; ++i)
  {
    Polygon* gptr = getPolygon(wkb, endptr);
    wkb = *endptr;
    ph->setPatchN(i ,gptr);
  }

  return ph;
}
