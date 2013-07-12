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
  \file terralib/rp/Functions.cpp

  \brief Raster Processing functions.
*/

// TerraLib
#include "../dataaccess2/dataset/DataSetType.h"
//#include "../dataaccess/dataset/DataSetTypePersistence.h"
//#include "../dataaccess/datasource/DataSourceFactory.h"
#include "../dataaccess2/utils/Utils.h"
#include "../datatype/Enums.h"
#include "../raster/BandProperty.h"
#include "../raster/Grid.h"
#include "../raster/RasterFactory.h"
#include "../raster/RasterProperty.h"
#include "Exception.h"
#include "Functions.h"
#include "Macros.h"
#include "RasterHandler.h"

// Boost
#include <boost/filesystem.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/shared_ptr.hpp>


// STL
#include <cstring>
#include <limits>
#include <map>
#include <memory>

namespace te
{
  namespace rp
  {
    bool createNewRaster( const te::rst::Grid& rasterGrid,
      const std::vector< te::rst::BandProperty* >& bandsProperties,
      const std::string& outDataSetName,
      te::da::DataSource& outDataSource,
      RasterHandler& outRasterHandler )
    {
      // Defining the raster properties

      std::auto_ptr< te::rst::RasterProperty > rasterPropertyPtr(
        new te::rst::RasterProperty( new te::rst::Grid( rasterGrid ),
        bandsProperties, std::map< std::string, std::string >(),
        false, 0, 0 ) );

      // acquiring a transactor instance

    
      // Creating a data set instance

      std::auto_ptr< te::da::DataSetType > dataSetTypePtr(
        new te::da::DataSetType( outDataSetName ) );
      dataSetTypePtr->add( rasterPropertyPtr.release() );

      std::map<std::string, std::string> opt;

      outDataSource.createDataSet( dataSetTypePtr.release(), opt );

      std::auto_ptr< te::da::DataSet > dataSetPtr( outDataSource.getDataSet(
        outDataSetName, te::common::FORWARDONLY) );

      if( dataSetPtr.get() == 0 )
      {
        return false;
      }

      // Creating a raster instance

      std::size_t rpos = te::da::GetFirstPropertyPos(dataSetPtr.get(), te::dt::RASTER_TYPE);

      std::auto_ptr< te::rst::Raster > rasterPtr( dataSetPtr->getRaster(rpos) );

      if( rasterPtr.get() )
      {
        outRasterHandler.reset( &outDataSource,
          dataSetPtr.release(),
          rasterPtr.release() );

        return true;
      }
      else
      {
        return false;
      }
    }

    bool createNewMemRaster( const te::rst::Grid& rasterGrid,
      std::vector< te::rst::BandProperty* > bandsProperties,
      RasterHandler& outRasterHandler )
    {
      // Creating a new memory datasource

      boost::shared_ptr< te::da::DataSource > dataSourcePtr;
      // Datasource factory
      /*= (
        te::da::DataSourceFactory::make( "MEM" ) );*/
      TERP_TRUE_OR_THROW( dataSourcePtr.get(), "Data source creation error" );

      // Defining the raster properties

      std::auto_ptr< te::rst::RasterProperty > rasterPropertyPtr(
        new te::rst::RasterProperty( new te::rst::Grid( rasterGrid ),
        bandsProperties, std::map< std::string, std::string >(),
        false, 0, 0 ) );

      // Creating a data set instance

      std::auto_ptr< te::da::DataSetType > dataSetTypePtr(
        new te::da::DataSetType( "createNewMemRaster" ) );
      dataSetTypePtr->add( rasterPropertyPtr.release() );

      std::map<std::string, std::string> opt;

      dataSourcePtr->createDataSet( dataSetTypePtr.release(), opt );

      boost::shared_ptr< te::da::DataSet > dataSetPtr( dataSourcePtr->getDataSet(
        "createNewMemRaster", te::common::FORWARDONLY) );

      if( dataSetPtr.get() == 0 )
      {
        return false;
      }

      // Creating a raster instance
      std::size_t rpos = te::da::GetFirstPropertyPos(dataSetPtr.get(), te::dt::RASTER_TYPE);

      boost::shared_ptr< te::rst::Raster > rasterPtr( dataSetPtr->getRaster(rpos) );

      if( rasterPtr.get() )
      {
        outRasterHandler.reset( dataSourcePtr,
          dataSetPtr, rasterPtr );

        return true;
      }
      else
      {
        return false;
      }
    }

