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
  \file terralib/gdal/Utils.cpp

  \brief Utility functions for GDAL.
*/

// TerraLib
#include "../common/StringUtils.h"
#include "../common/UnitOfMeasure.h"
#include "../common/UnitsOfMeasureManager.h"
#include "../dataaccess/dataset/DataSetType.h"
#include "../geometry/Coord2D.h"
#include "../geometry/Envelope.h"
#include "../geometry/Geometry.h"
#include "../ogr/Utils.h"
#include "../raster/BandProperty.h"
#include "../raster/Grid.h"
#include "../raster/RasterFactory.h"
#include "../raster/RasterProperty.h"
#include "../srs/SpatialReferenceSystemManager.h"
#include "Band.h"
#include "Exception.h"
#include "Raster.h"
#include "Utils.h"

// STL
#include <cmath>
#include <cstring>
#include <vector>

// GDAL
#include <gdalwarper.h>
#include <ogr_api.h>
#include <ogr_spatialref.h>
#include <ogrsf_frmts.h>

// Boost
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/scoped_array.hpp>

#define TL_B_PROP_DESC_KEY "TERRALIB_BAND_PROPERTY_GDAL_DESCRIPTION_KEY_TEXT"
#define TL_B_PROP_CATNAME_KEY "TERRALIB_BAND_PROPERTY_GDAL_CATEGORY_NAME_KEY_TEXT"

std::string te::gdal::GetSubDataSetName(const std::string& name, const std::string& driverName)
{
  std::vector<std::string> words;
  boost::split(words, name, boost::is_any_of(":"), boost::token_compress_on);
  
  if (words.size() < 3)
    return name;
  
  std::string sdname; 
  
  if (driverName == "HDF4")
  {
    // HDF4_SDS:subdataset_type:file_name:subdataset_index
    sdname = words[0] + ":" + words[1]  + ":" + words[3];
    if (words.size()>4)
      sdname = sdname + ":" + words[4];
    return sdname;
  }
  else if (driverName == "NITF")
  {
    // NITF_IM:image_index:file_name
    
    sdname = words[0] + ":" + words[1];
  }
  else if (driverName == "netCDF")
  {
    // NETCDF:filename:variable_name
    sdname = words[0] + ":" + words[2];
  }
  else 
  {
    // From GDAL documentation: "Currently, drivers which support subdatasets are: ADRG, ECRGTOC, GEORASTER, 
    // GTiff, HDF4, HDF5, netCDF, NITF, NTv2, OGDI, PDF, PostGISRaster, Rasterlite, 
    // RPFTOC, RS2, WCS, and WMS.". It seems that the default format is  FORMAT:variable:file_name
    for (size_t i=0; i<words.size()-1;++i)
      sdname = sdname + ":" + words[i];
  }
  
  return sdname;
}

te::rst::Grid* te::gdal::GetGrid(GDALDataset* gds)
{
  return GetGrid( gds, -1 );
}

te::rst::Grid* te::gdal::GetGrid(GDALDataset* gds, const int multiResLevel)
{
  if (!gds) return 0;
  if( multiResLevel != -1 )
  {
    if( gds->GetRasterCount() <= 0 ) return 0;
    if( multiResLevel >= gds->GetRasterBand( 1 )->GetOverviewCount() ) return 0;
  }
  
  // Defining SRID
  
  int srid = TE_UNKNOWN_SRS;
  
  // The calling of GetProjectionRef isn't thread safe, even for distinct datasets
  // under some linuxes
  boost::unique_lock< boost::mutex > lockGuard( getStaticMutex() );
  const char* projRef = gds->GetProjectionRef();
  lockGuard.release();
  getStaticMutex().unlock();
  
  if ( ( projRef != 0 ) && ( std::strlen( projRef ) > 0 ) )
  {
    char* projRef2 = (char*)projRef;
    char** projWKTPtr = &(projRef2);
    OGRSpatialReference oSRS;
    
    OGRErr ogrReturn = oSRS.importFromWkt( projWKTPtr );
    
    if( ogrReturn == OGRERR_NONE )
    {
      srid = te::ogr::Convert2TerraLibProjection(&oSRS);
    }
  }
  
  // Defining the number of rows / lines
  
  unsigned int NRows = 0;
  unsigned int nCols = 0;
  
  if( multiResLevel == -1 )
  {
    nCols = (unsigned int)gds->GetRasterXSize();
    NRows = (unsigned int)gds->GetRasterYSize();
  }
  else
  {
    nCols = (unsigned int)gds->GetRasterBand( 1 )->GetOverview( multiResLevel )->GetXSize();
    NRows = (unsigned int)gds->GetRasterBand( 1 )->GetOverview( multiResLevel )->GetYSize();
  }
  
  // Defining bounding box

  double gtp[6];
  te::rst::Grid* grid = 0;
  
  if( gds->GetGeoTransform(gtp) == CE_Failure )
  {
    grid = new te::rst::Grid(nCols, NRows, 1.0, 1.0, (te::gm::Envelope*)0, srid);    
  }
  else
  {
    double gridAffineParams[ 6 ];
    gridAffineParams[ 0 ] = gtp[ 1 ];
    gridAffineParams[ 1 ] = gtp[ 2 ];
    gridAffineParams[ 2 ] = gtp[ 0 ] + ( gtp[ 1 ] / 2.0 );
    gridAffineParams[ 3 ] = gtp[ 4 ];
    gridAffineParams[ 4 ] = gtp[ 5 ];
    gridAffineParams[ 5 ] = gtp[ 3 ] + ( gtp[ 5 ] / 2.0 );     
    
    if( multiResLevel == -1 )
    {
      grid = new te::rst::Grid(gridAffineParams, nCols, NRows, srid);
    }
    else
    {
      te::rst::Grid tempGrid(gridAffineParams, (unsigned int)gds->GetRasterXSize(),
        (unsigned int)gds->GetRasterYSize(), srid);
      grid = new te::rst::Grid( nCols, NRows, new te::gm::Envelope( *tempGrid.getExtent() ), srid );
    }
  }
  
  return grid;    
}

