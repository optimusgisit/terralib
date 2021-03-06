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
  \file terralib/gdal/Transactor.cpp
*/


// Boost
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

// TerraLib
#include "../common/StringUtils.h"
#include "../core/filesystem/FileSystem.h"
#include "../core/translator/Translator.h"
#include "../dataaccess/dataset/DataSetType.h"
#include "../dataaccess/query/DataSetName.h"
#include "../dataaccess/query/Select.h"
#include "../dataaccess/query/From.h"
#include "../dataaccess/query/FromItem.h"
#include "../geometry/Envelope.h"
#include "../raster/Grid.h"
#include "../raster/Raster.h"
#include "../raster/RasterProperty.h"
#include "DataSet.h"
#include "DataSource.h"
#include "Exception.h"
#include "Transactor.h"
#include "Utils.h"
#include "DataSetUseCounter.h"

// GDAL
#include <gdal_priv.h>

// STL
#include <algorithm>

std::auto_ptr<te::da::DataSetType> te::gdal::Transactor::getType(const std::string& dsfullname)
{
  DataSetUseCounter dsUseCounter( GetParentDataSetName( dsfullname ), DataSetsManager::MultipleAccessType );
  
  GDALDataset* ds = static_cast<GDALDataset*>(GDALOpen(dsfullname.c_str(), GA_ReadOnly));
  if (!ds)
  {
    return std::auto_ptr<te::da::DataSetType>();
  }
  
  te::da::DataSetType* ptr = new te::da::DataSetType("", 0);
  ptr->setTitle("raster");
  
  te::rst::Grid* grid = GetGrid(ds);
  
  std::vector<te::rst::BandProperty*> bprops;
  
  GetBandProperties(ds, bprops);
  
  te::rst::RasterProperty* rp = new te::rst::RasterProperty("raster");
  rp->set(grid);
  for (size_t i=0; i<bprops.size(); ++i)
    rp->add(bprops[i]);

  ptr->add(rp);     
  
  GDALClose(ds);
  
  return std::auto_ptr<te::da::DataSetType>(ptr);
}

te::gdal::Transactor::Transactor(const std::string& accessInfo):
  m_path (boost::filesystem::path(accessInfo))
{
}

te::gdal::Transactor::~Transactor()
{
}

te::da::DataSource* te::gdal::Transactor::getDataSource() const
{
  return 0;
}

void te::gdal::Transactor::getDataSetNames(const boost::filesystem::path& path, std::vector<std::string>& dsnames)
{  
  if (te::core::FileSystem::isRegularFile(path.string()))
  {
    std::string upcaseExtension = te::common::Convert2UCase( path.extension().string() );
    if( upcaseExtension[ 0 ] == '.' ) upcaseExtension = upcaseExtension.substr( 1, upcaseExtension.size() - 1);
    
    std::multimap< std::string, std::string > extensionsMap = GetGDALAllDriversUCaseExt2DriversMap();
    
    std::pair< std::multimap< std::string, std::string >::const_iterator,
      std::multimap< std::string, std::string >::const_iterator > extensionsRangeIts =
      extensionsMap.equal_range( upcaseExtension );
    
    if( extensionsRangeIts.first != extensionsRangeIts.second )
    {
      bool subDatasetsSupport = false;
      std::map< std::string, DriverMetadata >::const_iterator metaIt;
      while( extensionsRangeIts.first != extensionsRangeIts.second )
      {
        metaIt = GetGDALDriversMetadata().find( extensionsRangeIts.first->second );
        
        if( metaIt->second.m_subDatasetsSupport )
        {
          subDatasetsSupport = true;
          break;
        }
        ++extensionsRangeIts.first;
      }      
      
      if( subDatasetsSupport )
      {
        DataSetUseCounter dsUseCounter( path.string(), DataSetsManager::MultipleAccessType );
        
        GDALDataset* gds = static_cast<GDALDataset*>(GDALOpen(path.string().c_str(), GA_ReadOnly));
        if (!gds)
          return;
        
        char** subdatasets = gds->GetMetadata("SUBDATASETS");
        if(subdatasets == 0)
        {
          dsnames.push_back(path.leaf().string());
          GDALClose(gds);
          return;
        }
        
        for(char** i = subdatasets; *i != 0; ++i)
        {
          std::map<std::string, std::string> sdsmap;
          
          te::common::ExtractKVP(std::string(*i), sdsmap);
          
          if(sdsmap.begin()->first.find("_NAME") != std::string::npos)
          {
            std::string fullName = sdsmap.begin()->second;
            std::string subdsname = GetSubDataSetName(fullName, te::gdal::GetDriverName(path.string()));
            dsnames.push_back(subdsname);
          }
        }
        GDALClose(gds);
      }
      else
      {
        dsnames.push_back(path.leaf().string());
        return;
      }
    }
  }
  else 
  {
    for (boost::filesystem::directory_iterator it(path), itEnd; it !=itEnd; ++it)
      getDataSetNames(*it,dsnames);
  }
  return;
}

