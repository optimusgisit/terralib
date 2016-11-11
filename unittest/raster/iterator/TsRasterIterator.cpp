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
  \file terralib/unittest/raster/TsRasterIterator.cpp
 
  \brief A test suit for the Raster iterator class.
 */

// TerraLib
#include <terralib/raster.h>
#include <terralib/dataaccess.h>
#include <terralib/geometry.h>
#include <terralib/memory.h>
#include <terralib/rp.h>
#include "../Config.h"

// STL
#include <memory>

// Boost
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>

BOOST_AUTO_TEST_SUITE ( rasterIterator_tests )

void drawPolygonInRaster( const te::gm::Polygon& polygon,
  const unsigned char polValue,
  const te::rst::Grid& rasterGrid, const std::string& rasterFileName )
{
  std::unique_ptr< te::rst::Raster > rasterPointer;
  {
    std::vector< te::rst::BandProperty * > bandsProps;
    bandsProps.push_back( new te::rst::BandProperty( 0,
        te::dt::UCHAR_TYPE ) );
    bandsProps[ 0 ]->m_noDataValue = 255;

    std::map< std::string, std::string > rasterInfo;
    rasterInfo[ "URI" ] = rasterFileName;

    rasterPointer.reset( te::rst::RasterFactory::make( "GDAL",
      new te::rst::Grid( rasterGrid ), bandsProps,
      rasterInfo, 0, 0 ) );

    const unsigned int nLines = rasterPointer->getNumberOfRows();
    const unsigned nCols = rasterPointer->getNumberOfColumns();
    unsigned int band = 0;
    unsigned int line = 0;
    unsigned int col = 0;

    for( line = 0 ; line < nLines ; ++line )
    {
      for( col = 0 ; col < nCols ; ++col )
      {
        rasterPointer->setValue( col, line, 0, band );
      }
    }
  }

  te::rst::PolygonIterator< double > it =
      te::rst::PolygonIterator< double >::begin(rasterPointer.get(),
      &polygon);
  const te::rst::PolygonIterator< double > itEnd =
      te::rst::PolygonIterator< double >::end(rasterPointer.get(),
      &polygon);

  int col = 0;
  int row = 0;
  while (it != itEnd)
  {
    rasterPointer->setValue(it.getColumn(), it.getRow(), (double)polValue, 0);
    ++it;
  }
}

void CreateTestRaster( unsigned int nBands, unsigned int nLines,
  unsigned int nCols, boost::shared_ptr< te::rst::Raster >& rasterPointer, bool zero )
{
  std::vector< te::rst::BandProperty * > bandsProps;
  for( unsigned int bandsPropsIdx = 0 ; bandsPropsIdx < nBands ; ++bandsPropsIdx )
  {
    bandsProps.push_back( new te::rst::BandProperty( bandsPropsIdx,
      te::dt::UINT32_TYPE ) );
    bandsProps[ bandsPropsIdx ]->m_noDataValue = 0;
  }
  
  rasterPointer.reset( te::rst::RasterFactory::make( "MEM",
    new te::rst::Grid( nCols, nLines ), bandsProps,
    std::map< std::string, std::string >(), 0, 0 ) );
    
  unsigned int band = 0;
  unsigned int line = 0;
  unsigned int col = 0;
  double pixelValue = 0;
  
  for( band = 0 ; band < nBands ; ++band )
    for( line = 0 ; line < nLines ; ++line )
      for( col = 0 ; col < nCols ; ++col )
      {
        rasterPointer->setValue( col, line, pixelValue, band );
        if (!zero)
          ++pixelValue;
      }
}

