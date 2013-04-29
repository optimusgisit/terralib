/*  Copyright (C) 2011-2011 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/memory/ExpansibleBandBlocksManager.cpp
 
  \brief RAM cached and tiled raster band blocks manager.
*/

#include "ExpansibleBandBlocksManager.h"
#include "Exception.h"
#include "../raster/Band.h"
#include "../raster/BandProperty.h"
#include "../common/PlatformUtils.h"
#include "../common/Translator.h"

#include <algorithm>

#include <cassert>

void te::mem::ExpansibleBandBlocksManager::initState()
{
  m_isInitialized = false;
  m_maxNumberRAMBlocks = 0;
  m_maxDiskFilesSize = 0;
  m_maxBlockSizeBytes = 0;
  m_currSwapBlockPtr = 0;
  m_nextFIFOPositionOverSwapFifo = 0;
  
}

te::mem::ExpansibleBandBlocksManager::ExpansibleBandBlocksManager()
{
  initState();
}

te::mem::ExpansibleBandBlocksManager::~ExpansibleBandBlocksManager()
{
  free();
}

bool te::mem::ExpansibleBandBlocksManager::initialize( 
  const unsigned int maxNumberRAMBlocks,
  const std::vector< unsigned int>& numbersOfBlocksX,
  const std::vector< unsigned int>& numbersOfBlocksY,
  const std::vector< unsigned int>& blocksSizesBytes,
  const unsigned long int maxDiskFilesSize )
{
  if( numbersOfBlocksX.size() == 0 ) return false;
  if( numbersOfBlocksX.size() != numbersOfBlocksY.size() ) return false;
  if( numbersOfBlocksY.size() != blocksSizesBytes.size() ) return false;
  
  free();
  
  m_maxNumberRAMBlocks = maxNumberRAMBlocks;
  m_maxDiskFilesSize = maxDiskFilesSize;
  
  // Discovering boundaries
  
  m_maxBlockSizeBytes = 0;  
  unsigned int maxNumbersOfBlocksX = 0;
  unsigned int maxNumbersOfBlocksY = 0;
  unsigned int totalRequiredBlocks = 0;
  
  {
    for( unsigned int blockBIdx = 0 ; blockBIdx < numbersOfBlocksX.size() ;  
      ++blockBIdx )
    {
      if( blocksSizesBytes[ blockBIdx ] == 0 ) return false;
      
      if( m_maxBlockSizeBytes < blocksSizesBytes[ blockBIdx ] )
        m_maxBlockSizeBytes = blocksSizesBytes[ blockBIdx ];
      
      if( numbersOfBlocksX[ blockBIdx ] == 0 ) return false;
      
      if( maxNumbersOfBlocksX < numbersOfBlocksX[ blockBIdx ] )
        maxNumbersOfBlocksX = numbersOfBlocksX[ blockBIdx ];
      
      if( numbersOfBlocksY[ blockBIdx ] == 0 ) return false;
      
      if( maxNumbersOfBlocksY < numbersOfBlocksY[ blockBIdx ] )
        maxNumbersOfBlocksY = numbersOfBlocksY[ blockBIdx ];  
      
      totalRequiredBlocks += ( numbersOfBlocksX[ blockBIdx ] *
        numbersOfBlocksY[ blockBIdx ] );
    }
  }
  
  if( m_maxBlockSizeBytes > maxDiskFilesSize ) return false;
  
  // Allocating RAM blocks
  
  unsigned int allocatedRAMBlocksNumber = 0;
  
  try
  {
    m_ramBlocksPointers.resize( numbersOfBlocksX.size() );
    
    for( unsigned int blockBIdx = 0 ; blockBIdx < numbersOfBlocksX.size() ;  
      ++blockBIdx )
    {
      m_ramBlocksPointers[ blockBIdx ].resize( numbersOfBlocksY[ blockBIdx ] );
      
      for( unsigned int blockYIdx = 0 ; blockYIdx < numbersOfBlocksY[ blockBIdx ] ; 
        ++blockYIdx )
      {
        m_ramBlocksPointers[ blockBIdx ][ blockYIdx ].resize( numbersOfBlocksX[ blockBIdx ] );
        
        for( unsigned int blockXIdx = 0 ; blockXIdx < numbersOfBlocksX[ blockBIdx ] ; ++blockXIdx )
        {
          if( 
              ( allocatedRAMBlocksNumber < maxNumberRAMBlocks )
              &&
              ( blockYIdx < numbersOfBlocksY[ blockBIdx ] )
              &&
              ( blockXIdx < numbersOfBlocksX[ blockBIdx ] )
            )
          {
            RAMBlockHandlerT newBlockHandler( new BlockElementT[ m_maxBlockSizeBytes ] );
            
            m_activeRAMBlocksHandler.push_back( newBlockHandler );
              
            m_ramBlocksPointers[ blockBIdx ][ blockYIdx ][ blockXIdx ] =
              newBlockHandler.get();
              
            m_swapFifo.push_back( BlockIndex3D( blockBIdx, blockYIdx, blockXIdx ) );  
            
            ++allocatedRAMBlocksNumber;
          }
          else
          {
            m_ramBlocksPointers[ blockBIdx ][ blockYIdx ][ blockXIdx ] = 0;
          }
        }
      }
    }
  }
  catch(...)
  {
    free();
    return false;
  }
  
  // disk blocks must be used ?
  
  try
  {
    // allocating the swap block
    
    m_swapBlockHandler.reset( new BlockElementT[ m_maxBlockSizeBytes ] );
    
    m_currSwapBlockPtr = m_swapBlockHandler.get();
    
    // Allocating disk blocks
    
    const unsigned int requiredDiskBlocksNumber = totalRequiredBlocks -
      allocatedRAMBlocksNumber;
    
    std::vector< DiskBlockInfo > diskBlocksInfos;                       
    if( ! allocateDiskBlocks( requiredDiskBlocksNumber, diskBlocksInfos, 
      m_diskFilesHandler ) )
    {
      free();
      return false;
    }
    assert( ((unsigned int)diskBlocksInfos.size()) == requiredDiskBlocksNumber );
    
    unsigned int diskBlocksInfosIdx = 0;
    
    m_activeDiskBlocksInfo.resize( numbersOfBlocksX.size() );
    
    for( unsigned int blockBIdx = 0 ; blockBIdx < numbersOfBlocksX.size() ;  
      ++blockBIdx )
    {
      m_activeDiskBlocksInfo[ blockBIdx ].resize( numbersOfBlocksY[ blockBIdx ] );
      
      for( unsigned int blockYIdx = 0 ; blockYIdx < numbersOfBlocksY[ blockBIdx ] ; 
        ++blockYIdx )
      {
        m_activeDiskBlocksInfo[ blockBIdx ][ blockYIdx ].resize( numbersOfBlocksX[ blockBIdx ] );
        
        for( unsigned int blockXIdx = 0 ; blockXIdx < numbersOfBlocksX[ blockBIdx ] ; ++blockXIdx )
        {
          if( 
              ( m_ramBlocksPointers[ blockBIdx ][ blockYIdx ][ blockXIdx ] == 0 )
              &&
              ( blockYIdx < numbersOfBlocksY[ blockBIdx ] )
              &&
              ( blockXIdx < numbersOfBlocksX[ blockBIdx ] )
            )
          {
            assert( diskBlocksInfosIdx < (unsigned int)diskBlocksInfos.size() );
            m_activeDiskBlocksInfo[ blockBIdx ][ blockYIdx ][ blockXIdx ] =
              diskBlocksInfos[ diskBlocksInfosIdx ];
            ++diskBlocksInfosIdx;
          }
        }
      }
    }
  }
  catch(...)
  {
    free();
    return false;
  }    
  
  // finalizing
  
  m_isInitialized = true;
  
  return true;
}

