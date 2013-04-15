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
  \file terralib/rp/Matrix.h
  \brief Generic template matrix.
 */

#ifndef __TERRALIB_RP_INTERNAL_MATRIX_H
#define __TERRALIB_RP_INTERNAL_MATRIX_H

  #include "Functions.h"
  #include "Macros.h"
  
  #include "../common/PlatformUtils.h"

  #include <cstring>
  #include <cmath>
  #include <cstdio>
  #include <string>
  #include <vector>
  #include <memory>
  
  namespace te 
  {
    namespace rp
    {
      /*!
      \class Matrix
      \brief A generic template matrix
      */
      template< typename ElementType >
      class Matrix {
        public :
          
          /*!
          \brief Public matrix element type definition.
          */          
          typedef ElementType ElementTypeT;
          
          /*!
          \brief Memory polycy.
          */
          enum MemoryPolicy 
          {
            /*!
            \details Automatic memory policy ( Try to use RAM or DISK, if there is no 
            avaliable RAM ) DO NOT USE AutoMemPol FOR COMPLEX DATA TYPES.
            */
            AutoMemPol,
            /*!
            \details RAM memory policy.
            */
            RAMMemPol,
            /*!
            \details Disk memory policy ( virtual mapped memory ) -
            DO NOT USE DiskMemPol FOR COMPLEX DATA TYPES.
            */
            DiskMemPol
          };
          
          Matrix();      

          Matrix( const Matrix< ElementType >& external );

          virtual ~Matrix();
          
          /*!
          \brief Reset (clear) the active instance data.
          \note Other parameters will not be changed.
          
          \param memoryPolicy Memory policy.
          */
          void reset();      
          
          /*!
          \brief Reset (clear) the active instance data and update its
          internal parameters folowing the the new supplied parameters.
          \note Other parameters will not be changed.
          
          \param memoryPolicy Memory policy.
          */
          void reset( MemoryPolicy memoryPolicy );       
          
          /*!
          \brief Reset (clear) the active instance data and update its
          internal parameters folowing the the new supplied parameters.
          \note Other parameters will not be changed.
          \param lines Number of lines.
          \param columns Number of columns.
          \return true if OK, false on error.
          */
          bool reset( unsigned int lines, unsigned int columns );      

          /*!
          \brief Reset (clear) the active instance data and update its
          internal parameters folowing the the new supplied parameters.
          \note Other parameters will not be changed.
          
          \param lines Number of lines.
          \param columns Number of columns.
          \param memoryPolicy Memory policy.
          \return true if OK, false on error.
          */
          bool reset( unsigned int lines, unsigned int columns,
            MemoryPolicy memoryPolicy );       
            
          /*!
          \brief Reset (clear) the active instance data and update its
          internal parameters folowing the the new supplied parameters.
          \note Other parameters will not be changed.
          
          \param lines Number of lines.
          \param columns Number of columns.
          \param memoryPolicy Memory policy.
          \param maxMemPercentUsage The max amount of free memory to use 
          when necessary (suggested default:40).
          \return true if OK, false on error.
          */
          bool reset( unsigned int lines, unsigned int columns,
            MemoryPolicy memoryPolicy, unsigned char maxMemPercentUsage );             
            
          /*!
          \brief Reset (clear) the active instance data and update its
          internal parameters folowing the the new supplied parameters.
          \note Other parameters will not be changed.
          
          \param lines Number of lines.
          \param columns Number of columns.
          \param memoryPolicy Memory policy.
          \param maxTmpFileSize Max temp file size.
          \param maxMemPercentUsage The max amount of free memory to use 
          when necessary (suggested default:40).
          \return true if OK, false on error.
          */
          bool reset( unsigned int lines, unsigned int columns,
            MemoryPolicy memoryPolicy, unsigned long int maxTmpFileSize,
            unsigned char maxMemPercentUsage ); 
            
          /*!
          \brief Clear all allocated resources and go back to the initial
          default parameters.
          */      
          void clear();            
          
          /*!
          \brief The number of current matrix lines.
          
          \return The number of current matrix lines.
          */
          unsigned int getLinesNumber() const;
          
          /*!
          \brief The number of current matrix columns.
          
          \return The number of current matrix columns
          */
          unsigned int getColumnsNumber() const;
          
          /*!
          \brief Empty Matrix verification.
          
          \return true if the matrix is empty.
          */
          bool isEmpty() const;

          /*!
          \brief Operator = overload.
          
          \note The external memory policy will be used as reference.
          
          \param external External instance reference.
          \return A reference to the current matrix.
          */
          const Matrix< ElementType >& operator=( 
            const Matrix< ElementType >& external );

          /*!
          \brief Operator () overload.
          
          \param line Line number.
          \param column Column number.
          \return A reference to the required element.
          */
          inline ElementType& operator()( const unsigned int& line, 
            const unsigned int& column )
          {
            TERP_DEBUG_TRUE_OR_THROW( ( column < m_totalColumns ),
              "Invalid columns" )
                
            return getScanLine( line )[ column ];      
          };
          
          /*!
          \brief Operator () overload.
          
          \param line Line number.
          \param column Column number.
          \return A const reference to the required element.
          */
          inline const ElementType& operator()( const unsigned int& line, 
            const unsigned int& column ) const
          {
            TERP_DEBUG_TRUE_OR_THROW( ( column < m_totalColumns ),
              "Invalid columns" )
              
            return getScanLine( line )[ column ] ;      
          }; 
          
          /*!
          \brief Operator [] overload.
          
          \param line Line number.
          \param column Column number.
          \return A pointer to the required line.
          \note The returned pointer is garanteed to 
          be valid until an acess to another line occurs.
          \note Concurrent thread access to this method is guaranteed
          if RAMMemPol policy method is used.
          */
          inline ElementType* operator[]( const unsigned int& line )
          {
            return getScanLine( line );
          };
          
          /*!
          \brief Operator [] overload.
          
          \param line Line number.
          \param column Column number.
          \return A pointer to the required line.
          \note The returned pointer is garanteed to 
          be valid until an acess to another line occurs.
          \note Concurrent thread access to this method is guaranteed
          if RAMMemPol policy method is used.       
          */
          inline ElementType const* operator[]( const unsigned int& line ) const
          {
            return getScanLine( line );
          };      
          
          /*!
          \brief Returns the current memory policy.
          \return The current memory policy.
          */      
          MemoryPolicy getMemPolicy() const;
          
          /*!
          \brief Returns the current maximum temporary disk file size.
          \return Returns the current maximum temporary disk file size.
          */            
          unsigned long int getMaxTmpFileSize() const; 
          
          /*!
          \brief Returns the max amount of free memory to use when necessary.
          \return Returns the max amount of free memory to use when necessary.
          */
          unsigned char getMaxMemPercentUsage() const;
                
        protected :
        
          /*!
          \brief Disk lines info.
          */         
          class DiskLineInfo
          {
            public :
              
              FILE* m_filePtr;
              unsigned int m_fileOff;
              
              DiskLineInfo() : m_filePtr( 0 ), m_fileOff( 0 ) {};
              ~DiskLineInfo() {};
          };
          
          /*!
          \brief Openend disk files info node.
          */         
          class OpenDiskFileHandler
          {
            public :
              
              FILE* m_filePtr;
              
              OpenDiskFileHandler() : m_filePtr( 0 ) {};          
              ~OpenDiskFileHandler() 
              {
                if( m_filePtr ) 
                {
                  fclose( m_filePtr );
                }
              };
          };      
          
          /*!
          \brief Max bytes per temp file (for swapped tiles only, default: 2GB).
          */
          unsigned long int m_maxTmpFileSize;      
          
          /*!
          \brief the max amount of free memory to use when necessary.
          */        
          unsigned char m_maxMemPercentUsage;
          
          /*!
          \brief The total lines number (default:0).
          */
          unsigned int m_totalLines;
          
          /*!
          \brief The total columns number (default:0).
          */
          unsigned int m_totalColumns;     
          
          /*!
          \brief The used memory policy (default:RAMMemPol).
          */
          MemoryPolicy m_memoryPolicy;
          
          /*!
          \brief The index inside ramLinesIndexesVec_ of the next RAM
          line index that will be swapped to disk when a disk line
          is required.
          */        
          mutable unsigned int m_nextSwapLineRamLinesIndexesVecIdx;  

          /*!
          \brief A pointer to the current swap tile.
          */        
          mutable ElementType* m_currentSwapTilePtr;
          
          std::vector< ElementType* > m_memoryblocksHandler;

          /*!
          \brief A vector with pointers to all lines.
          */
          mutable std::vector< ElementType* > m_allLinesPtrsVec;     
          
          /*!
          \brief A vector with open disk files handler.
          */      
          std::vector< OpenDiskFileHandler > m_openDiskFilesHandler;
          
          /*!
          \brief Info of all lines, used when a line is on disk.
          */      
          mutable std::vector< DiskLineInfo > m_diskLinesInfoVec;
          
          /*!
          \brief The indexes of all lines loaded into RAM.
          */        
          mutable std::vector< unsigned int > m_ramLinesIndexesVec;          

          /*!
          \brief An auxiliar line used when swapping
          data to/from disk.
          */              
          mutable std::auto_ptr< ElementType > m_swapMemoryBlockHandler;      
        
          /*!
          \brief Reset the internal variables to the initial state.
          */      
          void init();
          
          /*!
          \brief Allocate disk lines.
          \param startingLineIdx Starting line index.
          \return true if OK, false on errors.
          */    
          bool allocateDiskLines( unsigned int startingLineIdx );      
          
          /*!
          \brief Returns a pointer to the required line.
          
          \param line Line number.
          \param column Column number.
          \return A pointer to the required line.
          \note The returned pointer is garanteed to 
          be valid until an acess to another line occurs.
          \note Concurrent thread access to this method is guaranteed
          if RAMMemPol policy method is used.       
          */
          ElementType* getScanLine( const unsigned int& line ) const;
            
          /*!
          \brief Create a new disk file.
          \param size The file size.
          \param fileptr The file pointer.
          \return true if OK. false on errors.
          */
          bool createNewDiskFile( unsigned long int size, FILE** fileptr ) const;      
      };

      template< typename ElementType >
      void Matrix< ElementType >::init()
      {
        m_maxTmpFileSize = 2ul * 1024ul * 1024ul * 1024ul;;
        m_maxMemPercentUsage = 40;
        m_totalLines = 0;
        m_totalColumns = 0;  
        m_memoryPolicy = RAMMemPol;
        m_nextSwapLineRamLinesIndexesVecIdx = 0;
        m_currentSwapTilePtr = 0;
      }

      template< typename ElementType >
      Matrix< ElementType >::Matrix()
      {
        init();
      }    
      
      template< typename ElementType >
      Matrix< ElementType >::Matrix( 
        const Matrix< ElementType >& external )
      {
        init();
        
        operator=( external );
      }

      template< typename ElementType >
        Matrix< ElementType >::~Matrix()
      {
        clear();
      }
      
      template< typename ElementType >
      void Matrix< ElementType >::reset()
      {
        reset( 0, 0, m_memoryPolicy, m_maxTmpFileSize, m_maxMemPercentUsage );
      }  
      
      template< typename ElementType >
      void Matrix< ElementType >::reset( MemoryPolicy memoryPolicy )
      {
        reset( 0, 0, memoryPolicy, m_maxTmpFileSize, m_maxMemPercentUsage );
      }   
      
      template< typename ElementType >
      bool Matrix< ElementType >::reset( unsigned int lines, 
        unsigned int columns )
      {
        return reset( lines, columns, m_memoryPolicy, m_maxTmpFileSize,
          m_maxMemPercentUsage );
      } 
      
      template< typename ElementType >
      bool Matrix< ElementType >::reset( unsigned int lines, 
        unsigned int columns, MemoryPolicy memoryPolicy )
      {
        return reset( lines, columns, memoryPolicy, m_maxTmpFileSize, 
          m_maxMemPercentUsage );
      }
      
      template< typename ElementType >
      bool Matrix< ElementType >::reset( unsigned int lines, 
        unsigned int columns, MemoryPolicy memoryPolicy, 
        unsigned char maxMemPercentUsage )
      {
        return reset( lines, columns, memoryPolicy, m_maxTmpFileSize, 
          maxMemPercentUsage );
      }
      
      template< typename ElementType >
      bool Matrix< ElementType >::reset( unsigned int lines, 
        unsigned int columns, MemoryPolicy memoryPolicy,
        unsigned long int maxTmpFileSize,
        unsigned char maxMemPercentUsage )
      {
        clear();
        
        // Updating the global vars
        
        m_maxTmpFileSize = maxTmpFileSize;
        m_maxMemPercentUsage = maxMemPercentUsage;
        m_memoryPolicy = memoryPolicy;
        
        /* Update the old buffer if necessary */
        
        if( ( lines > 0 ) && ( columns > 0 ) )
        {
          try
          {
            // Updating the global vars
            
            m_totalLines = lines;
            m_totalColumns = columns;    
            
            const unsigned int lineSizeBytes = (unsigned int)(
              sizeof( ElementType ) * m_totalColumns );
                  
            // Allocating the lines pointers vectpr
            
            m_allLinesPtrsVec.resize( m_totalLines, 0 );
            
            if( m_memoryPolicy == RAMMemPol )
            {
              m_memoryblocksHandler.resize( m_totalLines );
              
              for( unsigned int allLinesPtrsVecIdx = 0 ; allLinesPtrsVecIdx <
                m_totalLines ; ++allLinesPtrsVecIdx )
              {
                m_memoryblocksHandler[ allLinesPtrsVecIdx ] =
                  new ElementType[ m_totalColumns ];
                
                m_allLinesPtrsVec[ allLinesPtrsVecIdx ] = 
                  m_memoryblocksHandler[ allLinesPtrsVecIdx ];
              }
            }
            else
            { // AutoMemPol, DiskMemPol
            
              // Allocating the swap line pointer
              
              m_swapMemoryBlockHandler.reset( new ElementType[ m_totalColumns ] );
              m_currentSwapTilePtr = m_swapMemoryBlockHandler.get();
            
              // Defining the number of max RAM lines
              
              unsigned int maxRAMLines = 1;
              
              if( m_memoryPolicy == AutoMemPol )
              {
                // Defining the max number of RAM tiles
                
                const double totalPhysMem = (double)te::common::GetTotalPhysicalMemory();
                
                const double usedVMem = (double)te::common::GetUsedVirtualMemory();
                
                const double totalVMem = (double)te::common::GetTotalVirtualMemory();
                
                const double freeVMem = MIN( totalPhysMem, 
                  ( totalVMem - usedVMem ) );
                
                const double linesMem = ( ((double)m_maxMemPercentUsage) / 100.0 ) *
                  freeVMem;
                  
                maxRAMLines = (unsigned int)MAX( 1, linesMem / 
                  ((double)lineSizeBytes) );
              }        
              
              // Allocating RAM lines
              
              const unsigned int ramLinesIndexesVecSize = MIN( maxRAMLines, 
                m_totalLines );
                
              m_ramLinesIndexesVec.resize( ramLinesIndexesVecSize, 0 );
              m_memoryblocksHandler.resize( ramLinesIndexesVecSize );
              
              for( unsigned int allLinesPtrsVecIdx = 0 ; allLinesPtrsVecIdx <
                ramLinesIndexesVecSize ; ++allLinesPtrsVecIdx )
              {
                m_memoryblocksHandler[ allLinesPtrsVecIdx ] =  
                  new ElementType[ m_totalColumns ];
                
                m_allLinesPtrsVec[ allLinesPtrsVecIdx ] = 
                  m_memoryblocksHandler[ allLinesPtrsVecIdx ];                
                
                m_ramLinesIndexesVec[ allLinesPtrsVecIdx ] = allLinesPtrsVecIdx;
              }
              
              // Allocating Disk lines
            
              if( ! allocateDiskLines( ramLinesIndexesVecSize ) )
              {
                clear();
                TERP_LOG_AND_RETURN_FALSE( "Tiles allocation error" )
              }          
            }
          }
          catch(...)
          {
            clear();
            
            TERP_LOG_AND_RETURN_FALSE( "Tiles allocation error" )
          } 
        }
        
        return true;
      }
      
      template< typename ElementType >
      void Matrix< ElementType >::clear()
      {
        for( unsigned int memoryblocksHandlerIdx = 0 ; memoryblocksHandlerIdx <
          m_memoryblocksHandler.size() ; ++memoryblocksHandlerIdx )
        {
          if( m_memoryblocksHandler[ memoryblocksHandlerIdx ] )
          {
            delete[] ( m_memoryblocksHandler[ memoryblocksHandlerIdx ] );
          }
        }
        m_memoryblocksHandler.clear();
        
        m_allLinesPtrsVec.clear();
        
        m_openDiskFilesHandler.clear();
        
        m_diskLinesInfoVec.clear();
        
        m_ramLinesIndexesVec.clear();
        
        m_swapMemoryBlockHandler.reset();
        
        init();
      }    
      
      template< typename ElementType >
      unsigned int Matrix< ElementType >::getLinesNumber() const
      {
        return m_totalLines;
      }

      
      template< typename ElementType >
      unsigned int Matrix< ElementType >::getColumnsNumber() const
      {
        return m_totalColumns;
      }
      
      
      template< typename ElementType >
      bool Matrix< ElementType >::isEmpty() const
      {
        return ( m_totalLines == 0 ) ? true : false;
      }      

      template< typename ElementType >
      const Matrix< ElementType >& Matrix< ElementType >::operator=(
        const Matrix< ElementType >& external )
      {
        TERP_TRUE_OR_THROW( 
          reset( external.m_totalLines, external.m_totalColumns,
          m_memoryPolicy, m_maxTmpFileSize, m_maxMemPercentUsage ),
          "Unable to initiate the matrix object" );
        
        unsigned int column = 0;;
        ElementType const* inLinePtr = 0;
        ElementType* outLinePtr = 0;
        
        for( unsigned int line = 0 ; line < m_totalLines ; ++line ) 
        {
          inLinePtr = external.getScanLine( line );
          outLinePtr = getScanLine( line );
          
          for( column = 0 ; column < m_totalColumns ; ++column ) {
            outLinePtr[ column ] = inLinePtr[ column ];
          }
        }

        return *this;
      }
      
      template< typename ElementType >
      typename Matrix< ElementType >::MemoryPolicy Matrix< ElementType >::getMemPolicy() const
      {
        return m_memoryPolicy;
      }  
      
      template< typename ElementType >
      unsigned long int Matrix< ElementType >::getMaxTmpFileSize() const
      {
        return m_maxTmpFileSize;
      }
      
      template< typename ElementType >
      unsigned char Matrix< ElementType >::getMaxMemPercentUsage() const
      {
        return m_maxMemPercentUsage;
      }
      
      template< typename ElementType >
      bool Matrix< ElementType >::allocateDiskLines( unsigned int startingLineIdx )
      {
        const unsigned long int diskLinesNmb = m_totalLines - startingLineIdx;
        
        if( diskLinesNmb )
        {    
          const unsigned long int lineSizeBytes = (unsigned long int)(
            sizeof( ElementType ) * m_totalColumns );
              
          const unsigned long int maxLinesPerFile = ( unsigned long int )
            floor( ( (double)m_maxTmpFileSize ) / ( (double) lineSizeBytes ) );
              
          const unsigned long int maxFileSize = (unsigned long int)
            ( maxLinesPerFile * lineSizeBytes );
            
          const unsigned int openDiskFilesNumber = (unsigned int)ceil( ((double)diskLinesNmb) 
            / ((double)maxLinesPerFile) );

          // Allocating each file
          
          m_diskLinesInfoVec.resize( m_totalLines );
          
          unsigned int remainingLinesNmb = diskLinesNmb;
          unsigned int fileSize = 0;
          unsigned int fileLinesNumber = 0;    
          unsigned int diskLinesVecIdx = startingLineIdx;
          
          for( unsigned int fileIdx = 0 ; fileIdx < openDiskFilesNumber ; 
            ++fileIdx )
          {
            // Defining the current file size
            
            fileSize = maxFileSize;
            fileLinesNumber = maxLinesPerFile;
            
            if( remainingLinesNmb < maxLinesPerFile )
            {
              fileSize = (unsigned long int)( lineSizeBytes * remainingLinesNmb );
              fileLinesNumber = remainingLinesNmb;
            }
            
            remainingLinesNmb -= fileLinesNumber;
            
            // allocating the temporary file
            
            FILE* newFilePtr = 0;
            
            if( ! createNewDiskFile( fileSize, &( newFilePtr ) ) )
            {
              TERP_LOGERR( "Unable to create temporary disk file" );
              return false;           
            }
            else
            {
              m_openDiskFilesHandler.push_back( OpenDiskFileHandler() );
              m_openDiskFilesHandler.back().m_filePtr = newFilePtr;
              
              for( unsigned int lineIdx = 0; lineIdx < fileLinesNumber ; ++lineIdx )
              {
                m_diskLinesInfoVec[ diskLinesVecIdx ].m_filePtr = newFilePtr; 
                m_diskLinesInfoVec[ diskLinesVecIdx ].m_fileOff = lineIdx * 
                  lineSizeBytes;
                
                ++diskLinesVecIdx;
              }
            }
          }
        }
        
        return true;
      }
      
      template< typename ElementType >
      ElementType* Matrix< ElementType >::getScanLine( const unsigned int& line ) const
      {
        TERP_DEBUG_TRUE_OR_THROW( line < m_totalLines, "Invalid tile index" );
          
        if( m_allLinesPtrsVec[ line ] ) 
        {
          return m_allLinesPtrsVec[ line ];
        } 
        else 
        {
          // Finding the swap line index 

          TERP_DEBUG_TRUE_OR_THROW( m_nextSwapLineRamLinesIndexesVecIdx <
            m_ramLinesIndexesVec.size(), "Internal error" );
          const unsigned int swapLineIdx = m_ramLinesIndexesVec[ 
            m_nextSwapLineRamLinesIndexesVecIdx ];
          
          TERP_DEBUG_TRUE_OR_THROW( line < m_diskLinesInfoVec.size(), "Internal error" );
          DiskLineInfo& inLineData = m_diskLinesInfoVec[ line ];
          
          TERP_DEBUG_TRUE_OR_THROW( swapLineIdx < m_diskLinesInfoVec.size(), 
            "Internal error" )
          DiskLineInfo& outLineData = m_diskLinesInfoVec[ 
            swapLineIdx ];
            
          /* Reading the required tile into RAM */
          
          TERP_DEBUG_TRUE_OR_THROW( inLineData.m_filePtr, "Internal error" );
          TERP_TRUE_OR_THROW( 0 == fseek( inLineData.m_filePtr, 
            (long)( inLineData.m_fileOff ), SEEK_SET ),
            "File seek error" )
            
          TERP_DEBUG_TRUE_OR_THROW( m_currentSwapTilePtr, "Internal error" );
          TERP_TRUE_OR_THROW( 1 == fread( (void*)m_currentSwapTilePtr, 
            (size_t)( sizeof( ElementType ) * m_totalColumns ), 1, inLineData.m_filePtr ),
            "File read error" )
        
          /* Flushing the choosed tile to disk */
            
          TERP_TRUE_OR_THROW( 0 == fseek( inLineData.m_filePtr, 
            (long)( inLineData.m_fileOff ), SEEK_SET ),
            "File seek error" );
            
          TERP_DEBUG_TRUE_OR_THROW( swapLineIdx < m_allLinesPtrsVec.size(), 
            "Internal error" );          
          TERP_TRUE_OR_THROW( 1 == fwrite( (void*)m_allLinesPtrsVec[ 
            swapLineIdx ], 
            (size_t)( sizeof( ElementType ) * m_totalColumns ), 1, 
            inLineData.m_filePtr ), "File write error" )        
          
          // Updating the tile pointers
          
          ElementType* linePtr = m_allLinesPtrsVec[ swapLineIdx ];
          
          m_allLinesPtrsVec[ swapLineIdx ] = 0;
          
          m_allLinesPtrsVec[ line ] = m_currentSwapTilePtr;
          
          m_currentSwapTilePtr = linePtr;
          
          /* Updating the info vectors */
              
          outLineData.m_filePtr = inLineData.m_filePtr;
          outLineData.m_fileOff = inLineData.m_fileOff;
          
          inLineData.m_filePtr = 0;
          inLineData.m_fileOff = 0;    
          
          m_ramLinesIndexesVec[ m_nextSwapLineRamLinesIndexesVecIdx ] = line;
          m_nextSwapLineRamLinesIndexesVecIdx = 
            ( ( m_nextSwapLineRamLinesIndexesVecIdx + 1 ) % 
            m_ramLinesIndexesVec.size() );
          TERP_DEBUG_TRUE_OR_THROW( m_nextSwapLineRamLinesIndexesVecIdx <
            m_ramLinesIndexesVec.size(), "Internal error" );          
              
          return m_allLinesPtrsVec[ line ];
        }
      } 
      
      template< typename ElementType >
      bool Matrix< ElementType >::createNewDiskFile( unsigned long int size,
        FILE** fileptr ) const
      {
        //(*fileptr) = fopen( filename.c_str(), "wb+" );
        (*fileptr) = tmpfile();
        TERP_TRUE_OR_RETURN_FALSE( (*fileptr) != 0, "Invalid file pointer" )
        
        long seekoff = (long)( size - 1 );
        
        if( 0 != fseek( (*fileptr), seekoff, SEEK_SET ) )
        {
          fclose( (*fileptr) );
          TERP_LOGERR( "File seek error" );
          return false;
        }
      
        unsigned char c = '\0';
        if( 1 != fwrite( &c, 1, 1, (*fileptr) ) )
        {
          fclose( (*fileptr) );
          TERP_LOGERR( "File write error" );
          return false;
        }
          
        return true;
      }  
    }  //namespace rp
  } // namespace te
#endif //__TERRALIB_RP_INTERNAL_MATRIX_H