std::vector<std::string> te::gdal::Transactor::getDataSetNames()
{
  std::vector<std::string> dsnames;
  
  getDataSetNames(m_path,dsnames);
  
  return dsnames;
}

bool te::gdal::Transactor::hasDataSets(const boost::filesystem::path& path)
{
  return ( getNumberOfDataSets( path ) > 0 );
}
  
bool te::gdal::Transactor::hasDataSets()
{
  return hasDataSets(m_path);
}

size_t te::gdal::Transactor::getNumberOfDataSets(const boost::filesystem::path& path)
{
  std::vector<std::string> dsnames;
  getDataSetNames( path, dsnames );
  return dsnames.size();
}

std::size_t te::gdal::Transactor::getNumberOfDataSets()
{
  return getNumberOfDataSets(m_path);
}

std::auto_ptr<te::da::DataSetType> te::gdal::Transactor::getDataSetType(const std::string& name)
{
  std::string uri;
  return getDataSetType(m_path,name,uri);
}

std::auto_ptr<te::da::DataSetType> te::gdal::Transactor::getDataSetType(const boost::filesystem::path& path, const std::string& name, std::string& uri)
{
  if (te::core::FileSystem::isRegularFile(path.string()))
  {
    if (path.leaf() == name)
    { 
      // it is a regular file and the dataset we expect
      std::auto_ptr<te::da::DataSetType> dsty = getType(path.string());
      if (dsty.get())
      {
        dsty->setName(name);
        dsty->setTitle(name);
        uri = path.string();
        return dsty;
       }
      else
        return std::auto_ptr<te::da::DataSetType>();
    }
    else
    {
      std::string upcaseExtension = te::common::Convert2UCase( path.extension().string() );
      if( upcaseExtension[ 0 ] == '.' ) upcaseExtension = upcaseExtension.substr( 1, upcaseExtension.size() - 1);
      
      std::multimap< std::string, std::string > extensionsMap = GetGDALAllDriversUCaseExt2DriversMap();
      
      std::pair< std::multimap< std::string, std::string >::const_iterator,
        std::multimap< std::string, std::string >::const_iterator > extensionsRangeIts =
        extensionsMap.equal_range( upcaseExtension );      
        
      if( extensionsRangeIts.first != extensionsRangeIts.second )
      {        
        bool subDatasetsSupport = false;
        std::map< std::string, DriverMetadata >::const_iterator metaIt;
        while( extensionsRangeIts.first != extensionsRangeIts.second )
        {
          metaIt = GetGDALDriversMetadata().find( extensionsRangeIts.first->second );
          
          if( metaIt->second.m_subDatasetsSupport )
          {
            subDatasetsSupport = true;
            break;
          }
          ++extensionsRangeIts.first;
        }     
        
        if( subDatasetsSupport )
        {
          DataSetUseCounter dsUseCounter( GetParentDataSetName( path.string() ), DataSetsManager::MultipleAccessType );
          
          // it might be one of its sub datasets 
          GDALDataset* gds = static_cast<GDALDataset*>(GDALOpen(path.string().c_str(), GA_ReadOnly));
          if (!gds)
            return std::auto_ptr<te::da::DataSetType>();
          
          char** subdatasets = gds->GetMetadata("SUBDATASETS");
          if(subdatasets == 0)
          {
            GDALClose(gds);
            return std::auto_ptr<te::da::DataSetType>(); // it has no subdatasets
          }
          
          for(char** i = subdatasets; *i != 0; i=i+2)
          {
            std::string sds_name = std::string(*i);
            std::string sds_desc = std::string(*(i+1));
            
            unsigned pos = (unsigned)sds_name.find("=");
            std::string val = sds_name.substr(++pos);
            if(GetSubDataSetName(val, te::gdal::GetDriverName(path.string())) == name)
            {
              GDALClose(gds);
              
              uri = val;
              std::auto_ptr<te::da::DataSetType> dsty = getType(val);
              dsty->setName(name);
              
              pos = (unsigned)sds_desc.find("=");
              val = sds_desc.substr(++pos);
              dsty->setTitle(val);
              
              return dsty;
            }
          }
          GDALClose(gds);
        }
        else
        {
          return std::auto_ptr<te::da::DataSetType>(); // it has no subdatasets
        }
      }
      else
      {
        return std::auto_ptr<te::da::DataSetType>();
      }
    }
  }
  else 
  {
    for (boost::filesystem::directory_iterator it(path), itEnd; it != itEnd; ++it)
    {
      std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(*it,name,uri);
      if (dsty.get())
        return dsty;
    }
  }
  return std::auto_ptr<te::da::DataSetType>();
}