void te::mem::ExpansibleBandBlocksManager::free()
{
  m_activeRAMBlocksHandler.clear();
  m_inactiveRAMBlocksHandler.clear();
  m_ramBlocksPointers.clear();
  m_swapFifo.clear();
  m_activeDiskBlocksInfo.clear();  
  m_inactiveDiskBlocksInfo.clear();
  m_diskFilesHandler.clear();
  m_swapBlockHandler.reset();
  
  initState();
}

void* te::mem::ExpansibleBandBlocksManager::getBlockPointer(unsigned int band, 
  unsigned int x, unsigned int y )
{
  assert( m_isInitialized );
  
  assert( band < m_ramBlocksPointers.size() );
  assert( y < m_ramBlocksPointers[ band ].size() );
  assert( x <  m_ramBlocksPointers[ band ][ y ].size() );
  m_getBlockPointer_returnValue = m_ramBlocksPointers[ band ][ y ][ x ];
  
  if( m_getBlockPointer_returnValue )
  {
    return m_getBlockPointer_returnValue;
  }
  else if( m_inactiveRAMBlocksHandler.empty() )
  {
    // defining the blocks that will manipulated    
    
    assert( m_nextFIFOPositionOverSwapFifo < m_swapFifo.size() );
    BlockIndex3D& swapIndex = m_swapFifo[ m_nextFIFOPositionOverSwapFifo ];     

    assert( band < m_activeDiskBlocksInfo.size() );
    assert( y < m_activeDiskBlocksInfo[ band ].size() );
    assert( x <  m_activeDiskBlocksInfo[ band ][ y ].size() );
    DiskBlockInfo& inDiskInfo = m_activeDiskBlocksInfo[ band ][ y ][ x ];
  
    assert( swapIndex.m_dim0Index < m_activeDiskBlocksInfo.size() );
    assert( swapIndex.m_dim1Index < m_activeDiskBlocksInfo[ swapIndex.m_dim0Index ].size() );
    assert( swapIndex.m_dim2Index <  m_activeDiskBlocksInfo[ swapIndex.m_dim0Index ][ swapIndex.m_dim1Index ].size() );    
    DiskBlockInfo& outDiskInfo = m_activeDiskBlocksInfo[ swapIndex.m_dim0Index ][ 
      swapIndex.m_dim1Index ][ swapIndex.m_dim2Index ]; 
    
    // reading the required block into m_currSwapBlockPtr
    
    assert( inDiskInfo.m_filePtr );
    if( 0 != fseek( inDiskInfo.m_filePtr, 
      (long)( inDiskInfo.m_fileOff ), SEEK_SET ) )
    {
      throw Exception(TR_MEMORY("File seek error") );
    }
      
    assert( m_currSwapBlockPtr != 0  );
    if( 1 != fread( (void*)m_currSwapBlockPtr, (size_t)( m_maxBlockSizeBytes ), 
      1, inDiskInfo.m_filePtr ) )
    {
      throw Exception(TR_MEMORY("File read error") );
    }
    
    // Flushing the choosed tile to disk
    
    if( 0 != fseek( inDiskInfo.m_filePtr, 
      (long)( inDiskInfo.m_fileOff ), SEEK_SET ) )
    {
      throw Exception(TR_MEMORY("File seek error") );
    }
      
    assert( swapIndex.m_dim0Index < m_ramBlocksPointers.size() );
    assert( swapIndex.m_dim1Index < m_ramBlocksPointers[ swapIndex.m_dim0Index ].size() );
    assert( swapIndex.m_dim2Index <  m_ramBlocksPointers[ swapIndex.m_dim0Index ][ swapIndex.m_dim1Index ].size() );      
    assert( m_ramBlocksPointers[ swapIndex.m_dim0Index ][ swapIndex.m_dim1Index ][ swapIndex.m_dim2Index ] != 0 );
    if( 1 != fwrite( (void*)m_ramBlocksPointers[ swapIndex.m_dim0Index ][ swapIndex.m_dim1Index ][ swapIndex.m_dim2Index ], 
      (size_t)( m_maxBlockSizeBytes ), 1, 
      inDiskInfo.m_filePtr ) )
    {
      throw Exception(TR_MEMORY("File write error") );
    }
    
    // updating the indexing structures
    
    m_getBlockPointer_returnValue = m_currSwapBlockPtr;
    
    m_currSwapBlockPtr = m_ramBlocksPointers[ swapIndex.m_dim0Index ][ swapIndex.m_dim1Index ][ swapIndex.m_dim2Index ];
    
    m_ramBlocksPointers[ band ][ y ][ x ] = m_getBlockPointer_returnValue;
    
    m_ramBlocksPointers[ swapIndex.m_dim0Index ][ swapIndex.m_dim1Index ][ swapIndex.m_dim2Index ] = 0;
    
    swapIndex.m_dim0Index = band;
    swapIndex.m_dim1Index = y;
    swapIndex.m_dim2Index = x;
    
    outDiskInfo = inDiskInfo;
    
    inDiskInfo.m_filePtr = 0;
    inDiskInfo.m_fileOff = 0;
    
    m_nextFIFOPositionOverSwapFifo = ( ( m_nextFIFOPositionOverSwapFifo + 1 ) % 
      m_swapFifo.size() );
      
    // returning the required block pointer 
      
    return m_getBlockPointer_returnValue;
  }
  else
  {
    // Activating one inactive block
    
    m_activeRAMBlocksHandler.push_back( m_inactiveRAMBlocksHandler.back() );
    m_inactiveRAMBlocksHandler.pop_back();
    
    m_getBlockPointer_returnValue = m_activeRAMBlocksHandler.back().get();
    
    // reading the required block into m_currSwapBlockPtr
    
    assert( band < m_activeDiskBlocksInfo.size() );
    assert( y < m_activeDiskBlocksInfo[ band ].size() );
    assert( x <  m_activeDiskBlocksInfo[ band ][ y ].size() );    
    DiskBlockInfo& inDiskInfo = m_activeDiskBlocksInfo[ band ][ y ][ x ];     
    
    if( 0 != fseek( inDiskInfo.m_filePtr, 
      (long)( inDiskInfo.m_fileOff ), SEEK_SET ) )
    {
      throw Exception(TR_MEMORY("File seek error") );
    }
      
    assert( m_getBlockPointer_returnValue != 0  );
    if( 1 != fread( (void*)m_getBlockPointer_returnValue, (size_t)( m_maxBlockSizeBytes ), 
      1, inDiskInfo.m_filePtr ) )
    {
      throw Exception(TR_MEMORY("File read error") );
    }    
    
    // updating the indexing structures
    
    m_swapFifo.push_back( BlockIndex3D( band, x, y ) );
    
    m_ramBlocksPointers[ band ][ y ][ x ] = m_getBlockPointer_returnValue;
    
    inDiskInfo.m_fileOff = 0;
    inDiskInfo.m_filePtr = 0;

    return m_getBlockPointer_returnValue;
  }  
}

