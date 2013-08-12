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
  \file terralib/rp/Skeleton.h
  \brief Creation of skeleton imagems.
 */

#ifndef __TERRALIB_RP_INTERNAL_SKELETON_H
#define __TERRALIB_RP_INTERNAL_SKELETON_H

#include "Algorithm.h"
#include "Matrix.h"
#include "../raster/Raster.h"
#include "../raster/BandProperty.h"
#include "../raster/RasterFactory.h"
#include "../raster/Grid.h"

#include <boost/thread.hpp>

#include <memory>
#include <vector>
#include <map>
#include <string>

#include <climits>
#include <cfloat>

namespace te
{
  namespace rp
  {
    /*!
      \class Skeleton
      
      \brief Creation of skeleton imagems.
      
      \details The skeleton of a region may be defined via the media axis transformation MAT
      proposed by Blum [1967]. The MAT of a region R with border B is as follows:
      For each point p in R, we find its closest neightbor in B. If p has more than one
      such neighbor, it is said to belong to the medial axis (skeleton) of R.
      
      \note Reference: Normalized Gradient Vector Diffusion and Image Segmentation, Zeyun Yu, Chandrajit Bajaj.
      
      \note Reference: Image Segmentation Using Gradient Vector Diffusion and Region Merging, Zeyun Yu, Chandrajit Bajaj.
      
      \ingroup RPAlgorithms
     */
    class TERPEXPORT Skeleton : public Algorithm
    {
      public:
        
        /*!
          \class InputParameters
          \brief Skeleton input parameters
         */        
        class TERPEXPORT InputParameters : public AlgorithmInputParameters
        {
          public:
            
            te::rst::Raster const* m_inputRasterPtr; //!< Input raster.
            
            unsigned int m_inputRasterBand; //!< Bands to process from the input raster.
            
            te::rst::Raster const* m_inputMaskRasterPtr; //!< A pointer to an input raster (where pixels with zero velues will be ignored) or an null pointer if no input mask raster must be used.
            
            double m_diffusionThreshold; //!< A threshold for the finite differences iterative diffusion - valid range [0,1], higher values will cause more iterations to be performed.
            
            double m_diffusionRegularitation; //!< A regularization parameter to control the variation from one iteration to the next one (higher values will allow higher state changes between each iteration but can induce the system to be unstable, valid range [0,1] ).
            
            unsigned int m_diffusionMaxIterations; //!< The maximum number of iterations to perform (valid range: non-zero values).
            
            bool m_enableMultiThread; //!< Enable (true) the use of threads.
            
            InputParameters();
            
            InputParameters( const InputParameters& );
            
            ~InputParameters();
            
            //overload
            void reset() throw( te::rp::Exception );
            
            //overload
            const  InputParameters& operator=( const InputParameters& params );
            
            //overload
            AbstractParameters* clone() const;
        };
        
        /*!
          \class OutputParameters
          \brief Skeleton output parameters
         */        
        class TERPEXPORT OutputParameters : public AlgorithmOutputParameters
        {
          public:
            
            std::string m_rType; //!< Output raster data source type (as described in te::raster::RasterFactory ).
            
            std::map< std::string, std::string > m_rInfo; //!< The necessary information to create the output rasters (as described in te::raster::RasterFactory). 
            
            std::auto_ptr< te::rst::Raster > m_outputRasterPtr; //!< The generated output registered raster.
            
            OutputParameters();
            
            OutputParameters( const OutputParameters& );
            
            ~OutputParameters();
            
            //overload
            void reset() throw( te::rp::Exception );
            
            //overload
            const  OutputParameters& operator=( const OutputParameters& params );
            
            //overload
            AbstractParameters* clone() const;
        };        

        Skeleton();
        
        ~Skeleton();
       
        //overload
        bool execute( AlgorithmOutputParameters& outputParams ) throw( te::rp::Exception );
        
        //overload
        void reset() throw( te::rp::Exception );
        
        //overload
        bool initialize( const AlgorithmInputParameters& inputParams ) throw( te::rp::Exception );
        
        bool isInitialized() const;

      protected:
        
        /*!
          \class ApplyRansacThreadEntryThreadParams

          \brief Parameters used by the GTFilter::applyRansacThreadEntry method.
        */
        class ApplyVecDiffusionThreadParams
        {
          public:
            te::rp::Matrix< double > const * m_inputBufXPtr;
            te::rp::Matrix< double > const * m_inputBufYPtr;
            te::rp::Matrix< double > * m_outputBufXPtr;
            te::rp::Matrix< double > * m_outputBufYPtr;
            boost::mutex* m_mutexPtr;
            unsigned int m_firstRowIdx; //!< First row to process.
            unsigned int m_lastRowIdx; //!< Last row to process.
            double* m_currentIterationResiduePtr; //!< A pointer the the current iteration residue;
            double m_diffusionRegularitation;

            ApplyVecDiffusionThreadParams() {};
            
            ApplyVecDiffusionThreadParams( const ApplyVecDiffusionThreadParams& other )
            {
              operator=( other );
            };

