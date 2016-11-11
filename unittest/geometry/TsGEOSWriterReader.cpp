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
#include "TsGEOSWriterReader.h"
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

CPPUNIT_TEST_SUITE_REGISTRATION( TsGEOSWriterReader );

void TsGEOSWriterReader::setUp()
{
}

void TsGEOSWriterReader::tearDown()
{
}

void TsGEOSWriterReader::tcCreateGeometriesAndApplyReadTests()
{
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

// Suppose WKTReader is ok...
    //te::gm::WKTReader rt;
    //te::gm::Geometry* g1 = rt.read("POLYGON((10 18,15 18,15 22,10 22,10 18),(11 20,12 20,12 21,11 21,11 20),(13 19,14 19,14 20,13 20,13 19))");

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
  }

  //#endif
}

void TsGEOSWriterReader::tcCreatePointsFromWKTAndApplyGEOSTests()
{
//#ifdef TE_COMPILE_ALL

// load geometries from a file
  std::vector<std::string> wktGeom;

  loadWKT("/data/wkt_point.txt", wktGeom);
  for(size_t i = 0; i < wktGeom.size(); ++i)
  {
    std::string rr_wkt = wktGeom[i];

// Suppose WKTReader is ok...
    te::gm::Geometry* g1;
    CPPUNIT_ASSERT_NO_THROW(g1 = te::gm::WKTReader::read(rr_wkt.c_str()));
 
// Testing GeosWriter and GeosReader -------
    geos::geom::Geometry* geos_geom = te::gm::GEOSWriter::write(g1);
    te::gm::Geometry* tl_geom_from_geos = te::gm::GEOSReader::read(geos_geom);
    CPPUNIT_ASSERT(g1->equals(tl_geom_from_geos));
  }

//#endif
}

void TsGEOSWriterReader::tcCreateLinesFromWKTAndApplyGEOSTests()
{
//#ifdef TE_COMPILE_ALL

// load geometries from a file
  std::vector<std::string> wktGeom;

  loadWKT("/data/wkt_line.txt", wktGeom);
  for(size_t i = 0; i < wktGeom.size(); ++i)
  {
    std::string rr_wkt = wktGeom[i];

// Suppose WKTReader is ok...
    te::gm::Geometry* g1;
    CPPUNIT_ASSERT_NO_THROW(g1 = te::gm::WKTReader::read(rr_wkt.c_str()));
 
// Testing GeoWriter and GeosReader -------
    geos::geom::Geometry* geos_geom = te::gm::GEOSWriter::write(g1);
    te::gm::Geometry* tl_geom_from_geos = te::gm::GEOSReader::read(geos_geom);
    CPPUNIT_ASSERT(g1->equals(tl_geom_from_geos));

  }

  //#endif
}

void TsGEOSWriterReader::tcCreatePolygonFromWKTAndApplyGEOSTests()
{
//#ifdef TE_COMPILE_ALL

// load geometries from a file
  std::vector<std::string> wktGeom;

  loadWKT("/data/wkt_polygon.txt", wktGeom);
  for(size_t i = 0; i < wktGeom.size(); ++i)
  {
    std::string rr_wkt = wktGeom[i];

// Suppose WKTReader is ok...
    te::gm::Geometry* g1;
    CPPUNIT_ASSERT_NO_THROW(g1 = te::gm::WKTReader::read(rr_wkt.c_str()));

// Testing GeoWriter and GeosReader -------
    geos::geom::Geometry* geos_geom = te::gm::GEOSWriter::write(g1);
    te::gm::Geometry* tl_geom_from_geos = te::gm::GEOSReader::read(geos_geom);
    CPPUNIT_ASSERT(g1->equals(tl_geom_from_geos));
  }
//#endif
}

void TsGEOSWriterReader::tcCreateGeomCollectionFromWKTAndApplyGEOSTests()
{
//#ifdef TE_COMPILE_ALL

// load geometries from a file
  std::vector<std::string> wktGeom;

  loadWKT("/data/wkt_geomcollection.txt", wktGeom);
  for(size_t i = 0; i < wktGeom.size(); ++i)
  {
    std::string rr_wkt = wktGeom[i];

// Suppose WKTReader is ok...
    te::gm::Geometry* g1;
    CPPUNIT_ASSERT_NO_THROW(g1 = te::gm::WKTReader::read(rr_wkt.c_str()));
 
// Testing GeoWriter and GeosReader -------
    geos::geom::Geometry* geos_geom = te::gm::GEOSWriter::write(g1);
    te::gm::Geometry* tl_geom_from_geos = te::gm::GEOSReader::read(geos_geom);
    CPPUNIT_ASSERT(g1->equals(tl_geom_from_geos));
  }
//#endif
}

void TsGEOSWriterReader::loadGeometry(std::vector<WKBEntry>& hwkbVec) const
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

void TsGEOSWriterReader::loadWKT(std::vector<std::string>& geom_wkt) const
{
  geom_wkt.clear();

  FILE* f = fopen( te::core::CharEncoding::fromUTF8(TERRALIB_DATA_DIR "/wkt_line.txt").c_str(), "r");

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

void TsGEOSWriterReader::loadWKT(std::string filewkt, std::vector<std::string>& geom_wkt) const
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
