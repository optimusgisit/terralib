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
  \file terralib/rp/TiePointsMosaic.cpp
  \brief Create a mosaic from a set of rasters using tie-points.
*/

#include "TiePointsMosaic.h"

#include "Macros.h"
#include "Functions.h"
#include "../raster/Interpolator.h"
#include "../raster/Enums.h"
#include "../raster/RasterFactory.h"
#include "../raster/Grid.h"
#include "../raster/Band.h"
#include "../raster/BandProperty.h"
#include "../raster/PositionIterator.h"
#include "../raster/Utils.h"
#include "../memory/CachedRaster.h"
#include "../geometry/Envelope.h"
#include "../geometry/GTFactory.h"
#include "../geometry/Polygon.h"
#include "../geometry/LinearRing.h"
#include "../geometry/MultiPolygon.h"
#include "../srs/Converter.h"
#include "../common/progress/TaskProgress.h"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>

#include <climits>
#include <cfloat>
#include <cmath>
#include <memory>

namespace te
{
  namespace rp
  {

    TiePointsMosaic::InputParameters::InputParameters()
    {
      reset();
    }

    TiePointsMosaic::InputParameters::InputParameters( const InputParameters& other )
    {
      reset();
      operator=( other );
    }

    TiePointsMosaic::InputParameters::~InputParameters()
    {
      reset();
    }

    void TiePointsMosaic::InputParameters::reset() throw( te::rp::Exception )
    {
      m_feederRasterPtr = 0;
      m_inputRastersBands.clear();
      m_tiePoints.clear();
      m_geomTransfName = "Affine";
      m_interpMethod = te::rst::NearestNeighbor;
      m_noDataValue = 0.0;
      m_forceInputNoDataValue = false;
      m_blendMethod = te::rp::Blender::NoBlendMethod;
      m_autoEqualize = true;
      m_useRasterCache = true;
      m_enableProgress = false;
      m_enableMultiThread = true;
    }

    const TiePointsMosaic::InputParameters& TiePointsMosaic::InputParameters::operator=(
      const TiePointsMosaic::InputParameters& params )
    {
      reset();

      m_feederRasterPtr = params.m_feederRasterPtr;
      m_inputRastersBands = params.m_inputRastersBands;
      m_tiePoints = params.m_tiePoints;
      m_geomTransfName = params.m_geomTransfName;
      m_interpMethod = params.m_interpMethod;
      m_noDataValue = params.m_noDataValue;
      m_forceInputNoDataValue = params.m_forceInputNoDataValue;
      m_blendMethod = params.m_blendMethod;
      m_autoEqualize = params.m_autoEqualize;
      m_useRasterCache = params.m_useRasterCache;
      m_enableProgress = params.m_enableProgress;
      m_enableMultiThread = params.m_enableMultiThread;

      return *this;
    }

    te::common::AbstractParameters* TiePointsMosaic::InputParameters::clone() const
    {
      return new InputParameters( *this );
    }

    TiePointsMosaic::OutputParameters::OutputParameters()
    {
      reset();
    }

    TiePointsMosaic::OutputParameters::OutputParameters( const OutputParameters& other )
    {
      reset();
      operator=( other );
    }

    TiePointsMosaic::OutputParameters::~OutputParameters()
    {
      reset();
    }

    void TiePointsMosaic::OutputParameters::reset() throw( te::rp::Exception )
    {
      m_rType.clear();
      m_rInfo.clear();
      m_outputRasterPtr.reset();
    }

    const TiePointsMosaic::OutputParameters& TiePointsMosaic::OutputParameters::operator=(
      const TiePointsMosaic::OutputParameters& params )
    {
      reset();

      m_rType = params.m_rType;
      m_rInfo = params.m_rInfo;

      return *this;
    }

    te::common::AbstractParameters* TiePointsMosaic::OutputParameters::clone() const
    {
      return new OutputParameters( *this );
    }

    TiePointsMosaic::TiePointsMosaic()
    {
      reset();
    }

    TiePointsMosaic::~TiePointsMosaic()
    {
    }

