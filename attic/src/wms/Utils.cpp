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
  \file terralib/wms/Utils.cpp
   
  \brief Utility functions for WMS driver.
*/

// TerraLib
#include "../common/StringUtils.h"
#include "../geometry/Envelope.h"
#include "Config.h"
#include "Utils.h"

// Boost
#include <boost/lexical_cast.hpp>

// STL
#include <cassert>

void te::wms::BuildLayersInfo(char** subdatasets, std::map<std::string, WMSLayerInfo>& info)
{
  if(subdatasets == 0)
    return;

  for(char** i = subdatasets; *i != 0; i = i + 2)
  {
    std::string request = std::string(*i);     // Pseudo-GetMap request. e.g. "SUBDATASET_1_NAME=WMS:http://..."
    std::string title = std::string(*(i + 1)); // User-friendly layer name. e.g. "SUBDATASET_1_DESC=Seritinga-MG"

    // Removing the key of GDAL metadata
    std::size_t pos = request.find("=");
    assert(pos != std::string::npos);
    request = request.substr(pos + 1);

    // Removing the key of GDAL metadata
    pos = title.find("=");
    assert(pos != std::string::npos);
    title = title.substr(pos + 1);

    std::map<std::string, std::string> kvps;
    te::common::ExtractKVP(request, kvps, "&", "=", true);

    // Builds the informations from the current layer
    WMSLayerInfo currentLayerInfo = BuildLayerInfo(kvps);
    currentLayerInfo.m_title = title;
    currentLayerInfo.m_pseudoGetMapRequest = request;

    // Adding the informations
    info[currentLayerInfo.m_name] = currentLayerInfo;
  }
}

te::wms::WMSLayerInfo te::wms::BuildLayerInfo(const std::map<std::string, std::string>& kvp)
{
  WMSLayerInfo info;

  assert(kvp.find("LAYERS") != kvp.end());
  info.m_name = kvp.find("LAYERS")->second;

  std::map<std::string, std::string>::const_iterator it = kvp.find("BBOX");
  if(it != kvp.end())
    info.m_bbox = it->second;

  it = kvp.find("SRS");
  if(it != kvp.end())
    info.m_srs = it->second;

  it = kvp.find("CRS");
  if(it != kvp.end())
    info.m_srs = it->second;

  it = kvp.find("VERSION");
  if(it != kvp.end())
    info.m_version = it->second;

  return info;
}

void te::wms::ExtractRequestValues(const std::string& request,
                                   std::string& layer,
                                   std::string& width,
                                   std::string& height,
                                   std::string& format)
{
  assert(!request.empty());

  // Default parameters
  width  = TE_WMS_DEFAULT_WIDTH;
  height = TE_WMS_DEFAULT_HEIGHT;
  format = TE_WMS_DEFAULT_IMAGE_FORMAT;

  std::map<std::string, std::string> kvp;
  te::common::ExtractKVP(request, kvp, "&", "=", false);

  // Layers
  std::map<std::string, std::string>::const_iterator it = kvp.find("LAYER");
  if(it != kvp.end())
    layer = it->second;
  else
  {
    // Here assumes that is a simple request by name
    layer = request;
    return;
  }

  // Width
  it = kvp.find("WIDTH");
  if(it != kvp.end())
    width = it->second;

  // Height
  it = kvp.find("HEIGHT");
  if(it != kvp.end())
    height = it->second;

  // Format
  it = kvp.find("FORMAT");
  if(it != kvp.end())
    format = it->second;
}

std::string te::wms::BuildGetMapRequest(const std::string& serverUrl,
                                        const WMSLayerInfo& info,
                                        const std::string& width,
                                        const std::string& height,
                                        const std::string& format,
                                        const std::string& bbox)
{
  assert(!serverUrl.empty());

  // Remove "WMS:" from given server url, if exists
  std::string url = serverUrl;
  if(url.find("WMS:") != std::string::npos)
  {
    std::size_t pos = serverUrl.find(":");
    url = url.substr(pos + 1);
  }

  // Build the GetMap request using the GDAL format. Details: http://www.gdal.org/frmt_wms.html
  std::string request = "<GDAL_WMS>";

  // Service
  request += "<Service name=\"WMS\">";
    request += "<Version>" + info.m_version + "</Version>";
    request += "<ServerUrl>" + url + "</ServerUrl>";
    request += "<SRS>" + info.m_srs + "</SRS>";
    request += "<ImageFormat>" + format + "</ImageFormat>";
    request += "<Transparent>TRUE</Transparent>";
    request += "<Layers>" + info.m_name + "</Layers>";
  request += "</Service>";

  // Extract the BBOX corners
  std::string ulx, uly, lrx, lry;
  if(bbox.empty())
    GetBBOXValues(info.m_bbox, ulx, uly, lrx, lry);
  else
    GetBBOXValues(bbox, ulx, uly, lrx, lry);
  
  // DataWindow
  request += "<DataWindow>";
    request += "<UpperLeftX>"  + ulx + "</UpperLeftX>";
    request += "<UpperLeftY>"  + lry + "</UpperLeftY>";
    request += "<LowerRightX>" + lrx + "</LowerRightX>";
    request += "<LowerRightY>" + uly + "</LowerRightY>";
    request += "<SizeX>" + width + "</SizeX>";
    request += "<SizeY>" + height + "</SizeY>";
  request += "</DataWindow>";

  request += "<BandsCount>4</BandsCount>";

  // BlockSize
  request += "<BlockSizeX>" + width + "</BlockSizeX>";
  request += "<BlockSizeY>" + height + "</BlockSizeY>";

  // Cache (It is a test!)
  request += "<Cache>";
    request += "<Path>./gdalwmscache</Path>";
    request += "<Depth>2</Depth>";
  request += "</Cache>";

  request += "</GDAL_WMS>";

  return request;
}

void te::wms::GetBBOXValues(const std::string& bbox,
                            std::string& ulx, std::string& uly,
                            std::string& lrx, std::string& lry)
{
  std::vector<std::string> values;
  te::common::Tokenize(bbox, values, ",");

  assert(values.size() == 4);

  ulx = values[0];
  uly = values[1];
  lrx = values[2];
  lry = values[3];
}

void te::wms::GetBBOXValues(const te::gm::Envelope& e,
                            std::string& ulx, std::string& uly,
                            std::string& lrx, std::string& lry)
{
  ulx = boost::lexical_cast<std::string>(e.m_llx);
  uly = boost::lexical_cast<std::string>(e.m_lly);
  lrx = boost::lexical_cast<std::string>(e.m_urx);
  lry = boost::lexical_cast<std::string>(e.m_ury);
}

std::string te::wms::GetBBOX(const te::gm::Envelope& e)
{
  std::string ulx, uly, lrx, lry;
  GetBBOXValues(e, ulx, uly, lrx, lry);

  return ulx + "," + uly + "," + lrx + "," + lry;
}