void te::gdal::GetBandProperties(GDALDataset* gds, std::vector<te::rst::BandProperty*>& bprops)
{
  if (!gds)
    return;

  bprops.clear();
  
  int nBands = 0;
  
  if( gds->GetRasterCount() > 0 )
  {
    nBands = gds->GetRasterCount();
  }    

  // retrieve the information about each band
  for (int rasterIdx = 0; rasterIdx < nBands ; ++rasterIdx)
  {
    GDALRasterBand* rasterBand = gds->GetRasterBand( ( 1 + rasterIdx) );
    bprops.push_back( GetBandProperty( rasterBand, rasterIdx ) ); 
  }
}

te::rst::BandProperty* te::gdal::GetBandProperty(GDALRasterBand* gband,
   const unsigned int bandIndex )
{
  if (!gband)
    return 0;

  std::auto_ptr< te::rst::BandProperty > bprop( new te::rst::BandProperty(
    gband->GetBand()-1, GetTeDataType(gband->GetRasterDataType()), "") );
  
  bprop->m_idx = bandIndex;
  
  // te::rst::BandProperty* rb = new te::rst::BandProperty(rasterIdx, dt);
  bprop->m_colorInterp = GetTeColorInterpretation(gband->GetColorInterpretation());
  
  if( bprop->m_colorInterp == te::rst::PaletteIdxCInt )
  {
    GDALColorTable * gdalColorTablePtr = gband->GetColorTable();
    
    if( gdalColorTablePtr )
    {
      switch( gdalColorTablePtr->GetPaletteInterpretation() )
      {
        case GPI_Gray : 
        {
          bprop->m_paletteInterp = te::rst::GrayPalInt;
          break;
        }
        case GPI_RGB : // RGBA
        {
          bprop->m_paletteInterp = te::rst::RGBPalInt;
          break;
        }        
        case GPI_CMYK :
        {
          bprop->m_paletteInterp = te::rst::CMYKPalInt;
          break;
        }        
        case GPI_HLS :
        {
          bprop->m_paletteInterp = te::rst::HSLPalInt;
          break;
        }         
        default :
        {
          throw Exception(TE_TR("invalid palette interpretation"));
          break;
        }
      }
      
      te::rst::BandProperty::ColorEntry auxTLColorEntry;
      
      for( int cTableIdx = 0 ; cTableIdx < gdalColorTablePtr->GetColorEntryCount() ;
        ++cTableIdx )
      {
        auxTLColorEntry.c1 = (short)gdalColorTablePtr->GetColorEntry( cTableIdx )->c1;
        auxTLColorEntry.c2 = (short)gdalColorTablePtr->GetColorEntry( cTableIdx )->c2;
        auxTLColorEntry.c3 = (short)gdalColorTablePtr->GetColorEntry( cTableIdx )->c3;
        auxTLColorEntry.c4 = (short)gdalColorTablePtr->GetColorEntry( cTableIdx )->c4;
        
        bprop->m_palette.push_back( auxTLColorEntry );        
      }
        
    }
  }

  // find if there is a no data value
  
  int noDataValueIsUsed = 0;
  double nodataval = gband->GetNoDataValue(&noDataValueIsUsed);
  if (noDataValueIsUsed) 
    bprop->m_noDataValue = nodataval;
  
  // category names
  
  bool categoryNamesLoaded = false;
  
  {
    char** categoriesListPtr = gband->GetCategoryNames();
    
    if( categoriesListPtr )
    {
      const int categoriesListSize = CSLCount( categoriesListPtr );
      
      for( int categoriesListIdx = 0 ; categoriesListIdx < categoriesListSize ;
        ++categoriesListIdx )
      {
        bprop->m_categoryNames.push_back( std::string( 
          categoriesListPtr[ categoriesListIdx ] ) );
        
        categoryNamesLoaded = true;
      }
    }
  }
  
  // Description
  
  bool descriptionLoaded = false;
  
  if( gband->GetDescription() )
  {
    bprop->m_description = std::string( gband->GetDescription() );
    
    descriptionLoaded = true;
  }
  
  // Metadata & Description
  
  {
    char** metadataDomainListPtr = gband->GetMetadataDomainList();
    
    char** metadataListPtr = gband->GetMetadata( ( metadataDomainListPtr ? 
          metadataDomainListPtr[ 0 ] : 0 ) );
    
    if( metadataListPtr )
    {
      const int metadataListSize = CSLCount( metadataListPtr );
      char* namePtr = 0;
      const char* valuePtr = 0;
      const std::size_t catNameKeySize = std::strlen( TL_B_PROP_CATNAME_KEY );
      
      for( int metadataListIdx = 0 ; metadataListIdx < metadataListSize ;
        ++metadataListIdx )
      {
        valuePtr = CPLParseNameValue( metadataListPtr[ metadataListIdx ],
          &namePtr );
        
        if( std::strcmp( namePtr, TL_B_PROP_DESC_KEY ) == 0 )
        {
          if( descriptionLoaded == false )
          {
            bprop->m_description = std::string( valuePtr );
          }
        }
        else if( std::strncmp( namePtr, TL_B_PROP_CATNAME_KEY, catNameKeySize ) == 0 )
        {
          if( categoryNamesLoaded == false )
          {
            bprop->m_categoryNames.push_back( std::string( valuePtr ) );
          }
        }
        else
        {
          bprop->m_metadata.push_back( std::pair<std::string, std::string>(
            std::string( namePtr ), std::string( valuePtr ) ) );
        }
        
        VSIFree( namePtr ); 
      }
    }
    
    if( metadataDomainListPtr ) CSLDestroy( metadataDomainListPtr );
  }  
  
  // unit 

  std::string unitName = gband->GetUnitType();
  if (!unitName.empty())
    bprop->setUnitOfMeasure(te::common::UnitsOfMeasureManager::getInstance().find(unitName));
  
  // scale and offset
  
  bprop->m_valuesOffset = gband->GetOffset(0);
  bprop->m_valuesScale = gband->GetScale(0);
  
  // blocking scheme

  gband->GetBlockSize(&bprop->m_blkw, &bprop->m_blkh);
  bprop->m_nblocksx = (gband->GetXSize() + bprop->m_blkw - 1) / bprop->m_blkw;
  bprop->m_nblocksy = (gband->GetYSize() + bprop->m_blkh - 1) / bprop->m_blkh;

  return bprop.release();
}

