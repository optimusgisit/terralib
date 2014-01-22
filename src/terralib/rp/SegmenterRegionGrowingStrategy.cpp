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
  \file terralib/rp/SegmenterRegionGrowingStrategy.cpp
  \briefRaster region growing segmenter strategy.
*/

#include "SegmenterRegionGrowingStrategy.h"

#include "Macros.h"

#include "../raster/Band.h"
#include "../raster/BandProperty.h"
#include "../raster/RasterFactory.h"
#include "../raster/Grid.h"
#include "../datatype/Enums.h"
#include "../common/StringUtils.h"
#include "../common/progress/TaskProgress.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <limits>

#include <boost/lexical_cast.hpp>

// Baatz Edge Lengh
#define BAATZ_EL( featPtr ) featPtr[ 0 ]

// Baatz Compactness
#define BAATZ_CO( featPtr ) featPtr[ 1 ]

// Baatz Smoothness
#define BAATZ_SM( featPtr ) featPtr[ 2 ]

// Baatz sums
#define BAATZ_SU( featPtr, band ) featPtr[ 3 + band ]

// Baatz square sums
#define BAATZ_SS( featPtr, bandsNmb, band ) featPtr[ 3 + bandsNmb + band ]

// Baatz stddev
#define BAATZ_ST( featPtr, bandsNmb, band ) featPtr[ 3 + bandsNmb + bandsNmb + band ]

namespace
{
  static te::rp::SegmenterRegionGrowingStrategyFactory 
    segmenterRegionGrowingStrategyFactoryInstance;
}

namespace te
{
  namespace rp
  {
    //-------------------------------------------------------------------------
    
    SegmenterRegionGrowingStrategy::Parameters::Parameters()
    {
      reset();
    }
    
    SegmenterRegionGrowingStrategy::Parameters::~Parameters()
    {
    }    
    
    const SegmenterRegionGrowingStrategy::Parameters& 
    SegmenterRegionGrowingStrategy::Parameters::operator=( 
      const SegmenterRegionGrowingStrategy::Parameters& params )
    {
      reset();
      
      m_minSegmentSize = params.m_minSegmentSize;
      m_segmentsSimilarityThreshold = params.m_segmentsSimilarityThreshold;
      m_segmentFeatures = params.m_segmentFeatures;
      m_bandsWeights = params.m_bandsWeights;
      m_colorWeight = params.m_colorWeight;
      m_compactnessWeight = params.m_compactnessWeight;
      m_segmentsSimIncreaseSteps = params.m_segmentsSimIncreaseSteps;
      
      return *this;      
    }
    
    void SegmenterRegionGrowingStrategy::Parameters::reset() 
    throw( te::rp::Exception )
    {
      m_minSegmentSize = 100;
      m_segmentsSimilarityThreshold = 0.9;
      m_segmentFeatures = InvalidFeaturesType;      
      m_bandsWeights.clear();
      m_colorWeight = 0.5;
      m_compactnessWeight = 0.5;
      m_segmentsSimIncreaseSteps = 10;
    }
    
    te::common::AbstractParameters* SegmenterRegionGrowingStrategy::Parameters::clone() const
    {
      return new te::rp::SegmenterRegionGrowingStrategy::Parameters( *this );
    }

    //-------------------------------------------------------------------------
    
    SegmenterRegionGrowingStrategy::Merger::Merger()
    {
    }
    
    SegmenterRegionGrowingStrategy::Merger::~Merger()
    {
    } 

    //-------------------------------------------------------------------------
    
    SegmenterRegionGrowingStrategy::MeanMerger::MeanMerger( const unsigned int featuresNumber )
    : m_featuresNumber( featuresNumber )
    {
    }
    
    SegmenterRegionGrowingStrategy::MeanMerger::~MeanMerger()
    {
    }    
    
    SegmenterRegionGrowingSegment::FeatureType SegmenterRegionGrowingStrategy::MeanMerger::getSimilarity(
      SegmenterRegionGrowingSegment const * const segment1Ptr, 
      SegmenterRegionGrowingSegment const * const segment2Ptr, 
      SegmenterRegionGrowingSegment * const ) const
    {
      assert( segment1Ptr );
      assert( segment1Ptr->m_features );
      assert( segment2Ptr );
      assert( segment2Ptr->m_features );
      
      SegmenterRegionGrowingSegment::FeatureType dissValue = 0.0;
      SegmenterRegionGrowingSegment::FeatureType diffValue = 0.0;
        
      for( unsigned int meansIdx = 0 ; meansIdx < m_featuresNumber ; ++meansIdx )
      {
        diffValue = segment1Ptr->m_features[ meansIdx ] - 
          segment2Ptr->m_features[ meansIdx ];
          
        dissValue += ( diffValue * diffValue );
      }
        
      return 
        ( 
          1.0 
          - 
          std::min( 
            (SegmenterRegionGrowingSegment::FeatureType)1
            , 
            std::max( 
              (SegmenterRegionGrowingSegment::FeatureType)0
              , 
              std::sqrt( dissValue ) 
            ) 
          ) 
        );
    }
    
    void SegmenterRegionGrowingStrategy::MeanMerger::mergeFeatures( 
      SegmenterRegionGrowingSegment * const segment1Ptr, 
      SegmenterRegionGrowingSegment const * const segment2Ptr, 
      SegmenterRegionGrowingSegment const * const ) const
    {
      assert( segment1Ptr );
      assert( segment1Ptr->m_features );
      assert( segment2Ptr );
      assert( segment2Ptr->m_features );
        
      // Merging basic features
      
      segment1Ptr->m_size += segment2Ptr->m_size;
      
      segment1Ptr->m_xStart = std::min( 
        segment1Ptr->m_xStart, 
        segment2Ptr->m_xStart );
      segment1Ptr->m_xBound = std::max( 
        segment1Ptr->m_xBound, 
        segment2Ptr->m_xBound );

      segment1Ptr->m_yStart = std::min( 
        segment1Ptr->m_yStart, 
        segment2Ptr->m_yStart );
      segment1Ptr->m_yBound = std::max( 
        segment1Ptr->m_yBound, 
        segment2Ptr->m_yBound );
        
      // Merging specific features
        
      for( unsigned int meansIdx = 0 ; meansIdx < m_featuresNumber ; ++meansIdx )
      {
        segment1Ptr->m_features[ meansIdx ] = 
          (
            ( 
              segment1Ptr->m_features[ meansIdx ] 
              * 
              ((double)segment1Ptr->m_size ) 
            ) 
            +
            ( 
              segment2Ptr->m_features[ meansIdx ] 
              *
              ( (double)segment2Ptr->m_size) 
            )
          )
          / 
          (
            (double)
            ( 
              segment1Ptr->m_size 
              + 
              segment2Ptr->m_size
            )
          );
      }
    }
    
    //-------------------------------------------------------------------------
    
    SegmenterRegionGrowingStrategy::BaatzMerger::BaatzMerger(
      const double& colorWeight, const double& compactnessWeight,
      const std::vector< double >& bandsWeights,
      const SegmentsIdsMatrixT& segmentsIds,
      const SegmenterRegionGrowingStrategy::SegmentsIndexerT& segments)
      : 
        m_segmentsIds( segmentsIds ),
        m_segments( segments ),      
        m_allSegsCompactnessOffset( 0 ),
        m_allSegsCompactnessGain( 1.0 ),
        m_allSegsSmoothnessOffset( 0 ),
        m_allSegsSmoothnessGain( 0 ),
        m_colorWeight( (SegmenterRegionGrowingSegment::FeatureType)colorWeight ),
        m_compactnessWeight( (SegmenterRegionGrowingSegment::FeatureType)compactnessWeight )
    {
      m_bandsNumber = (unsigned int)bandsWeights.size();
      
      m_allSegsStdDevOffsets.resize( m_bandsNumber, 0 );
      m_allSegsStdDevGain.resize( m_bandsNumber, 1 );
      m_bandsWeights.resize( m_bandsNumber, 1 );
      
      for( unsigned int band = 0 ; band < m_bandsNumber ; ++band )
      {
        m_bandsWeights[ band ] = (SegmenterRegionGrowingSegment::FeatureType)
          bandsWeights[ band ];
      }
    }
    