bool te::mem::ExpansibleBandBlocksManager::addTopBlocks( 
  const unsigned int& expansionSize, const unsigned int& band,
  std::vector< BlockIndex3D >& addedBlocksCoords )
{
  assert( m_isInitialized );
  assert( band < m_ramBlocksPointers.size() );
  
  addedBlocksCoords.clear();
  
  if( expansionSize )
  {
    const unsigned int numberOfBlocksX = (unsigned int)
      m_ramBlocksPointers[ band ][ 0 ].size();
    
    m_ramBlocksPointers[ band ].insert( m_ramBlocksPointers[ band ].begin(), 
      expansionSize, 
      std::vector< BlockelementPtrT >( numberOfBlocksX ) );
      
    m_activeDiskBlocksInfo[ band ].insert( m_activeDiskBlocksInfo[ band ].begin(), 
      expansionSize, 
      std::vector< DiskBlockInfo >( numberOfBlocksX ) );      
    
    for( unsigned int blockYIdx = 0 ; blockYIdx < expansionSize ; ++blockYIdx )
    {
      for( unsigned int blockXIdx = 0 ; blockXIdx < numberOfBlocksX ; ++blockXIdx )
      {
        addedBlocksCoords.push_back( BlockIndex3D( band, blockYIdx, blockXIdx ) );
      }
    }
    
    shiftDim03DCoords( m_swapFifo, band, expansionSize, 0 );
      
    if( ! allocateAndActivateDiskBlocks( addedBlocksCoords ) ) return false;
  }
  
  return true;
}