    bool TiePointsMosaic::execute( AlgorithmOutputParameters& outputParams )
      throw( te::rp::Exception )
    {
      if( ! m_isInitialized ) return false;
      
      TiePointsMosaic::OutputParameters* outParamsPtr = dynamic_cast<
        TiePointsMosaic::OutputParameters* >( &outputParams );
      TERP_TRUE_OR_THROW( outParamsPtr, "Invalid paramters" );
      
      // progress
      
      std::auto_ptr< te::common::TaskProgress > progressPtr;
      if( m_inputParameters.m_enableProgress )
      {
        progressPtr.reset( new te::common::TaskProgress );
        
        progressPtr->setTotalSteps( 4 + m_inputParameters.m_feederRasterPtr->getObjsCount() );
        
        progressPtr->setMessage( "Mosaic" );
      }       
      
       // First pass: getting global mosaic info
       
      double mosaicXResolution = 0.0;
      double mosaicYResolution = 0.0;
      double mosaicLLX = DBL_MAX; // world coords
      double mosaicLLY = DBL_MAX; // world coords
      double mosaicURX = -1.0 * DBL_MAX; // world coords
      double mosaicURY = -1.0 * DBL_MAX; // world coords
      int mosaicSRID = 0;
      std::vector< double > mosaicBandsRangeMin;
      std::vector< double > mosaicBandsRangeMax;      
      te::rst::BandProperty mosaicBaseBandProperties( 0, 0, "" );
      std::vector< te::gm::Polygon > rastersBBoxes; // all rasters bounding boxes (under the first raster world coords.

      {
        std::vector< boost::shared_ptr< te::gm::GeometricTransformation > >
          eachRasterPixelToFirstRasterPixelGeomTransfms; 
          // Mapping indexed points from each raster to the first raster indexed points.
          // te::gm::GTParameters::TiePoint::first are mosaic reaster indexed points (lines/cols),
          // te::gm::GTParameters::TiePoint::second are the other rasters indexed points (lines/cols).  
          
        std::vector< te::rst::Grid > rastersGrids; //all rasters original grids under their original SRSs
        
        te::rst::Raster const* inputRasterPtr = 0;
        unsigned int inputRasterIdx = 0;
        te::srs::Converter convInstance;

        m_inputParameters.m_feederRasterPtr->reset();
        while( ( inputRasterPtr = m_inputParameters.m_feederRasterPtr->getCurrentObj() ) )
        {
          inputRasterIdx = m_inputParameters.m_feederRasterPtr->getCurrentOffset();
          TERP_TRUE_OR_RETURN_FALSE(
            inputRasterPtr->getAccessPolicy() & te::common::RAccess,
            "Invalid raster" );
            
          rastersGrids.push_back( (*inputRasterPtr->getGrid()) );

          // Defining the base mosaic info

          if( inputRasterIdx == 0 )
          {
            mosaicXResolution = inputRasterPtr->getGrid()->getResolutionX();
            mosaicYResolution = inputRasterPtr->getGrid()->getResolutionY();

            mosaicLLX = inputRasterPtr->getGrid()->getExtent()->m_llx;
            mosaicLLY = inputRasterPtr->getGrid()->getExtent()->m_lly;
            mosaicURX = inputRasterPtr->getGrid()->getExtent()->m_urx;
            mosaicURY = inputRasterPtr->getGrid()->getExtent()->m_ury;

            mosaicSRID = inputRasterPtr->getGrid()->getSRID();

            mosaicBaseBandProperties = *inputRasterPtr->getBand( 0 )->getProperty();
            
            // finding the current raster bounding box polygon (first raster world coordinates)
            
            te::gm::LinearRing* auxLinearRingPtr = new te::gm::LinearRing(5, te::gm::LineStringType);
            auxLinearRingPtr->setPoint( 0, mosaicLLX, mosaicURY );
            auxLinearRingPtr->setPoint( 1, mosaicURX, mosaicURY );
            auxLinearRingPtr->setPoint( 2, mosaicURX, mosaicLLY );
            auxLinearRingPtr->setPoint( 3, mosaicLLX, mosaicLLY );
            auxLinearRingPtr->setPoint( 4, mosaicLLX, mosaicURY );
            auxLinearRingPtr->setSRID( mosaicSRID );
            
            te::gm::Polygon auxPolygon( 0, te::gm::PolygonType, 0 );
            auxPolygon.push_back( auxLinearRingPtr );
            auxPolygon.setSRID( mosaicSRID );
            rastersBBoxes.push_back( auxPolygon );
          }
          else
          {
            te::gm::GTParameters transParams;

            if( ( inputRasterIdx == 1 ) ||
              ( m_inputParameters.m_tiePointsLinkType == InputParameters::FirstRasterLinkingTiePointsT ) )
            {
              transParams.m_tiePoints = m_inputParameters.m_tiePoints[ inputRasterIdx - 1 ];
            }
            else
            {
              // converting the current tie-points to map coords from the
              // current raster to the first one

              te::gm::GTParameters::TiePoint auxTP;
              const std::vector< te::gm::GTParameters::TiePoint >& inputTPs =
                m_inputParameters.m_tiePoints[ inputRasterIdx - 1 ];
              const unsigned int inputTPsSize = inputTPs.size();
              const te::gm::GeometricTransformation& lastTransf =
                (*eachRasterPixelToFirstRasterPixelGeomTransfms[ inputRasterIdx - 2 ].get());

              for( unsigned int inputTPsIdx = 0 ; inputTPsIdx < inputTPsSize ;
                ++inputTPsIdx )
              {
                auxTP.second = inputTPs[ inputTPsIdx ].second;
                lastTransf.inverseMap( inputTPs[ inputTPsIdx ].first, auxTP.first  );
                transParams.m_tiePoints.push_back( auxTP );
              }
            }
            
            // The transformation
            // inverse mapping current raster lines/cols into the first raster lines/cols.

            boost::shared_ptr< te::gm::GeometricTransformation > auxTransPtr( 
              te::gm::GTFactory::make( m_inputParameters.m_geomTransfName ) );
            TERP_TRUE_OR_RETURN_FALSE( auxTransPtr.get() != 0,
              "Geometric transformation instatiation error" );
            TERP_TRUE_OR_RETURN_FALSE( auxTransPtr->initialize( transParams ),
              "Geometric transformation parameters calcule error" );
            eachRasterPixelToFirstRasterPixelGeomTransfms.push_back( auxTransPtr );
            
            // The indexed detailed extent of input raster
            
            te::gm::LinearRing inRasterIndexedDetailedExtent( te::gm::LineStringType, 0, 0 );
            TERP_TRUE_OR_RETURN_FALSE( te::rp::GetIndexedDetailedExtent(
              *inputRasterPtr->getGrid(), inRasterIndexedDetailedExtent ),
              "Error creating the raster detailed extent" );               
            
            // The input rasters detailed extent over the expanded mosaic
            
            te::gm::LinearRing inRasterDetailedExtent( 
                inRasterIndexedDetailedExtent.size(), te::gm::LineStringType,
                mosaicSRID, (te::gm::Envelope*)0 );
            
            {
              double mappedX = 0;
              double mappedY = 0;
              double geoX = 0;
              double geoY = 0;

              for( unsigned int inRasterDetExtentIdx = 0 ; inRasterDetExtentIdx <
                inRasterIndexedDetailedExtent.size() ; ++inRasterDetExtentIdx )
              {
                auxTransPtr->inverseMap( 
                  inRasterIndexedDetailedExtent.getX( inRasterDetExtentIdx ),
                  inRasterIndexedDetailedExtent.getY( inRasterDetExtentIdx ),
                  mappedX,
                  mappedY);
                
                rastersGrids[ 0 ].gridToGeo( mappedX, mappedY, geoX,
                  geoY );                
                
                inRasterDetailedExtent.setPoint( inRasterDetExtentIdx, geoX, geoY );
              }            
            }            

            // expanding mosaic area

            mosaicLLX = std::min( mosaicLLX, inRasterDetailedExtent.getMBR()->getLowerLeftX() );
            mosaicLLY = std::min( mosaicLLY, inRasterDetailedExtent.getMBR()->getLowerLeftY() );

            mosaicURX = std::max( mosaicURX, inRasterDetailedExtent.getMBR()->getUpperRightX() );
            mosaicURY = std::max( mosaicURY, inRasterDetailedExtent.getMBR()->getUpperRightY() );

            // finding the current raster bounding box polygon (first raster world coordinates)

            te::gm::Polygon auxPolygon( 0, te::gm::PolygonType, 0 );
            auxPolygon.push_back( new te::gm::LinearRing( inRasterDetailedExtent ) );
            auxPolygon.setSRID( mosaicSRID );
            rastersBBoxes.push_back( auxPolygon );
          }

          // checking the input bands

          for( std::vector< unsigned int >::size_type inputRastersBandsIdx = 0 ;
            inputRastersBandsIdx <
            m_inputParameters.m_inputRastersBands[ inputRasterIdx ].size() ;
            ++inputRastersBandsIdx )
          {
            const unsigned int& currBand =
              m_inputParameters.m_inputRastersBands[ inputRasterIdx ][ inputRastersBandsIdx ];

            TERP_TRUE_OR_RETURN_FALSE( currBand < inputRasterPtr->getNumberOfBands(),
              "Invalid band" )
          }


          m_inputParameters.m_feederRasterPtr->moveNext();
        }
        

      }
      
      if( m_inputParameters.m_enableProgress )
      {
        progressPtr->pulse();
        if( ! progressPtr->isActive() ) return false;
      }      

      // creating the output raster
      
      te::rst::Raster* outputRasterPtr = 0;

      {
        mosaicBandsRangeMin.resize( 
          m_inputParameters.m_inputRastersBands[ 0 ].size(), 0 );
        mosaicBandsRangeMax.resize( 
          m_inputParameters.m_inputRastersBands[ 0 ].size(), 0 );        
        
        std::vector< te::rst::BandProperty* > bandsProperties;
        for( std::vector< unsigned int >::size_type bandIdx = 0 ;  bandIdx <
          m_inputParameters.m_inputRastersBands[ 0 ].size() ; ++bandIdx )
        {
          bandsProperties.push_back( new te::rst::BandProperty( mosaicBaseBandProperties ) );
          bandsProperties[ bandIdx ]->m_colorInterp = te::rst::GrayIdxCInt;
          bandsProperties[ bandIdx ]->m_noDataValue = m_inputParameters.m_noDataValue;
          
          te::rst::GetDataTypeRanges( bandsProperties[ bandIdx ]->m_type,
            mosaicBandsRangeMin[ bandIdx ],
            mosaicBandsRangeMax[ bandIdx ]);          
        }

        te::rst::Grid* outputGrid = new te::rst::Grid( mosaicXResolution,
          mosaicYResolution,  new te::gm::Envelope( mosaicLLX, mosaicLLY, mosaicURX,
          mosaicURY ), mosaicSRID );

        outParamsPtr->m_outputRasterPtr.reset(
          te::rst::RasterFactory::make(
            outParamsPtr->m_rType,
            outputGrid,
            bandsProperties,
            outParamsPtr->m_rInfo,
            0,
            0 ) );
        TERP_TRUE_OR_RETURN_FALSE( outParamsPtr->m_outputRasterPtr.get(),
          "Output raster creation error" );
          
        outputRasterPtr = outParamsPtr->m_outputRasterPtr.get();
      }
      
      std::auto_ptr< te::mem::CachedRaster > cachedOutputRasterInstancePtr;
      
      if( m_inputParameters.m_useRasterCache )
      {
        cachedOutputRasterInstancePtr.reset( new te::mem::CachedRaster(
          *(outParamsPtr->m_outputRasterPtr.get()), 25, 0 ) );   
          
        outputRasterPtr = cachedOutputRasterInstancePtr.get();
      }    
      
      if( m_inputParameters.m_enableProgress )
      {
        progressPtr->pulse();
        if( ! progressPtr->isActive() ) return false;
      }        
      
      // Finding the transformation mapping indexed points from each raster to the first raster indexed points.
      // te::gm::GTParameters::TiePoint::first are mosaic reaster indexed points (lines/cols),
      // te::gm::GTParameters::TiePoint::second are the other rasters indexed points (lines/cols).             
      
      std::vector< boost::shared_ptr< te::gm::GeometricTransformation > >
        eachRasterPixelToMosaicRasterPixelGeomTransfms; 
        
      {
        const double firstRasterColOffset = std::abs( rastersBBoxes[ 0 ].getMBR()->m_llx -
          outParamsPtr->m_outputRasterPtr->getGrid()->getExtent()->getLowerLeftX() ) /
          outParamsPtr->m_outputRasterPtr->getGrid()->getResolutionX();
        const double firstRasterLinOffset = std::abs( rastersBBoxes[ 0 ].getMBR()->m_ury -
          outParamsPtr->m_outputRasterPtr->getGrid()->getExtent()->getUpperRightY() ) /
          outParamsPtr->m_outputRasterPtr->getGrid()->getResolutionY();
          
        for( unsigned int tiePointsIdx = 0 ; tiePointsIdx < m_inputParameters.m_tiePoints.size() ;
          ++tiePointsIdx )
        {
          te::gm::GTParameters transfParams;
          transfParams.m_tiePoints = m_inputParameters.m_tiePoints[ tiePointsIdx ];
          
          const double prevRasterColOffset = std::abs( rastersBBoxes[ tiePointsIdx ].getMBR()->m_llx -
            outParamsPtr->m_outputRasterPtr->getGrid()->getExtent()->getLowerLeftX() ) /
            outParamsPtr->m_outputRasterPtr->getGrid()->getResolutionX();
          const double prevRasterLinOffset = std::abs( rastersBBoxes[ tiePointsIdx ].getMBR()->m_ury -
            outParamsPtr->m_outputRasterPtr->getGrid()->getExtent()->getUpperRightY() ) /
            outParamsPtr->m_outputRasterPtr->getGrid()->getResolutionY();          
          
          for( unsigned int tpIdx = 0 ; tpIdx < transfParams.m_tiePoints.size() ; 
            ++tpIdx )
          {
            if( m_inputParameters.m_tiePointsLinkType == InputParameters::FirstRasterLinkingTiePointsT )
            {
              transfParams.m_tiePoints[ tpIdx ].first.x += firstRasterColOffset;
              transfParams.m_tiePoints[ tpIdx ].first.y += firstRasterLinOffset;
            }
            else
            {
              transfParams.m_tiePoints[ tpIdx ].first.x += prevRasterColOffset;
              transfParams.m_tiePoints[ tpIdx ].first.y += prevRasterLinOffset;
            }
          }
          
          boost::shared_ptr< te::gm::GeometricTransformation > auxTransPtr( 
            te::gm::GTFactory::make( m_inputParameters.m_geomTransfName ) );
          TERP_TRUE_OR_RETURN_FALSE( auxTransPtr.get() != 0,
            "Geometric transformation instatiation error" );
          TERP_TRUE_OR_RETURN_FALSE( auxTransPtr->initialize( transfParams ),
            "Geometric transformation parameters calcule error" );
          eachRasterPixelToMosaicRasterPixelGeomTransfms.push_back( auxTransPtr );          
        }
      }
      
      if( m_inputParameters.m_enableProgress )
      {
        progressPtr->pulse();
        if( ! progressPtr->isActive() ) return false;
      }      

      // fill output with no data values

      {
        const unsigned int nBands = outputRasterPtr->getNumberOfBands();
        const unsigned int nRows = outputRasterPtr->getNumberOfRows();
        const unsigned int nCols = outputRasterPtr->getNumberOfColumns();
        unsigned int col = 0;
        unsigned int row = 0;
        unsigned int bandIdx = 0;

        for( bandIdx = 0 ; bandIdx < nBands ; ++bandIdx )
        {
          te::rst::Band& outBand = ( *( outputRasterPtr->getBand( bandIdx ) ) );

          for( row = 0 ; row < nRows ; ++row )
          {
            for( col = 0 ; col < nCols ; ++col )
            {
              outBand.setValue( col, row, m_inputParameters.m_noDataValue );
            }
          }
        }
      }
      
      if( m_inputParameters.m_enableProgress )
      {
        progressPtr->pulse();
        if( ! progressPtr->isActive() ) return false;
      }      
      
      // Copying the first image data to the output mosaic
      // and find the base mosaic mean and offset values

      std::vector< double > mosaicTargetMeans( outputRasterPtr->getNumberOfBands(), 0 );
      std::vector< double > mosaicTargetVariances( outputRasterPtr->getNumberOfBands(), 0 );

      {
        m_inputParameters.m_feederRasterPtr->reset();

        te::rst::Raster const* inputRasterPtr =
          m_inputParameters.m_feederRasterPtr->getCurrentObj();
        TERP_DEBUG_TRUE_OR_RETURN_FALSE( inputRasterPtr, "Invalid raster pointer" );

        double inXStartGeo = 0;
        double inYStartGeo = 0;
        inputRasterPtr->getGrid()->gridToGeo( 0.0, 0.0, inXStartGeo, inYStartGeo );
        
        double outFirstRowDouble = 0;
        double outFirstColDouble = 0;
        outputRasterPtr->getGrid()->geoToGrid( inXStartGeo, inYStartGeo,
          outFirstColDouble, outFirstRowDouble );
        
        const double outRowsBoundDouble = outFirstRowDouble +
          (double)( inputRasterPtr->getNumberOfRows() );
        const double outColsBoundDouble = outFirstColDouble +
          (double)( inputRasterPtr->getNumberOfColumns() );     
          
        const unsigned int outFirstRow = (unsigned int)std::max( 0u, 
          te::common::Round< double, unsigned int >( outFirstRowDouble ) );
        const unsigned int outFirstCol = (unsigned int)std::max( 0u, 
          te::common::Round< double, unsigned int >( outFirstColDouble ) );     
        
        const unsigned int outRowsBound = (unsigned int)std::min( 
          te::common::Round< double, unsigned int >( outRowsBoundDouble ),
          outputRasterPtr->getNumberOfRows() );
        const unsigned int outColsBound = (unsigned int)std::min( 
          te::common::Round< double, unsigned int >( outColsBoundDouble ),
          outputRasterPtr->getNumberOfColumns() );        

        const unsigned int nBands = (unsigned int)
          m_inputParameters.m_inputRastersBands[ 0 ].size();
        unsigned int outCol = 0;
        unsigned int outRow = 0;
        double inCol = 0;
        double inRow = 0;
        double bandNoDataValue = -1.0 * DBL_MAX;
        std::complex< double > pixelCValue = 0;
        te::rst::Interpolator interpInstance( inputRasterPtr,
          m_inputParameters.m_interpMethod );
        unsigned int inputBandIdx = 0;

        for( unsigned int inputRastersBandsIdx = 0 ; inputRastersBandsIdx <
          nBands ; ++inputRastersBandsIdx )
        {
          inputBandIdx =  m_inputParameters.m_inputRastersBands[ 0 ][
            inputRastersBandsIdx ] ;
          bandNoDataValue = m_inputParameters.m_forceInputNoDataValue ?
            m_inputParameters.m_noDataValue : inputRasterPtr->getBand( inputBandIdx
            )->getProperty()->m_noDataValue;
          te::rst::Band& outBand =
            (*outputRasterPtr->getBand( inputRastersBandsIdx ));
          unsigned long int validPixelsNumber = 0;

          double mean = 0;

          for( outRow = outFirstRow ; outRow < outRowsBound ; ++outRow )
          {
            inRow = ((double)outRow) - outFirstRowDouble;

            for( outCol = outFirstCol ; outCol < outColsBound ; ++outCol )
            {
              inCol = ((double)outCol) - outFirstColDouble;

              interpInstance.getValue( inCol, inRow, pixelCValue, inputBandIdx );

              if( pixelCValue.real() != bandNoDataValue )
              {
                outBand.setValue( outCol, outRow, pixelCValue );
                mean += pixelCValue.real();
                ++validPixelsNumber;
              }
            }
          }

          // variance calcule

          if( m_inputParameters.m_autoEqualize && ( validPixelsNumber > 0 ) )
          {
            mean /= ( (double)validPixelsNumber );
            mosaicTargetMeans[ inputRastersBandsIdx ] = mean;
            
            double& variance = mosaicTargetVariances[ inputRastersBandsIdx ];
            variance = 0;

            double pixelValue = 0;

            for( outRow = outFirstRow ; outRow < outRowsBound ; ++outRow )
            {
              for( outCol = outFirstCol ; outCol < outColsBound ; ++outCol )
              {
                outBand.getValue( outCol, outRow, pixelValue );

                if( pixelValue != m_inputParameters.m_noDataValue )
                {
                  variance += ( ( pixelValue - mean ) * ( pixelValue -
                    mean ) ) / ( (double)validPixelsNumber );
                }
              }
            }
          }
        }
      }
      
      if( m_inputParameters.m_enableProgress )
      {
        progressPtr->pulse();
        if( ! progressPtr->isActive() ) return false;
      }      
        
      // Initiating the mosaic bounding boxes union

      std::auto_ptr< te::gm::MultiPolygon > mosaicBBoxesUnionPtr( 
        new te::gm::MultiPolygon( 0, te::gm::MultiPolygonType, 
        rastersBBoxes[ 0 ].getSRID(), 0 ) );
      
      mosaicBBoxesUnionPtr->add( (te::gm::Polygon*)( rastersBBoxes[ 0 ].clone() ) );        
      
      // globals

      std::vector< unsigned int > outputRasterBands;
      std::vector< double > dummyRasterOffsets;
      std::vector< double > dummyRasterScales;
      for( unsigned int outputRasterBandsIdx = 0 ; outputRasterBandsIdx <
        outputRasterPtr->getNumberOfBands() ; ++outputRasterBandsIdx )
      {
        outputRasterBands.push_back( outputRasterBandsIdx );
        dummyRasterOffsets.push_back( 0.0 );
        dummyRasterScales.push_back( 1.0 );
      }
      
      // iterating over the other rasters
      
      te::rst::Raster const* nonCachedInputRasterPtr = 0;    
      
      m_inputParameters.m_feederRasterPtr->reset();
      m_inputParameters.m_feederRasterPtr->moveNext();

      while( ( nonCachedInputRasterPtr = m_inputParameters.m_feederRasterPtr->getCurrentObj() ) )
      {
        const unsigned int inputRasterIdx = m_inputParameters.m_feederRasterPtr->getCurrentOffset();

        te::rst::Raster const* inputRasterPtr = nonCachedInputRasterPtr;

        std::auto_ptr< te::mem::CachedRaster > cachedInputRasterPtr;
        if( m_inputParameters.m_useRasterCache )
        {
          cachedInputRasterPtr.reset( new te::mem::CachedRaster(
            *nonCachedInputRasterPtr, 25, 0 ) );
          inputRasterPtr = cachedInputRasterPtr.get();
        } 
        
        // Generating the offset and gain info for eath band from the current raster

        std::vector< double > currentRasterBandsOffsets = dummyRasterOffsets;
        std::vector< double > currentRasterBandsScales = dummyRasterScales;

        if( m_inputParameters.m_autoEqualize )
        {
          double currentRasterVariance = 0;
          double currentRasterMean = 0;

          for( unsigned int inputRastersBandsIdx = 0 ; inputRastersBandsIdx <
            m_inputParameters.m_inputRastersBands[ inputRasterIdx ].size() ;
            ++inputRastersBandsIdx )
          {
            const unsigned int inputBandIdx = m_inputParameters.m_inputRastersBands[ inputRasterIdx ][
              inputRastersBandsIdx ];
              
            if( 
                ( mosaicTargetMeans[ inputRastersBandsIdx ] != 0.0 )
                &&
                ( mosaicTargetVariances[ inputRastersBandsIdx ] != 0.0 )
              )
            {              
              calcBandStatistics( (*inputRasterPtr->getBand( inputBandIdx ) ),
                m_inputParameters.m_forceInputNoDataValue,
                m_inputParameters.m_noDataValue,
                currentRasterMean,
                currentRasterVariance );
              
              currentRasterBandsScales[ inputRastersBandsIdx ] =
                ( 
                  std::sqrt( mosaicTargetVariances[ inputRastersBandsIdx ] )
                   /
                  std::sqrt( currentRasterVariance ) 
                );
              currentRasterBandsOffsets[ inputRastersBandsIdx ] =
                ( 
                  mosaicTargetMeans[ inputRastersBandsIdx ]
                   - 
                  ( 
                    currentRasterBandsScales[ inputRastersBandsIdx ] 
                    * 
                    currentRasterMean 
                  ) 
                );
            }
          }
        }
        
        // blending with the mosaic boxes union

        te::rp::Blender blenderInstance;

        TERP_TRUE_OR_RETURN_FALSE( blenderInstance.initialize(
          *outputRasterPtr,
          outputRasterBands,
          *inputRasterPtr,
          m_inputParameters.m_inputRastersBands[ inputRasterIdx ],
          m_inputParameters.m_blendMethod,
          te::rst::NearestNeighbor,
          m_inputParameters.m_interpMethod,
          m_inputParameters.m_noDataValue,
          false,
          m_inputParameters.m_forceInputNoDataValue,
          dummyRasterOffsets,
          dummyRasterScales,
          currentRasterBandsOffsets,
          currentRasterBandsScales,
          mosaicBBoxesUnionPtr.get(),
          0,
          *( eachRasterPixelToMosaicRasterPixelGeomTransfms[ inputRasterIdx - 1 ] ),
          m_inputParameters.m_enableMultiThread ? 0 : 1,
          false ), 
          "Blender initiazing error" );
        
        TERP_TRUE_OR_RETURN_FALSE( blenderInstance.blendIntoRaster1(), 
          "Error blending images" );
 
        
        // updating the mosaic related polygons
        
        {
          // union of the current raster box with the current mosaic valid area under the mosaic SRID
          
          std::auto_ptr< te::gm::Geometry > unionMultiPolPtr; 
          unionMultiPolPtr.reset( mosaicBBoxesUnionPtr->Union(
            &( rastersBBoxes[ inputRasterIdx ] ) ) );
          TERP_TRUE_OR_RETURN_FALSE( unionMultiPolPtr.get(), "Invalid pointer" );
          unionMultiPolPtr->setSRID( mosaicBBoxesUnionPtr->getSRID() );
          
          if(
              ( unionMultiPolPtr->getGeomTypeId() == te::gm::MultiPolygonType )
              ||
              ( unionMultiPolPtr->getGeomTypeId() == te::gm::MultiPolygonZType )
              ||
              ( unionMultiPolPtr->getGeomTypeId() == te::gm::MultiPolygonMType )
              ||
              ( unionMultiPolPtr->getGeomTypeId() == te::gm::MultiPolygonZMType )
            )
          {
            mosaicBBoxesUnionPtr.reset( (te::gm::MultiPolygon*)unionMultiPolPtr.release() );
          }
          else if(
                   ( unionMultiPolPtr->getGeomTypeId() == te::gm::PolygonType )
                   ||
                   ( unionMultiPolPtr->getGeomTypeId() == te::gm::PolygonZType )
                   ||
                   ( unionMultiPolPtr->getGeomTypeId() == te::gm::PolygonMType )
                   ||
                   ( unionMultiPolPtr->getGeomTypeId() == te::gm::PolygonZMType ) 
                 )
          {
            te::gm::MultiPolygon* mPolPtr = new te::gm::MultiPolygon( 0, te::gm::MultiPolygonType,
              unionMultiPolPtr->getSRID(), 0 );
            mPolPtr->add( (te::gm::Polygon*)unionMultiPolPtr.release() );
            mosaicBBoxesUnionPtr.reset( mPolPtr );
          }
          else
          {
            TERP_LOGWARN( "Invalid union geometry type" );
          }
        }                
        
        // moving to the next raster
        
        cachedInputRasterPtr.reset();

        m_inputParameters.m_feederRasterPtr->moveNext();
        
        if( m_inputParameters.m_enableProgress )
        {
          progressPtr->pulse();
          if( ! progressPtr->isActive() ) return false;
        }          
      }
      
      return true;
    }