void Copy2DiskShp(std::vector<te::gm::Polygon*> polygons, std::string shpName) 
{
    std::auto_ptr< te::mem::DataSet> memDataSetPtr;

    std::auto_ptr<te::da::DataSetType> dataSetTypePtr(new te::da::DataSetType(shpName));

    te::gm::GeometryProperty* propPtr = new te::gm::GeometryProperty("polygon", 0,
        te::gm::PolygonType, true);
    propPtr->setSRID(polygons[0]->getSRID());
    dataSetTypePtr->add(propPtr);

    memDataSetPtr.reset(new te::mem::DataSet(dataSetTypePtr.get()));

    for (unsigned int polygonsIdx = 0; polygonsIdx < polygons.size(); polygonsIdx++)
    {
        te::mem::DataSetItem* dsItemPtr = new te::mem::DataSetItem(memDataSetPtr.get());
        dsItemPtr->setGeometry(0, (te::gm::Geometry*)polygons[polygonsIdx]->clone());

        memDataSetPtr->add(dsItemPtr);
    }

    std::string connInfo("file://" + shpName);

    std::unique_ptr<te::da::DataSource> dsOGR(te::da::DataSourceFactory::make("OGR", connInfo));
    dsOGR->open();

    memDataSetPtr->moveBeforeFirst();

    te::da::Create(dsOGR.get(), dataSetTypePtr.get(), memDataSetPtr.get());

    dsOGR->close();
}

bool Copy2DiskRaster( const te::rst::Raster& inputRaster,
  const std::string& fileName )
{
  if( !(inputRaster.getAccessPolicy() & te::common::RAccess ) )
  {
    return false;
  };
  
  const unsigned int nBands = inputRaster.getNumberOfBands();
  const unsigned int nCols = inputRaster.getNumberOfColumns();
  const unsigned int nRows = inputRaster.getNumberOfRows();
  unsigned int bandIdx =0;
  
  std::vector< te::rst::BandProperty* > bandsProperties;
  for( bandIdx = 0 ; bandIdx < nBands ; ++bandIdx )
  {
    bandsProperties.push_back( new te::rst::BandProperty(
      *( inputRaster.getBand( bandIdx )->getProperty() ) ) );        
  }
  
  std::auto_ptr< te::rst::Raster > outRasterPtr;  
  
  std::map< std::string, std::string > rInfo;
  rInfo[ "URI" ] = fileName;
  
  outRasterPtr.reset( te::rst::RasterFactory::make(
    "GDAL",
    new te::rst::Grid( *inputRaster.getGrid() ),
    bandsProperties,
    rInfo,
    0,
    0 ) );  
  
  double value = 0;
  unsigned int col = 0;
  unsigned int row = 0;
  
  for( bandIdx = 0 ; bandIdx < nBands ; ++bandIdx )
  {
    const te::rst::Band& inBand = *inputRaster.getBand( bandIdx );
    te::rst::Band& outBand = *outRasterPtr->getBand( bandIdx );
    
    for( row = 0 ; row < nRows ; ++row )
    {
      for( col = 0 ; col < nCols ; ++col )
      {
        inBand.getValue( col, row, value );
        outBand.setValue( col, row, value );
      }
    }
  }
  
  return true;
}

te::gm::LinearRing* createSquare(const double& xCenter, const double& yCenter,
                                                   const double& lateralSize, int srid)
{
    te::gm::LinearRing* s = new te::gm::LinearRing(5, te::gm::LineStringType, srid);
    double halfSize = lateralSize * 0.5;
    s->setPoint(0, xCenter - halfSize, yCenter - halfSize); // lower left
    s->setPoint(1, xCenter - halfSize, yCenter + halfSize); // upper left
    s->setPoint(2, xCenter + halfSize, yCenter + halfSize); // upper rigth
    s->setPoint(3, xCenter + halfSize, yCenter - halfSize); // lower rigth
    s->setPoint(4, xCenter - halfSize, yCenter - halfSize); // closing
    return s;
}

te::gm::LinearRing* createRotatedSquare(const double& xCenter, const double& yCenter,
                                                   const double& diagonalSize, int srid)
{
    te::gm::LinearRing* s = new te::gm::LinearRing(5, te::gm::LineStringType, srid);
    double halfSize = diagonalSize * 0.5;
    s->setPoint(0, xCenter - halfSize,  yCenter);
    s->setPoint(1, xCenter,             yCenter + halfSize);
    s->setPoint(2, xCenter + halfSize,  yCenter);
    s->setPoint(3, xCenter,             yCenter - halfSize);
    s->setPoint(4, xCenter - halfSize,  yCenter);
    return s;
}