bool te::mem::ExpansibleBandBlocksManager::addBottomBlocks( 
  const unsigned int& expansionSize, const unsigned int& band,
  std::vector< BlockIndex3D >& addedBlocksCoords )
{
  assert( m_isInitialized );
  assert( band < m_ramBlocksPointers.size() );
  
  addedBlocksCoords.clear();
  
  if( expansionSize )
  {
    const unsigned int numberOfBlocksX = (unsigned int)
      m_ramBlocksPointers[ band ][ 0 ].size();
    
    m_ramBlocksPointers[ band ].insert( m_ramBlocksPointers[ band ].end(), 
      expansionSize, 
      std::vector< BlockelementPtrT >( numberOfBlocksX ) );
      
    m_activeDiskBlocksInfo[ band ].insert( m_activeDiskBlocksInfo[ band ].end(), 
      expansionSize, 
      std::vector< DiskBlockInfo >( numberOfBlocksX ) );      
    
    for( unsigned int blockYIdx = m_activeDiskBlocksInfo[ band ].size() - 
      expansionSize ; blockYIdx < m_activeDiskBlocksInfo[ band ].size() ; 
      ++blockYIdx )
    {
      for( unsigned int blockXIdx = 0 ; blockXIdx < numberOfBlocksX ; ++blockXIdx )
      {
        addedBlocksCoords.push_back( BlockIndex3D( band, blockYIdx, blockXIdx ) ); 
      }
    }
      
    if( ! allocateAndActivateDiskBlocks( addedBlocksCoords ) ) return false;
  }
  
  return true;
}

