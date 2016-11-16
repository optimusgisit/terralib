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
  \file terralib/postgis/EWKBReader.cpp
   
  \brief An utility class for reading a PostGIS EWKB.
*/

// TerraLib
#include "../common/ByteSwapUtils.h"
#include "../common/Globals.h"
#include "../core/utils/HexUtils.h"
#include "../core/translator/Translator.h"
#include "../geometry.h"
#include "EWKBReader.h"
#include "Exception.h"
#include "Utils.h"

// STL
#include <cassert>
#include <cstring>

namespace te
{
  namespace pgis
  {
    template<class T> inline T* GetCoordSequence(const char* ewkb, const char** endptr, unsigned int gType, char byteOrder)
    {
        assert(ewkb && endptr);

        unsigned int nPts = 0;
        int srid = 0;

        if(gType & TE_EWKB_SRID_FLAG)
        {
          memcpy(&srid, ewkb, 4);
          memcpy(&nPts, ewkb + 4, 4);
          ewkb += 8;
        }
        else
        {
          memcpy(&nPts, ewkb, 4);
          ewkb += 4;
        }

        if(te::common::Globals::sm_machineByteOrder != byteOrder)
        {
          te::common::SwapBytes(srid);
          te::common::SwapBytes(nPts);
        }

        Convert2OGCWKBType(gType);

        T* cs = new T(nPts, (te::gm::GeomType)gType, srid, 0);

        switch(gType)
        {
          case te::gm::LineStringType :
            memcpy(cs->getCoordinates(), ewkb, 16 * nPts);
            ewkb += nPts * 16;
            break;
          
          case te::gm::LineStringZType :
            for(unsigned int i = 0; i < nPts; ++i)
            {
              memcpy(&(cs->getCoordinates()[i]), ewkb, 16);
              memcpy(&(cs->getZ()[i]), ewkb + 16, 8);
              ewkb += 24;
            }
            break;

          case te::gm::LineStringMType :
            for(unsigned int i = 0; i < nPts; ++i)
            {
              memcpy(&(cs->getCoordinates()[i]), ewkb, 16);
              memcpy(&(cs->getM()[i]), ewkb + 16, 8);
              ewkb += 24;
            }
            break;

          case te::gm::LineStringZMType :
            for(unsigned int i = 0; i < nPts; ++i)
            {
              memcpy(&(cs->getCoordinates()[i]), ewkb, 16);
              memcpy(&(cs->getZ()[i]), ewkb + 16, 8);
              memcpy(&(cs->getM()[i]), ewkb + 24, 8);
              ewkb += 32;
            }
            break;
        }
        
        if(te::common::Globals::sm_machineByteOrder != byteOrder)
        {
          for(unsigned int i = 0; i < nPts; ++i)
          {
            te::common::SwapBytes(cs->getCoordinates()[i].x);
            te::common::SwapBytes(cs->getCoordinates()[i].y);
          }

          if(cs->getZ())
            for(unsigned int i = 0; i < nPts; ++i)
              te::common::SwapBytes(cs->getZ()[i]);

          if(cs->getM())
            for(unsigned int i = 0; i < nPts; ++i)
              te::common::SwapBytes(cs->getM()[i]);
        }

        *endptr = ewkb;

        return cs;

    }
  }
}

te::gm::Geometry* te::pgis::EWKBReader::read(const char* ewkb)
{
  const char* aux = ewkb;

  return getGeometry(ewkb, &aux); 
}

te::gm::Geometry* te::pgis::EWKBReader::readHex(const char* hewkb)
{
  char* ewkb = te::core::Hex2Binary(hewkb);

  te::gm::Geometry* g = te::pgis::EWKBReader::read(ewkb);

  delete [] ewkb;

  return g;
}

