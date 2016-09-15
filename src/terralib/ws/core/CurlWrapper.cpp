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


te::ws::core::CurlWrapper::CurlWrapper()
{
  m_curl = std::shared_ptr<CURL>(curl_easy_init(), curl_easy_cleanup);
}


te::ws::core::CurlWrapper::~CurlWrapper()
{
}


size_t WriteFileCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
  std::FILE *writehere = (std::FILE *)data;
  return fwrite(ptr, size, nmemb, writehere);
}


int DownloadProgress(void *p,
                     curl_off_t dltotal, curl_off_t dlnow,
                     curl_off_t ultotal, curl_off_t ulnow)
{
  te::ws::core::CurlProgress* progress = (te::ws::core::CurlProgress*) p;

  std::stringstream ss;

  progress->m_task->setTotalSteps((int) dlnow);
  progress->m_task->setCurrentStep((int) dlnow);

  ss << dlnow;

  std::string::size_type sz;
  long numBytes = std::stol(ss.str(), &sz);

  long numKBytes = numBytes / 1000;

  progress->m_task->setMessage(progress->m_baseMessage + " " + std::to_string(numKBytes) + TE_TR(" KBytes received."));

  return 0;
}


void te::ws::core::CurlWrapper::downloadFile(const std::string& url, const std::string& filePath) const
{

  std::FILE* file = std::fopen(te::core::CharEncoding::fromUTF8(filePath).c_str(), "wb");

  if(!file)
    throw te::common::Exception(TE_TR("Could not open file to write!"));

  downloadFile(url, file);

  if (!file)
    throw te::common::Exception(TE_TR("Error at received file!"));
  else
    std::fclose(file);
}


void te::ws::core::CurlWrapper::downloadFile(const std::string &url, std::FILE* file) const
{
  curl_easy_reset(m_curl.get());

  curl_easy_setopt(m_curl.get(), CURLOPT_URL, url.c_str());

  // Get data to be written
  curl_easy_setopt(m_curl.get(), CURLOPT_WRITEFUNCTION, WriteFileCallback);

  // Set a pointer to our file
  curl_easy_setopt(m_curl.get(), CURLOPT_WRITEDATA, file);

  std::shared_ptr<te::common::TaskProgress> task;

  task.reset( new te::common::TaskProgress(m_taskMessage,
                                           te::common::TaskProgress::UNDEFINED, 100 ) );

  CurlProgress progress;

  progress.m_curl = this->m_curl;
  progress.m_task = task;
  progress.m_baseMessage = m_taskMessage;

  curl_easy_setopt(m_curl.get(), CURLOPT_XFERINFOFUNCTION, DownloadProgress);

  curl_easy_setopt(m_curl.get(), CURLOPT_XFERINFODATA, &progress);

  curl_easy_setopt(m_curl.get(), CURLOPT_NOPROGRESS, 0L);

  /* Perform the request, status will get the return code */
  CURLcode status = curl_easy_perform(m_curl.get());

  // Check for errors
  if(status != CURLE_OK)
    throw te::common::Exception(curl_easy_strerror(status));
}


std::string te::ws::core::CurlWrapper::getTaskMessage() const
{
  return m_taskMessage;
}

void te::ws::core::CurlWrapper::setTaskMessage(const std::string &taskMessage)
{
  m_taskMessage = taskMessage;
}


size_t read_stream_callback(char *buffer, size_t size, size_t nitems, void *instream)
{
  std::fstream* stream = static_cast<std::fstream*>(instream);
  std::streampos begin,end;

  if(!stream->is_open())
  {
    return -1;
  }

  stream->seekg (0, std::ios::end);
  end = stream->tellg();
  stream->seekg (0, std::ios::beg);
  begin = stream->tellg();

  long long int ret = end - begin;

  stream->get(buffer, ret + 1);

  return ret;
}