bool te::mem::ExpansibleBandBlocksManager::addLeftBlocks( 
  const unsigned int& expansionSize, const unsigned int& band,
  std::vector< BlockIndex3D >& addedBlocksCoords )
{
  assert( m_isInitialized );
  assert( band < m_ramBlocksPointers.size() );
  
  addedBlocksCoords.clear();
  
  if( expansionSize )
  {
    const unsigned int numberOfBlocksY = (unsigned int)
      m_ramBlocksPointers[ band ].size();
      
    for( unsigned int blockYIdx = 0 ; blockYIdx < numberOfBlocksY ; ++blockYIdx )
    {
      m_ramBlocksPointers[ band ][ blockYIdx ].insert( 
        m_ramBlocksPointers[ band ][ blockYIdx ].begin(), 
        expansionSize, BlockelementPtrT( 0 ) );      
        
      m_activeDiskBlocksInfo[ band ][ blockYIdx ].insert( 
        m_activeDiskBlocksInfo[ band ][ blockYIdx ].begin(), 
        expansionSize, DiskBlockInfo() );         
      
      for( unsigned int blockXIdx = 0 ; blockXIdx < expansionSize ; ++blockXIdx )
      {
        addedBlocksCoords.push_back( BlockIndex3D( band, blockYIdx, blockXIdx ) ); 
      }
    }      
    
    shiftDim03DCoords( m_swapFifo, band, 0, expansionSize );
      
    if( ! allocateAndActivateDiskBlocks( addedBlocksCoords ) ) return false;
  }
  
  return true;
}