BOOST_AUTO_TEST_CASE (rasterIteratorConstructor1_test)
{
}

BOOST_AUTO_TEST_CASE (rasterIteratorConstructor2_test)
{
}

BOOST_AUTO_TEST_CASE (rasterIteratorConstructor3_test)
{
}

BOOST_AUTO_TEST_CASE (rasterIteratorGetRow_test)
{
}

BOOST_AUTO_TEST_CASE (rasterIteratorGetCol_test)
{
}

BOOST_AUTO_TEST_CASE (polygonIterator1_test)
{
  boost::shared_ptr< te::rst::Raster > rasterPointer;
  CreateTestRaster( 3, 3, 3, rasterPointer, false );
  Copy2DiskRaster( *rasterPointer, "polygonIterator1_test.tif" );
  
  std::auto_ptr< te::gm::Polygon > polPtr( (te::gm::Polygon*)
    te::gm::GetGeomFromEnvelope( rasterPointer->getGrid()->getExtent(),
      rasterPointer->getGrid()->getSRID() ) );
    
  double pixelValueCounter = 0;

  for( unsigned int band = 0 ; band < rasterPointer->getNumberOfBands() ;
    ++band )
  {
    te::rst::PolygonIterator< double > it =
      te::rst::PolygonIterator< double >::begin( rasterPointer.get(),
      polPtr.get() );
    const te::rst::PolygonIterator< double > itEnd =
      te::rst::PolygonIterator< double >::end( rasterPointer.get(),
      polPtr.get() );
      
    int col = 0;
    int row = 0;
    int count = 0;
    while (it != itEnd)
    {
      BOOST_CHECK_EQUAL((double)pixelValueCounter, it[band]);
      BOOST_CHECK_EQUAL(row, (int)it.getRow());
      BOOST_CHECK_EQUAL(col, (int)it.getColumn());

      ++pixelValueCounter;
      ++it;
      count++;

      if (col < rasterPointer->getNumberOfColumns() - 1)
        col++;
      else {
        row++;
        col = 0;
      }
    }

    BOOST_CHECK_EQUAL(count, 9);
  }
}

