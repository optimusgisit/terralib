#include "WMSClient.h"

// TerraLib
#include "../../../../core/filesystem/FileSystem.h"
#include "../../../../core/translator/Translator.h"
#include "../../../core/CurlWrapper.h"
#include "../../../core/Utils.h"
#include "WMSClient.h"

// Boost
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <string>

te::ws::ogc::WMSClient::WMSClient(const std::string usrDataDir, const std::string uri, const std::string version) :
  m_version(version),
  m_uri(uri)
{
  m_dataDir = usrDataDir + "/wms/";

  std::string baseUrl = m_uri.scheme() + "://" + m_uri.host();

  if (!m_uri.port().empty())
  {
    baseUrl = baseUrl + ":" + m_uri.port();
  }

  baseUrl = baseUrl + m_uri.path() + "?";

  if (!m_uri.query().empty())
  {
    std::string query = m_uri.query();

    size_t endsWith = query.rfind("&");

    if (endsWith != (query.size() - 1))
    {
      query.append("&");
    }

    baseUrl = baseUrl + query;
  }

  m_uri = te::core::URI(baseUrl);

  m_curl = std::shared_ptr<te::ws::core::CurlWrapper>(new te::ws::core::CurlWrapper());

  if (te::core::FileSystem::isDirectory(usrDataDir) && !te::core::FileSystem::exists(m_dataDir))
    te::core::FileSystem::createDirectories(m_dataDir);
}

te::ws::ogc::WMSClient::~WMSClient()
{
}

void te::ws::ogc::WMSClient::updateCapabilities()
{
  std::string url = m_uri.uri();

  if (m_version == "1.3.0")
  {
    url = url + "SERVICE=WMS" + "&VERSION=" + m_version + "&REQUEST=GetCapabilities";
  }
  else
  {
    throw te::common::Exception(TE_TR("WMS version not supported!"));
  }

  m_curl->setTaskMessage(TE_TR("Getting Capabilities"));

  // Request the WCS Capabilities XML file
  std::string xmlPath = this->makeFileRequest(url, "capabilities.xml");

  // Parse the XML file into a struct
  m_capabilities = m_parser.parseCapabilities(xmlPath);
}

const te::ws::ogc::wms::WMSCapabilities& te::ws::ogc::WMSClient::getCapabilities() const
{
  return m_capabilities;
}

const te::ws::ogc::wms::WMSGetMapResponse te::ws::ogc::WMSClient::getMap(const te::ws::ogc::wms::WMSGetMapRequest &request) const
{
  te::ws::ogc::wms::WMSGetMapResponse response;

  //Sets base URL
  std::string url = m_uri.uri();

  if(m_version == "1.3.0")
  {
    url = url + "SERVICE=WMS" + "&VERSION=" + m_version + "&REQUEST=GetMap";

    if(request.m_layers.size() > 0)
    {
      url += "&LAYERS=";

      for(size_t i = 0; i < request.m_layers.size(); i++)
      {
        url += request.m_layers[i];

        if(i != request.m_layers.size() - 1)
        {
          url += ",";
        }
      }
    }

    if(request.m_styles.size() > 0)
    {
      url += "&STYLES=";

      for(size_t i = 0; i < request.m_styles.size(); i++)
      {
        url += request.m_styles[i];

        if(i != request.m_styles.size() - 1)
        {
          url += ",";
        }
      }
    }

    if(!request.m_srs.empty())
    {
      url += "&CRS=" + request.m_srs;
    }

    bool isInverted = te::ws::core::IsInvertedEPSG(request.m_srs);

    if(isInverted)
    {
      url += "&BBOX=";
      url += boost::lexical_cast<std::string>(request.m_boundingBox.m_minY);
      url += ",";
      url += boost::lexical_cast<std::string>(request.m_boundingBox.m_minX);
      url += ",";
      url += boost::lexical_cast<std::string>(request.m_boundingBox.m_maxY);
      url += ",";
      url += boost::lexical_cast<std::string>(request.m_boundingBox.m_maxX);
    }
    else
    {
      url += "&BBOX=";
      url += boost::lexical_cast<std::string>(request.m_boundingBox.m_minX);
      url += ",";
      url += boost::lexical_cast<std::string>(request.m_boundingBox.m_minY);
      url += ",";
      url += boost::lexical_cast<std::string>(request.m_boundingBox.m_maxX);
      url += ",";
      url += boost::lexical_cast<std::string>(request.m_boundingBox.m_maxY);
    }



    url += "&WIDTH=" + boost::lexical_cast<std::string>(request.m_width);
    url += "&HEIGHT=" + boost::lexical_cast<std::string>(request.m_height);

    url += "&FORMAT=" + request.m_format;

    if(request.m_transparent)
    {
      url += "&TRANSPARENT=TRUE";
    }

    if(!request.m_bgColor.empty())
    {
      url += "&BGCOLOR=" + request.m_bgColor;
    }

    if(!request.m_time.empty())
    {
      url += "&TIME=" + request.m_time;
    }
  }

  te::core::URI wmsRequest (url);

  std::string buffer;

  m_curl->get(wmsRequest, buffer);

  response.m_buffer = buffer;
  response.m_size = buffer.size();
  response.m_format = request.m_format;

  return response;
}

std::string te::ws::ogc::WMSClient::makeFileRequest(const std::string url, const std::string fileName) const
{
  std::string path = m_dataDir + fileName;

  m_curl->downloadFile(url, path);

  return path;
}

void te::ws::ogc::WMSClient::setCurlWrapper(te::ws::core::CurlWrapper* curlWrapper)
{
  m_curl.reset(curlWrapper);
}
