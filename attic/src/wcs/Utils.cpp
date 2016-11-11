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
  \file terralib/wcs/Utils.cpp
   
  \brief Utility functions for WCS driver.
*/

// TerraLib
#include "../common/StringUtils.h"
#include "Config.h"
#include "Utils.h"

// STL
#include <cassert>

std::string te::wcs::BuildRequest(const std::string& serviceURL, const std::string& coverageName, const te::gm::Envelope* /*e*/)
{
  // Build the request using the GDAL format. Details: http://www.gdal.org/frmt_wcs.html

  assert(!serviceURL.empty());
  assert(!coverageName.empty());

  std::string request = "<WCS_GDAL>";
    // Service
    request += "<ServiceURL>" + serviceURL + "</ServiceURL>";
    // CoverageName
    request += "<CoverageName>" + coverageName + "</CoverageName>";
  request += "</WCS_GDAL>";

  return request;
}