    SegmenterRegionGrowingStrategy::BaatzMerger::~BaatzMerger()
    {
    }    
    
    SegmenterRegionGrowingSegment::FeatureType SegmenterRegionGrowingStrategy::BaatzMerger::getSimilarity(
      SegmenterRegionGrowingSegment const * const segment1Ptr, 
      SegmenterRegionGrowingSegment const * const segment2Ptr, 
      SegmenterRegionGrowingSegment * const mergePreviewSegPtr ) const
    {
      assert( segment1Ptr );
      assert( segment1Ptr->m_features );
      assert( segment2Ptr );
      assert( segment2Ptr->m_features );      
      assert( mergePreviewSegPtr );
      
      // globals
      
      mergePreviewSegPtr->m_size = segment1Ptr->m_size + 
        segment2Ptr->m_size;
      TERP_DEBUG_TRUE_OR_THROW( mergePreviewSegPtr->m_size,
        "Internal error" );
      const SegmenterRegionGrowingSegment::FeatureType sizeUnionD = 
        (SegmenterRegionGrowingSegment::FeatureType)mergePreviewSegPtr->m_size;
      
      const SegmenterRegionGrowingSegment::FeatureType sizeSeg1D = 
        (SegmenterRegionGrowingSegment::FeatureType)segment1Ptr->m_size;      
      
      const SegmenterRegionGrowingSegment::FeatureType sizeSeg2D = 
        (SegmenterRegionGrowingSegment::FeatureType)segment2Ptr->m_size;      
      
      // Finding the form heterogeneity
      
      mergePreviewSegPtr->m_xStart = std::min( segment1Ptr->m_xStart,
        segment2Ptr->m_xStart );
      mergePreviewSegPtr->m_yStart = std::min( segment1Ptr->m_yStart,
        segment2Ptr->m_yStart );        
      mergePreviewSegPtr->m_xBound = std::max( segment1Ptr->m_xBound,
        segment2Ptr->m_xBound );      
      mergePreviewSegPtr->m_yBound = std::max( segment1Ptr->m_yBound,
        segment2Ptr->m_yBound ); 
        
      assert( mergePreviewSegPtr->m_xBound > mergePreviewSegPtr->m_xStart );
      assert( mergePreviewSegPtr->m_yBound > mergePreviewSegPtr->m_yStart );
        
      unsigned int touchingEdgeLength1 = 0;
      unsigned int touchingEdgeLength2 = 0;
      SegmenterRegionGrowingStrategy::getTouchingEdgeLength(
        m_segmentsIds, mergePreviewSegPtr->m_xStart, 
        mergePreviewSegPtr->m_yStart,
        mergePreviewSegPtr->m_xBound, 
        mergePreviewSegPtr->m_yBound, 
        segment1Ptr->m_id,
        segment2Ptr->m_id,
        touchingEdgeLength1,
        touchingEdgeLength2 );
      
      BAATZ_EL( mergePreviewSegPtr->m_features ) = 
        BAATZ_EL( segment1Ptr->m_features ) - touchingEdgeLength1 
        +
        BAATZ_EL( segment2Ptr->m_features ) - touchingEdgeLength2;
      
      BAATZ_CO( mergePreviewSegPtr->m_features ) = (SegmenterRegionGrowingSegment::FeatureType)(
        ((double)BAATZ_EL( mergePreviewSegPtr->m_features )) /
        std::sqrt( sizeUnionD ) );
        
      BAATZ_SM( mergePreviewSegPtr->m_features ) =
        BAATZ_EL( mergePreviewSegPtr->m_features ) 
        /
        (SegmenterRegionGrowingSegment::FeatureType)(
          (
            2 * ( mergePreviewSegPtr->m_xBound - mergePreviewSegPtr->m_xStart )
          )
          +
          (
            2 * ( mergePreviewSegPtr->m_yBound - mergePreviewSegPtr->m_yStart )
          )
        );
        
      const SegmenterRegionGrowingSegment::FeatureType hCompact = 
        (
          std::abs(
            BAATZ_CO( mergePreviewSegPtr->m_features )
            -
            (
              (
                (
                  BAATZ_CO( segment1Ptr->m_features )
                  *
                  sizeSeg1D
                )
                +
                (
                  BAATZ_CO( segment2Ptr->m_features )
                  *
                  sizeSeg2D
                )
              )
              /
              sizeUnionD
            )
          )
          +
          m_allSegsCompactnessOffset
        )
        *
        m_allSegsCompactnessGain;
      
      const SegmenterRegionGrowingSegment::FeatureType hSmooth =
        (
          std::abs(
            BAATZ_SM( mergePreviewSegPtr->m_features )
            -
            (
              (
                (
                  BAATZ_SM( segment1Ptr->m_features )
                  *
                  sizeSeg1D
                )
                +
                (
                  BAATZ_SM( segment2Ptr->m_features )
                  *
                  sizeSeg2D
                )
              )
              /
              sizeUnionD
            )
          )
          +
          m_allSegsSmoothnessOffset
        )
        *
        m_allSegsSmoothnessGain;
        
      const SegmenterRegionGrowingSegment::FeatureType hForm = 
        (
          (
            m_compactnessWeight 
            *
            hCompact
          )
          +
          (
            ( 1.0 - m_compactnessWeight )
            *
            hSmooth
          )
        );
      
      // Finding the color heterogeneity
      
      SegmenterRegionGrowingSegment::FeatureType hColor = 0;
      SegmenterRegionGrowingSegment::FeatureType sumUnion = 0;
      SegmenterRegionGrowingSegment::FeatureType squaresSumUnion = 0;
      SegmenterRegionGrowingSegment::FeatureType meanUnion = 0;
      SegmenterRegionGrowingSegment::FeatureType stdDevUnion = 0.0;      
      
      for( unsigned int sumsIdx = 0 ; sumsIdx < m_bandsNumber ; ++sumsIdx )
      {
        const SegmenterRegionGrowingSegment::FeatureType& sum1 = 
          BAATZ_SU( segment1Ptr->m_features, sumsIdx );
        
        const SegmenterRegionGrowingSegment::FeatureType& sum2 = 
          BAATZ_SU( segment2Ptr->m_features, sumsIdx );
        
        sumUnion = sum1 + sum2;
        BAATZ_SU( mergePreviewSegPtr->m_features, sumsIdx ) = sumUnion;        
        
        squaresSumUnion = BAATZ_SS( segment1Ptr->m_features, m_bandsNumber, sumsIdx ) +
          BAATZ_SS( segment2Ptr->m_features, m_bandsNumber, sumsIdx );
        BAATZ_SS( mergePreviewSegPtr->m_features, m_bandsNumber, sumsIdx ) = squaresSumUnion;
        
        meanUnion = ( sum1 + sum2 ) / sizeUnionD;
        
        stdDevUnion =
          (
            (
              squaresSumUnion
              -
              (
                2.0 * meanUnion * sumUnion
              )
              +
              (
                sizeUnionD * meanUnion * meanUnion
              )
            )
            /
            sizeUnionD
          );
        BAATZ_ST( mergePreviewSegPtr->m_features, m_bandsNumber, sumsIdx ) =
          stdDevUnion;        
         
        hColor += 
          ( 
            m_bandsWeights[ sumsIdx ]
            *
            (
              (
                std::abs(
                  stdDevUnion
                  -
                  (
                    (
                      (
                        BAATZ_ST( segment1Ptr->m_features, m_bandsNumber, sumsIdx )
                        *
                        sizeSeg1D
                      )
                      +
                      (
                        BAATZ_ST( segment2Ptr->m_features, m_bandsNumber, sumsIdx )
                        *
                        sizeSeg2D
                      )
                    )
                    /
                    sizeUnionD
                  )
                )
                +
                m_allSegsStdDevOffsets[ sumsIdx ]
              )
              *
              m_allSegsStdDevGain[ sumsIdx ]
            )
          );
      }
      
      return 1.0 - 
        (
          ( 
            hColor 
            * 
            m_colorWeight 
          ) 
          + 
          (
            ( 1.0 - m_colorWeight )
            *
            hForm
          )
        );       
    }
    
