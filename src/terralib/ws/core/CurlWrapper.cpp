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
  \file terralib/ws/core/CurlWrapper.cpp

  \brief A Wrapper for Lib Curl.

  \author Emerson Moraes
  \author Vinicius Campanha
 */

#include "CurlWrapper.h"

// Terralib
#include "../../../terralib/common/Exception.h"
#include "../../core/encoding/CharEncoding.h"
#include "../../../terralib/core/translator/Translator.h"

// LibCurl
#include <curl/curl.h>

// STL
#include <sstream>
#include <fstream>

struct CurlProgress
{
  te::common::TaskProgress* m_task;
  std::shared_ptr<CURL> m_curl;
  std::string m_baseMessage;

  CurlProgress() : m_task(0)
  {
  }

  ~CurlProgress()
  {
  }
};

struct te::ws::core::CurlWrapper::Impl
{
  std::shared_ptr<CURL>  m_curl;
  std::mutex             m_bufferMutex;
  std::string            m_taskMessage;
  std::string            m_response;
  long                   m_responseCode;
  std::string            m_username;
  std::string            m_password;
  te::ws::core::AuthenticationMethod  m_method;
};

te::ws::core::CurlWrapper::CurlWrapper()
{
  m_pimpl.reset(new Impl);
  m_pimpl->m_curl = std::shared_ptr<CURL>(curl_easy_init(), curl_easy_cleanup);
  m_pimpl->m_method = AuthenticationMethod::NOT_AUTH;
}


te::ws::core::CurlWrapper::~CurlWrapper()
{
}


void te::ws::core::CurlWrapper::clean()
{
  curl_easy_reset(m_pimpl->m_curl.get());

  m_pimpl->m_response = "";
  m_pimpl->m_responseCode = 0;

#ifndef NDEBUG
  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_NOSIGNAL, 1);
#endif
}

size_t WriteFileCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
  std::FILE *writehere = (std::FILE *)data;
  return fwrite(ptr, size, nmemb, writehere);
}



size_t WriteResponse(char* data, size_t size, size_t nmemb, std::string* buffer)
{
  if(buffer == 0)
      return 0;

  buffer->append(data, size * nmemb);

  return size * nmemb;
}


size_t read_stream_callback(char *buffer, size_t size, size_t nitems, void *instream)
{
  std::fstream* stream = static_cast<std::fstream*>(instream);

  if(!stream->is_open())
  {
    return 0;
  }

  size_t nbytes = size * nitems;

  size_t bytesWritten = stream->readsome(buffer, nbytes);

  return bytesWritten;
}

int DownloadProgress(void *p,
                     curl_off_t dltotal, curl_off_t dlnow,
                     curl_off_t ultotal, curl_off_t ulnow)
{
  CurlProgress* progress = (CurlProgress*) p;

  std::stringstream ss;

  progress->m_task->pulse();

  ss << dlnow;

  std::string::size_type sz;
  long numBytes = std::stol(ss.str(), &sz);

  long numKBytes = numBytes / 1000;

  progress->m_task->setMessage(progress->m_baseMessage + " " + std::to_string(numKBytes) + TE_TR(" KBytes received."));

  return 0;
}

void te::ws::core::CurlWrapper::downloadFile(const std::string& url, const std::string& filePath, te::common::TaskProgress* taskProgress)
{

  std::FILE* file = std::fopen(te::core::CharEncoding::fromUTF8(filePath).c_str(), "wb");

  if(!file)
    throw te::common::Exception(TE_TR("Could not open file to write!"));

  downloadFile(url, file, taskProgress);

  if (!file)
    throw te::common::Exception(TE_TR("Error at received file!"));
  else
    std::fclose(file);
}