std::auto_ptr<te::da::DataSet> te::gdal::Transactor::getDataSet(const std::string& name, 
                                                                te::common::TraverseType travType, 
                                                                bool /*connected*/,
                                                                const te::common::AccessPolicy accessPolicy) 
{
  std::string uri;
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(m_path,name,uri);
  if (!dsty.get())
    return std::auto_ptr<te::da::DataSet>();

  return std::auto_ptr<te::da::DataSet>(new DataSet(dsty,accessPolicy, uri)); 
}

std::auto_ptr<te::da::DataSet> te::gdal::Transactor::getDataSet(const std::string& name,
                                          const std::string& /*propertyName*/,
                                          const te::gm::Envelope* /*e*/,
                                          te::gm::SpatialRelation /*r*/,
                                          te::common::TraverseType travType, 
                                          bool connected,
                                          const te::common::AccessPolicy accessPolicy)
{
  return getDataSet(name, travType, connected, accessPolicy);
}

std::auto_ptr<te::da::DataSet> te::gdal::Transactor::getDataSet(const std::string& name,
                                                                const std::string& /*propertyName*/,
                                                                const te::gm::Geometry* /*g*/,
                                                                te::gm::SpatialRelation /*r*/,
                                                                te::common::TraverseType travType, 
                                                                bool connected,
                                                                const te::common::AccessPolicy accessPolicy)
{
  return getDataSet(name, travType, connected, accessPolicy);
}

std::auto_ptr<te::da::DataSet> te::gdal::Transactor::getDataSet(const std::string& name,
                                                                const te::da::ObjectIdSet* /*oids*/, 
                                                                te::common::TraverseType travType, 
                                                                bool connected,
                                                                const te::common::AccessPolicy accessPolicy)
{
  return getDataSet(name, travType, connected, accessPolicy);
}


boost::ptr_vector<te::dt::Property> te::gdal::Transactor::getProperties(const std::string& datasetName)
{
  boost::ptr_vector<te::dt::Property> properties;
  
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(datasetName);
  if (!dsty.get())
    return boost::ptr_vector<te::dt::Property>();
  
  const std::vector<te::dt::Property*>& props = dsty->getProperties();
  for(std::size_t i = 0; i < props.size(); ++i)
      properties.push_back(props[i]->clone());  
  
  return properties;
}

std::auto_ptr<te::dt::Property> te::gdal::Transactor::getProperty(const std::string& datasetName, const std::string& name)
{
  boost::ptr_vector<te::dt::Property> properties;
  
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(datasetName);
  if (!dsty.get())
    return std::auto_ptr<te::dt::Property>();
  
  const std::vector<te::dt::Property*>& props = dsty->getProperties();
  for(std::size_t i = 0; i < props.size(); ++i)
  {
    if (props[i]->getName() == name)
      return std::auto_ptr<te::dt::Property>(props[i]->clone()); 
  }
  
  return std::auto_ptr<te::dt::Property>();
}