void te::gdal::GetBands(te::gdal::Raster* rst, std::vector<te::gdal::Band*>& bands)
{
  if( !GetBands( rst, -1, bands )  )
  {
    throw Exception(TE_TR("Internal error"));
  }
}

bool te::gdal::GetBands(te::gdal::Raster* rst, int multiResLevel, std::vector<te::gdal::Band*>& bands)
{
  bands.clear();
  
  if( rst == 0 ) return false;
  
  GDALDataset* gds = rst->getGDALDataset();
  
  if( gds == 0 ) return false;
  
  if( ( gds->GetAccess() != GA_ReadOnly ) && ( gds->GetAccess() != GA_Update ) )
    return false;
  
  // Defining the number of bands
  
  int terralibBandsNumber = 0;
  
  if( gds->GetRasterCount() > 0 )
  {
    terralibBandsNumber = rst->getGDALDataset()->GetRasterCount();
  }  
  
  // Creating terralib bands
  
  int gdalBandIndex = 1;
  for (int terralibBandIndex = 0; terralibBandIndex < terralibBandsNumber; terralibBandIndex++)
  {
    if( multiResLevel == -1 )
    {
      bands.push_back( 
        new te::gdal::Band(
          rst, 
          terralibBandIndex,
          rst->getGDALDataset()->GetRasterBand( gdalBandIndex )
        ) 
      );
    }
    else
    {
      if( multiResLevel < gds->GetRasterBand( gdalBandIndex )->GetOverviewCount() )
      {
        bands.push_back( 
          new te::gdal::Band(
            rst, 
            terralibBandIndex,
            gds->GetRasterBand( gdalBandIndex )->GetOverview( multiResLevel )
          ) 
        );        
      }
      else
      {
        while( ! bands.empty() )
        {
          delete( bands.back() );
          bands.pop_back();
        }
        
        return false;
      }
    }   
    
    ++gdalBandIndex;
  }
  
  return true;
}