te::gm::Geometry* te::pgis::EWKBReader::getGeometry(const char* ewkb, const char** endptr)
{
  assert(ewkb && endptr);
  
  char byteOrder = 0;
  unsigned int gType = 0;

  memcpy(&byteOrder, ewkb, 1);
  memcpy(&gType, ewkb + 1, 4);

  if(te::common::Globals::sm_machineByteOrder != byteOrder)
    te::common::SwapBytes(gType);
  
  Convert2OGCWKBType(gType);

  switch(gType)
  {
    case te::gm::PointType:
    case te::gm::PointZType:
    case te::gm::PointMType:
    case te::gm::PointZMType:
      return getPoint(ewkb, endptr);

    case te::gm::LineStringType:
    case te::gm::LineStringZType:
    case te::gm::LineStringMType:
    case te::gm::LineStringZMType:
      return getLineString(ewkb, endptr);

    case te::gm::PolygonType:
    case te::gm::PolygonZType:
    case te::gm::PolygonMType:
    case te::gm::PolygonZMType:
      return getPolygon(ewkb, endptr);

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
      return getGeometryCollection(ewkb, endptr);

    /*case TriangleType:
    case te::gm::TriangleZType:
    case te::gm::TriangleMType:
    case te::gm::TriangleZMType:
      return getPolygon(ewkb, endptr);

    case te::gm::TINType:
    case te::gm::TINZType:
    case te::gm::TINMType:
    case te::gm::TINZMType:
    case te::gm::PolyhedralSurfaceType:
    case te::gm::PolyhedralSurfaceZType:
    case te::gm::PolyhedralSurfaceMType:
    case te::gm::PolyhedralSurfaceZMType:
      return getPolyhedralSurface(ewkb, endptr);*/

    default:
      assert(false);
      break;
  }

  return 0;
}

te::gm::Point* te::pgis::EWKBReader::getPoint(const char* ewkb, const char** endptr)
{
  assert(ewkb && endptr);

  char byteOrder = 0;
  unsigned int gType = 0;
  int srid = 0;
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  double m = 0.0;

  memcpy(&byteOrder, ewkb, 1);  
  memcpy(&gType, ewkb + 1, 4);

  if(te::common::Globals::sm_machineByteOrder != byteOrder)
    te::common::SwapBytes(gType);

  ewkb += 5;

  if(gType & TE_EWKB_SRID_FLAG)
  {    
    memcpy(&srid, ewkb, 4);
    ewkb += 4;
  }

  Convert2OGCWKBType(gType);

  memcpy(&x, ewkb, 8);        
  memcpy(&y, ewkb + 8, 8);

  ewkb += 16;

  if(gType != te::gm::PointType)
  {
    switch(gType)
    {
      case te::gm::PointZType:
        memcpy(&z, ewkb, 8);
        ewkb += 8;
        break;

      case te::gm::PointMType:
        memcpy(&m, ewkb, 8);
        ewkb += 8;
        break;

      default: // zm
        assert(gType == te::gm::PointZMType);
        memcpy(&z, ewkb, 8);
        memcpy(&m, ewkb + 8, 8);
        ewkb += 16;    
    }
  }

  *endptr = ewkb;

  if(te::common::Globals::sm_machineByteOrder != byteOrder)
  {
    te::common::SwapBytes(srid);
    te::common::SwapBytes(x);
    te::common::SwapBytes(y);
    te::common::SwapBytes(z);
    te::common::SwapBytes(m);
  }

  switch(gType)
  {
    case te::gm::PointType:
      return new te::gm::Point(x, y, srid, 0);

    case te::gm::PointZType:
      return new te::gm::PointZ(x, y, z, srid, 0);

    case te::gm::PointMType:
      return new te::gm::PointM(x, y, m, srid, 0);

    default: // zm
      assert(gType == te::gm::PointZMType);
      return new te::gm::PointZM(x, y, z, m, srid, 0);
  }
}

te::gm::LineString* te::pgis::EWKBReader::getLineString(const char* ewkb, const char** endptr)
{
  assert(ewkb && endptr);

  char byteOrder = 0;
  unsigned int gType = 0;

  memcpy(&byteOrder, ewkb, 1);
  memcpy(&gType, ewkb + 1, 4);

  ewkb += 5;

  if(te::common::Globals::sm_machineByteOrder != byteOrder)
    te::common::SwapBytes(gType);

  te::gm::LineString* l = GetCoordSequence<te::gm::LineString>(ewkb, endptr, gType, byteOrder);

  return l;
}

te::gm::LinearRing* te::pgis::EWKBReader::getLinearRing(const char* ewkb, te::gm::GeomType t, int srid, char wkbByteOrder, const char** endptr)
{
  assert(ewkb && endptr);

  te::gm::LinearRing* r = GetCoordSequence<te::gm::LinearRing>(ewkb, endptr, t, wkbByteOrder);
  r->setSRID(srid);

  return r;
}

