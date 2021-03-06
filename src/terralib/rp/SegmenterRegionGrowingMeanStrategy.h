/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/rp/SegmenterRegionGrowingMeanStrategy.h
  \brief Raster region growing segmenter Mean strategy.
 */

#ifndef __TERRALIB_RP_INTERNAL_SEGMENTERREGIONGROWINGMEANSTRATEGY_H
#define __TERRALIB_RP_INTERNAL_SEGMENTERREGIONGROWINGMEANSTRATEGY_H

#include "SegmenterStrategyFactory.h"
#include "SegmenterStrategy.h"
#include "SegmenterStrategyParameters.h"
#include "SegmenterRegionGrowingFunctions.h"
#include "SegmenterRegionGrowingSegment.h"
#include "SegmenterRegionGrowingSegmentsPool.h"
#include "SegmenterSegmentsBlock.h"
#include "SegmenterRegionGrowingMerger.h"
#include "Matrix.h"
#include "Config.h"

#include <vector>
#include <list>
#include <set>

namespace te
{
  namespace rp
  {
    /*!
      \class SegmenterRegionGrowingMeanStrategy
      \brief Raster region growing segmenter strategy.
      \ingroup rp_seg
     */
    class TERPEXPORT SegmenterRegionGrowingMeanStrategy : public SegmenterStrategy
    {
      public :
        
        /*!
          \class Parameters
          \brief Segmenter Parameters
         */        
        class TERPEXPORT Parameters : public SegmenterStrategyParameters
        {
          public:

            /**
            * \name Global parameters
            */
            /**@{*/              
            
            unsigned int m_minSegmentSize; //!< A positive minimum segment size (pixels number - default: 100).
            
            double m_segmentsSimilarityThreshold; //!< Segments similarity treshold - Use lower values to merge only those segments that are more similar - Higher values will allow more segments to be merged - valid values range: positive values - default: 0.03 ).
            
            unsigned int m_segmentsSimIncreaseSteps; //!< The maximum number of steps to increment the similarity threshold value for the cases where no segment merge occurred - zero will disable segment similarity threshold increments - defaul: 2.
            
            bool m_enableLocalMutualBestFitting; //!< If enabled, a merge only occurs between two segments if the minimum dissimilarity criteria is best fulfilled mutually (default: false).
            
            bool m_enableSameIterationMerges; //!< If enabled, a merged segment could be merged with another within the same iteration (default:false).            
            
            //@} 
            
            Parameters();
            
            ~Parameters();
            
            //overload 
            const Parameters& operator=( const Parameters& params );
            
            //overload
            void reset() throw( te::rp::Exception );
            
            //overload
            AbstractParameters* clone() const;
        };        
        
        ~SegmenterRegionGrowingMeanStrategy();
        
        SegmenterRegionGrowingMeanStrategy();
        
        //overload
        bool initialize( 
          SegmenterStrategyParameters const* const strategyParams ) 
          throw( te::rp::Exception );
          
        //overload
        void reset();
        
        //overload
        bool execute( 
          SegmenterIdsManager& segmenterIdsManager,
          const te::rp::SegmenterSegmentsBlock& block2ProcessInfo,
          const te::rst::Raster& inputRaster,
          const std::vector< unsigned int >& inputRasterBands,
          const std::vector< std::complex< double > >& inputRasterNoDataValues,
          const std::vector< std::complex< double > >& inputRasterBandMinValues,
          const std::vector< std::complex< double > >& inputRasterBandMaxValues,
          te::rst::Raster& outputRaster,
          const unsigned int outputRasterBand,
          const bool enableProgressInterface ) throw( te::rp::Exception );
        
        //overload         
        double getMemUsageEstimation( const unsigned int bandsToProcess,
          const unsigned int pixelsNumber ) const;
        
        //overload  
        unsigned int getOptimalBlocksOverlapSize() const;

        //overload
        bool shouldComputeMinMaxValues() const;

        //overload
        BlocksMergingMethod getBlocksMergingMethod() const;

    protected :
        /*!
          \brief Internal segments ids matrix type definition.
         */          
        typedef Matrix< SegmenterSegmentsBlock::SegmentIdDataType >
          SegmentsIdsMatrixT;
        
        /*!
          \brief true if this instance is initialized.
        */        
        bool m_isInitialized;

        /*!
          \brief Internal execution parameters.
        */        
        SegmenterRegionGrowingMeanStrategy::Parameters m_parameters;
        
        /*! \brief A pool of segments that can be reused on each strategy execution. */
        SegmenterRegionGrowingSegmentsPool< rg::MeanFeatureType > m_segmentsPool;
        
        /*! \brief A internal segments IDs matrix that can be reused  on each strategy execution. */
        SegmentsIdsMatrixT m_segmentsIdsMatrix;
        
        /*!
          \brief Initialize the segment objects container and the segment IDs container.
          \param segmenterIdsManager A segments ids manager to acquire unique segments ids.
          \param block2ProcessInfo Info about the block to process.
          \param inputRaster The input raster.
          \param inputRasterBands Input raster bands to use.
          \param inputRasterNoDataValues A vector of values to be used as input raster no-data values.
          \param inputRasterBandMinValues The minimum value present on each band.
          \param inputRasterBandMinValues The maximum value present on each band.          
          \param actSegsListHeadPtr A pointer the the active segments list head.
          \return true if OK, false on errors.
        */        
        bool initializeSegments( SegmenterIdsManager& segmenterIdsManager,
          const te::rp::SegmenterSegmentsBlock& block2ProcessInfo, 
          const te::rst::Raster& inputRaster,
          const std::vector< unsigned int >& inputRasterBands,
          const std::vector< std::complex< double > >& inputRasterNoDataValues,
          const std::vector< std::complex< double > >& inputRasterBandMinValues,
          const std::vector< std::complex< double > >& inputRasterBandMaxValues,
          SegmenterRegionGrowingSegment< rg::MeanFeatureType >** actSegsListHeadPtr );
    };
    
    /*!
      \class SegmenterRegionGrowingMeanStrategyFactory
      \brief Raster region growing segmenter strategy factory.
      \note Factory key: RegionGrowingMean
     */
    class TERPEXPORT SegmenterRegionGrowingMeanStrategyFactory : public 
      te::rp::SegmenterStrategyFactory
    {
      public:
        
        SegmenterRegionGrowingMeanStrategyFactory();
        
        ~SegmenterRegionGrowingMeanStrategyFactory();
   
        //overload
        te::rp::SegmenterStrategy* build();
        
    };    

  } // end namespace rp
}   // end namespace te

#endif  // __TERRALIB_RP_INTERNAL_SEGMENTERREGIONGROWINGMEANSTRATEGY_H