std::auto_ptr<te::dt::Property> te::gdal::Transactor::getProperty(const std::string& datasetName, std::size_t propertyPos)
{
  boost::ptr_vector<te::dt::Property> properties;
  
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(datasetName);
  if (!dsty.get())
    return std::auto_ptr<te::dt::Property>();
  
  const std::vector<te::dt::Property*>& props = dsty->getProperties();
  if (propertyPos<props.size() && propertyPos>0)
    return std::auto_ptr<te::dt::Property>(props[propertyPos]->clone()); 
  
  return std::auto_ptr<te::dt::Property>();
}

std::vector<std::string> te::gdal::Transactor::getPropertyNames(const std::string& datasetName)
{
  std::vector<std::string> pNames;
  
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(datasetName);
  if (dsty.get())
  {
    const std::vector<te::dt::Property*>& props = dsty->getProperties();
    for(std::size_t i = 0; i < props.size(); ++i)
      pNames.push_back(props[i]->getName());
  }  
  return pNames;
}

std::size_t te::gdal::Transactor::getNumberOfProperties(const std::string& datasetName)
{
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(datasetName);
  if (dsty.get())
    return dsty->getProperties().size(); 
  
  return 0;
}

bool te::gdal::Transactor::propertyExists(const std::string& datasetName, const std::string& name)
{
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(datasetName);
  if (dsty.get())
  {
    const std::vector<te::dt::Property*>& props = dsty->getProperties();
    for(std::size_t i = 0; i < props.size(); ++i)
      if(props[i]->getName()==name)
        return true;
  } 
  return false;
}

bool te::gdal::Transactor::propertyExists(const std::string& datasetName, size_t propertyPos)
{
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(datasetName);
  if (dsty.get())
    return (propertyPos < dsty->getProperties().size() && propertyPos>0);
  
  return false;
}

std::auto_ptr<te::da::DataSet> te::gdal::Transactor::query(const te::da::Select& q,
                                     te::common::TraverseType , 
                                     bool,
                                     const te::common::AccessPolicy accessPolicy)
{
  const te::da::From& from = q.from();
  
  if (from.empty())
    throw Exception(TE_TR("Can not process the Select object."));
  
  te::da::DataSetName* dsname = static_cast<te::da::DataSetName*>(from[0].clone());
  
  if (!dsname)
    throw Exception(TE_TR("Can not process the Select object."));
  
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(dsname->getName());
  
  delete dsname;
  
  if (!dsty.get())
    throw Exception(TE_TR("Can not process the Select object: dataset not found."));

  std::string uri = dsty->getTitle();
  return std::auto_ptr<te::da::DataSet>(new DataSet(dsty,accessPolicy,uri)); 
}

std::auto_ptr<te::da::DataSet> te::gdal::Transactor::query(const std::string& query,
                             te::common::TraverseType, 
                             bool,
                             const te::common::AccessPolicy accessPolicy)
{
  std::vector<std::string> words;
  std::string s;
  boost::split(words, s, boost::is_any_of(", "), boost::token_compress_on);
  
  std::vector<std::string>::const_iterator it = std::find(words.begin(), words.end(), "FROM");
  if (it== words.end())
    it =  std::find(words.begin(), words.end(), "from");
  
  if (it== words.end())
    throw Exception(TE_TR("Can not process the query expression."));
  
  ++it;
  if (it== words.end())
    throw Exception(TE_TR("Can not process the query expression."));  
  
  std::string dsname = *it;
  
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(dsname);
  if (!dsty.get())
    throw Exception(TE_TR("Can not process the Select object: dataset not found."));
  
  std::string uri = dsty->getTitle();
  return std::auto_ptr<te::da::DataSet>(new DataSet(dsty,accessPolicy,uri));
}

bool te::gdal::Transactor::dataSetExists(const std::string& name)
{
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(name);
  
  return (dsty.get() != 0);
}

