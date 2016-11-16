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
  \file terralib/ws/core/CurlWrapper.h

  \brief A Wrapper for Lib Curl.

  \author Emerson Moraes
  \author Vinicius Campanha
 */

#ifndef __TERRALIB_WS_CORE_CURLWRAPPER_H
#define __TERRALIB_WS_CORE_CURLWRAPPER_H

//STL
#include <memory>
#include <mutex>

#include "Config.h"
#include "../../common/progress/TaskProgress.h"
#include "../../core/uri/URI.h"

namespace te
{
  namespace ws
  {
    namespace core
    {

    TEWSCOREEXPORT enum AuthenticationMethod
    {
      NOT_AUTH    = 0,
      HTTP_BASIC  = 1,
      HTTP_DIGEST = 2
    };

    /*!
      \class CurlWrapper

      \brief An abstraction to Lib Curl functions.
    */
    class TEWSCOREEXPORT CurlWrapper
    {
    public:
      CurlWrapper();

      virtual ~CurlWrapper();

      /*!
        \brief Method to download a file retrieved from given URL and save into the specified file path.

        \param url URL that the file will be downloaded.
        \param filePath file path where downloaded file will be saved.
      */
      virtual void downloadFile(const std::string& url, const std::string& filePath, te::common::TaskProgress* taskProgress = 0);

      /*!
        \brief Method to download a file retrieved from given URL and save into the specified File.

        \param url URL that the file will be downloaded.
        \param file File where downloaded file will be write.
      */
      virtual void downloadFile(const std::string& url, std::FILE* file, te::common::TaskProgress* taskProgress = 0);


      /*!
        \brief This method gets the message that will be used on Progress Bar when
        this object is downloading some file.
      */
      virtual std::string getTaskMessage() const;

      /*!
        \brief This method sets the message that will be used on Progress Bar when
        this object is downloading some file.

        \param taskMessage new message.
      */
      virtual void setTaskMessage(const std::string& taskMessage);

      /*!
       * \brief Method to request a post in a determined URI
       * \param uri The URI with the address information to post
       * \param postFields The parameters and values to post
       * \param header A header to the post request
       */
      virtual void post(const te::core::URI& uri, const std::string &postFields, const::std::string &header);

      /*!
       * \brief Method to request a put with a file in a determined URI
       * \param uri The URI with the address information
       * \param file Path The full path of the file to put
       * \param header A header to the put request
       */
      virtual void putFile(const te::core::URI& uri, const std::string &filePath, const::std::string &header);

      /*!
       * \brief Method to request a put a file in a determined URI
       * \param uri The URI with the address information
       * \param file Contains the file opened by the fstream
       * \param header A header to the put request
       */
      virtual void putFile(const te::core::URI& uri, const std::fstream& file,  const::std::string &header);

      /*!
       * \brief Method to make a custom request, useful for performing a HTTP DELETE request.
       * \param uri The URI with the address information
       * \param request The custom request
       */
      virtual void customRequest(const te::core::URI& uri, const std::string& request, const std::string& body = std::string(""), const::std::string &header = std::string(""));

      /*!
       * \brief Method to make a GET request.
       * \param uri The URI with the address information
       * \param buffer Where the server answer will be write.
       */
      virtual void get(const te::core::URI &uri, std::string& buffer);


      /*!
       * \brief Returns the last operation response code
       * \return The response code
       */
      virtual const long responseCode() const;

      /*!
       * \brief Returns the last operation response if the protocol allows it
       * \return The response
       */
      virtual const std::string& response() const;

      /*!
       * \brief Sets the authentication method that will be used on the requests.
       *
       * \param method (e. g.: Basic or Digest)
       */
      virtual void setAuthenticationMethod(const AuthenticationMethod& method);

      /*!
       * \brief Gets current AuthenticationMethod.
       *
       * \return current AuthenticationMethod.
       */
      virtual AuthenticationMethod getAuthenticationMethod() const;

      /*!
       * \brief Sets the user name that will be used when an athentication method is set.
       *
       * \param username
       */
      virtual void setUsername(const std::string& username);

      /*!
       * \brief Gets the user name that will be used when an athentication method is set.
       *
       * \return username
       */
      virtual std::string getUsername() const;

      /*!
       * \brief Sets the password that will be used when an athentication method is set.
       *
       * \param password
       */
      virtual void setPassword(const std::string& password);

    protected:

      /*!
       * \brief Reset the curl handle and the class members
       */
      virtual void clean();

      /*!
       * \brief Adds HTTP Authentication parameters according to the current AuthenticationMethod.
       */
      virtual void addAuthParameters();

    private:
      struct Impl;

      std::unique_ptr<Impl>  m_pimpl;
    };

    }
  }
}

#endif