    void SegmenterRegionGrowingStrategy::BaatzMerger::mergeFeatures( 
      SegmenterRegionGrowingSegment * const segment1Ptr, 
      SegmenterRegionGrowingSegment const * const segment2Ptr , 
      SegmenterRegionGrowingSegment const * const mergePreviewSegPtr ) const
    {
      assert( segment1Ptr );
      assert( segment1Ptr->m_features );
      assert( segment2Ptr );
      assert( segment2Ptr->m_features );      
      assert( mergePreviewSegPtr );
      
      // Merging basic features
      
      segment1Ptr->m_size = mergePreviewSegPtr->m_size;
      segment1Ptr->m_xStart = mergePreviewSegPtr->m_xStart;
      segment1Ptr->m_xBound = mergePreviewSegPtr->m_xBound;
      segment1Ptr->m_yStart = mergePreviewSegPtr->m_yStart;
      segment1Ptr->m_yBound = mergePreviewSegPtr->m_yBound;
        
      // Merging specific features   
      
      memcpy( segment1Ptr->m_features, mergePreviewSegPtr->m_features, 3 + ( 3 *
        sizeof( SegmenterRegionGrowingSegment::FeatureType ) * m_bandsNumber ) );
    }    
    
    void SegmenterRegionGrowingStrategy::BaatzMerger::update()
    {
      SegmenterRegionGrowingStrategy::SegmentsIndexerT::const_iterator itB =
        m_segments.begin();
      const SegmenterRegionGrowingStrategy::SegmentsIndexerT::const_iterator itE =
        m_segments.end();        
      std::vector< SegmenterRegionGrowingSegment::FeatureType >::size_type dimIdx = 0;
      std::vector< SegmenterRegionGrowingSegment::FeatureType >::size_type dimsNumber = m_bandsNumber;
      
      std::vector< SegmenterRegionGrowingSegment::FeatureType > stdDevMin( dimsNumber,
        std::numeric_limits< SegmenterRegionGrowingSegment::FeatureType >::max() );
      std::vector< SegmenterRegionGrowingSegment::FeatureType > stdDevMax( dimsNumber, 
        -1.0 * std::numeric_limits< SegmenterRegionGrowingSegment::FeatureType >::max() );
      
      SegmenterRegionGrowingSegment::FeatureType compactnessMin = 
        std::numeric_limits< SegmenterRegionGrowingSegment::FeatureType >::max();
      SegmenterRegionGrowingSegment::FeatureType compactnessMax = -1.0 * 
        std::numeric_limits< SegmenterRegionGrowingSegment::FeatureType >::max();

      SegmenterRegionGrowingSegment::FeatureType smoothnessMin =
        std::numeric_limits< SegmenterRegionGrowingSegment::FeatureType >::max();
      SegmenterRegionGrowingSegment::FeatureType smoothnessMax = -1.0 *
        std::numeric_limits< SegmenterRegionGrowingSegment::FeatureType >::max();
      
      while( itB != itE )
      {
        for( dimIdx = 0; dimIdx < dimsNumber ; ++dimIdx )
        {
          if( stdDevMin[ dimIdx ] > BAATZ_ST( (*itB)->m_features, m_bandsNumber, dimIdx ) )
            stdDevMin[ dimIdx ] = BAATZ_ST( (*itB)->m_features, m_bandsNumber, dimIdx );
          if( stdDevMax[ dimIdx ] < BAATZ_ST( (*itB)->m_features, m_bandsNumber, dimIdx ) )
            stdDevMax[ dimIdx ] = BAATZ_ST( (*itB)->m_features, m_bandsNumber, dimIdx );          
        }
        
        if( compactnessMin > BAATZ_CO( (*itB)->m_features ) )
          compactnessMin = BAATZ_CO( (*itB)->m_features );        
        if( compactnessMax < BAATZ_CO( (*itB)->m_features ) )
          compactnessMax = BAATZ_CO( (*itB)->m_features );        
        
        if( smoothnessMin > BAATZ_SM( (*itB)->m_features ) )
          smoothnessMin = BAATZ_SM( (*itB)->m_features );
        if( smoothnessMax < BAATZ_SM( (*itB)->m_features ) )
          smoothnessMax = BAATZ_SM( (*itB)->m_features );
        
        ++itB;
      }
      
      for( dimIdx = 0; dimIdx < dimsNumber ; ++dimIdx )
      {
        if( stdDevMax[ dimIdx ] == stdDevMin[ dimIdx ] )
        {
          m_allSegsStdDevOffsets[ dimIdx ] = 0.0;
          
          if( stdDevMax[ dimIdx ] == 0.0 )
            m_allSegsStdDevGain[ dimIdx ] = 1.0;
          else
            m_allSegsStdDevGain[ dimIdx ] = 1.0 / stdDevMax[ dimIdx ];
        }
        else
        {
          m_allSegsStdDevOffsets[ dimIdx ] = -1.0 * stdDevMin[ dimIdx ];
          m_allSegsStdDevGain[ dimIdx ] = 1.0 / 
            ( stdDevMax[ dimIdx ] - stdDevMin[ dimIdx ] );
        }
      }      
      
      if( compactnessMax == compactnessMin )
      {
        m_allSegsCompactnessOffset = 0.0;
        
        if( compactnessMax == 0.0 )
          m_allSegsCompactnessGain = 1.0;
        else
          m_allSegsCompactnessGain = 1.0 / compactnessMax;
      }
      else
      {
        m_allSegsCompactnessOffset = -1.0  * compactnessMin;
        m_allSegsCompactnessGain = 1.0 / ( compactnessMax - compactnessMin );
      }
      
      if( smoothnessMax == smoothnessMin )
      {
        m_allSegsSmoothnessOffset = 0.0;
        
        if( smoothnessMax == 0.0 )
          m_allSegsSmoothnessGain = 1.0;
        else
          m_allSegsSmoothnessGain = 1.0 / smoothnessMax;
      }
      else
      {
        m_allSegsSmoothnessOffset = -1.0  * smoothnessMin;
        m_allSegsSmoothnessGain = 1.0 / ( smoothnessMax - smoothnessMin );      
      }
    }
    
    //-------------------------------------------------------------------------
    
    SegmenterRegionGrowingStrategy::SegmenterRegionGrowingStrategy()
    {
      m_isInitialized = false;
    }
    
    SegmenterRegionGrowingStrategy::~SegmenterRegionGrowingStrategy()
    {
      reset();
    }
    
    bool SegmenterRegionGrowingStrategy::initialize( 
      SegmenterStrategyParameters const* const strategyParams ) 
      throw( te::rp::Exception )
    {
      m_isInitialized = false;
      reset();
      
      SegmenterRegionGrowingStrategy::Parameters const* paramsPtr = 
        dynamic_cast< SegmenterRegionGrowingStrategy::Parameters const* >( strategyParams );
        
      if( paramsPtr )
      {
        m_parameters = *( paramsPtr );
        
        TERP_TRUE_OR_RETURN_FALSE( m_parameters.m_minSegmentSize > 0,
          "Invalid segmenter strategy parameter m_minSegmentSize" )
          
        TERP_TRUE_OR_RETURN_FALSE(  
          ( m_parameters.m_segmentsSimilarityThreshold >= 0.0 ),
          "Invalid segmenter strategy parameter m_segmentsSimilarityThreshold" )  
          
        TERP_TRUE_OR_RETURN_FALSE( m_parameters.m_segmentFeatures != 
          SegmenterRegionGrowingStrategy::Parameters::InvalidFeaturesType,
          "Invalid segmenter strategy parameter m_segmentFeatures" )              
          
        if( ! m_parameters.m_bandsWeights.empty() )
        {
          TERP_TRUE_OR_RETURN_FALSE( paramsPtr->m_bandsWeights.size(),
            "Invalid segmenter strategy parameter m_bandsWeights" );
          double bandsWeightsSum = 0;
          for( unsigned int bandsWeightsIdx = 0 ; bandsWeightsIdx < 
            paramsPtr->m_bandsWeights.size() ; ++bandsWeightsIdx )
          {
            bandsWeightsSum += paramsPtr->m_bandsWeights[ bandsWeightsIdx ];
          }
          TERP_TRUE_OR_RETURN_FALSE( bandsWeightsSum == 1.0,
            "Invalid segmenter strategy parameter m_bandsWeights" );        
        }
        
        m_isInitialized = true;
        
        return true;        
      } 
      else
      {
        return false;
      }
    }
    