    void TiePointsMosaic::reset() throw( te::rp::Exception )
    {
      m_inputParameters.reset();
      m_isInitialized = false;
    }

    bool TiePointsMosaic::initialize( const AlgorithmInputParameters& inputParams )
      throw( te::rp::Exception )
    {
      reset();

      TiePointsMosaic::InputParameters const* inputParamsPtr = dynamic_cast<
        TiePointsMosaic::InputParameters const* >( &inputParams );
      TERP_TRUE_OR_THROW( inputParamsPtr, "Invalid paramters pointer" );

      m_inputParameters = *inputParamsPtr;

      // Checking the feeder

      TERP_TRUE_OR_RETURN_FALSE( m_inputParameters.m_feederRasterPtr,
        "Invalid m_feederRasterPtr" )

      TERP_TRUE_OR_RETURN_FALSE(
        m_inputParameters.m_feederRasterPtr->getObjsCount() > 1,
        "Invalid number of rasters" )

      // checking m_inputRastersBands

      TERP_TRUE_OR_RETURN_FALSE(
        ((unsigned int)m_inputParameters.m_inputRastersBands.size()) ==
        m_inputParameters.m_feederRasterPtr->getObjsCount(),
        "Bands mismatch" );

      for( std::vector< std::vector< unsigned int > >::size_type
        inputRastersBandsIdx = 0 ;  inputRastersBandsIdx <
        m_inputParameters.m_inputRastersBands.size() ; ++inputRastersBandsIdx )
      {
        TERP_TRUE_OR_RETURN_FALSE( m_inputParameters.m_inputRastersBands[
          inputRastersBandsIdx ].size() > 0, "Invalid bands number" );

        TERP_TRUE_OR_RETURN_FALSE( m_inputParameters.m_inputRastersBands[
          inputRastersBandsIdx ].size() ==  m_inputParameters.m_inputRastersBands[
          0 ].size(), "Bands number mismatch" );
      }

      // checking other parameters

      TERP_TRUE_OR_RETURN_FALSE(
        ( m_inputParameters.m_tiePoints.size() ==
        ( m_inputParameters.m_feederRasterPtr->getObjsCount() - 1 ) ),
        "Bands mismatch" );

      m_isInitialized = true;

      return true;
    }