bool te::mem::ExpansibleBandBlocksManager::addRightBlocks( 
  const unsigned int& expansionSize, const unsigned int& band,
  std::vector< BlockIndex3D >& addedBlocksCoords )
{
  assert( m_isInitialized );
  assert( band < m_ramBlocksPointers.size() );
  
  addedBlocksCoords.clear();
  
  if( expansionSize )
  {
    const unsigned int numberOfBlocksY = (unsigned int)
      m_ramBlocksPointers[ band ].size();
      
    for( unsigned int blockYIdx = 0 ; blockYIdx < numberOfBlocksY ; ++blockYIdx )
    {
      m_ramBlocksPointers[ band ][ blockYIdx ].insert( 
        m_ramBlocksPointers[ band ][ blockYIdx ].end(), 
        expansionSize, BlockelementPtrT( 0 ) );      
        
      m_activeDiskBlocksInfo[ band ][ blockYIdx ].insert( 
        m_activeDiskBlocksInfo[ band ][ blockYIdx ].end(), 
        expansionSize, DiskBlockInfo() );         
      
      for( unsigned int blockXIdx = m_ramBlocksPointers[ band ][ blockYIdx ].size()
        - expansionSize ; blockXIdx < 
        m_ramBlocksPointers[ band ][ blockYIdx ].size() ; ++blockXIdx )
      {
        addedBlocksCoords.push_back( BlockIndex3D( band, blockYIdx, blockXIdx ) ); 
      }
    }      
      
    if( ! allocateAndActivateDiskBlocks( addedBlocksCoords ) ) return false;
  }
  
  return true;
}

bool te::mem::ExpansibleBandBlocksManager::addTopBands( 
  const unsigned int& expansionSize,
  std::vector< BlockIndex3D >& addedBlocksCoords )
{
  assert( m_isInitialized );
  
  addedBlocksCoords.clear();
  
  if( expansionSize )
  {
    const unsigned int numberOfBlocksY = (unsigned int)
      m_ramBlocksPointers[ 0 ].size();
    const unsigned int numberOfBlocksX = (unsigned int)
      m_ramBlocksPointers[ 0 ][ 0 ].size();
    
    m_ramBlocksPointers.insert( m_ramBlocksPointers.begin(), 
      expansionSize, 
      std::vector< std::vector< BlockelementPtrT > >( numberOfBlocksY ) );
      
    m_activeDiskBlocksInfo.insert( m_activeDiskBlocksInfo.begin(), 
      expansionSize, 
      std::vector< std::vector< DiskBlockInfo > >( numberOfBlocksY ) );      
    
    for( unsigned int expansionIdx = 0 ; expansionIdx < expansionSize ;
      ++expansionIdx )
    {
      for( unsigned int blockYIdx = 0 ; blockYIdx < numberOfBlocksY ; ++blockYIdx )
      {
        m_ramBlocksPointers[ expansionIdx ][ blockYIdx ].resize( numberOfBlocksX );
        m_activeDiskBlocksInfo[ expansionIdx ][ blockYIdx ].resize( numberOfBlocksX );
        
        for( unsigned int blockXIdx = 0 ; blockXIdx < numberOfBlocksX ; ++blockXIdx )
        {
          addedBlocksCoords.push_back( BlockIndex3D( expansionIdx, blockYIdx, blockXIdx ) ); 
        }
      }
    }
    
    shift3DCoords( m_swapFifo, expansionSize, 0, 0 );
      
    if( ! allocateAndActivateDiskBlocks( addedBlocksCoords ) ) return false;
  }
  
  return true;
}