    void SegmenterRegionGrowingStrategy::reset()
    {
      m_isInitialized = false;
      m_segmentsPool.clear();
      m_segmentsIdsMatrix.reset();
      m_parameters.reset();
    };
    
    bool SegmenterRegionGrowingStrategy::execute( 
      SegmenterIdsManager& segmenterIdsManager,
      const te::rst::Raster& inputRaster,
      const std::vector< unsigned int >& inputRasterBands,
      const std::vector< double >& inputRasterGains,
      const std::vector< double >& inputRasterOffsets,                                                   
      te::rst::Raster& outputRaster,
      const unsigned int outputRasterBand,
      const bool enableProgressInterface )
      throw( te::rp::Exception )
    {
      TERP_TRUE_OR_RETURN_FALSE( m_isInitialized,
        "Instance not initialized" )
        
      // Updating the bands weights info, if needed
      
      if( m_parameters.m_bandsWeights.empty() )
        m_parameters.m_bandsWeights.resize( inputRasterBands.size(), 1.0 /
        ((double)inputRasterBands.size()) );
      
      // Initiating the segments pool
      
      unsigned int segmentFeaturesSize = 0;
      switch( m_parameters.m_segmentFeatures )
      {
        case Parameters::MeanFeaturesType :
        {
          segmentFeaturesSize = inputRasterBands.size();
          break;
        }
        case Parameters::BaatzFeaturesType :
        {
          segmentFeaturesSize = 3 + ( 3 * inputRasterBands.size() );
          break;
        }
        default :
        {
          TERP_LOG_AND_THROW( "Invalid segment features type" );
          break;
        }
      }     
      // The number of segments plus 3 (due 3 auxiliary segments
      TERP_TRUE_OR_RETURN_FALSE( m_segmentsPool.initialize( 3 + ( inputRaster.getNumberOfRows() * 
        inputRaster.getNumberOfColumns() ), 
        segmentFeaturesSize ), "Segments pool initiation error" );     
        
//       {
//         // checking alignment        
//         SegmenterRegionGrowingSegment* auxSegPtr = 0;
//         unsigned int counter = 0;
//         while( auxSegPtr = m_segmentsPool.getNextSegment() )
//         {
//           for( unsigned int featureIdx = 0 ; featureIdx < auxSegPtr->m_featuresSize ;
//             ++featureIdx )
//           {
//             auxSegPtr->m_features[ featureIdx ] = (SegmenterRegionGrowingSegment::FeatureType)
//             counter;
//           }
//         }
//         m_segmentsPool.resetUseCounter();
//         counter = 0;
//         while( auxSegPtr = m_segmentsPool.getNextSegment() )
//         {
//           for( unsigned int featureIdx = 0 ; featureIdx < auxSegPtr->m_featuresSize ;
//             ++featureIdx )
//           {          
//             if( auxSegPtr->m_features[ featureIdx ] != (SegmenterRegionGrowingSegment::FeatureType)
//               counter ) throw;
//           }
//         }        
//       }
        
      SegmenterRegionGrowingSegment* auxSeg1Ptr = m_segmentsPool.getNextSegment();
      SegmenterRegionGrowingSegment* auxSeg2Ptr = m_segmentsPool.getNextSegment();
      SegmenterRegionGrowingSegment* auxSeg3Ptr = m_segmentsPool.getNextSegment();
      
      // Allocating the ids matrix
      
      if( ( m_segmentsIdsMatrix.getLinesNumber() != inputRaster.getNumberOfRows() ) ||
        ( m_segmentsIdsMatrix.getColumnsNumber() != inputRaster.getNumberOfColumns() ) )
      {
        TERP_TRUE_OR_RETURN_FALSE( m_segmentsIdsMatrix.reset( inputRaster.getNumberOfRows(), 
          inputRaster.getNumberOfColumns(),
          Matrix< SegmenterSegmentsBlock::SegmentIdDataType >::RAMMemPol ),
          "Error allocating segments Ids matrix" );
      }      
        
      // Initializing segments
        
      SegmentsIndexerT segmentsIndexer;
      TERP_TRUE_OR_RETURN_FALSE( initializeSegments( segmenterIdsManager,
        inputRaster, inputRasterBands, inputRasterGains,
        inputRasterOffsets, segmentsIndexer ), 
        "Segments initalization error" );
        
      // Creating the merger instance
      
      std::auto_ptr< Merger > mergerPtr;
      bool enablelocalMutualBestFitting = false;
      
      switch( m_parameters.m_segmentFeatures )
      {
        case Parameters::MeanFeaturesType :
        {
          mergerPtr.reset( new MeanMerger( inputRasterBands.size() ) );
          enablelocalMutualBestFitting = true;
          break;
        }
        case Parameters::BaatzFeaturesType :
        {
          mergerPtr.reset( new BaatzMerger( m_parameters.m_colorWeight,
            m_parameters.m_compactnessWeight, m_parameters.m_bandsWeights,
            m_segmentsIdsMatrix, segmentsIndexer ) );
          enablelocalMutualBestFitting = true;
          break;
        }
        default :
        {
          TERP_LOG_AND_THROW( "Invalid segment features type" );
          break;
        }
      }      
      
      // Progress interface
      
      std::auto_ptr< te::common::TaskProgress > progressPtr;
      if( enableProgressInterface )
      {
        progressPtr.reset( new te::common::TaskProgress );
        progressPtr->setTotalSteps( 100 );
        progressPtr->setMessage( "Segmentation" );
      }          
      
      // Segmentation loop
        
      SegmenterRegionGrowingSegment::FeatureType similarityThreshold = 1.0;
      const SegmenterRegionGrowingSegment::FeatureType similarityThresholdStep = 
        ( 
          ( 1.0 - ((SegmenterRegionGrowingSegment::FeatureType)m_parameters.m_segmentsSimilarityThreshold) )
          /
          ( (SegmenterRegionGrowingSegment::FeatureType)( m_parameters.m_segmentsSimIncreaseSteps + 1 ) )
        );
      unsigned int mergedSegments = 0;
      unsigned int maxMergedSegments = 0;
      int currStep = 0;
      
//       unsigned int mergetIterations = 0;
//       exportSegs2Tif( m_segmentsIdsMatrix, true, "merging" + 
//         te::common::Convert2String( mergetIterations ) + ".tif" );
      
      while ( true )
      {
        mergedSegments = mergeSegments( similarityThreshold, segmenterIdsManager, 
          *mergerPtr, enablelocalMutualBestFitting, segmentsIndexer,
          auxSeg1Ptr, auxSeg2Ptr, auxSeg3Ptr);
//         exportSegs2Tif( m_segmentsIdsMatrix, true, "merging" + 
//           te::common::Convert2String( ++mergetIterations ) + ".tif" );

        if( enableProgressInterface )
        {
          if( maxMergedSegments )
          {
            currStep = (int)
              ( 
                (
                  ( 
                    ( (double)( maxMergedSegments - mergedSegments ) ) 
                    / 
                    ((double)maxMergedSegments ) 
                  )
                ) 
                * 
                50.0
              );
            
            if( currStep > progressPtr->getCurrentStep() )
            {
              progressPtr->setCurrentStep( currStep );
            }
          }
          
          if( ! progressPtr->isActive() ) 
          {
            return false;
          }   
          
          if( maxMergedSegments < mergedSegments )
          {
            maxMergedSegments = mergedSegments;
          }                  
        }
        
        if( mergedSegments == 0 )
        {
          if( similarityThreshold == (SegmenterRegionGrowingSegment::FeatureType)m_parameters.m_segmentsSimilarityThreshold ) 
          {
            break;
          }
          else
          {
            similarityThreshold -= similarityThresholdStep;
            similarityThreshold = std::max( similarityThreshold,
              (SegmenterRegionGrowingSegment::FeatureType)m_parameters.m_segmentsSimilarityThreshold);
          }
        }
      }
      
      if( enableProgressInterface )
      {      
        progressPtr->setCurrentStep( 50 );
        if( ! progressPtr->isActive() ) 
        {
          return false;
        }         
      }
      
      if( m_parameters.m_minSegmentSize > 1 )
      {
        maxMergedSegments = 0;
        
        while( true )
        {
          mergedSegments = mergeSmallSegments( m_parameters.m_minSegmentSize, 
            segmenterIdsManager, *mergerPtr, segmentsIndexer, auxSeg1Ptr, auxSeg2Ptr );
//        exportSegs2Tif( segmentsIds, true, "mergingSmall" + 
//          te::common::Convert2String( mergetIterations ) + ".tif" );

          if( enableProgressInterface )
          {
            if( maxMergedSegments )
            {
              currStep = 50 + (int)
                ( 
                  (
                    ( 
                      ( (double)( maxMergedSegments - mergedSegments ) ) 
                      / 
                      ((double)maxMergedSegments ) 
                    )
                  ) 
                  * 
                  50.0
                );                
              
              if( currStep > progressPtr->getCurrentStep() )
              {
                progressPtr->setCurrentStep( currStep );
              }
            }
            
            if( ! progressPtr->isActive() ) 
            {
              return false;
            }          
            
            if( maxMergedSegments < mergedSegments )
            {
              maxMergedSegments = mergedSegments;
            }             
          }
          
          if( mergedSegments == 0 )
          {
            break;
          }
        }
      }
      
      if( enableProgressInterface )
      {      
        progressPtr->setCurrentStep( 100 );
        if( ! progressPtr->isActive() ) 
        {
          return false;
        }         
      }      
      
      // Flush result to the output raster
      
      {
        const unsigned int nLines = inputRaster.getNumberOfRows();
        const unsigned int nCols = inputRaster.getNumberOfColumns();
        unsigned int col = 0;
        SegmenterSegmentsBlock::SegmentIdDataType* segmentsIdsLinePtr = 0;
        
        for( unsigned int line = 0 ; line < nLines ; ++line )
        {
          segmentsIdsLinePtr = m_segmentsIdsMatrix[ line ];
          
          for( col = 0 ; col < nCols ; ++col )
          {
            outputRaster.setValue( col, line, segmentsIdsLinePtr[ col ], outputRasterBand );
          }
        }
      }
      
      return true;
    }
    
