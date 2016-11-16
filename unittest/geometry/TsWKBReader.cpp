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
#include "TsWKBReader.h"
#include "../Config.h"

// TerraLib
#include <terralib/common.h>
#include <terralib/common/Globals.h>
#include <terralib/core/encoding/CharEncoding.h>
#include <terralib/geometry.h>
#include <terralib/geometry/GEOSWriter.h>
#include <terralib/geometry/GEOSReader.h>
#include <terralib/core/utils/HexUtils.h>

// STL
#include <cstdio>
#include <cstdlib>
#include <cstring>

CPPUNIT_TEST_SUITE_REGISTRATION( TsWKBReader );

void TsWKBReader::setUp()
{
}

void TsWKBReader::tearDown()
{
}

void TsWKBReader::tcCreateGeometriesAndApplyReadTests()
{
  /*

  WKTreader r;
  te::gm::Geometry* g1 = r.read("POLYGON((10 18,15 18,15 22,10 22,10 18),(11 20,12 20,12 21,11 21,11 20),(13 19,14 19,14 20,13 20,13 19))");

// testando o wkb
  std::size_t wkb1size = 0;
  wkb1size = g1->getWKBSize();
  char* wkb1 = new char[wkb1size];
  g1->getWKB(wkb1);

  te::gm::Geometry* g2 = te::gm::Geometry::getGeomFromWKB(wkb1);

  ASSERT(g1->equals(g2)); // confirma se a escrita/leitura est� ok

  //////// posso desprezar
  wkb2size = g2->getWKBSize();

  assert(wkb1size == wkb2size);

  char* wkb2 = new char[wkb2size];
  g2->getWKB(wkb2);

  assert(memcmp(wkb1, wkb2) == 0);


  */
//#ifdef TE_COMPILE_ALL

// load geometries from a file
  std::vector<WKBEntry> hwkbVec;
  loadGeometry(hwkbVec);

  for(size_t i = 0; i < hwkbVec.size(); ++i)
  {
    const WKBEntry& hwkb = hwkbVec[i];

// convert WKT to a geometry
    te::gm::Geometry* g1(te::gm::WKTReader::read(hwkb.m_wkt.c_str()));
    g1->setSRID(hwkb.m_srid);

    std::size_t wkb1size = 0;
    wkb1size = g1->getWkbSize();
    char* wkb1 = new char[wkb1size];
    g1->getWkb(wkb1,te::common::Globals::sm_machineByteOrder);

// create a new geometry g2 using wkb1 from g1 => g2 must be equal g1
    te::gm::Geometry* g2 = te::gm::WKBReader::read(wkb1);
    CPPUNIT_ASSERT(g2->equals(g1));

// checking hex and binary
    char* hwkb1 = te::core::Binary2Hex(wkb1, wkb1size);
    std::size_t wkb2size = g2->getWkbSize();
    CPPUNIT_ASSERT(wkb1size == wkb2size);

    char* wkb2 = new char[wkb2size];
    g2->getWkb(wkb2,te::common::Globals::sm_machineByteOrder);
    char* hwkb2 = te::core::Binary2Hex(wkb2, wkb2size);

    CPPUNIT_ASSERT(strcmp(hwkb1, hwkb2) == 0);
    CPPUNIT_ASSERT(memcmp(wkb1, wkb2, wkb2size) == 0);

// Testing WKBReader
    te::gm::Geometry* geomRead;
    CPPUNIT_ASSERT_NO_THROW(geomRead = te::gm::WKBReader::read(wkb1));
    CPPUNIT_ASSERT(geomRead);
    CPPUNIT_ASSERT(geomRead->equals(g1));
    size_t size1 = geomRead->getWkbSize();
    char* wkb1Read = new char [size1];
    geomRead->getWkb(wkb1Read, te::common::Globals::sm_machineByteOrder);
    CPPUNIT_ASSERT(memcmp(wkb1, wkb1Read, size1) == 0);

    const char* validHWKB = hwkb.m_hwkb.c_str();
    CPPUNIT_ASSERT(strcmp(hwkb1, validHWKB) == 0);

    delete [] wkb1;
    delete [] wkb2;
    delete [] hwkb1;
    delete [] hwkb2;
    delete [] wkb1Read;
    delete g1; delete g2; delete geomRead;
  }

  //#endif
}