te::rst::RasterProperty* te::gdal::GetRasterProperty(std::string strAccessInfo)
{
  GDALDataset* gds = (GDALDataset*) GDALOpen(strAccessInfo.c_str(), GA_ReadOnly);
  if (!gds)
    return 0;

  te::rst::Grid* grid = GetGrid(gds);

  std::vector<te::rst::BandProperty*> bprops;

  te::gdal::GetBandProperties(gds, bprops);

  std::map<std::string, std::string> rinfo;

  te::rst::RasterProperty* rp = new te::rst::RasterProperty(grid, bprops, rinfo);

  GDALClose(gds);

  return rp;
}

te::gm::Envelope* te::gdal::GetExtent(std::string strAccessInfo)
{
  GDALDataset* gds = (GDALDataset*) GDALOpen(strAccessInfo.c_str(), GA_ReadOnly);
  if (!gds)
    return 0;
  te::rst::Grid* grid = te::gdal::GetGrid(gds);
  GDALClose(gds);
  te::gm::Envelope* ext = new te::gm::Envelope(*grid->getExtent());
  delete grid;
  return ext;
}

GDALDataset* te::gdal::CreateRaster(const std::string& name, te::rst::Grid* g, const std::vector<te::rst::BandProperty*>& bands, 
                          const std::map<std::string, std::string>& optParams)
{
  std::string driverName = GetDriverName(name);
  
  GDALDriverManager* dManPtr = GetGDALDriverManager();
  
  GDALDriver* driverPtr = dManPtr->GetDriverByName(driverName.c_str());
  
  if(!driverPtr)
    throw Exception("Could not create raster because the underlying driver was not found!");
  
  GDALDataType targetGDataType = te::gdal::GetGDALDataType(bands[0]->m_type);
  
  if (targetGDataType == GDT_Unknown)
    throw Exception("Could not create raster because of unknown band data type!");
  
  char** papszOptions = 0;
  
  std::map<std::string, std::string>::const_iterator it = optParams.begin();
  
  std::map<std::string, std::string>::const_iterator it_end = optParams.end();
  
  while(it != it_end)
  {
    if(it->first == "URI" || it->first == "SOURCE")
    {
      ++it;
      
      continue;
    }
    
    papszOptions = CSLSetNameValue(papszOptions, it->first.c_str(), it->second.c_str());
    
    ++it;
  }
  
  GDALDataset* poDataset;
  
  if (driverName == "JPEG" || driverName == "PNG")
  {
    GDALDriver* tmpDriverPtr = dManPtr->GetDriverByName("MEM");
    
    poDataset = tmpDriverPtr->Create(name.c_str(),
                                     g->getNumberOfColumns(),
                                     g->getNumberOfRows(),
                                     bands.size(),
                                     targetGDataType,
                                     papszOptions);
  }
  else
    poDataset = driverPtr->Create(name.c_str(),
                                  g->getNumberOfColumns(),
                                  g->getNumberOfRows(),
                                  bands.size(),
                                  targetGDataType,
                                  papszOptions);
  
  if(papszOptions)
    CSLDestroy(papszOptions);
  
  if(poDataset == 0)
    throw Exception("Could not create raster!");
  
  const double* gridAffineParams = g->getGeoreference();
  
  double gtp[6];
  
  gtp[0] = gridAffineParams[2] - ( gridAffineParams[ 0 ] / 2.0 ); 
  gtp[1] = gridAffineParams[0]; 
  gtp[2] = gridAffineParams[1]; 
  gtp[3] = gridAffineParams[5] - ( gridAffineParams[ 4 ] / 2.0  ); 
  gtp[4] = gridAffineParams[3]; 
  gtp[5] = gridAffineParams[4];
  
  poDataset->SetGeoTransform(gtp);
  
  OGRSpatialReference oSRS;
  
  OGRErr osrsErrorReturn = oSRS.importFromEPSG(g->getSRID());
  CPLErr setProjErrorReturn = CE_Fatal;
  
  if( osrsErrorReturn == OGRERR_NONE )
  {
    char* projWKTPtr = 0;
    
    osrsErrorReturn = oSRS.exportToWkt(&projWKTPtr);
    
    if( osrsErrorReturn == OGRERR_NONE )
    {
      setProjErrorReturn = poDataset->SetProjection(projWKTPtr);
    }
    
    OGRFree(projWKTPtr);
  }
  
  if( setProjErrorReturn != CE_None )
  {
    std::string wktStr = te::srs::SpatialReferenceSystemManager::getInstance().getWkt( 
      g->getSRID() );
    
    if( !wktStr.empty() )
    {
      setProjErrorReturn = poDataset->SetProjection(wktStr.c_str());
    }
  }
  
  int nb = static_cast<int>(bands.size());
  
  for(int dIdx = 0; dIdx < nb; ++dIdx)
  {
    GDALRasterBand* rasterBand = poDataset->GetRasterBand(1 + dIdx);
    
    GDALColorInterp ci = te::gdal::GetGDALColorInterpretation(bands[dIdx]->m_colorInterp);
    
    rasterBand->SetColorInterpretation(ci);
    
    if (ci == GCI_PaletteIndex)
    {
      GDALColorTable* gCTablePtr = new GDALColorTable(GPI_RGB);
      GDALColorEntry gCEntry;
      
      for (unsigned int pIdx=0 ; pIdx < bands[dIdx]->m_palette.size(); ++pIdx)
      {
        gCEntry.c1 = (short)bands[dIdx]->m_palette[pIdx].c1;
        gCEntry.c2 = (short)bands[dIdx]->m_palette[pIdx].c2;
        gCEntry.c3 = (short)bands[dIdx]->m_palette[pIdx].c3;
        gCEntry.c4 = (short)bands[dIdx]->m_palette[pIdx].c4;
        gCTablePtr->SetColorEntry(pIdx, &gCEntry);
      }
      
      rasterBand->SetColorTable(gCTablePtr);
      delete gCTablePtr;
    }
    rasterBand->SetNoDataValue(bands[dIdx]->m_noDataValue);
    rasterBand->SetOffset(bands[dIdx]->m_valuesOffset.real());
    rasterBand->SetScale(bands[dIdx]->m_valuesScale.real());
    
    // category names
    
    bool categoryNamesSaved = false;
    
    {
      char** categoryNamesListPtr = 0;
      
      for( std::size_t categoryNamesIdx = 0 ; categoryNamesIdx <
        bands[ dIdx ]->m_categoryNames.size() ; ++categoryNamesIdx )
      {
        if( ! bands[ dIdx ]->m_categoryNames[ categoryNamesIdx ].empty() )
        {
          categoryNamesListPtr = CSLAddString( categoryNamesListPtr, 
             bands[ dIdx ]->m_categoryNames[ categoryNamesIdx ].c_str() );
        }
      }
      
      if( categoryNamesListPtr )
      {
        if( rasterBand->SetCategoryNames( categoryNamesListPtr ) == CE_None )
        {
          categoryNamesSaved = true;
        }
        
        CSLDestroy(categoryNamesListPtr);
      }
    }
    
    // Description
    
    if( ! bands[ dIdx ]->m_description.empty() )
    {
       rasterBand->SetDescription( bands[ dIdx ]->m_description.c_str() );   
    }
    
    // Metadata
    
    {
      char** metadataListPtr = 0;
      
      // If categoy names saving failed, it will be saved as metadata
      
      if( 
          ( ! bands[ dIdx ]->m_categoryNames.empty() )
          &&
          ( categoryNamesSaved == false )
        )
      {
        for( std::size_t categoryNamesIdx = 0 ; categoryNamesIdx <
          bands[ dIdx ]->m_categoryNames.size() ; ++categoryNamesIdx )
        {
          if( ! bands[ dIdx ]->m_categoryNames[ categoryNamesIdx ].empty() )
          {
            metadataListPtr = CSLAddNameValue( metadataListPtr, 
               ( std::string( TL_B_PROP_CATNAME_KEY ) +
               boost::lexical_cast< std::string >( categoryNamesIdx ) ).c_str(),
               bands[ dIdx ]->m_categoryNames[ categoryNamesIdx ].c_str() );               
          }
        }
      }
      
      // Band description will be also saved as metadata
      
      if( ! bands[ dIdx ]->m_description.empty() )
      {
        metadataListPtr = CSLAddNameValue( metadataListPtr, 
           TL_B_PROP_DESC_KEY,
           bands[ dIdx ]->m_description.c_str() );   
      }
      
      // Other metadata
      
      for( std::size_t metadataIdx = 0 ; metadataIdx <
        bands[ dIdx ]->m_metadata.size() ; ++metadataIdx )
      {
        if( !
            ( bands[ dIdx ]->m_metadata[ metadataIdx ].first.empty() )
            ||
            ( bands[ dIdx ]->m_metadata[ metadataIdx ].second.empty() )
          )
        {
          metadataListPtr = CSLAddString( metadataListPtr, 
             ( bands[ dIdx ]->m_metadata[ metadataIdx ].first + "=" +
             bands[ dIdx ]->m_metadata[ metadataIdx ].second ).c_str() );
        }
      }
      
      if( metadataListPtr )
      {
        char** metadataDomainListPtr = rasterBand->GetMetadataDomainList();
        
        rasterBand->SetMetadata( metadataListPtr, ( metadataDomainListPtr ? 
          metadataDomainListPtr[ 0 ] : 0 ) );
        CSLDestroy(metadataListPtr);
        
        if( metadataDomainListPtr ) CSLDestroy( metadataDomainListPtr );
      }
    }
  }
  
  return poDataset;

}

