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
  \file terralib/unittest/rp/fusion/TsFusion.cpp

  \brief A test suit for the Fusion interface.
*/

// TerraLib
#include "../Config.h"
#include <terralib/rp.h>
#include <terralib/raster.h>

// Boost
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE (fusion_tests)

BOOST_AUTO_TEST_CASE(ihs_test)
{
  /* Openning input raster */
  
  std::map<std::string, std::string> auxRasterInfo;
  
  auxRasterInfo["URI"] = TERRALIB_DATA_DIR "/geotiff/cbers2b_rgb342_crop.tif";
  boost::shared_ptr< te::rst::Raster > lowResRasterPtr( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  BOOST_CHECK( lowResRasterPtr.get() );
  
  auxRasterInfo["URI"] = TERRALIB_DATA_DIR "/geotiff/cbers2b_hrc_crop.tif";
  boost::shared_ptr< te::rst::Raster > highResRasterPtr( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  BOOST_CHECK( highResRasterPtr.get() );
  
  /* Creating the algorithm parameters */
  
  te::rp::IHSFusion::InputParameters algoInputParams;
  
  algoInputParams.m_lowResRasterPtr = lowResRasterPtr.get();
  algoInputParams.m_lowResRasterRedBandIndex = 0;
  algoInputParams.m_lowResRasterGreenBandIndex = 1;
  algoInputParams.m_lowResRasterBlueBandIndex = 2;
  algoInputParams.m_highResRasterPtr = highResRasterPtr.get();
  algoInputParams.m_highResRasterBand = 0;
  algoInputParams.m_enableProgress = false;
  algoInputParams.m_interpMethod = te::rst::NearestNeighbor;
  algoInputParams.m_RGBMin = 0;
  algoInputParams.m_RGBMax = 0;

  te::rp::IHSFusion::OutputParameters algoOutputParams;
  
  algoOutputParams.m_rInfo["URI"] =  
    "terralib_unittest_rp_fusion_ihs.tif";  
  algoOutputParams.m_rType = "GDAL";
  
  /* Executing the algorithm */
  
  te::rp::IHSFusion algorithmInstance;
  
  BOOST_CHECK( algorithmInstance.initialize( algoInputParams ) );
  BOOST_CHECK( algorithmInstance.execute( algoOutputParams ) );
}

BOOST_AUTO_TEST_CASE(pca_test)
{
  /* Opening input raster */
  
  std::map<std::string, std::string> auxRasterInfo;
  
  auxRasterInfo["URI"] = TERRALIB_DATA_DIR "/geotiff/cbers2b_rgb342_crop.tif";
  boost::shared_ptr< te::rst::Raster > lowResRasterPtr( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  BOOST_CHECK( lowResRasterPtr.get() );
  
  auxRasterInfo["URI"] = TERRALIB_DATA_DIR "/geotiff/cbers2b_hrc_crop.tif";
  boost::shared_ptr< te::rst::Raster > highResRasterPtr( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  BOOST_CHECK( highResRasterPtr.get() );
  
  /* Creating the algorithm parameters */
  
  te::rp::PCAFusion::InputParameters algoInputParams;
  
  algoInputParams.m_lowResRasterPtr = lowResRasterPtr.get();
  algoInputParams.m_lowResRasterBands.push_back( 0 );
  algoInputParams.m_lowResRasterBands.push_back( 1 );
  algoInputParams.m_lowResRasterBands.push_back( 2 );
  algoInputParams.m_highResRasterPtr = highResRasterPtr.get();
  algoInputParams.m_highResRasterBand = 0;
  algoInputParams.m_enableProgress = true;
  algoInputParams.m_enableThreadedProcessing = false;
  algoInputParams.m_interpMethod = te::rst::NearestNeighbor;

  te::rp::PCAFusion::OutputParameters algoOutputParams;
  
  algoOutputParams.m_rInfo["URI"] =  
    "terralib_unittest_rp_fusion_pca.tif";  
  algoOutputParams.m_rType = "GDAL";
  
  /* Executing the algorithm */
  
  te::rp::PCAFusion algorithmInstance;
  
  BOOST_CHECK( algorithmInstance.initialize( algoInputParams ) );
  BOOST_CHECK( algorithmInstance.execute( algoOutputParams ) );
}

BOOST_AUTO_TEST_CASE(wisper_test)
{
  /* Opening input raster */
  
  std::map<std::string, std::string> auxRasterInfo;
  
  auxRasterInfo["URI"] = TERRALIB_DATA_DIR "/geotiff/cbers2b_rgb342_crop.tif";
  boost::shared_ptr< te::rst::Raster > lowResRasterPtr( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  BOOST_CHECK( lowResRasterPtr.get() );
  
  auxRasterInfo["URI"] = TERRALIB_DATA_DIR "/geotiff/cbers2b_hrc_crop.tif";
  boost::shared_ptr< te::rst::Raster > highResRasterPtr( te::rst::RasterFactory::open(
    auxRasterInfo ) );
  BOOST_CHECK( highResRasterPtr.get() );
  
  /* Creating the algorithm parameters */
  
  te::rp::WisperFusion::InputParameters algoInputParams;
  
  algoInputParams.m_lowResRasterPtr = lowResRasterPtr.get();
  algoInputParams.m_lowResRasterBands.push_back( 0 );
  algoInputParams.m_lowResRasterBands.push_back( 1 );
  algoInputParams.m_lowResRasterBands.push_back( 2 );
  algoInputParams.m_lowResRasterBandSensors.push_back( te::rp::srf::CBERS2BCCDB3Sensor );
  algoInputParams.m_lowResRasterBandSensors.push_back( te::rp::srf::CBERS2BCCDB4Sensor );
  algoInputParams.m_lowResRasterBandSensors.push_back( te::rp::srf::CBERS2BCCDB2Sensor );
  algoInputParams.m_highResRasterPtr = highResRasterPtr.get();
  algoInputParams.m_highResRasterBand = 0;
  algoInputParams.m_hiResRasterBandSensor = te::rp::srf::CBERS2BCCDB5PANSensor;
  algoInputParams.m_hiResRasterWaveletLevels = 0;
  algoInputParams.m_enableProgress = true;
  algoInputParams.m_interpMethod = te::rst::NearestNeighbor;
  algoInputParams.m_waveletFilterType = te::rp::TriangleWAFilter;

  te::rp::WisperFusion::OutputParameters algoOutputParams;
  
  algoOutputParams.m_rInfo["URI"] =  
    "terralib_unittest_rp_fusion_wisper.tif";  
  algoOutputParams.m_rType = "GDAL";
  
  /* Executing the algorithm */
  
  te::rp::WisperFusion algorithmInstance;
  
  BOOST_CHECK( algorithmInstance.initialize( algoInputParams ) );
  BOOST_CHECK( algorithmInstance.execute( algoOutputParams ) );
}

BOOST_AUTO_TEST_SUITE_END()