std::auto_ptr<te::gm::Envelope> te::gdal::Transactor::getExtent(const std::string& datasetName, std::size_t propertyPos)
{
  std::auto_ptr<te::dt::Property> pp = getProperty(datasetName,propertyPos);
  
  if (pp.get())
  {
    te::rst::RasterProperty* rp = static_cast<te::rst::RasterProperty*>(pp.get());
    te::gm::Envelope* env = rp->getGrid()->getExtent();
    return std::auto_ptr<te::gm::Envelope>(new te::gm::Envelope(env->getLowerLeftX(), env->getLowerLeftY(),
                                                                env->getUpperRightX(), env->getUpperRightY()));
  }
  return std::auto_ptr<te::gm::Envelope>();
}

std::auto_ptr<te::gm::Envelope> te::gdal::Transactor::getExtent(const std::string& datasetName, const std::string& propertyName)
{
  std::auto_ptr<te::dt::Property> pp = getProperty(datasetName,propertyName);
  
  if (pp.get())
  {
    te::rst::RasterProperty* rp = static_cast<te::rst::RasterProperty*>(pp.get());
    te::gm::Envelope* env = rp->getGrid()->getExtent();
    return std::auto_ptr<te::gm::Envelope>(new te::gm::Envelope(env->getLowerLeftX(), env->getLowerLeftY(),
                                                                env->getUpperRightX(), env->getUpperRightY()));
  }
  return std::auto_ptr<te::gm::Envelope>();
}

void te::gdal::Transactor::createDataSet(te::da::DataSetType* dt,
                                         const std::map<std::string, std::string>& options) 
{ 
  if (!te::core::FileSystem::isDirectory(m_path.string()))
    throw Exception(TE_TR("Create operation supported just on directory data sources."));
      
  te::rst::RasterProperty* rstp = static_cast<te::rst::RasterProperty*>(dt->getProperty(0));
  
  boost::filesystem::path paux(m_path);
  paux /= dt->getName();
  
  if (te::core::FileSystem::exists(paux.string()))
    throw Exception((boost::format(TE_TR("The datasource already has a dataset with this name (\"%1%\")!")) % dt->getName()).str());
  
  DataSetUseCounter dsUseCounter( paux.string(), DataSetsManager::SingleAccessType );
  
  GDALDataset* gds = te::gdal::CreateRaster(paux.string(), rstp->getGrid(), rstp->getBandProperties(),options);
  
  if (!gds)
    throw Exception(TE_TR("GDAL driver couldn't persist the raster file."));
  
  GDALClose(gds);
}

void te::gdal::Transactor::cloneDataSet(const std::string& name,
                                        const std::string& cloneName,
                                        const std::map<std::string, std::string>& options)
{
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(name); 
  
  if (!dsty.get())
    throw Exception(TE_TR("Dataset does not exist."));

  boost::filesystem::path mpath(dsty->getTitle());
  
  if (!te::core::FileSystem::isRegularFile(mpath.string()))
    throw Exception(TE_TR("Can not clone a dataset that it is not a raster file."));
  
  boost::filesystem::path newpath(mpath.parent_path() /= cloneName);
  te::core::FileSystem::copyFile(mpath.string(), newpath.string());
  
}

void te::gdal::Transactor::dropDataSet(const std::string& name)
{
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(name); 
  
  if (!dsty.get())
    throw Exception(TE_TR("Dataset does not exist."));
  
  boost::filesystem::path mpath(dsty->getTitle());
  if (!te::core::FileSystem::isRegularFile(mpath.string()))
    throw Exception(TE_TR("Can not drop a dataset that it is not a raster file."));

  te::core::FileSystem::remove(mpath.string());  
}

void te::gdal::Transactor::renameDataSet(const std::string& name, const std::string& newName)
{
  std::auto_ptr<te::da::DataSetType> dsty = getDataSetType(name); 
  
  if (!dsty.get())
    throw Exception(TE_TR("Dataset does not exist."));
  
  boost::filesystem::path mpath(dsty->getTitle());
  if (!te::core::FileSystem::isRegularFile(mpath.string()))
    throw Exception(TE_TR("Can not rename a dataset that it is not a raster file."));
  
  boost::filesystem::path newpath(mpath.parent_path() /= newName);
  te::core::FileSystem::rename(mpath.string(), newpath.string());
}
