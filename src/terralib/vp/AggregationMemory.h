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
  \file AggregationMemory.h
   
  \brief 
 
  \ingroup vp
*/

#ifndef __TERRALIB_VP_INTERNAL_AGGREGATION_MEMORY_H
#define __TERRALIB_VP_INTERNAL_AGGREGATION_MEMORY_H

//Terralib
#include "../common/Exception.h"
#include "../datatype/Property.h"
#include "../dataaccess/dataset/DataSetType.h"
#include "../geometry/Geometry.h"
#include "../memory/DataSet.h"
#include "../statistics/core/Enums.h"

#include "AggregationOp.h"
#include "Config.h"
#include "Enums.h"
#include "Exception.h"


// STL
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace te
{
  namespace vp
  {
    class TEVPEXPORT AggregationMemory : public AggregationOp
    {
    
    public:
      
      AggregationMemory();
      
      ~AggregationMemory();
      
      bool run() throw( te::common::Exception );
      
    private:
      
      std::map<std::string, std::string> calculateStringStats(const std::vector<te::mem::DataSetItem*>& items);
      
      std::map<std::string,double> calculateNumStats(const std::vector<te::mem::DataSetItem*>& items, std::string& modeProp, std::string& modeName);
      
      std::auto_ptr<te::da::DataSetType> buildOutDataSetType();

    }; // end class
  } // end namespace vp
}   // end namespace te

#endif  // __TERRALIB_VP_INTERNAL_AGGREGATION_MEMORY_H