    bool createNewGeotifRaster( const te::rst::Grid& rasterGrid,
      std::vector< te::rst::BandProperty* > bandsProperties,
      const std::string& fileName,
      RasterHandler& outRasterHandler )
    {
      boost::filesystem::path pathInfo( fileName );

      // Creating a new memory datasource

      boost::shared_ptr< te::da::DataSource > dataSourcePtr;
      // Datasource factory
     /* (
        te::da::DataSourceFactory::make( "GDAL" ) );*/
      if( dataSourcePtr.get() == 0 ) return false;

      std::map<std::string, std::string> outputRasterInfo;
      outputRasterInfo["URI"] = pathInfo.parent_path().string();
      dataSourcePtr->setConnectionInfo(outputRasterInfo);

      dataSourcePtr->open();
      if( ! dataSourcePtr->isOpened() ) return false;

      // Defining the raster properties

      std::auto_ptr< te::rst::RasterProperty > rasterPropertyPtr(
        new te::rst::RasterProperty( new te::rst::Grid( rasterGrid ),
        bandsProperties, std::map< std::string, std::string >(),
        false, 0, 0 ) );

      // Creating a data set instance

      std::auto_ptr< te::da::DataSetType > dataSetTypePtr(
        new te::da::DataSetType( pathInfo.filename().string() ) );
      dataSetTypePtr->add( rasterPropertyPtr.release() );

      std::map<std::string, std::string> opt;

      dataSourcePtr->createDataSet( dataSetTypePtr.release(), opt );

      boost::shared_ptr< te::da::DataSet > dataSetPtr( dataSourcePtr->getDataSet(
        pathInfo.filename().string(), te::common::FORWARDONLY));

      if( dataSetPtr.get() == 0 )
      {
        return false;
      }

      // Creating a raster instance
      std::size_t rpos = te::da::GetFirstPropertyPos(dataSetPtr.get(), te::dt::RASTER_TYPE);

      boost::shared_ptr< te::rst::Raster > rasterPtr( dataSetPtr->getRaster(rpos) );

      if( rasterPtr.get() )
      {
        outRasterHandler.reset( dataSourcePtr, dataSetPtr, rasterPtr );

        return true;
      }
      else
      {
        return false;
      }
    }

    void getDataTypeRange( const int dataType, double& min, double& max )
    {
      switch( dataType )
      {
        case te::dt::BIT_TYPE :
          min = 0;
          max = 1;
          break;
        case te::dt::CHAR_TYPE :
          min = (double)std::numeric_limits<char>::min();
          max = (double)std::numeric_limits<char>::max();
          break;
        case te::dt::UCHAR_TYPE :
          min = (double)std::numeric_limits<unsigned char>::min();
          max = (double)std::numeric_limits<unsigned char>::max();
          break;
        case te::dt::INT16_TYPE :
        case te::dt::CINT16_TYPE :
          min = (double)std::numeric_limits<short int>::min();
          max = (double)std::numeric_limits<short int>::max();
          break;
        case te::dt::UINT16_TYPE :
          min = (double)std::numeric_limits<unsigned short int>::min();
          max = (double)std::numeric_limits<unsigned short int>::max();
          break;
        case te::dt::INT32_TYPE :
        case te::dt::CINT32_TYPE :
          min = (double)std::numeric_limits<int>::min();
          max = (double)std::numeric_limits<int>::max();
          break;
        case te::dt::UINT32_TYPE :
          min = (double)std::numeric_limits<unsigned int>::min();
          max = (double)std::numeric_limits<unsigned int>::max();
          break;
        case te::dt::INT64_TYPE :
          min = (double)std::numeric_limits<long int>::min();
          max = (double)std::numeric_limits<long int>::max();
          break;
        case te::dt::UINT64_TYPE :
          min = (double)std::numeric_limits<unsigned long int>::min();
          max = (double)std::numeric_limits<unsigned long int>::max();
          break;
        case te::dt::FLOAT_TYPE :
        case te::dt::CFLOAT_TYPE :
          min = (double)std::numeric_limits<float>::min();
          max = (double)std::numeric_limits<float>::max();
          break;
        case te::dt::DOUBLE_TYPE :
        case te::dt::CDOUBLE_TYPE :
          min = (double)std::numeric_limits<double>::min();
          max = (double)std::numeric_limits<double>::max();
          break;
        default :
          throw te::rp::Exception( "Invalid data type" );
          break;
      };
    }