    double SegmenterRegionGrowingStrategy::getMemUsageEstimation(
      const unsigned int bandsToProcess,
      const unsigned int pixelsNumber ) const
    {
      TERP_TRUE_OR_THROW( m_isInitialized, "Instance not initialized" );
      
      // Test case: 6768 x 67923 = 45968256 pixels image
      // case1: 1 band
      // case2: 3 bands      
      
      double minM = 0;
      double maxM = 0;
      
      switch( m_parameters.m_segmentFeatures )
      {
        case Parameters::MeanFeaturesType :
        {
          minM = 17009.0; // MB
          maxM = 17711.0; // MB
          break;
        }
        case Parameters::BaatzFeaturesType :
        {
          minM = 23629.4; // MB
          maxM = 24597.0; // MB
          break;
        }
        default :
        {
          TERP_LOG_AND_THROW( "Invalid segment features type" );
          break;
        }
      } 
      
      if( bandsToProcess == 3 )
      {
        return maxM;
      }
      else
      {
        double dM = ( maxM - minM );
        double slope = dM / ( 3.0 - ((double)bandsToProcess) );
        
        return (
                 ( ((double)bandsToProcess) * slope ) + minM
               )
               *
               (
                 ((double)pixelsNumber) / 45968256.0 
               )
               *
               (
                 1024.0 * 1024.0
               );
      }
    }
    
    unsigned int SegmenterRegionGrowingStrategy::getOptimalBlocksOverlapSize() const
    {
      TERP_TRUE_OR_THROW( m_isInitialized, "Instance not initialized" );
      return (unsigned int)( std::sqrt( (double)m_parameters.m_minSegmentSize) );
    }
    