            ~ApplyVecDiffusionThreadParams() {};
            
            ApplyVecDiffusionThreadParams& operator=( const ApplyVecDiffusionThreadParams& other )
            {
              m_inputBufXPtr = other.m_inputBufXPtr;
              m_inputBufYPtr = other.m_inputBufYPtr;
              m_outputBufXPtr = other.m_outputBufXPtr;
              m_outputBufYPtr = other.m_outputBufYPtr;
              m_mutexPtr = other.m_mutexPtr;
              m_firstRowIdx = other.m_firstRowIdx;
              m_lastRowIdx = other.m_lastRowIdx;
              m_currentIterationResiduePtr = other.m_currentIterationResiduePtr;
              m_diffusionRegularitation = other.m_diffusionRegularitation;
              
              return *this;
            };
        };        
        
        Skeleton::InputParameters m_inputParameters; //!< Input execution parameters.
        
        bool m_isInitialized; //!< Tells if this instance is initialized.
        
        /*!
          \brief Create an gradient maps from the input image.
          \param edgeXMap The created edge X vectors map.
          \param edgeYMap The created edge Y vectors map.
          \return true if OK, false on errors.
         */          
        bool loadData( te::rp::Matrix< double >& rasterData ) const;        
        
        /*!
          \brief Create an gradient maps from the input image.
          \param inputData The input data.
          \param edgeXMap The created edge X vectors map.
          \param edgeYMap The created edge Y vectors map.
          \return true if OK, false on errors.
         */          
        bool getGradientMaps( 
          const te::rp::Matrix< double >& inputData,
          te::rp::Matrix< double >& gradXMap,
          te::rp::Matrix< double >& gradYMap ) const;
          
        template< typename MatrixElementT >
        bool applyMeanSmooth( 
          const te::rp::Matrix< MatrixElementT >& input,
          te::rp::Matrix< MatrixElementT >& output ) const
        {
          const unsigned int nRows = input.getLinesNumber();
          const unsigned int nCols = input.getColumnsNumber();
          
          if( ! output.reset( nRows,nCols ) )
            return false;          
          
          const unsigned int lastRowIdx = nRows - 1;
          const unsigned int lastColIdx = nCols - 1;
          unsigned int row = 0;
          unsigned int col = 0;          
          unsigned int nextRow = 0;
          unsigned int nextCol = 0;
          unsigned int prevRow = 0;
          unsigned int prevCol = 0;   
          
          for( row = 0 ; row < nRows ; ++row )
          {
            output[ row ][ 0 ] = input[ row ][ 0 ];
            output[ row ][ lastColIdx ] = input[ row ][ lastColIdx ];
          }
          
          for( col = 0 ; col < nCols ; ++col )
          {
            output[ 0 ][ col ] = input[ 0 ][ col ];
            output[ lastRowIdx ][ col ] = input[ lastRowIdx ][ col ];
          }           
          
          for( row = 1 ; row < lastRowIdx ; ++row )
          {
            prevRow = row - 1;
            nextRow = row + 1;
            
            for( col = 1 ; col < lastColIdx ; ++col )
            {
              prevCol = col - 1;
              nextCol = col + 1;
              
              output[ row ][ col ] =
                (
                  input[ row ][ prevCol ] 
                  + input[ row ][ nextCol ]
                  + input[ prevRow ][ prevCol ] 
                  + input[ prevRow ][ col ]
                  + input[ prevRow ][ nextCol ] 
                  + input[ nextRow ][ prevCol ] 
                  + input[ nextRow ][ col ]
                  + input[ nextRow ][ nextCol ]
                ) / 8.0;
            }
          }
          
          return true;
        };
        
        template< typename MatrixElementT >
        bool getMagnitude( 
          const te::rp::Matrix< double >& gradXMap,
          const te::rp::Matrix< double >& gradYMap,
          te::rp::Matrix< MatrixElementT >& magnitude ) const
        {
          assert( gradXMap.getColumnsNumber() == gradYMap.getColumnsNumber() );
          assert( gradXMap.getLinesNumber() == gradYMap.getLinesNumber() );
          
          const unsigned int nRows = gradXMap.getLinesNumber();
          const unsigned int nCols = gradXMap.getColumnsNumber();
          
          if( ( magnitude.getColumnsNumber() != nCols ) &&
            ( magnitude.getLinesNumber() != nRows ) )
          {
            if( ! magnitude.reset( nRows,nCols ) )
              return false;          
          }
          
          unsigned int row = 0;
          unsigned int col = 0; 
          double xValue = 0;
          double yValue = 0;
          
          for( row = 0 ; row < nRows ; ++row )
          {
            for( col = 0 ; col < nCols ; ++col )
            {
              xValue = gradXMap[ row ][ col ];
              yValue = gradYMap[ row ][ col ];
              
              magnitude[ row ][ col ] = std::sqrt( ( xValue * xValue ) + 
                ( yValue * yValue ) );
            }
          }
          
          return true;
        };        
          