void te::ws::core::CurlWrapper::downloadFile(const std::string &url, std::FILE* file, te::common::TaskProgress* taskProgress)
{
  clean();

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_URL, url.c_str());

  // Get data to be written
  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_WRITEFUNCTION, WriteFileCallback);

  // Set a pointer to our file
  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_WRITEDATA, file);

  te::common::TaskProgress* task = 0;

  if (!taskProgress)
    task = new te::common::TaskProgress(m_pimpl->m_taskMessage, te::common::TaskProgress::UNDEFINED, 100);
  else
    task = taskProgress;

  CurlProgress progress;

  progress.m_curl = m_pimpl->m_curl;
  progress.m_task = task;
  progress.m_baseMessage = m_pimpl->m_taskMessage;

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_XFERINFOFUNCTION, DownloadProgress);

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_XFERINFODATA, &progress);

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_NOPROGRESS, 0L);

  /* Perform the request, status will get the return code */
  CURLcode status = curl_easy_perform(m_pimpl->m_curl.get());

  if (!taskProgress)
    delete task;

  // Check for errors
  if(status != CURLE_OK)
    throw te::common::Exception(curl_easy_strerror(status));
}


std::string te::ws::core::CurlWrapper::getTaskMessage() const
{
  return m_pimpl->m_taskMessage;
}


void te::ws::core::CurlWrapper::setTaskMessage(const std::string &taskMessage)
{
  m_pimpl->m_taskMessage = taskMessage;
}


void te::ws::core::CurlWrapper::post(const te::core::URI &uri,
                                     const std::string &postFields,
                                     const::std::string &header)
{
  clean();

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_URL, uri.uri().c_str());

  char errbuf[CURL_ERROR_SIZE];

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_ERRORBUFFER, errbuf);
  errbuf[0] = 0;

  struct curl_slist* headers= nullptr;
  headers = curl_slist_append(headers, header.c_str());
  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_POSTFIELDS, postFields.c_str());

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_WRITEFUNCTION, WriteResponse);

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_WRITEDATA, &m_pimpl->m_response);

  /* Perform the request, status will get the return code */
  CURLcode status = curl_easy_perform(m_pimpl->m_curl.get());

  curl_slist_free_all(headers);

  // Check for errors
  if(status != CURLE_OK)
  {
    std::string msg = curl_easy_strerror(status) + (':' + std::string(errbuf));
    throw te::common::Exception(msg);
  }

  status = curl_easy_getinfo(m_pimpl->m_curl.get(), CURLINFO_RESPONSE_CODE, &m_pimpl->m_responseCode);

  // Check for errors
  if(status != CURLE_OK)
  {
    std::string msg = curl_easy_strerror(status) + (':' + std::string(errbuf));
    throw te::common::Exception(msg);
  }
}


void te::ws::core::CurlWrapper::putFile(const te::core::URI &uri, const std::string &filePath, const std::string &header)
{
  std::fstream stream(filePath);

  if(!stream.is_open())
  {
    throw te::common::Exception(TE_TR("Can't open the file!."));
  }

  putFile(uri, stream, header);
}


void te::ws::core::CurlWrapper::putFile(const te::core::URI &uri, const std::fstream& file, const::std::string &header)
{
  clean();

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_URL, uri.uri().c_str());

  char errbuf[CURL_ERROR_SIZE];

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_ERRORBUFFER, errbuf);
  errbuf[0] = 0;

  struct curl_slist* headers= nullptr;
  headers = curl_slist_append(headers, header.c_str());
  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_UPLOAD, 1L);

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_READFUNCTION, read_stream_callback);

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_READDATA, (const std::fstream*)&file);

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_WRITEFUNCTION, WriteResponse);

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_WRITEDATA, &m_pimpl->m_response);

  /* Perform the request, status will get the return code */
  CURLcode status = curl_easy_perform(m_pimpl->m_curl.get());

  curl_slist_free_all(headers);

  // Check for errors
  if(status != CURLE_OK)
  {
    std::string msg = curl_easy_strerror(status) + (':' + std::string(errbuf));
    throw te::common::Exception(msg);
  }

  status = curl_easy_getinfo(m_pimpl->m_curl.get(), CURLINFO_RESPONSE_CODE, &m_pimpl->m_responseCode);

  // Check for errors
  if(status != CURLE_OK)
  {
    std::string msg = curl_easy_strerror(status) + (':' + std::string(errbuf));
    throw te::common::Exception(msg);
  }
}