    bool SegmenterRegionGrowingStrategy::initializeSegments( 
      SegmenterIdsManager& segmenterIdsManager,
      const te::rst::Raster& inputRaster,
      const std::vector< unsigned int >& inputRasterBands,   
      const std::vector< double >& inputRasterGains,
      const std::vector< double >& inputRasterOffsets,                                                              
      SegmentsIndexerT& segsIndexer )
    {
      segsIndexer.clear();
      
      const unsigned int nLines = inputRaster.getNumberOfRows();
      const unsigned int nCols = inputRaster.getNumberOfColumns();
      const unsigned int inputRasterBandsSize = (unsigned int)
        inputRasterBands.size();
        
      // fiding band dummy values
      
      std::vector< double > bandDummyValues;
      
      {
        for( unsigned int inputRasterBandsIdx = 0 ; inputRasterBandsIdx < 
          inputRasterBandsSize ; ++inputRasterBandsIdx )
        {
          bandDummyValues.push_back( inputRaster.getBand( 
            inputRasterBands[ inputRasterBandsIdx ] )->getProperty()->m_noDataValue );
        }
      }
        
      // Indexing each segment
      
      unsigned int line = 0;
      unsigned int col = 0;      
      SegmenterRegionGrowingSegment* segmentPtr = 0;
      SegmenterRegionGrowingSegment* neighborSegmentPtr = 0;
      bool rasterValuesAreValid = true;
      unsigned int inputRasterBandsIdx = 0;
      double value = 0;
      const std::vector< double > dummyZeroesVector( inputRasterBandsSize, 0 );

      std::list< SegmenterSegmentsBlock::SegmentIdDataType > 
        unusedLineSegmentIds;
      
      std::vector< SegmenterSegmentsBlock::SegmentIdDataType > 
        lineSegmentIds;
      lineSegmentIds.reserve( nCols );
      
      std::vector< SegmenterRegionGrowingSegment::FeatureType > rasterValues;
      std::vector< SegmenterRegionGrowingSegment::FeatureType > rasterSquareValues;
      rasterValues.resize( inputRasterBandsSize, 0 );
      rasterSquareValues.resize( inputRasterBandsSize, 0 );
      std::vector< SegmenterRegionGrowingSegment* > usedSegPointers1( nCols, 0 );
      std::vector< SegmenterRegionGrowingSegment* > usedSegPointers2( nCols, 0 );
      std::vector< SegmenterRegionGrowingSegment* >* lastLineSegsPtrs = &usedSegPointers1;
      std::vector< SegmenterRegionGrowingSegment* >* currLineSegsPtrs = &usedSegPointers2;
      
      unsigned int rasterValuesIdx = 0;
      
      for( line = 0 ; line < nLines ; ++line )
      {
        segmenterIdsManager.getNewIDs( nCols, lineSegmentIds );
        
        for( col = 0 ; col < nCols ; ++col )
        {
          rasterValuesAreValid = true;
          
          for( inputRasterBandsIdx = 0 ; inputRasterBandsIdx < 
            inputRasterBandsSize ; ++inputRasterBandsIdx )
          {
            inputRaster.getValue( col, line, value, 
              inputRasterBands[ inputRasterBandsIdx ] );
              
            if( value == bandDummyValues[ inputRasterBandsIdx ] )
            {
              rasterValuesAreValid = false;
              break;
            }
            else
            {
              value += inputRasterOffsets[ inputRasterBandsIdx ];
              value *= inputRasterGains[ inputRasterBandsIdx ];
              
              rasterValues[ inputRasterBandsIdx ] = 
                (SegmenterRegionGrowingSegment::FeatureType)value;
              rasterSquareValues[ inputRasterBandsIdx ] = 
                (SegmenterRegionGrowingSegment::FeatureType)( value * value );
            }
          }
          
          // assotiating a segment object
          
          if( rasterValuesAreValid )
          {
            switch( m_parameters.m_segmentFeatures )
            {
              case Parameters::MeanFeaturesType :
              {
                segmentPtr = m_segmentsPool.getNextSegment();
                assert( segmentPtr );                
                
                for( rasterValuesIdx = 0 ; rasterValuesIdx < inputRasterBandsSize ;
                  ++rasterValuesIdx )
                {
                  segmentPtr->m_features[ rasterValuesIdx ] = rasterValues[ 
                    rasterValuesIdx ];
                }
                
                break;
              }
              case Parameters::BaatzFeaturesType :
              {
                segmentPtr = m_segmentsPool.getNextSegment();
                assert( segmentPtr ); 
                
                for( rasterValuesIdx = 0 ; rasterValuesIdx < inputRasterBandsSize ;
                  ++rasterValuesIdx )
                {
                  BAATZ_SU( segmentPtr->m_features, rasterValuesIdx ) =
                    rasterValues[ rasterValuesIdx ];
                  BAATZ_SS( segmentPtr->m_features, inputRasterBandsSize, rasterValuesIdx ) =
                    rasterSquareValues[ rasterValuesIdx ];
                  BAATZ_ST( segmentPtr->m_features, inputRasterBandsSize, rasterValuesIdx ) =
                    0.0;
                  BAATZ_EL( segmentPtr->m_features ) = 4;
                  BAATZ_CO( segmentPtr->m_features ) = 4;
                  BAATZ_SM( segmentPtr->m_features ) = 1;
                }                
                
                break;
              }
              default :
              {
                TERP_LOG_AND_THROW( "Invalid segment features type" );
                break;
              }
            }
            
            currLineSegsPtrs->operator[]( col ) = segmentPtr;
            
            segmentPtr->m_id = lineSegmentIds[ col ];
            segmentPtr->m_size = 1;
            segmentPtr->m_xStart = col;
            segmentPtr->m_xBound = col + 1;
            segmentPtr->m_yStart = line;
            segmentPtr->m_yBound = line + 1;
            
            m_segmentsIdsMatrix( line, col ) = segmentPtr->m_id;
            segsIndexer.insert( segmentPtr );
              
            // updating the neighboorhood info
            
            segmentPtr->clearNeighborSegments();
              
            if( line ) 
            { 
              neighborSegmentPtr = lastLineSegsPtrs->operator[]( col );
                
              if( neighborSegmentPtr )
              {
                segmentPtr->addNeighborSegment( neighborSegmentPtr );
                                    
                neighborSegmentPtr->addNeighborSegment( segmentPtr );
              }
            }
            
            if( col ) 
            { 
              neighborSegmentPtr = currLineSegsPtrs->operator[]( col - 1 );
                
              if( neighborSegmentPtr )
              {
                segmentPtr->addNeighborSegment( neighborSegmentPtr );
                                    
                neighborSegmentPtr->addNeighborSegment( segmentPtr );
              }
            }            
          }
          else // !rasterValueIsValid
          {
            m_segmentsIdsMatrix( line, col ) = 0;
            unusedLineSegmentIds.push_back( lineSegmentIds[ col ] );
            currLineSegsPtrs->operator[]( col ) = 0;
          }
        }
        
        // Free unused IDs
        
        if( ! unusedLineSegmentIds.empty() )
        {
          segmenterIdsManager.addFreeIDs( unusedLineSegmentIds );
          unusedLineSegmentIds.clear();
        }
        
        // Swapping the pointers to the vectors of used segment pointers
        
        if( lastLineSegsPtrs == ( &usedSegPointers1 ) )
        {
          lastLineSegsPtrs = &usedSegPointers2;
          currLineSegsPtrs = &usedSegPointers1;
        }
        else
        {
          lastLineSegsPtrs = &usedSegPointers1;
          currLineSegsPtrs = &usedSegPointers2;
        }
      }
      
      return true;
    }
    
    unsigned int SegmenterRegionGrowingStrategy::mergeSegments( 
      const SegmenterRegionGrowingSegment::FeatureType similarityThreshold,
      SegmenterIdsManager& segmenterIdsManager,
      Merger& merger,
      const bool enablelocalMutualBestFitting,
      SegmentsIndexerT& segsIndexer,
      SegmenterRegionGrowingSegment* auxSeg1Ptr,
      SegmenterRegionGrowingSegment* auxSeg2Ptr,
      SegmenterRegionGrowingSegment* auxSeg3Ptr)
    {
      unsigned int mergedSegmentsNumber = 0;
      unsigned int segmentsLine = 0;
      unsigned int segmentsLineBound = 0;
      unsigned int segmentCol = 0;
      unsigned int segmentColStart = 0;
      unsigned int segmentColBound = 0;
      SegmenterRegionGrowingSegment* maxForwardSimilaritySegmentPtr = 0;
      SegmenterRegionGrowingSegment::FeatureType forwardSimilarityValue = 0;
      SegmenterRegionGrowingSegment::FeatureType maxForwardSimilarityValue = 0;  
      SegmenterRegionGrowingSegment* maxBackwardSimilaritySegmentPtr = 0;
      SegmenterRegionGrowingSegment::FeatureType backwardSimilarityValue = 0;
      SegmenterRegionGrowingSegment::FeatureType maxBackwardSimilarityValue = 0;             
      SegmenterSegmentsBlock::SegmentIdDataType* segmentsIdsLinePtr = 0;      
      SegmenterSegmentsBlock::SegmentIdDataType currentSegmentId = 0;      
      std::list< SegmenterSegmentsBlock::SegmentIdDataType > freeSegmentIds;
      unsigned int neighborSegIdx = 0;
      
      // Updating the merger state
      
      merger.update();
      
      // iterating over each segment
      
      SegmentsIndexerT::iterator segsIt = segsIndexer.begin();
      
      while( segsIt != segsIndexer.end() )
      {
        // finding the neighbor segment with minimum dissimilary value
        // related to the current sement
        
        maxForwardSimilaritySegmentPtr = 0;
        maxForwardSimilarityValue = -1.0 *
          std::numeric_limits< SegmenterRegionGrowingSegment::FeatureType >::max();
        
        for( neighborSegIdx = 0 ; neighborSegIdx < (*segsIt)->m_neighborSegmentsSize ;
          ++neighborSegIdx )
        {
          if( (*segsIt)->m_neighborSegments[ neighborSegIdx ] )
          {
            forwardSimilarityValue = merger.getSimilarity( (*segsIt),
              (*segsIt)->m_neighborSegments[ neighborSegIdx ], auxSeg1Ptr );
            assert( forwardSimilarityValue >= 0 );
            assert( forwardSimilarityValue <= 1 );
              
            if( ( forwardSimilarityValue > similarityThreshold ) &&
              ( forwardSimilarityValue > maxForwardSimilarityValue ) )
            {
              maxForwardSimilarityValue = forwardSimilarityValue;
              maxForwardSimilaritySegmentPtr = (*segsIt)->m_neighborSegments[ neighborSegIdx ];
              auxSeg3Ptr->operator=( *auxSeg1Ptr );
            }
          }
        }
        
        // does the neighbor wants to merge back ?
        
        if( enablelocalMutualBestFitting && ( maxForwardSimilaritySegmentPtr != 0 ) )
        {
          // Calculating all neighbor neighbor segments dissimilarity          
          
          maxBackwardSimilaritySegmentPtr = 0;
          backwardSimilarityValue = 0;
          maxBackwardSimilarityValue = -1.0 *
            std::numeric_limits< SegmenterRegionGrowingSegment::FeatureType >::max();          
          
          for( neighborSegIdx = 0 ; neighborSegIdx < maxForwardSimilaritySegmentPtr->m_neighborSegmentsSize ;
            ++neighborSegIdx )
          {
            if( maxForwardSimilaritySegmentPtr->m_neighborSegments[ neighborSegIdx ] )
            {
              backwardSimilarityValue = 
                merger.getSimilarity( maxForwardSimilaritySegmentPtr, 
                maxForwardSimilaritySegmentPtr->m_neighborSegments[ neighborSegIdx ], auxSeg2Ptr );
              assert( backwardSimilarityValue >= 0 );
              assert( backwardSimilarityValue <= 1 );                
                
              if( backwardSimilarityValue > maxBackwardSimilarityValue )
              {
                maxBackwardSimilarityValue = backwardSimilarityValue;
                maxBackwardSimilaritySegmentPtr = 
                  maxForwardSimilaritySegmentPtr->m_neighborSegments[ neighborSegIdx ];
              }
            }
          }
          
          if( maxBackwardSimilaritySegmentPtr !=  (*segsIt) )
          {
            maxForwardSimilaritySegmentPtr = 0;
          }
        }
        
        // If the maximum similary neighbor was found it will be merged
        
        if( maxForwardSimilaritySegmentPtr )
        {
          // merging segment data
          
          merger.mergeFeatures( (*segsIt), maxForwardSimilaritySegmentPtr,
            auxSeg3Ptr );
            
          (*segsIt)->removeNeighborSegment( maxForwardSimilaritySegmentPtr );
            
          // updating the max similarity segment neighborhood segments
          // with the current segment
          
          for( neighborSegIdx = 0 ; neighborSegIdx < maxForwardSimilaritySegmentPtr->m_neighborSegmentsSize ;
            ++neighborSegIdx )
          {
            if( 
                ( maxForwardSimilaritySegmentPtr->m_neighborSegments[ neighborSegIdx ] != 0 )
                &&
                ( maxForwardSimilaritySegmentPtr->m_neighborSegments[ neighborSegIdx ] != (*segsIt) )
              )
            {
              // adding the max similarity neighborhood segments to the 
              // current one, if it is not already there            
              
              (*segsIt)->addNeighborSegment( 
                maxForwardSimilaritySegmentPtr->m_neighborSegments[ neighborSegIdx ] );
                
              // adding the current segment into the max similarity 
              // neighborhood segments list, if it is not already there              
              
              maxForwardSimilaritySegmentPtr->m_neighborSegments[ neighborSegIdx ]->addNeighborSegment( 
                (*segsIt) );
                
              // removing the merged segment reference from its neighbor
              // list      
              
              maxForwardSimilaritySegmentPtr->m_neighborSegments[ neighborSegIdx ]->removeNeighborSegment( 
                maxForwardSimilaritySegmentPtr );
            }
          }          
          
          // updating the segments Ids container matrix
          
          segmentsLineBound = maxForwardSimilaritySegmentPtr->m_yBound;
          segmentColStart = maxForwardSimilaritySegmentPtr->m_xStart;
          segmentColBound = maxForwardSimilaritySegmentPtr->m_xBound;          
          currentSegmentId = (*segsIt)->m_id;
            
          for( segmentsLine = maxForwardSimilaritySegmentPtr->m_yStart ; 
            segmentsLine < segmentsLineBound ; ++segmentsLine )
          {
            segmentsIdsLinePtr = m_segmentsIdsMatrix[ segmentsLine ];
            
            for( segmentCol = segmentColStart ; segmentCol < 
              segmentColBound ; ++segmentCol )
            {
              if( segmentsIdsLinePtr[ segmentCol ] ==
                maxForwardSimilaritySegmentPtr->m_id )
              {
                segmentsIdsLinePtr[ segmentCol ] = currentSegmentId;
              }
            }
          }
          
          // removing the  merged segment from  the global 
          // segments container
          // The merged segment id will be given back to ids manager
          
          maxForwardSimilaritySegmentPtr->clearNeighborSegments();
            
          segsIndexer.erase( maxForwardSimilaritySegmentPtr );
          
          freeSegmentIds.push_back( maxForwardSimilaritySegmentPtr->m_id );
          
          ++mergedSegmentsNumber;
        }
        
        ++segsIt;
      }
      
      // give back the free unused sement ids
      
      if( ! freeSegmentIds.empty() )
      {
        segmenterIdsManager.addFreeIDs( freeSegmentIds );
      }
      
      return mergedSegmentsNumber;
    }
    