GDALDataset* te::gdal::CreateRaster(te::rst::Grid* g,
                                    const std::vector<te::rst::BandProperty*>& bands,
                                    const std::map<std::string, std::string>& optParams)
{
  assert(g);
  assert(bands.size() > 0);

  std::string accessInfo = GetGDALConnectionInfo(optParams);
  return te::gdal::CreateRaster(accessInfo, g, bands, optParams);
}

GDALDataset* te::gdal::GetRasterHandle(const std::string strAccessInfo, te::common::AccessPolicy policy)
{
  GDALAccess gpolicy = GA_ReadOnly;

  if(policy == te::common::WAccess || policy == te::common::RWAccess)
    gpolicy = GA_Update;

  GDALDataset* gds = (GDALDataset*) GDALOpen(strAccessInfo.c_str(), gpolicy);

  return gds;
}

std::string te::gdal::MakePGConnectionStr(const std::map<std::string, std::string>& dsInfo)
{
/*PG":host='<host>' port:'<port>' dbname='<dbname>' user='<user>' password='<password>' [schema='<schema>'] [table='<raster_table>'] [where='<sql_where>'] [mode='<working_mode>']"*/

  std::map<std::string, std::string>::const_iterator it = dsInfo.find("host");
  std::map<std::string, std::string>::const_iterator it_end = dsInfo.end();
  std::string connInfo = "PG:";

  if(it != it_end)
    connInfo += " host = " + it->second;

  it = dsInfo.find("port");

  if(it != it_end)
    connInfo += " port = " + it->second;

  it = dsInfo.find("dbname");

  if(it != it_end)
    connInfo += " dbname = " + it->second;

  it = dsInfo.find("user");

  if(it != it_end)
    connInfo += " user = " + it->second;

  it = dsInfo.find("password");

  if(it != it_end)
    connInfo += " password = " + it->second;

  it = dsInfo.find("schema");

  if(it != it_end)
    connInfo += " schema = " + it->second;

  it = dsInfo.find("table");

  if(it != it_end)
    connInfo += " table = " + it->second;

  it = dsInfo.find("where");

  if(it != it_end)
    connInfo += " where = " + it->second;

  it = dsInfo.find("mode");

  if(it != it_end)
    connInfo += " mode = " + it->second;

  return connInfo;
}