    void convert2DoublesVector( void* inputVector, const int inputVectorDataType,
      unsigned int inputVectorSize, double* outputVector )
    {
      switch( inputVectorDataType )
      {
        case te::dt::CHAR_TYPE :
        {
          char* vPtr = (char*)inputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            outputVector[ idx ] = (double)vPtr[ idx ];
          break;
        }
        case te::dt::BIT_TYPE :
        case te::dt::UCHAR_TYPE :
        {
          unsigned char* vPtr = (unsigned char*)inputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            outputVector[ idx ] = (double)vPtr[ idx ];
          break;
        }
        case te::dt::INT16_TYPE :
        {
          short int* vPtr = (short int*)inputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            outputVector[ idx ] = (double)vPtr[ idx ];
          break;
        }
        case te::dt::CINT16_TYPE :
        {
          std::complex< short int >* vPtr = (std::complex< short int >*)
            inputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            outputVector[ idx ] = (double)vPtr[ idx ].real();
          break;
        }
        case te::dt::UINT16_TYPE :
        {
          unsigned short int* vPtr = (unsigned short int*)inputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            outputVector[ idx ] = (double)vPtr[ idx ];
          break;
        }
        case te::dt::INT32_TYPE :
        {
          int* vPtr = (int*)inputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            outputVector[ idx ] = (double)vPtr[ idx ];
          break;
        }
        case te::dt::CINT32_TYPE :
        {
          std::complex< int >* vPtr = (std::complex< int >*)
            inputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            outputVector[ idx ] = (double)vPtr[ idx ].real();
          break;
        }
        case te::dt::UINT32_TYPE :
        {
          unsigned int* vPtr = (unsigned int*)inputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            outputVector[ idx ] = (double)vPtr[ idx ];
          break;
        }
        case te::dt::INT64_TYPE :
        {
          long int* vPtr = (long int*)inputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            outputVector[ idx ] = (double)vPtr[ idx ];
          break;
        }
        case te::dt::UINT64_TYPE :
        {
          unsigned long int* vPtr = (unsigned long int*)inputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            outputVector[ idx ] = (double)vPtr[ idx ];
          break;
        }
        case te::dt::FLOAT_TYPE :
        {
          float* vPtr = (float*)inputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            outputVector[ idx ] = (double)vPtr[ idx ];
          break;
        }
        case te::dt::CFLOAT_TYPE :
        {
          std::complex< float >* vPtr = (std::complex< float >*)
            inputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            outputVector[ idx ] = (double)vPtr[ idx ].real();
          break;
        }
        case te::dt::DOUBLE_TYPE :
        {
          memcpy( outputVector, inputVector, inputVectorSize * sizeof( double ) );
          break;
        }
        case te::dt::CDOUBLE_TYPE :
        {
          std::complex< double >* vPtr = (std::complex< double >*)
            inputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            outputVector[ idx ] = (double)vPtr[ idx ].real();
          break;
        }
        default :
          throw te::rp::Exception( "Invalid data type" );
          break;
      };
    }

    void convertDoublesVector( double* inputVector,
      unsigned int inputVectorSize, const int outputVectorDataType,
      void* outputVector )
    {
      switch( outputVectorDataType )
      {
        case te::dt::CHAR_TYPE :
        {
          char* vPtr = (char*)outputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            vPtr[ idx ] = (char)inputVector[ idx ];
          break;
        }
        case te::dt::BIT_TYPE :
        case te::dt::UCHAR_TYPE :
        {
          unsigned char* vPtr = (unsigned char*)outputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            vPtr[ idx ] = (unsigned char)inputVector[ idx ];
          break;
        }
        case te::dt::INT16_TYPE :
        {
          short int* vPtr = (short int*)outputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            vPtr[ idx ] = (short int)inputVector[ idx ];
          break;
        }
        case te::dt::CINT16_TYPE :
        {
          std::complex< short int >* vPtr = (std::complex< short int >*)
            outputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            vPtr[ idx ]= ( (short int)inputVector[ idx ] );
          break;
        }
        case te::dt::UINT16_TYPE :
        {
          unsigned short int* vPtr = (unsigned short int*)outputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            vPtr[ idx ] = (unsigned short int)inputVector[ idx ];
          break;
        }
        case te::dt::INT32_TYPE :
        {
          int* vPtr = (int*)outputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            vPtr[ idx ] = (int)inputVector[ idx ];
          break;
        }
        case te::dt::CINT32_TYPE :
        {
          std::complex< int >* vPtr = (std::complex< int >*)
            outputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            vPtr[ idx ] = ( (int)inputVector[ idx ] );
          break;
        }
        case te::dt::UINT32_TYPE :
        {
          unsigned int* vPtr = (unsigned int*)outputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            vPtr[ idx ] = (unsigned int)inputVector[ idx ];
          break;
        }
        case te::dt::INT64_TYPE :
        {
          long int* vPtr = (long int*)outputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            vPtr[ idx ] = (long int)inputVector[ idx ];
          break;
        }
        case te::dt::UINT64_TYPE :
        {
          unsigned long int* vPtr = (unsigned long int*)outputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            vPtr[ idx ] = (unsigned long int)inputVector[ idx ];
          break;
        }
        case te::dt::FLOAT_TYPE :
        {
          float* vPtr = (float*)outputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            vPtr[ idx ] = (float)inputVector[ idx ];
          break;
        }
        case te::dt::CFLOAT_TYPE :
        {
          std::complex< float >* vPtr = (std::complex< float >*)
            outputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            vPtr[ idx ] = ( (float)inputVector[ idx ] );
          break;
        }
        case te::dt::DOUBLE_TYPE :
        {
          memcpy( outputVector, outputVector, inputVectorSize * sizeof( double ) );
          break;
        }
        case te::dt::CDOUBLE_TYPE :
        {
          std::complex< double >* vPtr = (std::complex< double >*)
            outputVector;
          for( register unsigned int idx = 0 ; idx < inputVectorSize ; ++idx )
            vPtr[ idx ] = ( (double)inputVector[ idx ] );
          break;
        }
        default :
          throw te::rp::Exception( "Invalid data type" );
          break;
      };
    }

  } // end namespace rp
}   // end namespace te