    unsigned int SegmenterRegionGrowingStrategy::mergeSmallSegments(
      const unsigned int minSegmentSize,
      SegmenterIdsManager& segmenterIdsManager,
      Merger& merger,
      SegmentsIndexerT& segsIndexer,
      SegmenterRegionGrowingSegment* auxSeg1Ptr,
      SegmenterRegionGrowingSegment* auxSeg2Ptr )
    {
      unsigned int mergedSegmentsNumber = 0;
      SegmenterRegionGrowingSegment* currSmallSegPtr = 0;
      SegmenterRegionGrowingSegment* maxForwardSimilaritySegmentPtr = 0;
      SegmenterRegionGrowingSegment::FeatureType forwardSimilarityValue = 0;
      SegmenterRegionGrowingSegment::FeatureType maxForwardSimilarityValue = 0;        
      unsigned int segmentsLine = 0;
      unsigned int segmentsLineBound = 0;
      unsigned int segmentCol = 0;
      unsigned int segmentColStart = 0;
      unsigned int segmentColBound = 0;      
      SegmenterSegmentsBlock::SegmentIdDataType* segmentsIdsLinePtr = 0;
      SegmenterSegmentsBlock::SegmentIdDataType currentSegmentId = 0;
      unsigned int neighborSegIdx = 0;
      
      // Updating the merger state
      
      merger.update();
      
      // iterating over each segment      
      
      SegmentsIndexerT::iterator segsIt = segsIndexer.begin();
      
      while( segsIt != segsIndexer.end() )
      {
        currSmallSegPtr = (*segsIt);
        
        // is this a small segment ?
        
        if( currSmallSegPtr->m_size < minSegmentSize )
        {
          // Looking for the closest neighboorhood segment
          
          maxForwardSimilaritySegmentPtr = 0;
          maxForwardSimilarityValue = -1.0 *
            std::numeric_limits< SegmenterRegionGrowingSegment::FeatureType >::max();
            
          for( neighborSegIdx = 0 ; neighborSegIdx < currSmallSegPtr->m_neighborSegmentsSize ;
            ++neighborSegIdx )
          {
            if( currSmallSegPtr->m_neighborSegments[ neighborSegIdx ] )
            {
              forwardSimilarityValue = merger.getSimilarity( currSmallSegPtr,
                currSmallSegPtr->m_neighborSegments[ neighborSegIdx ], auxSeg1Ptr );
              assert( forwardSimilarityValue >= 0 );
              assert( forwardSimilarityValue <= 1 );              
                
              if( forwardSimilarityValue > maxForwardSimilarityValue )
              {
                maxForwardSimilarityValue = forwardSimilarityValue;
                maxForwardSimilaritySegmentPtr = currSmallSegPtr->m_neighborSegments[ neighborSegIdx ];
                auxSeg2Ptr->operator=( *auxSeg1Ptr );
              }
            }
          }            
          
          // If the minimum dissimilary neighbor was found it will be merged
        
          if( maxForwardSimilaritySegmentPtr )
          {          
            // merging the small segment data into there
            // closes segment data
            
            merger.mergeFeatures( maxForwardSimilaritySegmentPtr,
              currSmallSegPtr, auxSeg2Ptr );
              
            maxForwardSimilaritySegmentPtr->removeNeighborSegment( currSmallSegPtr );
              
            // updating the the small segment neighborhood segments
            // with the current segment
            
            for( neighborSegIdx = 0 ; neighborSegIdx < currSmallSegPtr->m_neighborSegmentsSize ;
              ++neighborSegIdx )
            {
              if( 
                  ( currSmallSegPtr->m_neighborSegments[ neighborSegIdx ] != 0 )
                  &&
                  ( currSmallSegPtr->m_neighborSegments[ neighborSegIdx ] != maxForwardSimilaritySegmentPtr )
                )
              {
                // adding the small segment neighborhood segments to the 
                // closest segment, if it is not already there         
                
                maxForwardSimilaritySegmentPtr->addNeighborSegment( 
                  currSmallSegPtr->m_neighborSegments[ neighborSegIdx ] );
                  
                // adding the closest segment into the small segment 
                // neighborhood segments list, if it is not already there           
                
                currSmallSegPtr->m_neighborSegments[ neighborSegIdx ]->addNeighborSegment( 
                  maxForwardSimilaritySegmentPtr );
                  
                // removing the small segment reference from its neighbor
                // list   
                
                currSmallSegPtr->m_neighborSegments[ neighborSegIdx ]->removeNeighborSegment( 
                  currSmallSegPtr );
              }
            }             
            
            // updating the segments Ids container matrix
            
            segmentsLineBound = currSmallSegPtr->m_yBound;
            segmentColStart = currSmallSegPtr->m_xStart;
            segmentColBound = currSmallSegPtr->m_xBound;          
            currentSegmentId = currSmallSegPtr->m_id;
              
            for( segmentsLine = currSmallSegPtr->m_yStart ; 
              segmentsLine < segmentsLineBound ; ++segmentsLine )
            {
              segmentsIdsLinePtr = m_segmentsIdsMatrix[ segmentsLine ];
              
              for( segmentCol = segmentColStart ; segmentCol < 
                segmentColBound ; ++segmentCol )
              {
                if( segmentsIdsLinePtr[ segmentCol ] ==
                  currentSegmentId )
                {
                  segmentsIdsLinePtr[ segmentCol ] = 
                    maxForwardSimilaritySegmentPtr->m_id;
                }
              }
            }
            
            // removing the small segment from the  global segments container
            // The merged segment id will be given back to ids manager
            
            currSmallSegPtr->clearNeighborSegments();

            ++segsIt;
              
            segsIndexer.erase( currSmallSegPtr );
            
            segmenterIdsManager.addFreeID( currentSegmentId );
            
            ++mergedSegmentsNumber;
          }
          else
          {
            ++segsIt;
          }
        }
        else
        {
          ++segsIt;
        }
      }
      
      return mergedSegmentsNumber;
    }
    