bool te::gdal::RecognizesSRID(unsigned int srid)
{
  OGRSpatialReference oSRS;
  oSRS.importFromEPSG(srid);
  char* coutWKT = NULL;
  oSRS.exportToWkt(&coutWKT);
  std::string outwkt(coutWKT);
  return (!outwkt.empty());
}

/* This function is based on the WARP tutorial in http://www.gdal.org */
bool te::gdal::ReprojectRaster(te::rst::Raster const * const rin, te::rst::Raster* rout)
{
  assert(rin);
  assert(rout);

  te::gdal::Raster const* grin  = static_cast<te::gdal::Raster const *>(rin);
  te::gdal::Raster* grout = static_cast<te::gdal::Raster*>(rout);

  GDALDatasetH hSrcDS = grin->getGDALDataset();
  if (hSrcDS == 0)
    return false;

  GDALDatasetH hDstDS = grout->getGDALDataset();
  if (hDstDS == 0)
    return false;

  int nBands = GDALGetRasterCount(hSrcDS);

  /* Define warp options */
  GDALWarpOptions *psWarpOptions = GDALCreateWarpOptions();
  psWarpOptions->hSrcDS = hSrcDS;
  psWarpOptions->hDstDS = hDstDS;
  psWarpOptions->nBandCount = nBands;
  psWarpOptions->panSrcBands = (int*)CPLMalloc(sizeof(int)*psWarpOptions->nBandCount);
  psWarpOptions->panDstBands = (int*)CPLMalloc(sizeof(int)*psWarpOptions->nBandCount);
  for (int b = 0; b < psWarpOptions->nBandCount; b++)
  {
    psWarpOptions->panSrcBands[b] = b+1;
    psWarpOptions->panDstBands[b] = b+1;
  }

  /* Establish reprojection transformer */
  psWarpOptions->pTransformerArg = GDALCreateGenImgProjTransformer(hSrcDS, GDALGetProjectionRef(hSrcDS),
                                                                   hDstDS, GDALGetProjectionRef(hDstDS),
                                                                   FALSE, 0.0, 1);
  psWarpOptions->pfnTransformer = GDALGenImgProjTransform;

  /* Initialize and execute the warp operation */
  GDALWarpOperation oOperation;
  oOperation.Initialize(psWarpOptions);
  oOperation.WarpRegion(0, 0, GDALGetRasterXSize(hDstDS), GDALGetRasterYSize(hDstDS));
    
  /* Close transformer and images */
  GDALDestroyGenImgProjTransformer(psWarpOptions->pTransformerArg);
  GDALDestroyWarpOptions(psWarpOptions);

  return true;
}

