/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled applications.

    TerraLib is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraLib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with TerraLib. See COPYING. If not, write to
    TerraLib Team at <terralib-team@terralib.org>.
 */

// Unit-Test TerraLib
#include "TsGeometry.h"
#include "../Config.h"

// TerraLib
#include <terralib/Defines.h>
#include <terralib/common.h>
#include <terralib/common/Globals.h>
#include <terralib/core/encoding/CharEncoding.h>
#include <terralib/geometry.h>
#include <terralib/core/utils/HexUtils.h>

// STL
#include <cstdio>
#include <cstdlib>
#include <cstring>

CPPUNIT_TEST_SUITE_REGISTRATION( TsGeometry );

void TsGeometry::setUp()
{
}

void TsGeometry::tearDown()
{
}

void TsGeometry::tcMisc()
{
//#ifdef TE_COMPILE_ALL
  te::gm::Point* pt2D = new te::gm::Point(2378);

  CPPUNIT_ASSERT(typeid(*pt2D) == typeid(te::gm::Point));
  CPPUNIT_ASSERT(pt2D->getDimension() == te::gm::P);
  CPPUNIT_ASSERT(pt2D->getCoordinateDimension() == 2);
  CPPUNIT_ASSERT(pt2D->getGeometryType() == "Point");
  CPPUNIT_ASSERT(pt2D->getGeomTypeId() == te::gm::PointType);
  CPPUNIT_ASSERT(pt2D->getSRID() == 2378);
  
  delete pt2D;

  te::gm::Point* ptM = new te::gm::PointM(2378);

  CPPUNIT_ASSERT(typeid(*ptM) == typeid(te::gm::PointM));
  CPPUNIT_ASSERT(ptM->getDimension() == te::gm::P);
  CPPUNIT_ASSERT(ptM->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(ptM->getGeometryType() == "Point");
  CPPUNIT_ASSERT(ptM->getGeomTypeId() == te::gm::PointMType);
  CPPUNIT_ASSERT(ptM->getSRID() == 2378);

  delete ptM;

  te::gm::Point* ptZ = new te::gm::PointZ(2378);

  CPPUNIT_ASSERT(typeid(*ptZ) == typeid(te::gm::PointZ));
  CPPUNIT_ASSERT(ptZ->getDimension() == te::gm::P);
  CPPUNIT_ASSERT(ptZ->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(ptZ->getGeometryType() == "Point");
  CPPUNIT_ASSERT(ptZ->getGeomTypeId() == te::gm::PointZType);  
  CPPUNIT_ASSERT(ptZ->getSRID() == 2378);

  delete ptZ;

  te::gm::Point* ptZM = new te::gm::PointZM(2378);

  CPPUNIT_ASSERT(typeid(*ptZM) == typeid(te::gm::PointZM));
  CPPUNIT_ASSERT(ptZM->getDimension() == te::gm::P);
  CPPUNIT_ASSERT(ptZM->getCoordinateDimension() == 4);
  CPPUNIT_ASSERT(ptZM->getGeometryType() == "Point");
  CPPUNIT_ASSERT(ptZM->getGeomTypeId() == te::gm::PointZMType);
  CPPUNIT_ASSERT(ptZM->getSRID() == 2378);

  delete ptZM;

  te::gm::LineString* l2D = new te::gm::LineString(te::gm::LineStringType, 2378);

  CPPUNIT_ASSERT(typeid(*l2D) == typeid(te::gm::LineString));
  CPPUNIT_ASSERT(l2D->getDimension() == te::gm::L);
  CPPUNIT_ASSERT(l2D->getCoordinateDimension() == 2);
  CPPUNIT_ASSERT(l2D->getGeometryType() == "LineString");
  CPPUNIT_ASSERT(l2D->getGeomTypeId() == te::gm::LineStringType);  
  CPPUNIT_ASSERT(l2D->getSRID() == 2378);

  delete l2D;

  te::gm::LineString* lM = new te::gm::LineString(te::gm::LineStringMType, 2378);

  CPPUNIT_ASSERT(typeid(*lM) == typeid(te::gm::LineString));
  CPPUNIT_ASSERT(lM->getDimension() == te::gm::L);
  CPPUNIT_ASSERT(lM->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(lM->getGeometryType() == "LineString");
  CPPUNIT_ASSERT(lM->getGeomTypeId() == te::gm::LineStringMType);  
  CPPUNIT_ASSERT(lM->getSRID() == 2378);

  delete lM;

  te::gm::LineString* lZ = new te::gm::LineString(te::gm::LineStringZType, 2378);

  CPPUNIT_ASSERT(typeid(*lZ) == typeid(te::gm::LineString));
  CPPUNIT_ASSERT(lZ->getDimension() == te::gm::L);
  CPPUNIT_ASSERT(lZ->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(lZ->getGeometryType() == "LineString");
  CPPUNIT_ASSERT(lZ->getGeomTypeId() == te::gm::LineStringZType);
  CPPUNIT_ASSERT(lZ->getSRID() == 2378);

  delete lZ;

  te::gm::LineString* lZM = new te::gm::LineString(te::gm::LineStringZMType, 2378);

  CPPUNIT_ASSERT(typeid(*lZM) == typeid(te::gm::LineString));
  CPPUNIT_ASSERT(lZM->getDimension() == te::gm::L);
  CPPUNIT_ASSERT(lZM->getCoordinateDimension() == 4);
  CPPUNIT_ASSERT(lZM->getGeometryType() == "LineString");
  CPPUNIT_ASSERT(lZM->getGeomTypeId() == te::gm::LineStringZMType);
  CPPUNIT_ASSERT(lZM->getSRID() == 2378);

  delete lZM;

  te::gm::Polygon* pol2D = new te::gm::Polygon(0, te::gm::PolygonType, 2378);

  CPPUNIT_ASSERT(typeid(*pol2D) == typeid(te::gm::Polygon));
  CPPUNIT_ASSERT(pol2D->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(pol2D->getCoordinateDimension() == 2);
  CPPUNIT_ASSERT(pol2D->getGeometryType() == "Polygon");
  CPPUNIT_ASSERT(pol2D->getGeomTypeId() == te::gm::PolygonType);
  CPPUNIT_ASSERT(pol2D->getSRID() == 2378);

  delete pol2D;

  te::gm::Polygon* polM = new te::gm::Polygon(0, te::gm::PolygonMType, 2378);

  CPPUNIT_ASSERT(typeid(*polM) == typeid(te::gm::Polygon));
  CPPUNIT_ASSERT(polM->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(polM->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(polM->getGeometryType() == "Polygon");
  CPPUNIT_ASSERT(polM->getGeomTypeId() == te::gm::PolygonMType);
  CPPUNIT_ASSERT(polM->getSRID() == 2378);

  delete polM;

  te::gm::Polygon* polZ = new te::gm::Polygon(0, te::gm::PolygonZType, 2378);

  CPPUNIT_ASSERT(typeid(*polZ) == typeid(te::gm::Polygon));
  CPPUNIT_ASSERT(polZ->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(polZ->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(polZ->getGeometryType() == "Polygon");
  CPPUNIT_ASSERT(polZ->getGeomTypeId() == te::gm::PolygonZType);
  CPPUNIT_ASSERT(polZ->getSRID() == 2378);

  delete polZ;

  te::gm::Polygon* polZM = new te::gm::Polygon(0, te::gm::PolygonZMType, 2378);

  CPPUNIT_ASSERT(typeid(*polZM) == typeid(te::gm::Polygon));
  CPPUNIT_ASSERT(polZM->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(polZM->getCoordinateDimension() == 4);
  CPPUNIT_ASSERT(polZM->getGeometryType() == "Polygon");
  CPPUNIT_ASSERT(polZM->getGeomTypeId() == te::gm::PolygonZMType);
  CPPUNIT_ASSERT(polZM->getSRID() == 2378);

  delete polZM;

  te::gm::GeometryCollection* gc2D = new te::gm::GeometryCollection(0, te::gm::GeometryCollectionType, 2378);

  CPPUNIT_ASSERT(typeid(*gc2D) == typeid(te::gm::GeometryCollection));
  CPPUNIT_ASSERT(gc2D->getDimension() == te::gm::P);
  CPPUNIT_ASSERT(gc2D->getCoordinateDimension() == 2);
  CPPUNIT_ASSERT(gc2D->getGeometryType() == "GeometryCollection");
  CPPUNIT_ASSERT(gc2D->getGeomTypeId() == te::gm::GeometryCollectionType);
  CPPUNIT_ASSERT(gc2D->getSRID() == 2378);

  delete gc2D;

  te::gm::GeometryCollection* gcM = new te::gm::GeometryCollection(0, te::gm::GeometryCollectionMType, 2378);

  CPPUNIT_ASSERT(typeid(*gcM) == typeid(te::gm::GeometryCollection));
  CPPUNIT_ASSERT(gcM->getDimension() == te::gm::P);
  CPPUNIT_ASSERT(gcM->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(gcM->getGeometryType() == "GeometryCollection");
  CPPUNIT_ASSERT(gcM->getGeomTypeId() == te::gm::GeometryCollectionMType);
  CPPUNIT_ASSERT(gcM->getSRID() == 2378);

  delete gcM;

  te::gm::GeometryCollection* gcZ = new te::gm::GeometryCollection(0, te::gm::GeometryCollectionZType, 2378);

  CPPUNIT_ASSERT(typeid(*gcZ) == typeid(te::gm::GeometryCollection));
  CPPUNIT_ASSERT(gcZ->getDimension() == te::gm::P);
  CPPUNIT_ASSERT(gcZ->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(gcZ->getGeometryType() == "GeometryCollection");
  CPPUNIT_ASSERT(gcZ->getGeomTypeId() == te::gm::GeometryCollectionZType);
  CPPUNIT_ASSERT(gcZ->getSRID() == 2378);

  delete gcZ;

  te::gm::GeometryCollection* gcZM = new te::gm::GeometryCollection(0, te::gm::GeometryCollectionZMType, 2378);

  CPPUNIT_ASSERT(typeid(*gcZM) == typeid(te::gm::GeometryCollection));
  CPPUNIT_ASSERT(gcZM->getDimension() == te::gm::P);
  CPPUNIT_ASSERT(gcZM->getCoordinateDimension() == 4);
  CPPUNIT_ASSERT(gcZM->getGeometryType() == "GeometryCollection");
  CPPUNIT_ASSERT(gcZM->getGeomTypeId() == te::gm::GeometryCollectionZMType);
  CPPUNIT_ASSERT(gcZM->getSRID() == 2378);

  delete gcZM;

  te::gm::MultiPoint* mpt2D = new te::gm::MultiPoint(0, te::gm::MultiPointType, 2378);

  CPPUNIT_ASSERT(typeid(*mpt2D) == typeid(te::gm::MultiPoint));
  CPPUNIT_ASSERT(mpt2D->getDimension() == te::gm::P);
  CPPUNIT_ASSERT(mpt2D->getCoordinateDimension() == 2);
  CPPUNIT_ASSERT(mpt2D->getGeometryType() == "MultiPoint");
  CPPUNIT_ASSERT(mpt2D->getGeomTypeId() == te::gm::MultiPointType);
  CPPUNIT_ASSERT(mpt2D->getSRID() == 2378);

  delete mpt2D;

  te::gm::MultiPoint* mptM = new te::gm::MultiPoint(0, te::gm::MultiPointMType, 2378);

  CPPUNIT_ASSERT(typeid(*mptM) == typeid(te::gm::MultiPoint));
  CPPUNIT_ASSERT(mptM->getDimension() == te::gm::P);
  CPPUNIT_ASSERT(mptM->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(mptM->getGeometryType() == "MultiPoint");
  CPPUNIT_ASSERT(mptM->getGeomTypeId() == te::gm::MultiPointMType);
  CPPUNIT_ASSERT(mptM->getSRID() == 2378);

  delete mptM;

  te::gm::MultiPoint* mptZ = new te::gm::MultiPoint(0, te::gm::MultiPointZType, 2378);

  CPPUNIT_ASSERT(typeid(*mptZ) == typeid(te::gm::MultiPoint));
  CPPUNIT_ASSERT(mptZ->getDimension() == te::gm::P);
  CPPUNIT_ASSERT(mptZ->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(mptZ->getGeometryType() == "MultiPoint");
  CPPUNIT_ASSERT(mptZ->getGeomTypeId() == te::gm::MultiPointZType);
  CPPUNIT_ASSERT(mptZ->getSRID() == 2378);

  delete mptZ;

  te::gm::MultiPoint* mptZM = new te::gm::MultiPoint(0, te::gm::MultiPointZMType, 2378);

  CPPUNIT_ASSERT(typeid(*mptZM) == typeid(te::gm::MultiPoint));
  CPPUNIT_ASSERT(mptZM->getDimension() == te::gm::P);
  CPPUNIT_ASSERT(mptZM->getCoordinateDimension() == 4);
  CPPUNIT_ASSERT(mptZM->getGeometryType() == "MultiPoint");
  CPPUNIT_ASSERT(mptZM->getGeomTypeId() == te::gm::MultiPointZMType);
  CPPUNIT_ASSERT(mptZM->getSRID() == 2378);

  delete mptZM;

  te::gm::MultiLineString* ml2D = new te::gm::MultiLineString(0, te::gm::MultiLineStringType, 2378);

  CPPUNIT_ASSERT(typeid(*ml2D) == typeid(te::gm::MultiLineString));
  CPPUNIT_ASSERT(ml2D->getDimension() == te::gm::L);
  CPPUNIT_ASSERT(ml2D->getCoordinateDimension() == 2);
  CPPUNIT_ASSERT(ml2D->getGeometryType() == "MultiLineString");
  CPPUNIT_ASSERT(ml2D->getGeomTypeId() == te::gm::MultiLineStringType);
  CPPUNIT_ASSERT(ml2D->getSRID() == 2378);

  delete ml2D;

  te::gm::MultiLineString* mlM = new te::gm::MultiLineString(0, te::gm::MultiLineStringMType, 2378);

  CPPUNIT_ASSERT(typeid(*mlM) == typeid(te::gm::MultiLineString));
  CPPUNIT_ASSERT(mlM->getDimension() == te::gm::L);
  CPPUNIT_ASSERT(mlM->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(mlM->getGeometryType() == "MultiLineString");
  CPPUNIT_ASSERT(mlM->getGeomTypeId() == te::gm::MultiLineStringMType);
  CPPUNIT_ASSERT(mlM->getSRID() == 2378);

  delete mlM;

  te::gm::MultiLineString* mlZ = new te::gm::MultiLineString(0, te::gm::MultiLineStringZType, 2378);

  CPPUNIT_ASSERT(typeid(*mlZ) == typeid(te::gm::MultiLineString));
  CPPUNIT_ASSERT(mlZ->getDimension() == te::gm::L);
  CPPUNIT_ASSERT(mlZ->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(mlZ->getGeometryType() == "MultiLineString");
  CPPUNIT_ASSERT(mlZ->getGeomTypeId() == te::gm::MultiLineStringZType);
  CPPUNIT_ASSERT(mlZ->getSRID() == 2378);

  delete mlZ;

  te::gm::MultiLineString* mlZM = new te::gm::MultiLineString(0, te::gm::MultiLineStringZMType, 2378);

  CPPUNIT_ASSERT(typeid(*mlZM) == typeid(te::gm::MultiLineString));
  CPPUNIT_ASSERT(mlZM->getDimension() == te::gm::L);
  CPPUNIT_ASSERT(mlZM->getCoordinateDimension() == 4);
  CPPUNIT_ASSERT(mlZM->getGeometryType() == "MultiLineString");
  CPPUNIT_ASSERT(mlZM->getGeomTypeId() == te::gm::MultiLineStringZMType);
  CPPUNIT_ASSERT(mlZM->getSRID() == 2378);

  delete mlZM;

  te::gm::MultiPolygon* mpoly2D = new te::gm::MultiPolygon(0, te::gm::MultiPolygonType, 2378);

  CPPUNIT_ASSERT(typeid(*mpoly2D) == typeid(te::gm::MultiPolygon));
  CPPUNIT_ASSERT(mpoly2D->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(mpoly2D->getCoordinateDimension() == 2);
  CPPUNIT_ASSERT(mpoly2D->getGeometryType() == "MultiPolygon");
  CPPUNIT_ASSERT(mpoly2D->getGeomTypeId() == te::gm::MultiPolygonType);
  CPPUNIT_ASSERT(mpoly2D->getSRID() == 2378);

  delete mpoly2D;

  te::gm::MultiPolygon* mpolyM = new te::gm::MultiPolygon(0, te::gm::MultiPolygonMType, 2378);

  CPPUNIT_ASSERT(typeid(*mpolyM) == typeid(te::gm::MultiPolygon));
  CPPUNIT_ASSERT(mpolyM->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(mpolyM->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(mpolyM->getGeometryType() == "MultiPolygon");
  CPPUNIT_ASSERT(mpolyM->getGeomTypeId() == te::gm::MultiPolygonMType);
  CPPUNIT_ASSERT(mpolyM->getSRID() == 2378);

  delete mpolyM;

  te::gm::MultiPolygon* mpolyZ = new te::gm::MultiPolygon(0, te::gm::MultiPolygonZType, 2378);

  CPPUNIT_ASSERT(typeid(*mpolyZ) == typeid(te::gm::MultiPolygon));
  CPPUNIT_ASSERT(mpolyZ->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(mpolyZ->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(mpolyZ->getGeometryType() == "MultiPolygon");
  CPPUNIT_ASSERT(mpolyZ->getGeomTypeId() == te::gm::MultiPolygonZType);
  CPPUNIT_ASSERT(mpolyZ->getSRID() == 2378);

  delete mpolyZ;

  te::gm::MultiPolygon* mpolyZM = new te::gm::MultiPolygon(0, te::gm::MultiPolygonZMType, 2378);

  CPPUNIT_ASSERT(typeid(*mpolyZM) == typeid(te::gm::MultiPolygon));
  CPPUNIT_ASSERT(mpolyZM->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(mpolyZM->getCoordinateDimension() == 4);
  CPPUNIT_ASSERT(mpolyZM->getGeometryType() == "MultiPolygon");
  CPPUNIT_ASSERT(mpolyZM->getGeomTypeId() == te::gm::MultiPolygonZMType);
  CPPUNIT_ASSERT(mpolyZM->getSRID() == 2378);

  delete mpolyZM;

  te::gm::PolyhedralSurface* polyhedral2D = new te::gm::PolyhedralSurface(0, te::gm::PolyhedralSurfaceType, 2378);

  CPPUNIT_ASSERT(typeid(*polyhedral2D) == typeid(te::gm::PolyhedralSurface));
  CPPUNIT_ASSERT(polyhedral2D->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(polyhedral2D->getCoordinateDimension() == 2);
  CPPUNIT_ASSERT(polyhedral2D->getGeometryType() == "PolyhedralSurface");
  CPPUNIT_ASSERT(polyhedral2D->getGeomTypeId() == te::gm::PolyhedralSurfaceType);
  CPPUNIT_ASSERT(polyhedral2D->getSRID() == 2378);

  delete polyhedral2D;

  te::gm::PolyhedralSurface* polyhedralM = new te::gm::PolyhedralSurface(0, te::gm::PolyhedralSurfaceMType, 2378);

  CPPUNIT_ASSERT(typeid(*polyhedralM) == typeid(te::gm::PolyhedralSurface));
  CPPUNIT_ASSERT(polyhedralM->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(polyhedralM->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(polyhedralM->getGeometryType() == "PolyhedralSurface");
  CPPUNIT_ASSERT(polyhedralM->getGeomTypeId() == te::gm::PolyhedralSurfaceMType);
  CPPUNIT_ASSERT(polyhedralM->getSRID() == 2378);

  delete polyhedralM;

  te::gm::PolyhedralSurface* polyhedralZ = new te::gm::PolyhedralSurface(0, te::gm::PolyhedralSurfaceZType, 2378);

  CPPUNIT_ASSERT(typeid(*polyhedralZ) == typeid(te::gm::PolyhedralSurface));
  CPPUNIT_ASSERT(polyhedralZ->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(polyhedralZ->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(polyhedralZ->getGeometryType() == "PolyhedralSurface");
  CPPUNIT_ASSERT(polyhedralZ->getGeomTypeId() == te::gm::PolyhedralSurfaceZType);
  CPPUNIT_ASSERT(polyhedralZ->getSRID() == 2378);

  delete polyhedralZ;

  te::gm::PolyhedralSurface* polyhedralZM = new te::gm::PolyhedralSurface(0, te::gm::PolyhedralSurfaceZMType, 2378);

  CPPUNIT_ASSERT(typeid(*polyhedralZM) == typeid(te::gm::PolyhedralSurface));
  CPPUNIT_ASSERT(polyhedralZM->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(polyhedralZM->getCoordinateDimension() == 4);
  CPPUNIT_ASSERT(polyhedralZM->getGeometryType() == "PolyhedralSurface");
  CPPUNIT_ASSERT(polyhedralZM->getGeomTypeId() == te::gm::PolyhedralSurfaceZMType);
  CPPUNIT_ASSERT(polyhedralZM->getSRID() == 2378);

  delete polyhedralZM;

  te::gm::TIN* tin2D = new te::gm::TIN(0, te::gm::TINType, 2378);

  CPPUNIT_ASSERT(typeid(*tin2D) == typeid(te::gm::TIN));
  CPPUNIT_ASSERT(tin2D->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(tin2D->getCoordinateDimension() == 2);
  CPPUNIT_ASSERT(tin2D->getGeometryType() == "Tin");
  CPPUNIT_ASSERT(tin2D->getGeomTypeId() == te::gm::TINType);
  CPPUNIT_ASSERT(tin2D->getSRID() == 2378);

  delete tin2D;

  te::gm::TIN* tinM = new te::gm::TIN(0, te::gm::TINMType, 2378);

  CPPUNIT_ASSERT(typeid(*tinM) == typeid(te::gm::TIN));
  CPPUNIT_ASSERT(tinM->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(tinM->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(tinM->getGeometryType() == "Tin");
  CPPUNIT_ASSERT(tinM->getGeomTypeId() == te::gm::TINMType);
  CPPUNIT_ASSERT(tinM->getSRID() == 2378);

  delete tinM;

  te::gm::TIN* tinZ = new te::gm::TIN(0, te::gm::TINZType, 2378);

  CPPUNIT_ASSERT(typeid(*tinZ) == typeid(te::gm::TIN));
  CPPUNIT_ASSERT(tinZ->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(tinZ->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(tinZ->getGeometryType() == "Tin");
  CPPUNIT_ASSERT(tinZ->getGeomTypeId() == te::gm::TINZType);
  CPPUNIT_ASSERT(tinZ->getSRID() == 2378);

  delete tinZ;

  te::gm::TIN* tinZM = new te::gm::TIN(0, te::gm::TINZMType, 2378);

  CPPUNIT_ASSERT(typeid(*tinZM) == typeid(te::gm::TIN));
  CPPUNIT_ASSERT(tinZM->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(tinZM->getCoordinateDimension() == 4);
  CPPUNIT_ASSERT(tinZM->getGeometryType() == "Tin");
  CPPUNIT_ASSERT(tinZM->getGeomTypeId() == te::gm::TINZMType);
  CPPUNIT_ASSERT(tinZM->getSRID() == 2378);

  delete tinZM;

  te::gm::Triangle* tri2D = new te::gm::Triangle(te::gm::TriangleType, 2378);

  CPPUNIT_ASSERT(typeid(*tri2D) == typeid(te::gm::Triangle));
  CPPUNIT_ASSERT(tri2D->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(tri2D->getCoordinateDimension() == 2);
  CPPUNIT_ASSERT(tri2D->getGeometryType() == "Polygon");
  CPPUNIT_ASSERT(tri2D->getGeomTypeId() == te::gm::TriangleType);
  CPPUNIT_ASSERT(tri2D->getSRID() == 2378);

  delete tri2D;

  te::gm::Triangle* triM = new te::gm::Triangle(te::gm::TriangleMType, 2378);

  CPPUNIT_ASSERT(typeid(*triM) == typeid(te::gm::Triangle));
  CPPUNIT_ASSERT(triM->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(triM->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(triM->getGeometryType() == "Polygon");
  CPPUNIT_ASSERT(triM->getGeomTypeId() == te::gm::TriangleMType);
  CPPUNIT_ASSERT(triM->getSRID() == 2378);

  delete triM;

  te::gm::Triangle* triZ = new te::gm::Triangle(te::gm::TriangleZType, 2378);

  CPPUNIT_ASSERT(typeid(*triZ) == typeid(te::gm::Triangle));
  CPPUNIT_ASSERT(triZ->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(triZ->getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(triZ->getGeometryType() == "Polygon");
  CPPUNIT_ASSERT(triZ->getGeomTypeId() == te::gm::TriangleZType);
  CPPUNIT_ASSERT(triZ->getSRID() == 2378);

  delete triZ;

  te::gm::Triangle* triZM = new te::gm::Triangle(te::gm::TriangleZMType, 2378);

  CPPUNIT_ASSERT(typeid(*triZM) == typeid(te::gm::Triangle));
  CPPUNIT_ASSERT(triZM->getDimension() == te::gm::A);
  CPPUNIT_ASSERT(triZM->getCoordinateDimension() == 4);
  CPPUNIT_ASSERT(triZM->getGeometryType() == "Polygon");
  CPPUNIT_ASSERT(triZM->getGeomTypeId() == te::gm::TriangleZMType);
  CPPUNIT_ASSERT(triZM->getSRID() == 2378);

  delete triZM;
//#endif
}

void TsGeometry::tcCreatePoint()
{
//#ifdef TE_COMPILE_ALL
  te::gm::Point pt(27.3, 28.5, 4326, 0);

  CPPUNIT_ASSERT(pt.getX() == 27.3);
  CPPUNIT_ASSERT(pt.getY() == 28.5);
  CPPUNIT_ASSERT(pt.getZ() == TE_DOUBLE_NOT_A_NUMBER);
  CPPUNIT_ASSERT(pt.getM() == TE_DOUBLE_NOT_A_NUMBER);
  CPPUNIT_ASSERT(pt.getDimension() == te::gm::P);
  CPPUNIT_ASSERT(pt.getCoordinateDimension() == 2);
  CPPUNIT_ASSERT(pt.getGeometryType() == "Point");
  CPPUNIT_ASSERT(pt.getGeomTypeId() == te::gm::PointType);
  CPPUNIT_ASSERT(pt.getSRID() == 4326);
  pt.setSRID(4128);
  CPPUNIT_ASSERT(pt.getSRID() == 4128);
  pt.setSRID(4326);
  CPPUNIT_ASSERT(pt.getSRID() == 4326);
  //pt.transform(4128);
  //CPPUNIT_ASSERT(pt.getSRID() == 4128);
  std::auto_ptr<te::gm::Geometry> envelope(pt.getEnvelope());
  CPPUNIT_ASSERT(envelope->getGeomTypeId() == te::gm::PolygonType);
  CPPUNIT_ASSERT(pt.getMBR());
  CPPUNIT_ASSERT(pt.getMBR()->m_llx == pt.getX());
  CPPUNIT_ASSERT(pt.getMBR()->m_lly == pt.getY());
  CPPUNIT_ASSERT(pt.getMBR()->m_urx == pt.getX());
  CPPUNIT_ASSERT(pt.getMBR()->m_ury == pt.getY());
  pt.setX(18.9);
  pt.setY(9.81);
  pt.computeMBR(false);
  CPPUNIT_ASSERT(pt.getMBR()->m_llx == 18.9);
  CPPUNIT_ASSERT(pt.getMBR()->m_lly == 9.81);
  CPPUNIT_ASSERT(pt.getMBR()->m_urx == 18.9);
  CPPUNIT_ASSERT(pt.getMBR()->m_ury == 9.81); 
  CPPUNIT_ASSERT(pt.asText() == "point(18.9 9.81)");

// WKB test
  {
    size_t size = pt.getWkbSize();
    CPPUNIT_ASSERT(size == 21);
    char* wkb = new char[size];
    pt.getWkb(wkb, te::common::Globals::sm_machineByteOrder);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01010000006666666666E632401F85EB51B89E2340";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  {
    size_t size = 0;
    char* wkb = pt.asBinary(size);
    CPPUNIT_ASSERT(size == 21);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01010000006666666666E632401F85EB51B89E2340";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  CPPUNIT_ASSERT(pt.isEmpty() == false);
  CPPUNIT_ASSERT(pt.isSimple() == true);
  CPPUNIT_ASSERT(pt.isValid() == true);
  CPPUNIT_ASSERT(pt.is3D() == false);
  CPPUNIT_ASSERT(pt.isMeasured() == false);
  std::auto_ptr<te::gm::Geometry> boundary(pt.getBoundary());

  CPPUNIT_ASSERT(pt.getNPoints() == 1);

  std::auto_ptr<te::gm::Geometry> pt_clone(static_cast<te::gm::Geometry*>(pt.clone()));
  CPPUNIT_ASSERT(pt.equals((pt_clone.get())));
  CPPUNIT_ASSERT(pt.disjoint(pt_clone.get()) == false);
  CPPUNIT_ASSERT(pt.intersects(pt_clone.get()));
  CPPUNIT_ASSERT(pt.touches(pt_clone.get()) == false);
//#endif
}

void TsGeometry::tcCreatePointZ()
{
//#ifdef TE_COMPILE_ALL
  te::gm::PointZ pt(27.3, 28.5, 99.9, 4326, 0);

  CPPUNIT_ASSERT(pt.getX() == 27.3);
  CPPUNIT_ASSERT(pt.getY() == 28.5);
  CPPUNIT_ASSERT(pt.getZ() == 99.9);
  CPPUNIT_ASSERT(pt.getM() == TE_DOUBLE_NOT_A_NUMBER);
  CPPUNIT_ASSERT(pt.getDimension() == te::gm::P);
  CPPUNIT_ASSERT(pt.getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(pt.getGeometryType() == "Point");
  CPPUNIT_ASSERT(pt.getGeomTypeId() == te::gm::PointZType);
  CPPUNIT_ASSERT(pt.getSRID() == 4326);
  pt.setSRID(4128);
  CPPUNIT_ASSERT(pt.getSRID() == 4128);
  pt.setSRID(4326);
  CPPUNIT_ASSERT(pt.getSRID() == 4326);
  //pt.transform(4128);
  //CPPUNIT_ASSERT(pt.getSRID() == 4128);
  std::auto_ptr<te::gm::Geometry> envelope(pt.getEnvelope());
  CPPUNIT_ASSERT(envelope->getGeomTypeId() == te::gm::PolygonType);
  CPPUNIT_ASSERT(pt.getMBR());
  CPPUNIT_ASSERT(pt.getMBR()->m_llx == pt.getX());
  CPPUNIT_ASSERT(pt.getMBR()->m_lly == pt.getY());
  CPPUNIT_ASSERT(pt.getMBR()->m_urx == pt.getX());
  CPPUNIT_ASSERT(pt.getMBR()->m_ury == pt.getY());
  pt.setX(18.9);
  pt.setY(9.81);
  pt.setZ(77.7);
  pt.computeMBR(false);
  CPPUNIT_ASSERT(pt.getMBR()->m_llx == 18.9);
  CPPUNIT_ASSERT(pt.getMBR()->m_lly == 9.81);
  CPPUNIT_ASSERT(pt.getMBR()->m_urx == 18.9);
  CPPUNIT_ASSERT(pt.getMBR()->m_ury == 9.81); 
  CPPUNIT_ASSERT(pt.asText() == "point z(18.9 9.81 77.7)");

// WKB test
  {
    size_t size = pt.getWkbSize();
    CPPUNIT_ASSERT(size == 29);
    char* wkb = new char[size];
    pt.getWkb(wkb, te::common::Globals::sm_machineByteOrder);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01E90300006666666666E632401F85EB51B89E2340CDCCCCCCCC6C5340";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  {
    size_t size = 0;
    char* wkb = pt.asBinary(size);
    CPPUNIT_ASSERT(size == 29);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01E90300006666666666E632401F85EB51B89E2340CDCCCCCCCC6C5340";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  CPPUNIT_ASSERT(pt.isEmpty() == false);
  CPPUNIT_ASSERT(pt.isSimple() == true);
  CPPUNIT_ASSERT(pt.isValid() == true);
  CPPUNIT_ASSERT(pt.is3D() == true);
  CPPUNIT_ASSERT(pt.isMeasured() == false);
  std::auto_ptr<te::gm::Geometry> boundary(pt.getBoundary());
  //boundary->getGeomTypeId() ==
  CPPUNIT_ASSERT(pt.getNPoints() == 1);

  std::auto_ptr<te::gm::Geometry> pt_clone(static_cast<te::gm::Geometry*>(pt.clone()));
  CPPUNIT_ASSERT(pt.equals((pt_clone.get())));
  CPPUNIT_ASSERT(pt.disjoint(pt_clone.get()) == false);
  CPPUNIT_ASSERT(pt.intersects(pt_clone.get()));
  CPPUNIT_ASSERT(pt.touches(pt_clone.get()) == false);
//#endif
}

void TsGeometry::tcCreatePointM()
{
//#ifdef TE_COMPILE_ALL
  te::gm::PointM pt(27.3, 28.5, 99.9, 4326, 0);

  CPPUNIT_ASSERT(pt.getX() == 27.3);
  CPPUNIT_ASSERT(pt.getY() == 28.5);
  CPPUNIT_ASSERT(pt.getZ() == TE_DOUBLE_NOT_A_NUMBER);
  CPPUNIT_ASSERT(pt.getM() == 99.9);
  CPPUNIT_ASSERT(pt.getDimension() == te::gm::P);
  CPPUNIT_ASSERT(pt.getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(pt.getGeometryType() == "Point");
  CPPUNIT_ASSERT(pt.getGeomTypeId() == te::gm::PointMType);
  CPPUNIT_ASSERT(pt.getSRID() == 4326);
  pt.setSRID(4128);
  CPPUNIT_ASSERT(pt.getSRID() == 4128);
  pt.setSRID(4326);
  CPPUNIT_ASSERT(pt.getSRID() == 4326);
  //pt.transform(4128);
  //CPPUNIT_ASSERT(pt.getSRID() == 4128);
  std::auto_ptr<te::gm::Geometry> envelope(pt.getEnvelope());
  CPPUNIT_ASSERT(envelope->getGeomTypeId() == te::gm::PolygonType);
  CPPUNIT_ASSERT(pt.getMBR());
  CPPUNIT_ASSERT(pt.getMBR()->m_llx == pt.getX());
  CPPUNIT_ASSERT(pt.getMBR()->m_lly == pt.getY());
  CPPUNIT_ASSERT(pt.getMBR()->m_urx == pt.getX());
  CPPUNIT_ASSERT(pt.getMBR()->m_ury == pt.getY());
  pt.setX(18.9);
  pt.setY(9.81);
  pt.setM(77.7);
  pt.computeMBR(false);
  CPPUNIT_ASSERT(pt.getMBR()->m_llx == 18.9);
  CPPUNIT_ASSERT(pt.getMBR()->m_lly == 9.81);
  CPPUNIT_ASSERT(pt.getMBR()->m_urx == 18.9);
  CPPUNIT_ASSERT(pt.getMBR()->m_ury == 9.81); 
  CPPUNIT_ASSERT(pt.asText() == "point m(18.9 9.81 77.7)");

// WKB test
  {
    size_t size = pt.getWkbSize();
    CPPUNIT_ASSERT(size == 29);
    char* wkb = new char[size];
    pt.getWkb(wkb, te::common::Globals::sm_machineByteOrder);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01D10700006666666666E632401F85EB51B89E2340CDCCCCCCCC6C5340";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  {
    size_t size = 0;
    char* wkb = pt.asBinary(size);
    CPPUNIT_ASSERT(size == 29);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01D10700006666666666E632401F85EB51B89E2340CDCCCCCCCC6C5340";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  CPPUNIT_ASSERT(pt.isEmpty() == false);
  CPPUNIT_ASSERT(pt.isSimple() == true);
  CPPUNIT_ASSERT(pt.isValid() == true);
  CPPUNIT_ASSERT(pt.is3D() == false);
  CPPUNIT_ASSERT(pt.isMeasured() == true);
  std::auto_ptr<te::gm::Geometry> boundary(pt.getBoundary());
  //boundary->getGeomTypeId() ==
  CPPUNIT_ASSERT(pt.getNPoints() == 1);

  std::auto_ptr<te::gm::Geometry> pt_clone(static_cast<te::gm::Geometry*>(pt.clone()));
  CPPUNIT_ASSERT(pt.equals(pt_clone.get()));
  CPPUNIT_ASSERT(pt.disjoint(pt_clone.get()) == false);
  CPPUNIT_ASSERT(pt.intersects(pt_clone.get()));
  CPPUNIT_ASSERT(pt.touches(pt_clone.get()) == false);
//#endif
}

void TsGeometry::tcCreatePointZM()
{
//#ifdef TE_COMPILE_ALL
  te::gm::PointZM pt(27.3, 28.5, 99.9, 180.0, 4326, 0);

  CPPUNIT_ASSERT(pt.getX() == 27.3);
  CPPUNIT_ASSERT(pt.getY() == 28.5);
  CPPUNIT_ASSERT(pt.getZ() == 99.9);
  CPPUNIT_ASSERT(pt.getM() == 180.0);
  CPPUNIT_ASSERT(pt.getDimension() == te::gm::P);
  CPPUNIT_ASSERT(pt.getCoordinateDimension() == 4);
  CPPUNIT_ASSERT(pt.getGeometryType() == "Point");
  CPPUNIT_ASSERT(pt.getGeomTypeId() == te::gm::PointZMType);
  CPPUNIT_ASSERT(pt.getSRID() == 4326);
  pt.setSRID(4128);
  CPPUNIT_ASSERT(pt.getSRID() == 4128);
  pt.setSRID(4326);
  CPPUNIT_ASSERT(pt.getSRID() == 4326);
  //pt.transform(4128);
  //CPPUNIT_ASSERT(pt.getSRID() == 4128);
  std::auto_ptr<te::gm::Geometry> envelope(pt.getEnvelope());
  CPPUNIT_ASSERT(envelope->getGeomTypeId() == te::gm::PolygonType);
  CPPUNIT_ASSERT(pt.getMBR());
  CPPUNIT_ASSERT(pt.getMBR()->m_llx == pt.getX());
  CPPUNIT_ASSERT(pt.getMBR()->m_lly == pt.getY());
  CPPUNIT_ASSERT(pt.getMBR()->m_urx == pt.getX());
  CPPUNIT_ASSERT(pt.getMBR()->m_ury == pt.getY());
  pt.setX(18.9);
  pt.setY(9.81);
  pt.setZ(77.7);
  pt.setM(55.5);
  pt.computeMBR(false);
  CPPUNIT_ASSERT(pt.getMBR()->m_llx == 18.9);
  CPPUNIT_ASSERT(pt.getMBR()->m_lly == 9.81);
  CPPUNIT_ASSERT(pt.getMBR()->m_urx == 18.9);
  CPPUNIT_ASSERT(pt.getMBR()->m_ury == 9.81); 
  CPPUNIT_ASSERT(pt.asText() == "point zm(18.9 9.81 77.7 55.5)");

// WKB test
  {
    size_t size = pt.getWkbSize();
    CPPUNIT_ASSERT(size == 37);
    char* wkb = new char[size];
    pt.getWkb(wkb, te::common::Globals::sm_machineByteOrder);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01B90B00006666666666E632401F85EB51B89E2340CDCCCCCCCC6C53400000000000C04B40";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  {
    size_t size = 0;
    char* wkb = pt.asBinary(size);
    CPPUNIT_ASSERT(size == 37);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01B90B00006666666666E632401F85EB51B89E2340CDCCCCCCCC6C53400000000000C04B40";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  CPPUNIT_ASSERT(pt.isEmpty() == false);
  CPPUNIT_ASSERT(pt.isSimple() == true);
  CPPUNIT_ASSERT(pt.isValid() == true);
  CPPUNIT_ASSERT(pt.is3D() == true);
  CPPUNIT_ASSERT(pt.isMeasured() == true);
  std::auto_ptr<te::gm::Geometry> boundary(pt.getBoundary());
  CPPUNIT_ASSERT(pt.getNPoints() == 1);

  std::auto_ptr<te::gm::Geometry> pt_clone(static_cast<te::gm::Geometry*>(pt.clone()));
  CPPUNIT_ASSERT(pt.equals(pt_clone.get()));
  CPPUNIT_ASSERT(pt.disjoint(pt_clone.get()) == false);
  CPPUNIT_ASSERT(pt.intersects(pt_clone.get()));
  CPPUNIT_ASSERT(pt.touches(pt_clone.get()) == false);
//#endif
}

void TsGeometry::tcCreateLineString()
{
//#ifdef TE_COMPILE_ALL
  te::gm::LineString line(3, te::gm::LineStringType, 4326);
  line.setPoint(0, 1.0, 1.5);
  line.setPoint(1, 3.0, 3.3);
  line.setPoint(2, 3.1, 2.1);

  CPPUNIT_ASSERT(line.getX(0) == 1.0);
  CPPUNIT_ASSERT(line.getY(0) == 1.5);
  CPPUNIT_ASSERT(line.getX(1) == 3.0);
  CPPUNIT_ASSERT(line.getY(1) == 3.3);
  CPPUNIT_ASSERT(line.getX(2) == 3.1);
  CPPUNIT_ASSERT(line.getY(2) == 2.1);
  CPPUNIT_ASSERT(line.getDimension() == te::gm::L);
  CPPUNIT_ASSERT(line.getCoordinateDimension() == 2);
  CPPUNIT_ASSERT(line.getGeometryType() == "LineString");
  CPPUNIT_ASSERT(line.getGeomTypeId() == te::gm::LineStringType);
  CPPUNIT_ASSERT(line.getSRID() == 4326);
  line.setSRID(4128);
  CPPUNIT_ASSERT(line.getSRID() == 4128);
  line.setSRID(4326);
  CPPUNIT_ASSERT(line.getSRID() == 4326);
  //line.transform(4128);
  //CPPUNIT_ASSERT(line.getSRID() == 4128);
  std::auto_ptr<te::gm::Geometry> envelope(line.getEnvelope());
  CPPUNIT_ASSERT(envelope->getGeomTypeId() == te::gm::PolygonType);
  CPPUNIT_ASSERT(line.getMBR());
  CPPUNIT_ASSERT(line.getMBR()->m_llx == line.getX(0));
  CPPUNIT_ASSERT(line.getMBR()->m_lly == line.getY(0));
  CPPUNIT_ASSERT(line.getMBR()->m_urx == line.getX(2));
  CPPUNIT_ASSERT(line.getMBR()->m_ury == line.getY(1));
  line.setX(2, 18.9);
  line.setY(2, 9.81);
  line.computeMBR(false);
  CPPUNIT_ASSERT(line.getMBR()->m_llx == line.getX(0));
  CPPUNIT_ASSERT(line.getMBR()->m_lly == line.getY(0));
  CPPUNIT_ASSERT(line.getMBR()->m_urx == line.getX(2));
  CPPUNIT_ASSERT(line.getMBR()->m_ury == line.getY(2)); 
  CPPUNIT_ASSERT(line.asText() == "linestring(1 1.5,3 3.3,18.9 9.81)");

  // WKB test
  {
    size_t size = line.getWkbSize();
    CPPUNIT_ASSERT(size == 57);
    char* wkb = new char[size];
    line.getWkb(wkb, te::common::Globals::sm_machineByteOrder);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "010200000003000000000000000000F03F000000000000F83F00000000000008406666666666660A406666666666E632401F85EB51B89E2340";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  {
    size_t size = 0;
    char* wkb = line.asBinary(size);
    CPPUNIT_ASSERT(size == 57);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "010200000003000000000000000000F03F000000000000F83F00000000000008406666666666660A406666666666E632401F85EB51B89E2340";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  CPPUNIT_ASSERT(line.isEmpty() == false);
  CPPUNIT_ASSERT(line.isSimple() == true);
  CPPUNIT_ASSERT(line.isValid() == true);
  CPPUNIT_ASSERT(line.is3D() == false);
  CPPUNIT_ASSERT(line.isMeasured() == false);
  std::auto_ptr<te::gm::Geometry> boundary(line.getBoundary());
  //boundary->getGeomTypeId() ==
  CPPUNIT_ASSERT(line.getNPoints() == 3);

  std::auto_ptr<te::gm::Geometry> line_clone(static_cast<te::gm::Geometry*>(line.clone()));
  CPPUNIT_ASSERT(line.equals(line_clone.get()));
  CPPUNIT_ASSERT(line.disjoint(line_clone.get()) == false);
  CPPUNIT_ASSERT(line.intersects(line_clone.get()));
  CPPUNIT_ASSERT(line.touches(line_clone.get()) == false);
//#endif
}

void TsGeometry::tcCreateLineStringZ()
{
//#ifdef TE_COMPILE_ALL
  te::gm::LineString line(3, te::gm::LineStringZType, 4326);
  line.setPointZ(0, 1.0, 1.5, 8.1);
  line.setPointZ(1, 3.0, 3.3, 9.2);
  line.setPointZ(2, 3.1, 2.1, 10.3);

  CPPUNIT_ASSERT(line.getX(0) == 1.0);
  CPPUNIT_ASSERT(line.getY(0) == 1.5);
  CPPUNIT_ASSERT(line.getZ(0) == 8.1);
  CPPUNIT_ASSERT(line.getX(1) == 3.0);
  CPPUNIT_ASSERT(line.getY(1) == 3.3);
  CPPUNIT_ASSERT(line.getZ(1) == 9.2);
  CPPUNIT_ASSERT(line.getX(2) == 3.1);
  CPPUNIT_ASSERT(line.getY(2) == 2.1);
  CPPUNIT_ASSERT(line.getZ(2) == 10.3);
  CPPUNIT_ASSERT(line.getDimension() == te::gm::L);
  CPPUNIT_ASSERT(line.getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(line.getGeometryType() == "LineString");
  CPPUNIT_ASSERT(line.getGeomTypeId() == te::gm::LineStringZType);
  CPPUNIT_ASSERT(line.getSRID() == 4326);
  line.setSRID(4128);
  CPPUNIT_ASSERT(line.getSRID() == 4128);
  line.setSRID(4326);
  CPPUNIT_ASSERT(line.getSRID() == 4326);
  //line.transform(4128);
  //CPPUNIT_ASSERT(line.getSRID() == 4128);
  std::auto_ptr<te::gm::Geometry> envelope(line.getEnvelope());
  CPPUNIT_ASSERT(envelope->getGeomTypeId() == te::gm::PolygonType);
  CPPUNIT_ASSERT(line.getMBR());
  CPPUNIT_ASSERT(line.getMBR()->m_llx == line.getX(0));
  CPPUNIT_ASSERT(line.getMBR()->m_lly == line.getY(0));
  CPPUNIT_ASSERT(line.getMBR()->m_urx == line.getX(2));
  CPPUNIT_ASSERT(line.getMBR()->m_ury == line.getY(1));
  line.setX(2, 18.9);
  line.setY(2, 9.81);
  line.setZ(2, 19.81);
  line.computeMBR(false);
  CPPUNIT_ASSERT(line.getMBR()->m_llx == line.getX(0));
  CPPUNIT_ASSERT(line.getMBR()->m_lly == line.getY(0));
  CPPUNIT_ASSERT(line.getMBR()->m_urx == line.getX(2));
  CPPUNIT_ASSERT(line.getMBR()->m_ury == line.getY(2)); 
  CPPUNIT_ASSERT(line.asText() == "linestring z(1 1.5 8.1,3 3.3 9.2,18.9 9.81 19.81)");

  // WKB test
  {
    size_t size = line.getWkbSize();
    CPPUNIT_ASSERT(size == 81);
    char* wkb = new char[size];
    line.getWkb(wkb, te::common::Globals::sm_machineByteOrder);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01EA03000003000000000000000000F03F000000000000F83F333333333333204000000000000008406666666666660A4066666666666622406666666666E632401F85EB51B89E23408FC2F5285CCF3340";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  {
    size_t size = 0;
    char* wkb = line.asBinary(size);
    CPPUNIT_ASSERT(size == 81);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01EA03000003000000000000000000F03F000000000000F83F333333333333204000000000000008406666666666660A4066666666666622406666666666E632401F85EB51B89E23408FC2F5285CCF3340";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }  

  CPPUNIT_ASSERT(line.isEmpty() == false);
  CPPUNIT_ASSERT(line.isSimple() == true);
  CPPUNIT_ASSERT(line.isValid() == true);
  CPPUNIT_ASSERT(line.is3D() == true);
  CPPUNIT_ASSERT(line.isMeasured() == false);
  std::auto_ptr<te::gm::Geometry> boundary(line.getBoundary());
  //boundary->getGeomTypeId() ==
  CPPUNIT_ASSERT(line.getNPoints() == 3);

  std::auto_ptr<te::gm::Geometry> line_clone(static_cast<te::gm::Geometry*>(line.clone()));
  CPPUNIT_ASSERT(line.equals(line_clone.get()));
  CPPUNIT_ASSERT(line.disjoint(line_clone.get()) == false);
  CPPUNIT_ASSERT(line.intersects(line_clone.get()));
  CPPUNIT_ASSERT(line.touches(line_clone.get()) == false);
//#endif
}

void TsGeometry::tcCreateLineStringM()
{
//#ifdef TE_COMPILE_ALL
  te::gm::LineString line(3, te::gm::LineStringMType, 4326);
  line.setPointM(0, 1.0, 1.5, 8.1);
  line.setPointM(1, 3.0, 3.3, 9.2);
  line.setPointM(2, 3.1, 2.1, 10.3);

  CPPUNIT_ASSERT(line.getX(0) == 1.0);
  CPPUNIT_ASSERT(line.getY(0) == 1.5);
  CPPUNIT_ASSERT(line.getM(0) == 8.1);
  CPPUNIT_ASSERT(line.getX(1) == 3.0);
  CPPUNIT_ASSERT(line.getY(1) == 3.3);
  CPPUNIT_ASSERT(line.getM(1) == 9.2);
  CPPUNIT_ASSERT(line.getX(2) == 3.1);
  CPPUNIT_ASSERT(line.getY(2) == 2.1);
  CPPUNIT_ASSERT(line.getM(2) == 10.3);
  CPPUNIT_ASSERT(line.getDimension() == te::gm::L);
  CPPUNIT_ASSERT(line.getCoordinateDimension() == 3);
  CPPUNIT_ASSERT(line.getGeometryType() == "LineString");
  CPPUNIT_ASSERT(line.getGeomTypeId() == te::gm::LineStringMType);
  CPPUNIT_ASSERT(line.getSRID() == 4326);
  line.setSRID(4128);
  CPPUNIT_ASSERT(line.getSRID() == 4128);
  line.setSRID(4326);
  CPPUNIT_ASSERT(line.getSRID() == 4326);
  //line.transform(4128);
  //CPPUNIT_ASSERT(line.getSRID() == 4128);
  std::auto_ptr<te::gm::Geometry> envelope(line.getEnvelope());
  CPPUNIT_ASSERT(envelope->getGeomTypeId() == te::gm::PolygonType);
  CPPUNIT_ASSERT(line.getMBR());
  CPPUNIT_ASSERT(line.getMBR()->m_llx == line.getX(0));
  CPPUNIT_ASSERT(line.getMBR()->m_lly == line.getY(0));
  CPPUNIT_ASSERT(line.getMBR()->m_urx == line.getX(2));
  CPPUNIT_ASSERT(line.getMBR()->m_ury == line.getY(1));
  line.setX(2, 18.9);
  line.setY(2, 9.81);
  line.setM(2, 19.81);
  line.computeMBR(false);
  CPPUNIT_ASSERT(line.getMBR()->m_llx == line.getX(0));
  CPPUNIT_ASSERT(line.getMBR()->m_lly == line.getY(0));
  CPPUNIT_ASSERT(line.getMBR()->m_urx == line.getX(2));
  CPPUNIT_ASSERT(line.getMBR()->m_ury == line.getY(2)); 
  CPPUNIT_ASSERT(line.asText() == "linestring m(1 1.5 8.1,3 3.3 9.2,18.9 9.81 19.81)");

  // WKB test
  {
    size_t size = line.getWkbSize();
    CPPUNIT_ASSERT(size == 81);
    char* wkb = new char[size];
    line.getWkb(wkb, te::common::Globals::sm_machineByteOrder);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01D207000003000000000000000000F03F000000000000F83F333333333333204000000000000008406666666666660A4066666666666622406666666666E632401F85EB51B89E23408FC2F5285CCF3340";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  {
    size_t size = 0;
    char* wkb = line.asBinary(size);
    CPPUNIT_ASSERT(size == 81);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01D207000003000000000000000000F03F000000000000F83F333333333333204000000000000008406666666666660A4066666666666622406666666666E632401F85EB51B89E23408FC2F5285CCF3340";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  CPPUNIT_ASSERT(line.isEmpty() == false);
  CPPUNIT_ASSERT(line.isSimple() == true);
  CPPUNIT_ASSERT(line.isValid() == true);
  CPPUNIT_ASSERT(line.is3D() == false);
  CPPUNIT_ASSERT(line.isMeasured() == true);
  std::auto_ptr<te::gm::Geometry> boundary(line.getBoundary());
  //boundary->getGeomTypeId() ==
  CPPUNIT_ASSERT(line.getNPoints() == 3);

  std::auto_ptr<te::gm::Geometry> line_clone(static_cast<te::gm::Geometry*>(line.clone()));
  CPPUNIT_ASSERT(line.equals(line_clone.get()));
  CPPUNIT_ASSERT(line.disjoint(line_clone.get()) == false);
  CPPUNIT_ASSERT(line.intersects(line_clone.get()));
  CPPUNIT_ASSERT(line.touches(line_clone.get()) == false);
//#endif
}

void TsGeometry::tcCreateLineStringZM()
{
//#ifdef TE_COMPILE_ALL
  te::gm::LineString line(3, te::gm::LineStringZMType, 4326);
  line.setPointZM(0, 1.0, 1.5, 8.1, 21.3);
  line.setPointZM(1, 3.0, 3.3, 9.2, 98.4);
  line.setPointZM(2, 3.1, 2.1, 10.3, 63.7);

  CPPUNIT_ASSERT(line.getX(0) == 1.0);
  CPPUNIT_ASSERT(line.getY(0) == 1.5);
  CPPUNIT_ASSERT(line.getZ(0) == 8.1);
  CPPUNIT_ASSERT(line.getM(0) == 21.3);
  CPPUNIT_ASSERT(line.getX(1) == 3.0);
  CPPUNIT_ASSERT(line.getY(1) == 3.3);
  CPPUNIT_ASSERT(line.getZ(1) == 9.2);
  CPPUNIT_ASSERT(line.getM(1) == 98.4);
  CPPUNIT_ASSERT(line.getX(2) == 3.1);
  CPPUNIT_ASSERT(line.getY(2) == 2.1);
  CPPUNIT_ASSERT(line.getZ(2) == 10.3);
  CPPUNIT_ASSERT(line.getM(2) == 63.7);
  CPPUNIT_ASSERT(line.getDimension() == te::gm::L);
  CPPUNIT_ASSERT(line.getCoordinateDimension() == 4);
  CPPUNIT_ASSERT(line.getGeometryType() == "LineString");
  CPPUNIT_ASSERT(line.getGeomTypeId() == te::gm::LineStringZMType);
  CPPUNIT_ASSERT(line.getSRID() == 4326);
  line.setSRID(4128);
  CPPUNIT_ASSERT(line.getSRID() == 4128);
  line.setSRID(4326);
  CPPUNIT_ASSERT(line.getSRID() == 4326);
  //line.transform(4128);
  //CPPUNIT_ASSERT(line.getSRID() == 4128);
  std::auto_ptr<te::gm::Geometry> envelope(line.getEnvelope());
  CPPUNIT_ASSERT(envelope->getGeomTypeId() == te::gm::PolygonType);
  CPPUNIT_ASSERT(line.getMBR());
  CPPUNIT_ASSERT(line.getMBR()->m_llx == line.getX(0));
  CPPUNIT_ASSERT(line.getMBR()->m_lly == line.getY(0));
  CPPUNIT_ASSERT(line.getMBR()->m_urx == line.getX(2));
  CPPUNIT_ASSERT(line.getMBR()->m_ury == line.getY(1));
  line.setX(2, 18.9);
  line.setY(2, 9.81);
  line.setZ(2, 19.81);
  line.setM(2, 43.5);
  line.computeMBR(false);
  CPPUNIT_ASSERT(line.getMBR()->m_llx == line.getX(0));
  CPPUNIT_ASSERT(line.getMBR()->m_lly == line.getY(0));
  CPPUNIT_ASSERT(line.getMBR()->m_urx == line.getX(2));
  CPPUNIT_ASSERT(line.getMBR()->m_ury == line.getY(2)); 
  CPPUNIT_ASSERT(line.asText() == "linestring zm(1 1.5 8.1 21.3,3 3.3 9.2 98.4,18.9 9.81 19.81 43.5)");

  // WKB test
  {
    size_t size = line.getWkbSize();
    CPPUNIT_ASSERT(size == 105);
    char* wkb = new char[size];
    line.getWkb(wkb, te::common::Globals::sm_machineByteOrder);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01BA0B000003000000000000000000F03F000000000000F83F3333333333332040CDCCCCCCCC4C354000000000000008406666666666660A4066666666666622409A999999999958406666666666E632401F85EB51B89E23408FC2F5285CCF33400000000000C04540";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  {
    size_t size = 0;
    char* wkb = line.asBinary(size);
    CPPUNIT_ASSERT(size == 105);
    char* hwkb = te::core::Binary2Hex(wkb, size);
    delete [] wkb;
    const char* validHWKB = "01BA0B000003000000000000000000F03F000000000000F83F3333333333332040CDCCCCCCCC4C354000000000000008406666666666660A4066666666666622409A999999999958406666666666E632401F85EB51B89E23408FC2F5285CCF33400000000000C04540";
    CPPUNIT_ASSERT(strcmp(hwkb, validHWKB) == 0);
    delete [] hwkb;
  }

  CPPUNIT_ASSERT(line.isEmpty() == false);
  CPPUNIT_ASSERT(line.isSimple() == true);
  CPPUNIT_ASSERT(line.isValid() == true);
  CPPUNIT_ASSERT(line.is3D() == true);
  CPPUNIT_ASSERT(line.isMeasured() == true);
  std::auto_ptr<te::gm::Geometry> boundary(line.getBoundary());
  //boundary->getGeomTypeId() ==
  CPPUNIT_ASSERT(line.getNPoints() == 3);

  std::auto_ptr<te::gm::Geometry> line_clone(static_cast<te::gm::Geometry*>(line.clone()));
  CPPUNIT_ASSERT(line.equals(line_clone.get()));
  CPPUNIT_ASSERT(line.disjoint(line_clone.get()) == false);
  CPPUNIT_ASSERT(line.intersects(line_clone.get()));
  CPPUNIT_ASSERT(line.touches(line_clone.get()) == false);
//#endif
}

void TsGeometry::tcCreatePolygon()
{
//#ifdef TE_COMPILE_ALL
  te::gm::Polygon poly(1000, te::gm::PolygonType, 4326, 0);

  CPPUNIT_ASSERT(poly.getExteriorRing() == 0);
  CPPUNIT_ASSERT(poly.getNumInteriorRings() == 999);
  CPPUNIT_ASSERT(poly.getNumRings() == 1000);
  poly.setNumRings(10);
  CPPUNIT_ASSERT(poly.getExteriorRing() == 0);
  CPPUNIT_ASSERT(poly.getNumInteriorRings() == 9);
  CPPUNIT_ASSERT(poly.getNumRings() == 10);
  CPPUNIT_ASSERT(poly.getInteriorRingN(1) == 0);
  CPPUNIT_ASSERT(poly.getRingN(9) == 0);
  CPPUNIT_ASSERT(poly[9] == 0);
  poly.setRingN(4, 0);
  poly.removeRingN(3);
  CPPUNIT_ASSERT(poly.getExteriorRing() == 0);
  CPPUNIT_ASSERT(poly.getNumInteriorRings() == 8);
  CPPUNIT_ASSERT(poly.getNumRings() == 9);
  poly.add(0);
  poly.push_back(0);
  CPPUNIT_ASSERT(poly.getExteriorRing() == 0);
  CPPUNIT_ASSERT(poly.getNumInteriorRings() == 10);
  CPPUNIT_ASSERT(poly.getNumRings() == 11);
  poly.clear();
  CPPUNIT_ASSERT(poly.getNumInteriorRings() == 0);
  CPPUNIT_ASSERT(poly.getNumRings() == 0);
  CPPUNIT_ASSERT(poly.getArea() == 0.0);
  std::auto_ptr<te::gm::Geometry> centroid1(poly.getCentroid());
  std::auto_ptr<te::gm::Coord2D> centroid2(poly.getCentroidCoord());
  std::auto_ptr<te::gm::Geometry> centroid3(poly.getPointOnSurface());
  std::auto_ptr<te::gm::Coord2D> centroid4(poly.getCoordOnSurface());
  CPPUNIT_ASSERT(poly.getDimension() == te::gm::A);
  CPPUNIT_ASSERT(poly.getCoordinateDimension() == 2);
  CPPUNIT_ASSERT(poly.getGeometryType() == "Polygon");
  CPPUNIT_ASSERT(poly.getGeomTypeId() == te::gm::PolygonType);
  CPPUNIT_ASSERT(poly.getSRID() == 4326);
  poly.setSRID(4128);
  CPPUNIT_ASSERT(poly.getSRID() == 4128);
  poly.setSRID(4326);
  CPPUNIT_ASSERT(poly.getSRID() == 4326);
  //poly.transform(4128);
  //CPPUNIT_ASSERT(poly.getSRID() == 4128);
  std::auto_ptr<te::gm::Geometry> envelope(poly.getEnvelope());
  CPPUNIT_ASSERT(envelope->getGeomTypeId() == te::gm::PolygonType);
  CPPUNIT_ASSERT(poly.getMBR());
  std::string polytext = poly.asText();
  CPPUNIT_ASSERT(poly.asText() == "polygon()");


  CPPUNIT_ASSERT(poly.isEmpty() == true);
  CPPUNIT_ASSERT(poly.isSimple() == true);
  CPPUNIT_ASSERT(poly.isValid() == true);
  CPPUNIT_ASSERT(poly.is3D() == false);
  CPPUNIT_ASSERT(poly.isMeasured() == false);
  std::auto_ptr<te::gm::Geometry> boundary(poly.getBoundary());
  /*size_t np = */poly.getNPoints();
  //CPPUNIT_ASSERT(poly.getNPoints() == 1);

  std::auto_ptr<te::gm::Geometry> clone(static_cast<te::gm::Geometry*>(poly.clone()));
  CPPUNIT_ASSERT(poly.equals(clone.get()));
  CPPUNIT_ASSERT(poly.disjoint(clone.get()) == false);
  CPPUNIT_ASSERT(poly.intersects(clone.get()));
  CPPUNIT_ASSERT(poly.touches(clone.get()));
//#endif
}

void TsGeometry::tcCreatePolygonZ()
{
//#ifdef TE_COMPILE_ALL
//#endif
}

void TsGeometry::tcCreatePolygonM()
{
//#ifdef TE_COMPILE_ALL
//#endif
}

void TsGeometry::tcCreatePolygonZM()
{
//#ifdef TE_COMPILE_ALL
//#endif
}

void TsGeometry::tcTransform()
{
//#ifdef TE_COMPILE_ALL
// load test data
  std::vector<WKBEntry> hwkbVec;
  loadGeometry(hwkbVec);

// for each entry, test geometry basic methods
  std::cout << std::endl <<"   Reading geom from wkt, transforming to New SRID and transforing back to the Orig SRID" << std::endl;
  for(size_t i = 0; i < hwkbVec.size(); ++i)
  {
    const WKBEntry& hwkb = hwkbVec[i];

// converting WKT into a TL geometry
    std::auto_ptr<te::gm::Geometry> g(te::gm::WKTReader::read(hwkb.m_wkt.c_str()));
    CPPUNIT_ASSERT(g.get());
    g->setSRID(hwkb.m_srid);
    std::cout << std::endl;
    std::cout << "   SRID=      " << hwkb.m_srid << " asText= " << hwkb.m_wkt <<  std::endl;

// transforming geom to new SRID=3031 (SRID 4128 is not found??) (SRID 4291 - does not change enough the coordinates)
    CPPUNIT_ASSERT_NO_THROW(g->transform(3031));
    std::string ustring1 = g->asText();
    std::cout << "   New SRID=  " << g->getSRID() << " asText= " << ustring1 << std::endl;

// transforming back to the original SRID
    CPPUNIT_ASSERT_NO_THROW(g->transform(hwkb.m_srid ));
    std::string ustring2 = g->asText();
    std::cout << "   Orig SRID= " << g->getSRID() << " asText= " << ustring2 << std::endl;

    CPPUNIT_ASSERT(g->asText() == te::common::Convert2LCase(hwkb.m_wkt));
    std::cout << std::endl;
  }

//#endif
}

void TsGeometry::tcAsText()
{
//#ifdef TE_COMPILE_ALL
// load test data
  std::vector<WKBEntry> hwkbVec;
  loadGeometry(hwkbVec);

// for each entry, test conversion of geometry to text
  std::cout << std::endl << "  Create a geom from wkt and check geom->asText()..."  << std::endl; 
  for(size_t i = 0; i < hwkbVec.size(); ++i)
  {
    const WKBEntry& hwkb = hwkbVec[i];

// convert WKT into a TL geometry
    std::auto_ptr<te::gm::Geometry> g(te::gm::WKTReader::read(hwkb.m_wkt.c_str()));
    CPPUNIT_ASSERT(g.get());
    g->setSRID(hwkb.m_srid);

// testing g->asText()
    std::cout << std::endl << "  hwkb.m_wkt  = " << hwkb.m_wkt << std::endl; 
    std::cout << std::endl << "  g->asText() = " << g->asText() << std::endl;

    CPPUNIT_ASSERT(g->asText() == te::common::Convert2LCase(hwkb.m_wkt));
  }
  std::cout << std::endl;
//#endif
}

void TsGeometry::tcBasicGeometryMethods()
{
//#ifdef TE_COMPILE_ALL
// load test data
  std::vector<WKBEntry> hwkbVec;
  loadGeometry(hwkbVec);

// for each entry, test geometry basic methods
  for(size_t i = 0; i < hwkbVec.size(); ++i)
  {
    const WKBEntry& hwkb = hwkbVec[i];

// convert HWKB to WKB and read the WKB into a TL geometry
    std::auto_ptr<te::gm::Geometry> g(te::gm::WKBReader::read(te::core::Hex2Binary(hwkb.m_hwkb.c_str())));
    CPPUNIT_ASSERT(g.get());
    g->setSRID(hwkb.m_srid);

// test geometry basic methods
    CPPUNIT_ASSERT(g->getDimension() == static_cast<te::gm::Dimensionality>(hwkb.m_dimensonality));
    CPPUNIT_ASSERT(g->getCoordinateDimension() == hwkb.m_ndims);
    CPPUNIT_ASSERT(te::common::Convert2UCase(g->getGeometryType()) == hwkb.m_geomType);
    CPPUNIT_ASSERT(g->getGeomTypeId() == static_cast<te::gm::GeomType>(hwkb.m_geomTypeId));
    CPPUNIT_ASSERT(g->getSRID() == hwkb.m_srid);   
    std::auto_ptr<te::gm::Geometry> envelope(g->getEnvelope());
    CPPUNIT_ASSERT(envelope->getGeomTypeId() == te::gm::PolygonType);
    CPPUNIT_ASSERT(g->getMBR());

    std::cout << std::endl << g->asText() << std::endl;
    std::cout << "hwkb.m_wkt=" << hwkb.m_wkt << std::endl; 
    size_t wkbSize;
    char* myWkb = g->asBinary(wkbSize);
    char* myHwkb = te::core::Binary2Hex(myWkb, wkbSize);
    delete [] myWkb;
    CPPUNIT_ASSERT(myHwkb == hwkb.m_hwkb);
    delete [] myHwkb;

    CPPUNIT_ASSERT(g->isEmpty() == false);    

    if(g->getGeomTypeId() != te::gm::GeometryCollectionType)
    {
      CPPUNIT_ASSERT(g->isSimple() == true);
      CPPUNIT_ASSERT(g->isValid() == true);
      std::auto_ptr<te::gm::Geometry> boundary;
      CPPUNIT_ASSERT_NO_THROW(boundary.reset(g->getBoundary()));
      CPPUNIT_ASSERT(boundary.get());
    }
    CPPUNIT_ASSERT(g->is3D() == hwkb.m_is3D);
    CPPUNIT_ASSERT(g->isMeasured() == hwkb.m_isMeasured);
  }

//#endif
}

void TsGeometry::tcSpatialRelationsMethods()
{
//#ifdef TE_COMPILE_ALL

//#endif
}

void TsGeometry::loadGeometry(std::vector<WKBEntry>& hwkbVec) const
{
  hwkbVec.clear();

  FILE* f = fopen( te::core::CharEncoding::fromUTF8(TERRALIB_DATA_DIR "/hwkb.csv").c_str(), "r");

  CPPUNIT_ASSERT(f != NULL);

  while(!feof(f))
  {
    std::vector<std::string> values;
    std::string value("");

    while(!feof(f))
    {
      char c = (char)(fgetc(f));
      
      if(c == '\n')
      {
        values.push_back(value);
        break;
      }

      if(c == ';')
      {
        values.push_back(value);
        value = "";
      }
      else
        value += c;
    }

    if(values.size() != 9)
      break;

    WKBEntry fe;

    fe.m_hwkb = values[0];    
    fe.m_srid = atoi(values[1].c_str());
    fe.m_geomType = values[2];
    fe.m_dimensonality = atoi(values[3].c_str());
    fe.m_ndims = atoi(values[4].c_str());
    fe.m_wkt = values[5];
    fe.m_geomTypeId = atoi(values[6].c_str());
    fe.m_is3D = values[7] == "t";
    fe.m_isMeasured = values[8] == "t";

    hwkbVec.push_back(fe);
  }

  fclose(f);
}

