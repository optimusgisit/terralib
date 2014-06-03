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
  \file TsBlender.cpp

  \brief A test suit for the Blender interface.
 */

#include "TsBlender.h"
#include "../Config.h"

#include <terralib/rp/Blender.h>
#include <terralib/raster/Raster.h>
#include <terralib/raster/Band.h>
#include <terralib/raster/BandProperty.h>
#include <terralib/raster/RasterFactory.h>
#include <terralib/geometry/GTFactory.h>
#include <terralib/common/progress/ConsoleProgressViewer.h>
#include <terralib/common/progress/ProgressManager.h>

#include <memory>

CPPUNIT_TEST_SUITE_REGISTRATION( TsBlender );

void TsBlender::PixelByPixelNoBlendTest()
{
  // openning input rasters
  
  std::map<std::string, std::string> auxRasterInfo;
  
  auxRasterInfo["URI"] = TE_DATA_DIR "/data/rasters/cbers_rgb342_crop2.tif";
  std::auto_ptr< te::rst::Raster > inputRaster1Pointer ( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  CPPUNIT_ASSERT( inputRaster1Pointer.get() );    
  
  auxRasterInfo["URI"] = TE_DATA_DIR "/data/rasters/cbers_rgb342_crop1.tif";
  std::auto_ptr< te::rst::Raster > inputRaster2Pointer ( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  CPPUNIT_ASSERT( inputRaster2Pointer.get() );
  
  // Creating blender parameters
  
  std::vector< unsigned int > raster1Bands;
  raster1Bands.push_back( 0 );
  raster1Bands.push_back( 1 );
  raster1Bands.push_back( 2 );
  
  std::vector< unsigned int > raster2Bands;
  raster2Bands.push_back( 1 );
  raster2Bands.push_back( 2 );
  raster2Bands.push_back( 0 );
  
  const std::vector< double > pixelOffsets1( 3, 0.0 );
  const std::vector< double > pixelScales1( 3, 1.0 );
  const std::vector< double > pixelOffsets2( 3, 0.0 );
  const std::vector< double > pixelScales2( 3, 1.0 );
  
  te::gm::GTParameters transParams;
  transParams.m_tiePoints.push_back( te::gm::GTParameters::TiePoint( 
     te::gm::Coord2D( 292, 538 ), te::gm::Coord2D( 0, 0 ) ) );
  transParams.m_tiePoints.push_back( 
    te::gm::GTParameters::TiePoint( 
      te::gm::Coord2D( 
        transParams.m_tiePoints[ 0 ].first.x + inputRaster2Pointer->getNumberOfColumns() - 1, 
        transParams.m_tiePoints[ 0 ].first.y + inputRaster2Pointer->getNumberOfRows() - 1
      ), 
      te::gm::Coord2D( 
        inputRaster2Pointer->getNumberOfColumns() - 1,
        inputRaster2Pointer->getNumberOfRows() - 1 ) 
      ) 
    );        
  std::auto_ptr< te::gm::GeometricTransformation > transPtr( 
    te::gm::GTFactory::make( "RST" ) );
  CPPUNIT_ASSERT( transPtr->initialize( transParams ) );    
  
  // Initiating the blender instance
    
  te::rp::Blender blender;
  
  CPPUNIT_ASSERT( blender.initialize(
    *inputRaster1Pointer,
    raster1Bands,
    *inputRaster2Pointer,
    raster2Bands,
    te::rp::Blender::NoBlendMethod,
    te::rst::Interpolator::NearestNeighbor,
    te::rst::Interpolator::NearestNeighbor,
    0.0,
    false,
    pixelOffsets1,
    pixelScales1,
    pixelOffsets2,
    pixelScales2,
    0,
    0,
    *transPtr,
    1,
    false ) );
  
  // Creating the output image
  
  std::map<std::string, std::string> outputRasterInfo;
  outputRasterInfo["URI"] = "terralib_unittest_rp_Blender_PixelByPixelNoBlendTest_Test.tif";
  
  std::vector< te::rst::BandProperty* > bandsProperties;
  for( unsigned int inRasterBandsIdx = 0 ; inRasterBandsIdx <
    raster1Bands.size() ; ++inRasterBandsIdx )
  {
    bandsProperties.push_back( new te::rst::BandProperty(
      *( inputRaster1Pointer->getBand( raster1Bands[ inRasterBandsIdx ] )->getProperty() ) ) );
  }
  
  te::rst::Grid* gridPtr = new te::rst::Grid( *( inputRaster1Pointer->getGrid() ) );
  gridPtr->setNumberOfColumns( gridPtr->getNumberOfColumns() * 2 );
  gridPtr->setNumberOfRows( gridPtr->getNumberOfRows() * 2 );
  
  std::auto_ptr< te::rst::Raster > outRasterPtr( te::rst::RasterFactory::make(
    "GDAL",
    gridPtr,
    bandsProperties,
    outputRasterInfo,
    0,
    0 ) );
    
  CPPUNIT_ASSERT( outRasterPtr.get() );
  
  std::vector< double > values( outRasterPtr->getNumberOfBands() );
  for( unsigned int row = 0 ; row < outRasterPtr->getNumberOfRows() ; ++row )
  {
    for( unsigned int col = 0 ; col < outRasterPtr->getNumberOfColumns() ; ++col )
    {
      blender.getBlendedValues( row, col, values );
      
      for( unsigned int band = 0 ; band < values.size() ; ++band )
      {
        outRasterPtr->setValue( col, row, values[ band ], band );
      }
    }
  }
}

void TsBlender::PixelByPixelEucBlendTest()
{
  // openning input rasters
  
  std::map<std::string, std::string> auxRasterInfo;
  
  auxRasterInfo["URI"] = TE_DATA_DIR "/data/rasters/cbers_rgb342_crop2.tif";
  std::auto_ptr< te::rst::Raster > inputRaster1Pointer ( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  CPPUNIT_ASSERT( inputRaster1Pointer.get() );    
  
  auxRasterInfo["URI"] = TE_DATA_DIR "/data/rasters/cbers_rgb342_crop1.tif";
  std::auto_ptr< te::rst::Raster > inputRaster2Pointer ( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  CPPUNIT_ASSERT( inputRaster2Pointer.get() );
  
  // Creating blender parameters
  
  std::vector< unsigned int > raster1Bands;
  raster1Bands.push_back( 0 );
  raster1Bands.push_back( 1 );
  raster1Bands.push_back( 2 );
  
  std::vector< unsigned int > raster2Bands;
  raster2Bands.push_back( 1 );
  raster2Bands.push_back( 2 );
  raster2Bands.push_back( 0 );
  
  const std::vector< double > pixelOffsets1( 3, 0.0 );
  const std::vector< double > pixelScales1( 3, 1.0 );
  const std::vector< double > pixelOffsets2( 3, 0.0 );
  const std::vector< double > pixelScales2( 3, 1.0 );
  
  te::gm::GTParameters transParams;
  transParams.m_tiePoints.push_back( te::gm::GTParameters::TiePoint( 
     te::gm::Coord2D( 292, 538 ), te::gm::Coord2D( 0, 0 ) ) );
  transParams.m_tiePoints.push_back( 
    te::gm::GTParameters::TiePoint( 
      te::gm::Coord2D( 
        transParams.m_tiePoints[ 0 ].first.x + inputRaster2Pointer->getNumberOfColumns() - 1, 
        transParams.m_tiePoints[ 0 ].first.y + inputRaster2Pointer->getNumberOfRows() - 1
      ), 
      te::gm::Coord2D( 
        inputRaster2Pointer->getNumberOfColumns() - 1,
        inputRaster2Pointer->getNumberOfRows() - 1 ) 
      ) 
    );     
  std::auto_ptr< te::gm::GeometricTransformation > transPtr( 
    te::gm::GTFactory::make( "RST" ) );
  CPPUNIT_ASSERT( transPtr->initialize( transParams ) );    
  
  // Initiating the blender instance
    
  te::rp::Blender blender;
  
  CPPUNIT_ASSERT( blender.initialize(
    *inputRaster1Pointer,
    raster1Bands,
    *inputRaster2Pointer,
    raster2Bands,
    te::rp::Blender::EuclideanDistanceMethod,
    te::rst::Interpolator::NearestNeighbor,
    te::rst::Interpolator::NearestNeighbor,
    0.0,
    false,
    pixelOffsets1,
    pixelScales1,
    pixelOffsets2,
    pixelScales2,
    0,
    0,
    *transPtr,
    1,
    false ) );
  
  // Creating the output image
  
  std::map<std::string, std::string> outputRasterInfo;
  outputRasterInfo["URI"] = "terralib_unittest_rp_Blender_PixelByPixelEucBlendTest_Test.tif";
  
  std::vector< te::rst::BandProperty* > bandsProperties;
  for( unsigned int inRasterBandsIdx = 0 ; inRasterBandsIdx <
    raster1Bands.size() ; ++inRasterBandsIdx )
  {
    bandsProperties.push_back( new te::rst::BandProperty(
      *( inputRaster1Pointer->getBand( raster1Bands[ inRasterBandsIdx ] )->getProperty() ) ) );
  }
  
  te::rst::Grid* gridPtr = new te::rst::Grid( *( inputRaster1Pointer->getGrid() ) );
  gridPtr->setNumberOfColumns( gridPtr->getNumberOfColumns() * 2 );
  gridPtr->setNumberOfRows( gridPtr->getNumberOfRows() * 2 );
  
  std::auto_ptr< te::rst::Raster > outRasterPtr( te::rst::RasterFactory::make(
    "GDAL",
    gridPtr,
    bandsProperties,
    outputRasterInfo,
    0,
    0 ) );
    
  CPPUNIT_ASSERT( outRasterPtr.get() );
  
  std::vector< double > values( outRasterPtr->getNumberOfBands() );
  for( unsigned int row = 0 ; row < outRasterPtr->getNumberOfRows() ; ++row )
  {
    for( unsigned int col = 0 ; col < outRasterPtr->getNumberOfColumns() ; ++col )
    {
      blender.getBlendedValues( row, col, values );
      
      for( unsigned int band = 0 ; band < values.size() ; ++band )
      {
        outRasterPtr->setValue( col, row, values[ band ], band );
      }
    }
  }
}

void TsBlender::FullRasterBlendTest()
{
  // Progress interface
  te::common::ConsoleProgressViewer progressViewerInstance;
  int viewerId = te::common::ProgressManager::getInstance().addViewer( &progressViewerInstance );  
  
  // openning input rasters
  
  std::map<std::string, std::string> auxRasterInfo;
  
  auxRasterInfo["URI"] = TE_DATA_DIR "/data/rasters/cbers_rgb342_crop2.tif";
  std::auto_ptr< te::rst::Raster > inputRaster1Pointer ( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  CPPUNIT_ASSERT( inputRaster1Pointer.get() );    
  
  auxRasterInfo["URI"] = TE_DATA_DIR "/data/rasters/cbers_rgb342_crop1.tif";
  std::auto_ptr< te::rst::Raster > inputRaster2Pointer ( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  CPPUNIT_ASSERT( inputRaster2Pointer.get() );
  
  // Creating the output image
  
  std::auto_ptr< te::rst::Raster > outRasterPtr;
  
  {
    std::map<std::string, std::string> outputRasterInfo;
    outputRasterInfo["URI"] = "terralib_unittest_rp_Blender_FullRasterBlendTest_Test.tif";
    
    std::vector< te::rst::BandProperty* > bandsProperties;
    for( unsigned int inRasterBandsIdx = 0 ; inRasterBandsIdx <
      inputRaster1Pointer->getNumberOfBands() ; ++inRasterBandsIdx )
    {
      bandsProperties.push_back( new te::rst::BandProperty(
        *( inputRaster1Pointer->getBand( 0 )->getProperty() ) ) );
      bandsProperties[ inRasterBandsIdx ]->m_noDataValue = 0.0;
    }
    
    te::rst::Grid* gridPtr = new te::rst::Grid( *( inputRaster1Pointer->getGrid() ) );
    gridPtr->setNumberOfColumns( gridPtr->getNumberOfColumns() * 2 );
    gridPtr->setNumberOfRows( gridPtr->getNumberOfRows() * 2 );
    
    outRasterPtr.reset( te::rst::RasterFactory::make(
      "GDAL",
      gridPtr,
      bandsProperties,
      outputRasterInfo,
      0,
      0 ) );
    CPPUNIT_ASSERT( outRasterPtr.get() );
    
    double uLX = 0;
    double uLY = 0;
    inputRaster1Pointer->getGrid()->gridToGeo( 0, 0, uLX, uLY );
    
    double outULCol = 0;
    double outULRow = 0;    
    outRasterPtr->getGrid()->geoToGrid( uLX, uLY, outULCol, outULRow );
    
    const unsigned int inputNRows = inputRaster1Pointer->getNumberOfRows();
    const unsigned int inputNCols = inputRaster1Pointer->getNumberOfColumns();
    const unsigned int outputNRows = outRasterPtr->getNumberOfRows();
    const unsigned int outputNCols = outRasterPtr->getNumberOfColumns();    
    unsigned int row = 0;
    unsigned int col = 0;
    double value = 0;
    double outNoDataValue = 0;
    
    for( unsigned int band = 0; band < inputRaster1Pointer->getNumberOfBands();
      ++band )
    {
      outNoDataValue = outRasterPtr->getBand( band )->getProperty()->m_noDataValue;
      
      for( row = 0 ; row < outputNRows ; ++row )
      {
        for( col = 0; col < outputNCols ; ++col )
        {
          outRasterPtr->setValue( col, row, outNoDataValue, band );
        }
      }      
      
      for( row = 0 ; row < inputNRows ; ++row )
      {
        for( col = 0; col < inputNCols ; ++col )
        {
          inputRaster1Pointer->getValue( col, row, value, band );
          outRasterPtr->setValue( col + outULCol, row + outULRow, value, band );
        }
      }
    }
  }
  
  // Creating blender parameters
  
  std::vector< unsigned int > raster1Bands;
  raster1Bands.push_back( 0 );
  raster1Bands.push_back( 1 );
  raster1Bands.push_back( 2 );
  
  std::vector< unsigned int > raster2Bands;
  raster2Bands.push_back( 1 );
  raster2Bands.push_back( 2 );
  raster2Bands.push_back( 0 );
  
  const std::vector< double > pixelOffsets1( 3, 0.0 );
  const std::vector< double > pixelScales1( 3, 1.0 );
  const std::vector< double > pixelOffsets2( 3, 0.0 );
  const std::vector< double > pixelScales2( 3, 1.0 );
  
  te::gm::GTParameters transParams;
  transParams.m_tiePoints.push_back( te::gm::GTParameters::TiePoint( 
     te::gm::Coord2D( 292, 538 ), te::gm::Coord2D( 0, 0 ) ) );
  transParams.m_tiePoints.push_back( 
    te::gm::GTParameters::TiePoint( 
      te::gm::Coord2D( 
        transParams.m_tiePoints[ 0 ].first.x + inputRaster2Pointer->getNumberOfColumns() - 1, 
        transParams.m_tiePoints[ 0 ].first.y + inputRaster2Pointer->getNumberOfRows() - 1
      ), 
      te::gm::Coord2D( 
        inputRaster2Pointer->getNumberOfColumns() - 1,
        inputRaster2Pointer->getNumberOfRows() - 1 ) 
      ) 
    );        
  std::auto_ptr< te::gm::GeometricTransformation > transPtr( 
    te::gm::GTFactory::make( "RST" ) );
  CPPUNIT_ASSERT( transPtr->initialize( transParams ) );    
  
  // Initiating the blender instance
    
  te::rp::Blender blender;
  
  CPPUNIT_ASSERT( blender.initialize(
    *outRasterPtr,
    raster1Bands,
    *inputRaster2Pointer,
    raster2Bands,
    te::rp::Blender::NoBlendMethod,
    te::rst::Interpolator::NearestNeighbor,
    te::rst::Interpolator::NearestNeighbor,
    0.0,
    false,
    pixelOffsets1,
    pixelScales1,
    pixelOffsets2,
    pixelScales2,
    0,
    0,
    *transPtr,
    1,
    true ) );
  
  CPPUNIT_ASSERT( blender.blendIntoRaster1() );
  
  te::common::ProgressManager::getInstance().removeViewer( viewerId ); 
}

void TsBlender::ThreadedFullRasterBlendTest()
{
  // Progress interface
  te::common::ConsoleProgressViewer progressViewerInstance;
  int viewerId = te::common::ProgressManager::getInstance().addViewer( &progressViewerInstance );  
  
  // openning input rasters
  
  std::map<std::string, std::string> auxRasterInfo;
  
  auxRasterInfo["URI"] = TE_DATA_DIR "/data/rasters/cbers_rgb342_crop2.tif";
  std::auto_ptr< te::rst::Raster > inputRaster1Pointer ( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  CPPUNIT_ASSERT( inputRaster1Pointer.get() );    
  
  auxRasterInfo["URI"] = TE_DATA_DIR "/data/rasters/cbers_rgb342_crop1.tif";
  std::auto_ptr< te::rst::Raster > inputRaster2Pointer ( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  CPPUNIT_ASSERT( inputRaster2Pointer.get() );
  
  // Creating the output image
  
  std::auto_ptr< te::rst::Raster > outRasterPtr;
  
  {
    std::map<std::string, std::string> outputRasterInfo;
    outputRasterInfo["URI"] = "terralib_unittest_rp_Blender_ThreadedFullRasterBlendTest_Test.tif";
    
    std::vector< te::rst::BandProperty* > bandsProperties;
    for( unsigned int inRasterBandsIdx = 0 ; inRasterBandsIdx <
      inputRaster1Pointer->getNumberOfBands() ; ++inRasterBandsIdx )
    {
      bandsProperties.push_back( new te::rst::BandProperty(
        *( inputRaster1Pointer->getBand( 0 )->getProperty() ) ) );
      bandsProperties[ inRasterBandsIdx ]->m_noDataValue = 0.0;
    }
    
    te::rst::Grid* gridPtr = new te::rst::Grid( *( inputRaster1Pointer->getGrid() ) );
    gridPtr->setNumberOfColumns( gridPtr->getNumberOfColumns() * 2 );
    gridPtr->setNumberOfRows( gridPtr->getNumberOfRows() * 2 );
    
    outRasterPtr.reset( te::rst::RasterFactory::make(
      "GDAL",
      gridPtr,
      bandsProperties,
      outputRasterInfo,
      0,
      0 ) );
    CPPUNIT_ASSERT( outRasterPtr.get() );
    
    double uLX = 0;
    double uLY = 0;
    inputRaster1Pointer->getGrid()->gridToGeo( 0, 0, uLX, uLY );
    
    double outULCol = 0;
    double outULRow = 0;    
    outRasterPtr->getGrid()->geoToGrid( uLX, uLY, outULCol, outULRow );
    
    const unsigned int inputNRows = inputRaster1Pointer->getNumberOfRows();
    const unsigned int inputNCols = inputRaster1Pointer->getNumberOfColumns();
    const unsigned int outputNRows = outRasterPtr->getNumberOfRows();
    const unsigned int outputNCols = outRasterPtr->getNumberOfColumns();    
    unsigned int row = 0;
    unsigned int col = 0;
    double value = 0;
    double outNoDataValue = 0;
    
    for( unsigned int band = 0; band < inputRaster1Pointer->getNumberOfBands();
      ++band )
    {
      outNoDataValue = outRasterPtr->getBand( band )->getProperty()->m_noDataValue;
      
      for( row = 0 ; row < outputNRows ; ++row )
      {
        for( col = 0; col < outputNCols ; ++col )
        {
          outRasterPtr->setValue( col, row, outNoDataValue, band );
        }
      }      
      
      for( row = 0 ; row < inputNRows ; ++row )
      {
        for( col = 0; col < inputNCols ; ++col )
        {
          inputRaster1Pointer->getValue( col, row, value, band );
          outRasterPtr->setValue( col + outULCol, row + outULRow, value, band );
        }
      }
    }
  }
  
  // Creating blender parameters
  
  std::vector< unsigned int > raster1Bands;
  raster1Bands.push_back( 0 );
  raster1Bands.push_back( 1 );
  raster1Bands.push_back( 2 );
  
  std::vector< unsigned int > raster2Bands;
  raster2Bands.push_back( 1 );
  raster2Bands.push_back( 2 );
  raster2Bands.push_back( 0 );
  
  const std::vector< double > pixelOffsets1( 3, 0.0 );
  const std::vector< double > pixelScales1( 3, 1.0 );
  const std::vector< double > pixelOffsets2( 3, 0.0 );
  const std::vector< double > pixelScales2( 3, 1.0 );
  
  te::gm::GTParameters transParams;
  transParams.m_tiePoints.push_back( te::gm::GTParameters::TiePoint( 
     te::gm::Coord2D( 292, 538 ), te::gm::Coord2D( 0, 0 ) ) );
  transParams.m_tiePoints.push_back( 
    te::gm::GTParameters::TiePoint( 
      te::gm::Coord2D( 
        transParams.m_tiePoints[ 0 ].first.x + inputRaster2Pointer->getNumberOfColumns() - 1, 
        transParams.m_tiePoints[ 0 ].first.y + inputRaster2Pointer->getNumberOfRows() - 1
      ), 
      te::gm::Coord2D( 
        inputRaster2Pointer->getNumberOfColumns() - 1,
        inputRaster2Pointer->getNumberOfRows() - 1 ) 
      ) 
    );        
  std::auto_ptr< te::gm::GeometricTransformation > transPtr( 
    te::gm::GTFactory::make( "RST" ) );
  CPPUNIT_ASSERT( transPtr->initialize( transParams ) );    
  
  // Initiating the blender instance
    
  te::rp::Blender blender;
  
  CPPUNIT_ASSERT( blender.initialize(
    *outRasterPtr,
    raster1Bands,
    *inputRaster2Pointer,
    raster2Bands,
    te::rp::Blender::NoBlendMethod,
    te::rst::Interpolator::NearestNeighbor,
    te::rst::Interpolator::NearestNeighbor,
    0.0,
    false,
    pixelOffsets1,
    pixelScales1,
    pixelOffsets2,
    pixelScales2,
    0,
    0,
    *transPtr,
    0,
    true ) );
  
  CPPUNIT_ASSERT( blender.blendIntoRaster1() );
  
  te::common::ProgressManager::getInstance().removeViewer( viewerId ); 
}