    bool TiePointsMosaic::isInitialized() const
    {
      return m_isInitialized;
    }

    void TiePointsMosaic::calcBandStatistics( const te::rst::Band& band,
      const bool& forceNoDataValue,
      const double& noDataValue,
      double& mean, double& variance )
    {
      mean = 0;
      variance = 0;

      double internalNoDataValue = 0;
      if( forceNoDataValue )
        internalNoDataValue = noDataValue;
      else
        internalNoDataValue = band.getProperty()->m_noDataValue;

      const unsigned int nCols = band.getProperty()->m_blkw *
        band.getProperty()->m_nblocksx;
      const unsigned int nLines = band.getProperty()->m_blkh *
        band.getProperty()->m_nblocksy;

      double pixelsNumber = 0;
      double value = 0;
      unsigned int col = 0;
      unsigned int line = 0;

      for( line = 0 ; line < nLines ; ++line )
        for( col = 0 ; col < nCols ; ++col )
        {
          band.getValue( col, line, value );

          if( value != internalNoDataValue )
          {
            mean += value;
            ++pixelsNumber;
          }
        }

      if( pixelsNumber != 0.0 )
      {
        mean /= pixelsNumber;

        for( line = 0 ; line < nLines ; ++line )
          for( col = 0 ; col < nCols ; ++col )
          {
            band.getValue( col, line, value );

            if( value != internalNoDataValue )
            {
              variance += ( ( value - mean ) * ( value - mean ) ) / pixelsNumber;
            }
          }

      }
    }

  } // end namespace rp
}   // end namespace te

