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
  \file terralib/ws/ogc/wms/client/WCSClient.h

  \brief WS Client for OGC WMS

  \author Emerson Moraes
*/

#ifndef __TERRALIB_WS_OGC_WCS_CLIENT_WMS_H
#define __TERRALIB_WS_OGC_WCS_CLIENT_WMS_H

// STL
#include <memory>
#include <vector>
#include <map>
#include <string>

// TerraLib
#include "../../../../core/uri/URI.h"
#include "../../../core/CurlWrapper.h"
#include "Config.h"
#include "DataTypes.h"
#include "XMLParser.h"


namespace te
{
  namespace ws
  {
    namespace ogc
    {

    /*!
      \class WMSClient

      \brief A class to retrieve information and data from a Web Map Service.
    */
    class TEOGCWMSEXPORT WMSClient
    {
    public:

      /*!
        \brief Class constructor. It initializes the m_uri and m_version class members

        \param usrDataDir  Directory to store used temporary XML and images from WMS server.
        \param uri         The adress of WMS server.
        \param version     The WMS version.
      */
      WMSClient(const std::string usrDataDir = "",const std::string uri = "", const std::string version = "1.3.0");

      /*!
        \brief Default destructor.
      */
      ~WMSClient();

      /*!
        \brief Method to get the capabilities from a WMS server and store in m_capabilities member

               To access the information contained in the m_capabilities, use the getCapabilities() method.

      */
      void updateCapabilities();

      /*!
        \brief Return the WMSCapabilities member.

               The WMSCapabilities will be empty until the updateCapabilities() method its called.

        \return Returns the WMSCapabilities member.
      */
      const te::ws::ogc::wms::WMSCapabilities& getCapabilities() const;

      /*!
        \brief Return the WMSGetMapResponse member.

               Make the GetMapRequest and return the server answer as a WMSGetMapResponse.

        \return Returns a path to a file
      */
      const te::ws::ogc::wms::WMSGetMapResponse getMap(const te::ws::ogc::wms::WMSGetMapRequest& request) const;

      /*!
        \brief Executes a request on a WMS server

        \param url The complete url of request

        \return Returns a path to a file
      */
      std::string makeFileRequest(const std::string url, const std::string fileName) const;

      /*!
       * \brief Sets the TerraLib CurlWrapper to be used internally.
       *
       *        By default it is an instance of te::ws::core::CurlWrapper class.
       *
       * \param curlWrapper Wrapper to be used.
       */
      void setCurlWrapper(te::ws::core::CurlWrapper* curlWrapper);


    private:
      std::string                                m_version;
      std::string                                m_dataDir;
      te::core::URI                              m_uri;
      te::ws::ogc::wms::WMSCapabilities          m_capabilities;
      std::shared_ptr<te::ws::core::CurlWrapper> m_curl;
      te::ws::ogc::wms::XMLParser                m_parser;

    };
    }
  }
}

#endif //__TERRALIB_WS_OGC_WCS_CLIENT_WMS_H