std::string te::gdal::GetDriverName(const std::string& name)
{
// check if it is a filename, and tries to use its extension
  boost::filesystem::path mpath(name.c_str());

  std::string ext = te::common::Convert2UCase(mpath.extension().string());
  if( ext[ 0 ] == '.' ) ext = ext.substr( 1, ext.size() - 1);
  
  std::multimap< std::string, std::string > extensionsMap = GetGDALAllDriversUCaseExt2DriversMap();
  
  std::multimap< std::string, std::string >::const_iterator exttMapIt =
    extensionsMap.find( ext );
    
  if( exttMapIt == extensionsMap.end() )
  {
    return std::string();
  }
  else
  {
    return exttMapIt->second;
  }
}

std::string te::gdal::GetGDALConnectionInfo(const std::map<std::string, std::string>& connInfo)
{
  std::map<std::string, std::string>::const_iterator it = connInfo.find("URI");

  if(it != connInfo.end())
    return it->second;

  it = connInfo.find("SOURCE");

  if(it != connInfo.end())
    return it->second;

  throw Exception(TE_TR("Invalid data source connection information!."));
}

void te::gdal::Vectorize(GDALRasterBand* band, std::vector<te::gm::Geometry*>& geometries)
{
// create data source of geometries in memory
  //OGRSFDriver *ogrDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("Memory");
  GDALDriver* ogrDriver = GetGDALDriverManager()->GetDriverByName("Memory");

  //OGRDataSource* ds = ogrDriver->CreateDataSource("ds_vectorize", NULL);
  GDALDataset* ds = ogrDriver->Create("ds_vectorize", 0, 0, 0, GDT_Unknown, NULL);

  OGRLayer* ogrLayer = ds->CreateLayer("vectorization", NULL, wkbMultiPolygon, NULL);

  ogrLayer->CreateField(new OGRFieldDefn("id", OFTInteger));

// call polygonize function from gdal
  if (GDALPolygonize(band, NULL , ogrLayer, 0, NULL, NULL, NULL) == CE_Failure)
    return;

// convert geometries to terralib
  for (int g = 0; g < ogrLayer->GetFeatureCount(); g++)
    geometries.push_back(te::ogr::Convert2TerraLib(ogrLayer->GetFeature(g)->GetGeometryRef()));

  //OGRDataSource::DestroyDataSource(ds);
  GDALClose(ds);
}

void te::gdal::Rasterize(std::vector<te::gm::Geometry*> geometries, GDALDataset* outraster)
{
// define list of bands (using single band)
  std::vector<int> bandList;

  bandList.push_back(1);

  std::vector<OGRGeometryH> ogrGeometries;

  std::vector<double> burnValues;

// defining vector of ogr geometries and different values for each one in raster
  int bvalue = 254;
  for (std::size_t g = 0; g < geometries.size(); g++)
  {
    burnValues.push_back(bvalue % 255);

    bvalue = bvalue >= 127? bvalue - 126: bvalue > 255? 0: bvalue + 127;

    ogrGeometries.push_back(te::ogr::Convert2OGR(geometries[g]));
  }

  GDALRasterizeGeometries(outraster, bandList.size(), &(bandList[0]), ogrGeometries.size(), &(ogrGeometries[0]), NULL, NULL, &(burnValues[0]), NULL, NULL, NULL);
}

