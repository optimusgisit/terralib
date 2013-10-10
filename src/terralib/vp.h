/*  Copyright (C) 2001-2009 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled applications.

    TerraLib is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraLib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TerraLib. See COPYING. If not, write to
    TerraLib Team at <terralib-team@terralib.org>.
 */

/*!
  \file terralib/vp.h

  \brief This file contains forward declarations for the TerraLib Vector Processing module.
*/

#ifndef __TERRALIB_FW_INTERNAL_VP_H
#define __TERRALIB_FW_INTERNAL_VP_H

namespace te
{
  namespace vp
  {
    bool Aggregation(const std::string& inDataset,
                     te::da::DataSource* inDatasource,
                     const std::vector<te::dt::Property*>& groupingProperties,
                     const std::map<te::dt::Property*, std::vector<te::stat::StatisticalSummary> >& statisticalSummary,
                     const std::string& outDataset,
                     te::da::DataSource* outDatasource);
  } 
}

#endif  // __TERRALIB_FW_INTERNAL_VP_H