te::gm::Polygon* te::pgis::EWKBReader::getPolygon(const char* ewkb, const char** endptr)
{
  assert(ewkb && endptr);

  char byteOrder = 0;
  unsigned int gType = 0;
  int srid = 0;
  unsigned int nRings = 0;

  memcpy(&byteOrder, ewkb, 1);
  memcpy(&gType, ewkb + 1, 4);

  if(te::common::Globals::sm_machineByteOrder != byteOrder)
    te::common::SwapBytes(gType);

  if(gType & TE_EWKB_SRID_FLAG)
  {
    memcpy(&srid, ewkb + 5, 4);
    memcpy(&nRings, ewkb + 9, 4);
    ewkb += 13;
  }
  else
  {
    memcpy(&nRings, ewkb + 5, 4);
    ewkb += 9;
  }

  if(te::common::Globals::sm_machineByteOrder != byteOrder)
  {
    te::common::SwapBytes(srid);
    te::common::SwapBytes(nRings);
  }

  Convert2OGCWKBType(gType);
  
  te::gm::Polygon* p = 0;
  
  te::gm::GeomType rType = te::gm::UnknownGeometryType;

  switch(gType)
  {
    case te::gm::PolygonType:
      rType = te::gm::LineStringType;
      p = new te::gm::Polygon(nRings, te::gm::PolygonType, srid, 0);
      break;

    case te::gm::PolygonZType:
      rType = te::gm::LineStringZType;
      p = new te::gm::Polygon(nRings, te::gm::PolygonZType, srid, 0);
      break;

    case te::gm::PolygonMType:
      rType = te::gm::LineStringMType;
      p = new te::gm::Polygon(nRings, te::gm::PolygonMType, srid, 0);
      break;

    case te::gm::PolygonZMType:
      rType = te::gm::LineStringZMType;
      p = new te::gm::Polygon(nRings, te::gm::PolygonZMType, srid, 0);
      break;

    default:
      assert(false);
      break;
  }

  *endptr = ewkb;

  if(p == 0)
    return 0;

  for(unsigned int i = 0; i < nRings; ++i)
  {
    te::gm::LinearRing* r = getLinearRing(ewkb, rType, srid, byteOrder, endptr);
    ewkb = *endptr;
    p->getRings()[i] = r;
  }

  return p;
}

te::gm::GeometryCollection* te::pgis::EWKBReader::getGeometryCollection(const char* ewkb, const char** endptr)
{
  assert(ewkb && endptr);

  char byteOrder = 0;
  unsigned int gType = 0;
  int srid = 0;
  unsigned int nGeoms = 0;

  memcpy(&byteOrder, ewkb, 1);
  memcpy(&gType, ewkb + 1, 4);

  if(te::common::Globals::sm_machineByteOrder != byteOrder)
    te::common::SwapBytes(gType);

  if(gType & TE_EWKB_SRID_FLAG)
  {
    memcpy(&srid, ewkb + 5, 4);
    memcpy(&nGeoms, ewkb + 9, 4);
    ewkb += 13;
  }
  else
  {
    memcpy(&nGeoms, ewkb + 5, 4);
    ewkb += 9;
  }

  if(te::common::Globals::sm_machineByteOrder != byteOrder)
  {
    te::common::SwapBytes(srid);
    te::common::SwapBytes(nGeoms);
  }

  Convert2OGCWKBType(gType);
  
  te::gm::GeometryCollection* gc = 0;

  switch(gType)
  {
    case te::gm::MultiPolygonType:
    case te::gm::MultiPolygonZType:
    case te::gm::MultiPolygonMType:
    case te::gm::MultiPolygonZMType:
      gc = new te::gm::MultiPolygon(nGeoms, (te::gm::GeomType)gType, srid, 0);
      break;

    case te::gm::MultiPointType:
    case te::gm::MultiPointZType:
    case te::gm::MultiPointMType:
    case te::gm::MultiPointZMType:
      gc = new te::gm::MultiPoint(nGeoms, (te::gm::GeomType)gType, srid, 0);
      break;

    case te::gm::MultiLineStringType:
    case te::gm::MultiLineStringZType:
    case te::gm::MultiLineStringMType:
    case te::gm::MultiLineStringZMType:
      gc = new te::gm::MultiLineString(nGeoms, (te::gm::GeomType)gType, srid, 0);
      break;

    case te::gm::GeometryCollectionType:
    case te::gm::GeometryCollectionZType:
    case te::gm::GeometryCollectionMType:
    case te::gm::GeometryCollectionZMType:
      gc = new te::gm::GeometryCollection(nGeoms, (te::gm::GeomType)gType, srid, 0);
      break;

    default:
      assert(false);
      break;
  }

  *endptr = ewkb;

  if(gc == 0)
    return 0;

  for(unsigned int i = 0; i < nGeoms; ++i)
  {
    te::gm::Geometry* g = getGeometry(ewkb, endptr);
    g->setSRID(srid);
    ewkb = *endptr;
    gc->getGeometries()[i] = g;
  }

  return gc;
}