void te::ws::core::CurlWrapper::customRequest(const te::core::URI &uri, const std::string& request, const std::string& body, const::std::string &header)
{
  clean();

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_URL, uri.uri().c_str());

  char errbuf[CURL_ERROR_SIZE];

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_ERRORBUFFER, errbuf);
  errbuf[0] = 0;

  struct curl_slist* headers= nullptr;
  headers = curl_slist_append(headers, header.c_str());
  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_CUSTOMREQUEST, request.c_str());

  if(!body.empty())
  {
    curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_POSTFIELDS, body.c_str());
  }

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_WRITEFUNCTION, WriteResponse);

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_WRITEDATA, &m_pimpl->m_response);

  /* Perform the request, status will get the return code */
  CURLcode status = curl_easy_perform(m_pimpl->m_curl.get());

  curl_slist_free_all(headers);

  if(status != CURLE_OK)
  {
    std::string msg = curl_easy_strerror(status) + (':' + std::string(errbuf));
    throw te::common::Exception(msg);
  }

  status = curl_easy_getinfo(m_pimpl->m_curl.get(), CURLINFO_RESPONSE_CODE, &m_pimpl->m_responseCode);

  // Check for errors
  if(status != CURLE_OK)
  {
    std::string msg = curl_easy_strerror(status) + (':' + std::string(errbuf));
    throw te::common::Exception(msg);
  }
}


void te::ws::core::CurlWrapper::get(const te::core::URI &uri, std::string &buffer)
{
  clean();

  buffer.clear();

  std::string url = uri.uri();

  m_pimpl->m_bufferMutex.lock();

  curl_easy_reset(m_pimpl->m_curl.get());

  char errbuf [CURL_ERROR_SIZE];

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_ERRORBUFFER, errbuf);
  errbuf[0] = 0;

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_URL, url.c_str());

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_WRITEFUNCTION, WriteResponse);

  curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_WRITEDATA, &buffer);

  if(m_pimpl->m_method != AuthenticationMethod::NOT_AUTH)
  {
    this->addAuthParameters();
  }


  /* Perform the request, status will get the return code */
  CURLcode status = curl_easy_perform(m_pimpl->m_curl.get());

  m_pimpl->m_bufferMutex.unlock();

  m_pimpl->m_response = buffer;

  if(status != CURLE_OK)
  {
    std::string msg = curl_easy_strerror(status) + (':' + std::string(errbuf));
    throw te::common::Exception(msg);
  }

  status = curl_easy_getinfo(m_pimpl->m_curl.get(), CURLINFO_RESPONSE_CODE, &m_pimpl->m_responseCode);

  // Check for errors
  if(status != CURLE_OK)
  {
    std::string msg = curl_easy_strerror(status) + (':' + std::string(errbuf));
    throw te::common::Exception(msg);
  }
}


const long te::ws::core::CurlWrapper::responseCode() const
{
  return m_pimpl->m_responseCode;
}


const std::string& te::ws::core::CurlWrapper::response() const
{
  return m_pimpl->m_response;
}

void te::ws::core::CurlWrapper::setAuthenticationMethod(const te::ws::core::AuthenticationMethod &method)
{
  m_pimpl->m_method = method;
}

te::ws::core::AuthenticationMethod te::ws::core::CurlWrapper::getAuthenticationMethod() const
{
  return m_pimpl->m_method;
}

void te::ws::core::CurlWrapper::setUsername(const std::string &username)
{
  m_pimpl->m_username = username;
}

std::string te::ws::core::CurlWrapper::getUsername() const
{
  return m_pimpl->m_username;
}

void te::ws::core::CurlWrapper::setPassword(const std::string &password)
{
  m_pimpl->m_password = password;
}

void te::ws::core::CurlWrapper::addAuthParameters()
{
  if(m_pimpl->m_username.empty() || m_pimpl->m_password.empty())
  {
    return;
  }

  std::string userAndPassword = m_pimpl->m_username + std::string(":") + m_pimpl->m_password;

  switch(m_pimpl->m_method)
  {

  case NOT_AUTH:
    break;

  case HTTP_BASIC:
    curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_USERPWD, userAndPassword.c_str());
    break;

  case HTTP_DIGEST:
    curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
    curl_easy_setopt(m_pimpl->m_curl.get(), CURLOPT_USERPWD, userAndPassword.c_str());
    break;

  }
}