bool te::mem::ExpansibleBandBlocksManager::addBottomBands( const unsigned int& expansionSize,
  std::vector< BlockIndex3D >& addedBlocksCoords )
{
  assert( m_isInitialized );
  
  addedBlocksCoords.clear();
  
  if( expansionSize )
  {
    const unsigned int numberOfBlocksY = (unsigned int)
      m_ramBlocksPointers[ m_ramBlocksPointers.size() - 1 ].size();
    const unsigned int numberOfBlocksX = (unsigned int)
      m_ramBlocksPointers[ m_ramBlocksPointers.size() - 1 ][ 0 ].size();
    
    m_ramBlocksPointers.insert( m_ramBlocksPointers.end(), 
      expansionSize, 
      std::vector< std::vector< BlockelementPtrT > >( numberOfBlocksY ) );
      
    m_activeDiskBlocksInfo.insert( m_activeDiskBlocksInfo.end(), 
      expansionSize, 
      std::vector< std::vector< DiskBlockInfo > >( numberOfBlocksY ) );      
    
    for( unsigned int expansionIdx =  m_ramBlocksPointers.size() - expansionSize ; 
      expansionIdx < m_ramBlocksPointers.size() ; ++expansionIdx )
    {
      for( unsigned int blockYIdx = 0 ; blockYIdx < numberOfBlocksY ; ++blockYIdx )
      {
        m_ramBlocksPointers[ expansionIdx ][ blockYIdx ].resize( numberOfBlocksX );
        m_activeDiskBlocksInfo[ expansionIdx ][ blockYIdx ].resize( numberOfBlocksX );
        
        for( unsigned int blockXIdx = 0 ; blockXIdx < numberOfBlocksX ; ++blockXIdx )
        {
          addedBlocksCoords.push_back( BlockIndex3D( expansionIdx, blockYIdx, blockXIdx ) ); 
        }
      }
    }
    
    if( ! allocateAndActivateDiskBlocks( addedBlocksCoords ) ) return false;
  }
  
  return true;
}

bool te::mem::ExpansibleBandBlocksManager::allocateDiskBlocks( 
  const unsigned int blocksNumber,
  std::vector< DiskBlockInfo >& diskBlocksInfos,                       
  OpenDiskFilesHandlerT& diskFilesHandler ) const
{
  assert( m_maxDiskFilesSize > 0 );
  assert( m_maxBlockSizeBytes > 0 );
  assert( m_maxDiskFilesSize >= m_maxBlockSizeBytes );
  
  diskBlocksInfos.clear();
  diskFilesHandler.clear();

  const unsigned int maxBlocksPerFile = m_maxDiskFilesSize / m_maxBlockSizeBytes;

  unsigned int remainnigBlocksNumber = blocksNumber;
    
  while( remainnigBlocksNumber )
  {
    const unsigned int fileBlocksNumber = std::min( maxBlocksPerFile, remainnigBlocksNumber );
    const unsigned long int fileSizeBytes = ((unsigned long int)fileBlocksNumber) *
      ((unsigned long int)m_maxBlockSizeBytes);
    assert( fileSizeBytes <= m_maxDiskFilesSize );
      
    boost::shared_ptr< OpenDiskFileHandler > newFileHandlerPtr( new OpenDiskFileHandler );
    if( ! createNewDiskFile( fileSizeBytes, &( newFileHandlerPtr->m_filePtr ) ) )
    {
      return false;
    }
    diskFilesHandler.push_back( newFileHandlerPtr );
    
    for( unsigned int fileBlockIdx = 0 ; fileBlockIdx < fileBlocksNumber ;
      ++fileBlockIdx )
    {
      diskBlocksInfos.push_back( DiskBlockInfo() );
      diskBlocksInfos.back().m_filePtr = newFileHandlerPtr->m_filePtr;
      diskBlocksInfos.back().m_fileOff = ((unsigned long int)fileBlockIdx) * 
        ((unsigned long int)m_maxBlockSizeBytes);
    }
      
    remainnigBlocksNumber -= fileBlocksNumber;
  }
    
  return true;
}