BOOST_AUTO_TEST_CASE (polygonIterator2_test)
{
    boost::shared_ptr< te::rst::Raster > rasterPointer;
    {
      const unsigned int nLines = 30;
      const unsigned int nCols = 30;
      
      std::vector< te::rst::BandProperty * > bandsProps;
      bandsProps.push_back( new te::rst::BandProperty( 0,
          te::dt::UCHAR_TYPE ) );
      bandsProps[ 0 ]->m_noDataValue = 0;
      
      rasterPointer.reset( te::rst::RasterFactory::make( "MEM",
        new te::rst::Grid( nCols, nLines ), bandsProps,
        std::map< std::string, std::string >(), 0, 0 ) );
        
      unsigned int band = 0;
      unsigned int line = 0;
      unsigned int col = 0;
      
      for( line = 0 ; line < nLines ; ++line )
      {
        for( col = 0 ; col < nCols ; ++col )
        {
          rasterPointer->setValue( col, line, 0, band );
        }
      }
    }

    std::auto_ptr< te::gm::Polygon > internalPol_1( new te::gm::Polygon(0, te::gm::PolygonType,
      rasterPointer->getSRID()) );
    te::gm::LinearRing* l1 = createSquare(2, -2, 1, rasterPointer->getSRID());
    internalPol_1->add(l1);

    std::auto_ptr< te::gm::Polygon > internalPol_2( new te::gm::Polygon(0, te::gm::PolygonType,
      rasterPointer->getSRID()) );
    te::gm::LinearRing* l2 = createSquare(3, -5.5, 3, rasterPointer->getSRID());
    internalPol_2->add(l2);

    std::auto_ptr< te::gm::Polygon > internalPol_3( new te::gm::Polygon(0, te::gm::PolygonType,
      rasterPointer->getSRID()) );
    te::gm::LinearRing* l3 = createSquare(7, -6, 2.5, rasterPointer->getSRID());
    internalPol_3->add(l3);
    
    std::auto_ptr< te::gm::Polygon > internalPol_4( new te::gm::Polygon(0, te::gm::PolygonType,
      rasterPointer->getSRID()) );
    te::gm::LinearRing* l4 = new te::gm::LinearRing(6, te::gm::LineStringType, rasterPointer->getSRID());
    l4->setPoint(0, 4.5, -2.5); // starting
    l4->setPoint(1, 6.5, -3.25);
    l4->setPoint(2, 7.5, -2.75);
    l4->setPoint(3, 7.5, -1.5);
    l4->setPoint(4, 5.5, -1.5);
    l4->setPoint(5, 4.5, -2.5); // closing
    internalPol_4->add(l4);
    
    std::auto_ptr< te::gm::Polygon > internalPol_5( new te::gm::Polygon(0, te::gm::PolygonType,
      rasterPointer->getSRID()) );
    te::gm::LinearRing* l5 = new te::gm::LinearRing(13, te::gm::LineStringType, rasterPointer->getSRID());
    l5->setPoint(0, 9, -1);
    l5->setPoint(1, 10, -1);
    l5->setPoint(2, 10, -2);
    l5->setPoint(3, 12, -2);
    l5->setPoint(4, 12, -1);
    l5->setPoint(5, 16, -1);
    l5->setPoint(6, 16, -3);
    l5->setPoint(7, 15, -3);
    l5->setPoint(8, 15, -2);
    l5->setPoint(9, 13, -2);
    l5->setPoint(10, 13, -3);
    l5->setPoint(11, 9, -3);
    l5->setPoint(12, 9, -1);
    internalPol_5->add(l5);
    
    std::auto_ptr< te::gm::Polygon > internalPol_6( new te::gm::Polygon(0, te::gm::PolygonType,
      rasterPointer->getSRID()) );
    te::gm::LinearRing* l6 = new te::gm::LinearRing(5, te::gm::LineStringType, rasterPointer->getSRID());
    l6->setPoint(0, 9, -5);
    l6->setPoint(1, 10, -5);
    l6->setPoint(2, 10, -6);
    l6->setPoint(3, 9, -6);
    l6->setPoint(4, 9, -5);
    internalPol_6->add(l6);
    
    std::auto_ptr< te::gm::Polygon > internalPol_7( new te::gm::Polygon(0, te::gm::PolygonType,
      rasterPointer->getSRID()) );
    te::gm::LinearRing* l7 = createSquare(14, -6.5, 4, rasterPointer->getSRID());
    internalPol_7->add( l7 );    
    
    std::auto_ptr< te::gm::Polygon > internalPol_8( new te::gm::Polygon(0, te::gm::PolygonType,
      rasterPointer->getSRID()) );
    te::gm::LinearRing* l8 = createRotatedSquare(4, -11, 4, rasterPointer->getSRID());
    internalPol_8->add( l8 );     
    
    std::auto_ptr< te::gm::Polygon > internalPol_9( new te::gm::Polygon(0, te::gm::PolygonType,
      rasterPointer->getSRID()) );
    te::gm::LinearRing* l9 = createRotatedSquare(9.5, -11.5, 4, rasterPointer->getSRID());
    internalPol_9->add( l9 );         
    
    std::auto_ptr< te::gm::Polygon > internalPol_10( new te::gm::Polygon(0, te::gm::PolygonType,
      rasterPointer->getSRID()) );
    te::gm::LinearRing* l10 = createRotatedSquare(4, -17, 5, rasterPointer->getSRID());
    internalPol_10->add( l10 );    
    
    std::auto_ptr< te::gm::Polygon > internalPol_11( new te::gm::Polygon(0, te::gm::PolygonType,
      rasterPointer->getSRID()) );
    te::gm::LinearRing* l11 = createRotatedSquare(11.5, -17.5, 5, rasterPointer->getSRID());
    internalPol_11->add( l11 );       
    
    std::auto_ptr< te::gm::Polygon > externalPol( (te::gm::Polygon*) te::gm::GetGeomFromEnvelope(
      rasterPointer->getGrid()->getExtent(), rasterPointer->getGrid()->getSRID()) );
    externalPol->add((te::gm::LinearRing*)l1->clone());
    externalPol->add((te::gm::LinearRing*)l2->clone());
    externalPol->add((te::gm::LinearRing*)l3->clone());
    externalPol->add((te::gm::LinearRing*)l4->clone());    
    externalPol->add((te::gm::LinearRing*)l5->clone());    
    externalPol->add((te::gm::LinearRing*)l6->clone());    
    externalPol->add((te::gm::LinearRing*)l7->clone());    
    externalPol->add((te::gm::LinearRing*)l8->clone());    
    externalPol->add((te::gm::LinearRing*)l9->clone());    
    externalPol->add((te::gm::LinearRing*)l10->clone());    
    externalPol->add((te::gm::LinearRing*)l11->clone());    
    
    // Polygons vector
    
    std::vector<te::gm::Polygon* > polVector;
    polVector.push_back(internalPol_1.get());    
    polVector.push_back(internalPol_2.get());    
    polVector.push_back(internalPol_3.get());    
    polVector.push_back(internalPol_4.get());    
    polVector.push_back(internalPol_5.get());    
    polVector.push_back(internalPol_6.get());    
    polVector.push_back(internalPol_7.get());    
    polVector.push_back(internalPol_8.get());    
    polVector.push_back(internalPol_9.get());    
    polVector.push_back(internalPol_10.get());    
    polVector.push_back(internalPol_11.get());    
    polVector.push_back(externalPol.get());
    
    Copy2DiskShp( polVector, "polygonIterator2_test.shp" );

    double pixelValueCounter = 0;

    for (int countPol = 0; countPol < polVector.size(); countPol++)
    {
      te::rst::PolygonIterator< double > it =
          te::rst::PolygonIterator< double >::begin(rasterPointer.get(),
          polVector[countPol]);
      const te::rst::PolygonIterator< double > itEnd =
          te::rst::PolygonIterator< double >::end(rasterPointer.get(),
          polVector[countPol]);

      int col = 0;
      int row = 0;
      while (it != itEnd)
      {
          if (it[0] == 0)
              rasterPointer->setValue(it.getColumn(), it.getRow(), (countPol + 1), 0);
          else
              rasterPointer->setValue(it.getColumn(), it.getRow(), 15, 0);

          ++it;
      }
    }
    
    Copy2DiskRaster( *rasterPointer, "polygonIterator2_test.tif" );
}