        /*!
          \brief Create an Edge strenght Map from the input data.
          \param inputMap The input map.
          \param edgeStrengthMap The edge strength map (zero or positive values).
          \return true if OK, false on errors.
         */          
        bool getEdgeStrengthMap( 
          const te::rp::Matrix< double >& inputMap,
          te::rp::Matrix< double >& edgeStrengthMap ) const;       
          
        /*!
          \brief Generate an initial normalized edge vector field.
          \param edgeStrengthMap The edge strength map.
          \param createUnitVectors If true, normalized (unit) vectors will be created.
          \param edgeVecXMap The generated vector field X component.
          \param edgeVecYMap The generated vector field Y component.
          \return true if OK, false on errors.
         */            
        bool getEdgeVecField( 
          const te::rp::Matrix< double >& edgeStrengthMap,
          const bool createUnitVectors,
          te::rp::Matrix< double >& edgeVecXMap,
          te::rp::Matrix< double >& edgeVecYMap ) const;            
        
        /*!
          \brief Create a tiff file from a matrix.
          \param matrix The matrix.
          \param normalize Enable/disable pixel normalization;
          \param tifFileName Tif file name.
        */      
        template< typename MatrixElementT >
        void createTifFromMatrix( 
          const te::rp::Matrix< MatrixElementT >& matrix,
          const bool normalize,
          const std::string& tifFileName ) const
        {
          std::map<std::string, std::string> rInfo;
          rInfo["URI"] = tifFileName + ".tif";
          
          std::vector<te::rst::BandProperty*> bandsProperties;
          if( normalize )
            bandsProperties.push_back(new te::rst::BandProperty( 0, te::dt::UCHAR_TYPE, "" ));
          else
            bandsProperties.push_back(new te::rst::BandProperty( 0, te::dt::DOUBLE_TYPE, "" ));
          bandsProperties[0]->m_colorInterp = te::rst::GrayIdxCInt;
          bandsProperties[0]->m_noDataValue = -1.0 * DBL_MAX;

          te::rst::Grid* newgrid = new te::rst::Grid( matrix.getColumnsNumber(),
            matrix.getLinesNumber(), 0, -1 );

          std::auto_ptr< te::rst::Raster > outputRasterPtr(
            te::rst::RasterFactory::make( "GDAL", newgrid, bandsProperties, rInfo, 0, 0));
          TERP_TRUE_OR_THROW( outputRasterPtr.get(), "Output raster creation error");
              
          unsigned int line = 0;
          unsigned int col = 0;
          const unsigned int nLines = matrix.getLinesNumber();
          const unsigned int nCols = matrix.getColumnsNumber();
          double value = 0;

          double gain = 1.0;
          double offset = 0.0;
          if( normalize )
          {
            double rasterDataMin = DBL_MAX;
            double rasterDataMax = -1.0 * DBL_MIN;
            for( line = 0 ; line < nLines ; ++line )
            {
              for( col = 0 ; col < nCols ; ++col )
              {
                value = (double)matrix[ line ][ col ];
                if( value < rasterDataMin )
                  rasterDataMin = value;
                if( value > rasterDataMax )
                  rasterDataMax = value;
              }
            }
              
            if( rasterDataMax == rasterDataMin )
            {
              gain = 0.0;
              offset = 0.0;
            }
            else
            {
              gain = 255.0 / ( rasterDataMax - rasterDataMin );
              offset = -1.0 * ( rasterDataMin );
            }
          }
          
          for( line = 0 ; line < nLines ; ++line )
          {
            for( col = 0 ; col < nCols ; ++col )
            {
              value = (double)matrix[ line ][ col ];
            
              if( normalize )
              {
                value += offset;
                value *= gain;
                value = std::max( 0.0, value );
                value = std::min( 255.0, value );
              }
              
              outputRasterPtr->setValue( col, line, value, 0 );
            }
          }
        };  
          
        /*!
          \brief Create a tiff file from a vector field.
          \param inputVecFieldX The vector decomposed X component;
          \param inputVecFieldY The vector decomposed Y component;
          \param backGroundMapPtr An optional background image (0 means no background image).
          \param vecPixelStep The step between each vector.
          \param tifFileName Tif file name.
        */             
        void createTifFromVecField( 
          const te::rp::Matrix< double >& inputVecFieldX, 
          const te::rp::Matrix< double >& inputVecFieldY,
          te::rp::Matrix< double > const * const backGroundMapPtr,
          const unsigned int vecPixelStep,
          const std::string& tifFileName ) const;   
          
        bool applyVecDiffusion( 
          const te::rp::Matrix< double >& inputX, 
          const te::rp::Matrix< double >& inputY,
          const te::rp::Matrix< double >& backgroundData, 
          te::rp::Matrix< double >& outputX, 
          te::rp::Matrix< double >& outputY ) const;     
          
        /*! 
          \brief Vector diffusion thread entry.

          \param paramsPtr A pointer to the thread parameters.
        */
        static void applyVecDiffusionThreadEntry( ApplyVecDiffusionThreadParams* paramsPtr);          

    };

  } // end namespace rp
}   // end namespace te

#endif