bool te::mem::ExpansibleBandBlocksManager::allocateAndActivateDiskBlocks( 
  const std::vector< BlockIndex3D >& blocksIndxes )
{
  assert( m_isInitialized );
  
  if( ! blocksIndxes.empty() )
  {
    // re-activating old inactive disk blocks
    
    unsigned int blocksIndxesIdx = 0;
    
    while( ( blocksIndxesIdx < blocksIndxes.size() ) && 
      ( ! m_inactiveDiskBlocksInfo.empty() ) )
    {
      DiskBlockInfo inactiveBInfo = m_inactiveDiskBlocksInfo.back();
      m_inactiveDiskBlocksInfo.pop_back();
      assert( inactiveBInfo.m_filePtr );
      
      const BlockIndex3D& bIdx = blocksIndxes[ blocksIndxesIdx ];
      
      assert( bIdx.m_dim0Index < m_activeDiskBlocksInfo.size() );
      assert( bIdx.m_dim1Index < m_activeDiskBlocksInfo[ bIdx.m_dim0Index ].size() );
      assert( bIdx.m_dim2Index < m_activeDiskBlocksInfo[ bIdx.m_dim0Index ][ bIdx.m_dim1Index ].size() );      
      assert( m_activeDiskBlocksInfo[ bIdx.m_dim0Index ][ bIdx.m_dim1Index ][
        bIdx.m_dim2Index ].m_filePtr == 0 );
      m_activeDiskBlocksInfo[ bIdx.m_dim0Index ][ bIdx.m_dim1Index ][
        bIdx.m_dim2Index ] = inactiveBInfo;
      
      ++blocksIndxesIdx;
    }
    
    // Allocating new disk blocks
    
    const unsigned int newDiskBlocksNumber = (unsigned int)(
      blocksIndxes.size() - blocksIndxesIdx );
      
    if( newDiskBlocksNumber )
    {
      std::vector< DiskBlockInfo > newDiskBlocksInfos;
      OpenDiskFilesHandlerT newDiskFilesHandler;
      if( ! allocateDiskBlocks( newDiskBlocksNumber, newDiskBlocksInfos, 
        newDiskFilesHandler ) )
        return false;
      assert( newDiskBlocksInfos.size() == newDiskBlocksNumber );
      
      m_diskFilesHandler.insert( m_diskFilesHandler.end(), 
        newDiskFilesHandler.begin(), newDiskFilesHandler.end() );
      
      unsigned int newDiskBlocksInfosIdx = 0;
      while( blocksIndxesIdx < blocksIndxes.size() )
      {
        const BlockIndex3D& bIdx = blocksIndxes[ blocksIndxesIdx ];
        
        assert( bIdx.m_dim0Index < m_activeDiskBlocksInfo.size() );
        assert( bIdx.m_dim1Index < m_activeDiskBlocksInfo[ bIdx.m_dim0Index ].size() );
        assert( bIdx.m_dim2Index < m_activeDiskBlocksInfo[ bIdx.m_dim0Index ][ bIdx.m_dim1Index ].size() );
        assert( m_activeDiskBlocksInfo[ bIdx.m_dim0Index ][ bIdx.m_dim1Index ][
          bIdx.m_dim2Index ].m_filePtr == 0 );
        assert( newDiskBlocksInfosIdx < newDiskBlocksInfos.size() );
        m_activeDiskBlocksInfo[ bIdx.m_dim0Index ][ bIdx.m_dim1Index ][
          bIdx.m_dim2Index ] = newDiskBlocksInfos[ newDiskBlocksInfosIdx ];        
        assert( m_activeDiskBlocksInfo[ bIdx.m_dim0Index ][ bIdx.m_dim1Index ][
          bIdx.m_dim2Index ].m_filePtr );
        
        ++newDiskBlocksInfosIdx;
        ++blocksIndxesIdx;
      }      
    }
  }
  
  return true;
}

bool te::mem::ExpansibleBandBlocksManager::createNewDiskFile( unsigned long int size,
  FILE** fileptr ) const
{
  //(*fileptr) = fopen( filename.c_str(), "wb+" );
  (*fileptr) = tmpfile();
  if( (*fileptr) == 0 )
    return false;
  
  long seekoff = (long)( size - 1 );
  
  if( 0 != fseek( (*fileptr), seekoff, SEEK_SET ) )
  {
    fclose( (*fileptr) );
    return false;
  }

  unsigned char c = '\0';
  if( 1 != fwrite( &c, 1, 1, (*fileptr) ) )
  {
    fclose( (*fileptr) );
    return false;
  }
    
  return true;
}

        