BOOST_AUTO_TEST_CASE (polygonIterator3_outsidepol_test)
{
  te::rst::Grid rasterGrid( 3, 3 );
  std::vector<te::gm::Polygon*> polVector( 1 );

  {
    std::auto_ptr< te::gm::Polygon > polygonPtr( new te::gm::Polygon(0, te::gm::PolygonType,
      0) );
    te::gm::LinearRing* linearRingPtr = createSquare(0, 0, 2.0, 0);
    polygonPtr->add(linearRingPtr);
    polVector[ 0 ] = polygonPtr.get();
    Copy2DiskShp( polVector, "polygonIterator3_outsidepol_test1.shp" );
    drawPolygonInRaster( *polygonPtr, 1, rasterGrid,
      "polygonIterator3_outsidepol_test1.tif" );
  }

  {
    std::auto_ptr< te::gm::Polygon > polygonPtr( new te::gm::Polygon(0, te::gm::PolygonType,
      0) );
    te::gm::LinearRing* linearRingPtr = createSquare(
      rasterGrid.getExtent()->m_urx,
      rasterGrid.getExtent()->m_lly,
      2.0, 0);
    polygonPtr->add(linearRingPtr);
    polVector[ 0 ] = polygonPtr.get();
    Copy2DiskShp( polVector, "polygonIterator3_outsidepol_test2.shp" );
    drawPolygonInRaster( *polygonPtr, 1, rasterGrid,
      "polygonIterator3_outsidepol_test2.tif" );
  }

  {
    std::auto_ptr< te::gm::Polygon > polygonPtr( new te::gm::Polygon(0, te::gm::PolygonType,
      0) );
    te::gm::LinearRing* linearRingPtr = createSquare(
      rasterGrid.getExtent()->getCenter().x,
      rasterGrid.getExtent()->getCenter().y,
      4.0, 0);
    polygonPtr->add(linearRingPtr);
    polVector[ 0 ] = polygonPtr.get();
    Copy2DiskShp( polVector, "polygonIterator3_outsidepol_test3.shp" );
    drawPolygonInRaster( *polygonPtr, 1, rasterGrid,
      "polygonIterator3_outsidepol_test3.tif" );
  }

  {
    std::auto_ptr< te::gm::Polygon > polygonPtr( new te::gm::Polygon(0, te::gm::PolygonType,
      0) );
    te::gm::LinearRing* linearRingPtr = createSquare(
      rasterGrid.getExtent()->m_urx +
      rasterGrid.getResolutionX(),
      rasterGrid.getExtent()->m_ury +
      rasterGrid.getResolutionY(),
      2.0, 0);
    polygonPtr->add(linearRingPtr);
    polVector[ 0 ] = polygonPtr.get();
    Copy2DiskShp( polVector, "polygonIterator3_outsidepol_test4.shp" );
    drawPolygonInRaster( *polygonPtr, 1, rasterGrid,
      "polygonIterator3_outsidepol_test4.tif" );
  }

  {
    std::auto_ptr< te::gm::Polygon > polygonPtr( new te::gm::Polygon(0, te::gm::PolygonType,
      0) );
    te::gm::LinearRing* linearRingPtr = createSquare(
      rasterGrid.getExtent()->m_urx +
      ( 2 * rasterGrid.getResolutionX() ),
      rasterGrid.getExtent()->getCenter().y,
      2.0, 0);
    polygonPtr->add(linearRingPtr);
    polVector[ 0 ] = polygonPtr.get();
    Copy2DiskShp( polVector, "polygonIterator3_outsidepol_test5.shp" );
    drawPolygonInRaster( *polygonPtr, 1, rasterGrid,
      "polygonIterator3_outsidepol_test5.tif" );
  }

  {
    std::auto_ptr< te::gm::Polygon > polygonPtr( new te::gm::Polygon(0, te::gm::PolygonType,
      0) );
    te::gm::LinearRing* linearRingPtr = new te::gm::LinearRing(9, te::gm::LineStringType, 0);
    linearRingPtr->setPoint(0, -1, 1);
    linearRingPtr->setPoint(1, 0, 1);
    linearRingPtr->setPoint(2, 0, -3);
    linearRingPtr->setPoint(3, 3, -3);
    linearRingPtr->setPoint(4, 3, 1);
    linearRingPtr->setPoint(5, 4, 1);
    linearRingPtr->setPoint(6, 4, -4);
    linearRingPtr->setPoint(7, -1, -4);
    linearRingPtr->setPoint(8, -1, 1);
    polygonPtr->add(linearRingPtr);
    polVector[ 0 ] = polygonPtr.get();
    Copy2DiskShp( polVector, "polygonIterator3_outsidepol_test6.shp" );
    drawPolygonInRaster( *polygonPtr, 1, rasterGrid,
      "polygonIterator3_outsidepol_test6.tif" );
  }
}

BOOST_AUTO_TEST_SUITE_END ()
