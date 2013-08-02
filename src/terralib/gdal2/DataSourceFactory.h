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
 \file terralib/gdal/DataSourceFactory.h
 
 \brief This is the concrete implementation of a factory that builds data sources managed by the GDAL Library.
 */

#ifndef __TERRALIB_GDAL_INTERNAL_DATASOURCEFACTORY_H
#define __TERRALIB_GDAL_INTERNAL_DATASOURCEFACTORY_H

// TerraLib
#include "../dataaccess2/datasource/DataSourceFactory.h"
#include "Config.h"

namespace te
{
  namespace gdal
  {
    class TEGDALEXPORT DataSourceFactory : public te::da::DataSourceFactory
    {
    public:
      
      const std::string& getType() const;
      
      void getConnectionParameters(std::vector<std::string>& params) const;
      
      ~DataSourceFactory();
      
    protected:        
      
      te::da::DataSource* build();
      
      DataSourceFactory();
      
    private:
      
      static DataSourceFactory sm_factory; //!< The global In-Memory data source factory.
      static std::string sm_factory_identifier;
    };
  } // end namespace gdal
}   // end namespace te

#endif  // __TERRALIB_GDAL_INTERNAL_DATASOURCEFACTORY_H