    void  SegmenterRegionGrowingStrategy::exportSegs2Tif( 
      const SegmentsIdsMatrixT& segmentsIds, bool normto8bits,
      const std::string& fileName )
    {
      std::map<std::string, std::string> rinfo; 
      rinfo["SOURCE"] = fileName;
      
      const unsigned int linesNmb = segmentsIds.getLinesNumber();
      const unsigned int colsNmb = segmentsIds.getColumnsNumber();
 
      te::rst::Grid* gridPtr = new te::rst::Grid( colsNmb, linesNmb );

      std::vector< te::rst::BandProperty* > bandsProps;
      bandsProps.push_back( new te::rst::BandProperty( 0, 
        (normto8bits ? te::dt::UCHAR_TYPE : te::dt::UINT32_TYPE) ) );
      
      te::rst::Raster* rasterPtr = te::rst::RasterFactory::make( "GDAL", 
        gridPtr, bandsProps, rinfo );
      TERP_TRUE_OR_THROW( rasterPtr, "Invalid pointer" )
      
      unsigned int col = 0;
      unsigned int line = 0 ;
      
      double offset = 0.0;
      double scale = 1.0;
      
      if( normto8bits )
      {
        double minValue = DBL_MAX;
        double maxValue = -1.0 * DBL_MAX;
        double value = 0;
      
        for( line = 0 ; line < linesNmb ; ++line )
        {
          for( col = 0 ; col < colsNmb ; ++col )
          {
            value = (double)segmentsIds( line, col );
            
            if( value > maxValue ) maxValue = value;
            if( value < minValue ) minValue = value;
          }
        }
        
        offset = minValue;
        scale = 254.0 / ( maxValue - minValue );
      }
      
      double value = 0;
      
      for( line = 0 ; line < linesNmb ; ++line )
      {
        for( col = 0 ; col < colsNmb ; ++col )
        {
          value = ( ((double)segmentsIds( line, col )) - offset ) * scale;
          TERP_TRUE_OR_THROW( value <= 255.0, "Invalid value:" +
            boost::lexical_cast< std::string >( value ) )
          
          rasterPtr->setValue( col, line, value , 0 );
        }
      }
      
      delete rasterPtr;
    }
    
    void SegmenterRegionGrowingStrategy::getTouchingEdgeLength( 
      const SegmentsIdsMatrixT& segsIds,
      const unsigned int& xStart, const unsigned int& yStart,
      const unsigned int& xBound, const unsigned int& yBound,
      const SegmenterSegmentsBlock::SegmentIdDataType& id1,
      const SegmenterSegmentsBlock::SegmentIdDataType& id2,
      unsigned int& edgeLength1,
      unsigned int& edgeLength2 )
    {
      const unsigned int colsNumber = segsIds.getColumnsNumber();
      const unsigned int linesNumber = segsIds.getLinesNumber();
      
      TERP_DEBUG_TRUE_OR_THROW( xStart < colsNumber,
        "Internal Error" )
      TERP_DEBUG_TRUE_OR_THROW( xBound <= colsNumber,
        "Internal Error" )
      TERP_DEBUG_TRUE_OR_THROW( yStart < linesNumber,
        "Internal Error" )
      TERP_DEBUG_TRUE_OR_THROW( yBound <= linesNumber,
        "Internal Error" )        
      TERP_DEBUG_TRUE_OR_THROW( xStart < xBound, "Internal Error" )
      TERP_DEBUG_TRUE_OR_THROW( yStart < yBound, "Internal Error" )        
      
      // finding the touching pixels
      
      edgeLength1 = 0;
      edgeLength2 = 0;
      
      unsigned int xIdx = 0;
      const unsigned int lastColIdx = colsNumber - 1;
      const unsigned int lastLineIdx = linesNumber - 1;
      
      for( unsigned int yIdx = yStart ; yIdx < yBound ; ++yIdx )
      {
        for( xIdx = xStart; xIdx < xBound ; ++xIdx )
        {
          if( segsIds[ yIdx ][ xIdx ] == id1 )
          {
            if( yIdx ) 
              if( segsIds[ yIdx - 1 ][ xIdx ] == id2 )
              {
                ++edgeLength1;
                continue;
              }
            if( xIdx ) 
              if( segsIds[ yIdx ][ xIdx - 1 ] == id2 )
              {
                ++edgeLength1;
                continue;
              }  
            if( yIdx < lastLineIdx) 
              if( segsIds[ yIdx + 1 ][ xIdx ] == id2 )
              {
                ++edgeLength1;
                continue;
              }              
            if( xIdx < lastColIdx ) 
              if( segsIds[ yIdx ][ xIdx + 1 ] == id2 )
              {
                ++edgeLength1;
                continue;
              }              
          }
          else if( segsIds[ yIdx ][ xIdx ] == id2 )
          {
            if( yIdx ) 
              if( segsIds[ yIdx - 1 ][ xIdx ] == id1 )
              {
                ++edgeLength2;
                continue;
              }
            if( xIdx ) 
              if( segsIds[ yIdx ][ xIdx - 1 ] == id1 )
              {
                ++edgeLength2;
                continue;
              }  
            if( yIdx < lastLineIdx) 
              if( segsIds[ yIdx + 1 ][ xIdx ] == id1 )
              {
                ++edgeLength2;
                continue;
              }              
            if( xIdx < lastColIdx ) 
              if( segsIds[ yIdx ][ xIdx + 1 ] == id1 )
              {
                ++edgeLength2;
                continue;
              }              
          }            
        }
      }
    }
    
    //-------------------------------------------------------------------------
    
    SegmenterRegionGrowingStrategyFactory::SegmenterRegionGrowingStrategyFactory()
    : te::rp::SegmenterStrategyFactory( "RegionGrowing" )
    {
    }
    
    SegmenterRegionGrowingStrategyFactory::~SegmenterRegionGrowingStrategyFactory()
    {
    }    
    
    te::rp::SegmenterStrategy* SegmenterRegionGrowingStrategyFactory::build()
    {
      return new te::rp::SegmenterRegionGrowingStrategy();
    }
    
  } // end namespace rp
}   // end namespace te    