void te::ws::core::CurlWrapper::post(const te::core::URI &uri, const std::string &postFields, const::std::string &header) const
{
  curl_easy_reset(m_curl.get());

  curl_easy_setopt(m_curl.get(), CURLOPT_URL, uri.uri().c_str());

  struct curl_slist* headers= nullptr;
  headers = curl_slist_append(headers, header.c_str());
  curl_easy_setopt(m_curl.get(), CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(m_curl.get(), CURLOPT_POSTFIELDS, postFields.c_str());

  /* Perform the request, status will get the return code */
  CURLcode status = curl_easy_perform(m_curl.get());

  curl_slist_free_all(headers);

  // Check for errors
  if(status != CURLE_OK)
    throw te::common::Exception(curl_easy_strerror(status));
}


void te::ws::core::CurlWrapper::put(const te::core::URI &uri, const std::string &content, const std::string &header) const
{
  std::fstream* stream = new std::fstream("file.txt");
  std::streampos begin,end;

  if(!stream->is_open())
  {
    throw te::common::Exception(TE_TR("Can't open the file!."));
  }

  *stream << content;
  stream->flush();

  stream->seekg (0, std::ios::end);
  end = stream->tellg();
  stream->seekg (0, std::ios::beg);
  begin = stream->tellg();

  put(uri, stream, (end-begin),header);
}


void te::ws::core::CurlWrapper::put(const te::core::URI &uri, const void *content, size_t size,  const::std::string &header) const
{
  curl_easy_reset(m_curl.get());

  curl_easy_setopt(m_curl.get(), CURLOPT_URL, uri.uri().c_str());

  struct curl_slist* headers= nullptr;
  headers = curl_slist_append(headers, header.c_str());
  curl_easy_setopt(m_curl.get(), CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(m_curl.get(), CURLOPT_UPLOAD, true);

  curl_easy_setopt(m_curl.get(), CURLOPT_READFUNCTION, read_stream_callback);

  curl_easy_setopt(m_curl.get(), CURLOPT_READDATA, content);

  curl_easy_setopt(m_curl.get(), CURLOPT_INFILESIZE_LARGE, (curl_off_t)size);

  /* Perform the request, status will get the return code */
  CURLcode status = curl_easy_perform(m_curl.get());

  curl_slist_free_all(headers);

  delete content;

  // Check for errors
  if(status != CURLE_OK)
    throw te::common::Exception(curl_easy_strerror(status));
}


void te::ws::core::CurlWrapper::putFile(const te::core::URI &uri, const std::string &filePath, const std::string &header) const
{
  std::fstream *stream = new std::fstream(filePath);
  std::streampos begin,end;

  if(!stream->is_open())
  {
    throw te::common::Exception(TE_TR("Can't open the file!."));
  }

  stream->seekg (0, std::ios::end);
  end = stream->tellg();
  stream->seekg (0, std::ios::beg);
  begin = stream->tellg();

  long long int size = end-begin;

  putFile(uri, stream, size,header);
}


void te::ws::core::CurlWrapper::putFile(const te::core::URI &uri, const void *file, const size_t &size,  const::std::string &header) const
{
  curl_easy_reset(m_curl.get());

  curl_easy_setopt(m_curl.get(), CURLOPT_URL, uri.uri().c_str());

  struct curl_slist* headers= nullptr;
  headers = curl_slist_append(headers, header.c_str());
  curl_easy_setopt(m_curl.get(), CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(m_curl.get(), CURLOPT_UPLOAD, 1L);

  curl_easy_setopt(m_curl.get(), CURLOPT_READFUNCTION, read_stream_callback);

  curl_easy_setopt(m_curl.get(), CURLOPT_READDATA, file);

  curl_easy_setopt(m_curl.get(), CURLOPT_INFILESIZE_LARGE, (curl_off_t)size);

  /* Perform the request, status will get the return code */
  CURLcode status = curl_easy_perform(m_curl.get());

  curl_slist_free_all(headers);

  // Check for errors
  if(status != CURLE_OK)
    throw te::common::Exception(curl_easy_strerror(status));
}