bool te::gdal::IsSubDataSet( const std::string& uri )
{
  std::size_t firstIdx = uri.find( ":" );
  
  if( firstIdx < uri.size() )
  {
    std::size_t secondIdx = uri.find( ":", firstIdx + 1 );
    
    if( secondIdx < uri.size() )
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

std::string te::gdal::GetParentDataSetName(const std::string& subDataSetName)
{
  if( IsSubDataSet( subDataSetName ) )
  {
    std::size_t firstIdx = subDataSetName.find( ":" );
    
    if( firstIdx < subDataSetName.size() )
    {
      std::size_t secondIdx = subDataSetName.find( ":", firstIdx + 1 );
      
      if( secondIdx < subDataSetName.size() )
      {
        return subDataSetName.substr( secondIdx + 1, subDataSetName.size() - firstIdx - 1);
      }
      else
      {
        return subDataSetName;
      }
    }
    else
    {
      return subDataSetName;
    }
  }
  else
  {
    return subDataSetName;
  }
}

boost::mutex& te::gdal::getStaticMutex()
{
  static boost::mutex getStaticMutexStaticMutex;
  return getStaticMutexStaticMutex;
}

std::map< std::string, te::gdal::DriverMetadata >&  te::gdal::GetGDALDriversMetadata()
{
  static std::map< std::string, DriverMetadata > driversMetadataMap;
  
  if(driversMetadataMap.empty() )
  {
    GDALDriverManager* driverManagerPtr = GetGDALDriverManager();
    
    int driversCount = driverManagerPtr->GetDriverCount();
    const char* valuePtr = 0;
    
    for( int driverIndex = 0 ; driverIndex < driversCount ; ++driverIndex )
    {
      GDALDriver* driverPtr = driverManagerPtr->GetDriver(driverIndex);
      
      if( driverPtr )
      {
        DriverMetadata auxMD;
        auxMD.m_driverName = driverPtr->GetDescription();
        
        CPLStringList metaInfo( driverPtr->GetMetadata(), false );
        CPLStringList metaDomainList( driverPtr->GetMetadataDomainList(), true );        
        
        valuePtr = metaInfo.FetchNameValue( GDAL_DMD_EXTENSIONS );
        if( valuePtr )
        {
          te::common::Tokenize( std::string( valuePtr ), auxMD.m_extensions, " " );
        }
        
        valuePtr = metaInfo.FetchNameValue( GDAL_DMD_LONGNAME );
        if( valuePtr ) auxMD.m_longName = valuePtr;    
        
        valuePtr = metaInfo.FetchNameValue( GDAL_DMD_SUBDATASETS );
        if( ( valuePtr != 0 ) && ( std::strcmp( "YES", valuePtr ) == 0 ) )
        {
          auxMD.m_subDatasetsSupport = true;
        }
        else
        {
          auxMD.m_subDatasetsSupport = false;
        }
        
        valuePtr = metaInfo.FetchNameValue( GDAL_DCAP_RASTER );
        if( ( valuePtr != 0 ) && ( std::strcmp( "YES", valuePtr ) == 0 ) )
        {
          auxMD.m_isRaster = true;
        }
        else
        {
          auxMD.m_isRaster = false;
        }        
        
        valuePtr = metaInfo.FetchNameValue( GDAL_DCAP_VECTOR );
        if( ( valuePtr != 0 ) && ( std::strcmp( "YES", valuePtr ) == 0 ) )
        {
          auxMD.m_isVector = true;
        }
        else
        {
          auxMD.m_isVector = false;
        }          
        
        driversMetadataMap[ auxMD.m_driverName ] = auxMD;
      }
    }
  }
  
  return driversMetadataMap;
}

std::multimap< std::string, std::string >& te::gdal::GetGDALRasterDriversUCaseExt2DriversMap()
{  
  static std::multimap< std::string, std::string > extensionsMap;
  
  if(extensionsMap.empty() )
  {
    const std::map< std::string, DriverMetadata >& driversMetadata = GetGDALDriversMetadata();
    
    for( std::map< std::string, DriverMetadata >::const_iterator it = driversMetadata.begin() ;
      it != driversMetadata.end() ; ++it )
    {
      if( 
          ( ! it->second.m_extensions.empty() ) 
          &&
          it->second.m_isRaster 
        )
      {
        for( std::size_t extensionsIdx = 0 ; extensionsIdx < it->second.m_extensions.size() ;
          ++extensionsIdx )
        {        
          extensionsMap.insert( std::pair< std::string, std::string >(
            te::common::Convert2UCase( it->second.m_extensions[ extensionsIdx ] ), it->first ) );;
        }
      }
    }
  }
  
  return extensionsMap;
}

std::multimap< std::string, std::string >& te::gdal::GetGDALVectorDriversUCaseExt2DriversMap()
{  
  static std::multimap< std::string, std::string > extensionsMap;
  
  if(extensionsMap.empty() )
  {
    const std::map< std::string, DriverMetadata >& driversMetadata = GetGDALDriversMetadata();
    
    for( std::map< std::string, DriverMetadata >::const_iterator it = driversMetadata.begin() ;
      it != driversMetadata.end() ; ++it )
    {
      if( 
          ( ! it->second.m_extensions.empty() ) 
          &&
          it->second.m_isVector 
        )
      {
        for( std::size_t extensionsIdx = 0 ; extensionsIdx < it->second.m_extensions.size() ;
          ++extensionsIdx )
        {        
          extensionsMap.insert( std::pair< std::string, std::string >(
            te::common::Convert2UCase( it->second.m_extensions[ extensionsIdx ] ), it->first ) );;
        }
      }
    }
  }
  
  return extensionsMap;
}

std::multimap< std::string, std::string > te::gdal::GetGDALAllDriversUCaseExt2DriversMap()
{  
  std::multimap< std::string, std::string > extensionsMap( GetGDALVectorDriversUCaseExt2DriversMap() );
  
  std::multimap< std::string, std::string >::const_iterator it = 
    GetGDALRasterDriversUCaseExt2DriversMap().begin();
  std::multimap< std::string, std::string >::const_iterator itEnd = 
    GetGDALRasterDriversUCaseExt2DriversMap().end();
    
  while( it != itEnd )
  {
    extensionsMap.insert( std::pair< std::string, std::string >( it->first, it->second ) );
    ++it;
  }
  
  return extensionsMap;
}

