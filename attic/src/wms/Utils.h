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
  \file terralib/wms/Utils.h
   
  \brief Utility functions for WMS driver.
*/

#ifndef __TERRALIB_WMS_INTERNAL_UTILS_H
#define __TERRALIB_WMS_INTERNAL_UTILS_H

// TerraLib
#include "WMSLayerInfo.h"

// STL
#include <map>
#include <string>

namespace te
{
// Forward declaration
  namespace gm
  {
    class Envelope;
  }

  namespace wms
  {
    void BuildLayersInfo(char** subdatasets, std::map<std::string, WMSLayerInfo>& info);

    WMSLayerInfo BuildLayerInfo(const std::map<std::string, std::string>& kvp);

    void ExtractRequestValues(const std::string& request,
                              std::string& layer,
                              std::string& width,
                              std::string& height,
                              std::string& format);

    std::string BuildGetMapRequest(const std::string& serverUrl,
                                   const WMSLayerInfo& info,
                                   const std::string& width,
                                   const std::string& height,
                                   const std::string& format,
                                   const std::string& bbox = "");

    void GetBBOXValues(const std::string& bbox,
                       std::string& ulx, std::string& uly,
                       std::string& lrx, std::string& lry);

    void GetBBOXValues(const te::gm::Envelope& e,
                       std::string& ulx, std::string& uly,
                       std::string& lrx, std::string& lry);

    std::string GetBBOX(const te::gm::Envelope& e);

  } // end namespace wms
}   // end namespace te

#endif  // __TERRALIB_WMS_INTERNAL_UTILS_H