void TsWKBReader::tcCreateGeomFromWKTAndApplyReadTests()
{
//#ifdef TE_COMPILE_ALL

// load geometries from a file
  std::vector<std::string> wktGeom;

  loadWKT("/data/wkt_geom.txt", wktGeom);
  for(size_t i = 0; i < wktGeom.size(); ++i)
  {
    std::string rr_wkt = wktGeom[i];

// Suppose WKTReader is ok...
    te::gm::Geometry* g1;
    CPPUNIT_ASSERT_NO_THROW(g1 = te::gm::WKTReader::read(rr_wkt.c_str()));

    std::size_t wkb1size = 0;
    wkb1size = g1->getWkbSize();
    char* wkb1 = new char[wkb1size];
    g1->getWkb(wkb1,te::common::Globals::sm_machineByteOrder);

// Testing GeoWriter and GeosReader -------
    geos::geom::Geometry* geos_geom = te::gm::GEOSWriter::write(g1);
    te::gm::Geometry* tl_geom_from_geos = te::gm::GEOSReader::read(geos_geom);
    CPPUNIT_ASSERT(g1->equals(tl_geom_from_geos));
// End of test GeoWriter and GeosReader ---


// Testing WKBReader based on g1 and wkb1
    te::gm::Geometry* geomRead;
    CPPUNIT_ASSERT_NO_THROW(geomRead = te::gm::WKBReader::read(wkb1));
    CPPUNIT_ASSERT(geomRead->equals(g1));
    te::gm::Geometry* g2 = te::gm::WKBReader::read(wkb1);
    CPPUNIT_ASSERT(g2->equals(g1));

    size_t size1 = geomRead->getWkbSize();
    char* wkb1Read = new char [size1];
    geomRead->getWkb(wkb1Read, te::common::Globals::sm_machineByteOrder);
    CPPUNIT_ASSERT(memcmp(wkb1Read, wkb1, size1) == 0);

    std::size_t wkb2size = g2->getWkbSize();
    char* wkb2 = new char[wkb2size];
    g2->getWkb(wkb2,te::common::Globals::sm_machineByteOrder);
    CPPUNIT_ASSERT(wkb1size == wkb2size);
    CPPUNIT_ASSERT(memcmp(wkb1, wkb2, wkb2size) == 0);

//Testing Binary2Hex
    char* hwkb1 = te::core::Binary2Hex(wkb1, wkb1size);
    char* hwkb2 = te::core::Binary2Hex(wkb2, wkb2size);
    CPPUNIT_ASSERT(strcmp(hwkb1, hwkb2) == 0);

    delete [] wkb1;
    delete [] wkb2;
    delete [] hwkb1;
    delete [] hwkb2;
    delete [] wkb1Read;
    delete g1; delete g2; delete geomRead;
    //delete geos_geom; //TODO: deletion of pointer to incomplete type geos::geom::Geometry - no destructor called
  }

  //#endif
}

void TsWKBReader::tcBasicGeometryMethods()
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
    //CPPUNIT_ASSERT_NO_THROW(g->transform(4128));
    //CPPUNIT_ASSERT(g->getSRID() == 4128);
    std::auto_ptr<te::gm::Geometry> envelope(g->getEnvelope());
    CPPUNIT_ASSERT(envelope->getGeomTypeId() == te::gm::PolygonType);
    CPPUNIT_ASSERT(g->getMBR());
    std::cout << hwkb.m_wkt << std::endl;
    std::string  ustring = g->asText();
    //CPPUNIT_ASSERT_MESSAGE("multipolygon->asText() in not working",  te::common::Convert2UCase(g->asText()) == hwkb.m_wkt);
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

void TsWKBReader::loadGeometry(std::vector<WKBEntry>& hwkbVec) const
{
  hwkbVec.clear();

  FILE* f = fopen( TERRALIB_DATA_DIR "/hwkb.csv", "r");

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

void TsWKBReader::loadWKT(std::vector<std::string>& geom_wkt) const
{
  geom_wkt.clear();

  FILE* f = fopen( te::core::CharEncoding::fromUTF8(TERRALIB_DATA_DIR "/wkt_line.txt").c_str() , "r");

  CPPUNIT_ASSERT(f != NULL);

  while(!feof(f))
  {
    std::vector<std::string>  values;
    std::string value("");

    while(!feof(f))
    {
      char c = (char)(fgetc(f));

      if(c == '\n')
      {
        geom_wkt.push_back(value);
        break;
      }
      else
        value += c;
    }
  }
  fclose(f);
}

void TsWKBReader::loadWKT(std::string filewkt, std::vector<std::string>& geom_wkt) const
{
  geom_wkt.clear();

  std::string  filename = TERRALIB_DATA_DIR  + filewkt;
  FILE* f = fopen( te::core::CharEncoding::fromUTF8(filename).c_str() , "r");

  CPPUNIT_ASSERT(f != NULL);

  while(!feof(f))
  {
    std::vector<std::string>  values;
    std::string value("");

    while(!feof(f))
    {
      char c = (char)(fgetc(f));

      if(c == '\n')
      {
        geom_wkt.push_back(value);
        break;
      }
      else
        value += c;
    }
  }
  fclose(f);
}
