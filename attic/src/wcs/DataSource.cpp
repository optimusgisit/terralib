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
  \file terralib/wcs/DataSource.cpp

  \brief Implementation of the data source for the WCS driver.
*/

// TerraLib
#include "../dataaccess/datasource/DataSourceTransactor.h"
#include "../common/StringUtils.h"
#include "../core/translator/Translator.h"
#include "../core/uri/URI.h"
#include "../core/utils/URI.h"
#include "DataSource.h"
#include "Exception.h"
#include "Transactor.h"
#include "Utils.h"

// GDAL
#include <gdal_priv.h>

te::da::DataSourceCapabilities te::wcs::DataSource::sm_capabilities;

te::wcs::DataSource::DataSource(const std::string& connInfo)
  : te::da::DataSource(connInfo), 
  m_isOpened(false)
{
}

te::wcs::DataSource::DataSource(const te::core::URI& uri)
  : te::da::DataSource(uri),
  m_isOpened(false)
{
}


te::wcs::DataSource::~DataSource()
{}

std::string te::wcs::DataSource::getType() const
{
  return TE_WCS_DRIVER_IDENTIFIER;
}

std::auto_ptr<te::da::DataSourceTransactor> te::wcs::DataSource::getTransactor()
{
  if(!m_isOpened)
    throw Exception(TE_TR("The data source is not opened!"));

  std::map<std::string, std::string> kvp = te::core::Expand(m_uri.query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  std::map<std::string, std::string>::const_iterator itend = kvp.end();

  return std::auto_ptr<te::da::DataSourceTransactor>(new Transactor(m_uri.uri(), kvp["COVERAGE_NAME"]));
}

void te::wcs::DataSource::open()
{
  if(m_isOpened)
    return;

  verifyConnectionInfo();

  std::map<std::string, std::string> kvp = te::core::Expand(m_uri.query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  std::map<std::string, std::string>::const_iterator itend = kvp.end();

  std::string request = BuildRequest(m_uri.uri(), kvp["COVERAGE_NAME"]);

  GDALDataset* gds = static_cast<GDALDataset*>(GDALOpen(request.c_str(), GA_ReadOnly));
  if(gds == 0)
    throw Exception(TE_TR("Error establishing connection with the informed server!"));

  GDALClose(gds);

  m_isOpened = true;
}

void te::wcs::DataSource::close()
{
  m_isOpened = false;
}

bool te::wcs::DataSource::isOpened() const
{
  return m_isOpened;
}

bool te::wcs::DataSource::isValid() const
{
  if (m_isOpened)
    return true;

  verifyConnectionInfo();

  GDALDataset* gds = static_cast<GDALDataset*>(GDALOpen(m_uri.uri().c_str(), GA_ReadOnly));
  if (gds == 0)
    return false;

  GDALClose(gds);

  return true;
}

const te::da::DataSourceCapabilities& te::wcs::DataSource::getCapabilities() const
{
  return sm_capabilities;
}

void te::wcs::DataSource::setCapabilities(const te::da::DataSourceCapabilities& capabilities)
{
  sm_capabilities = capabilities;
}

const te::da::SQLDialect* te::wcs::DataSource::getDialect() const
{
  return 0;
}

void te::wcs::DataSource::create(const std::string& /*connInfo*/)
{
  throw Exception(TE_TR("The create() method is not supported by the WCS driver!"));
}

void te::wcs::DataSource::drop(const std::string& /*connInfo*/ )
{
  throw Exception(TE_TR("The drop() method is not supported by the WCS driver!"));
}

bool te::wcs::DataSource::exists(const std::string& connInfo)
{
  if(connInfo.empty())
    return false;

  const te::core::URI aux(connInfo);
  if(!aux.isValid())
    return false;

  std::string path = aux.path();
  if(path.empty())
    return false;

  GDALDataset* gds = static_cast<GDALDataset*>(GDALOpen(path.c_str(), GA_ReadOnly));
  if(gds == 0)
    return false;

  GDALClose(gds);

  return true;
}

std::vector<std::string> te::wcs::DataSource::getDataSourceNames(const std::string& /*connInfo*/)
{
  return std::vector<std::string>();
}

std::vector<te::core::EncodingType> te::wcs::DataSource::getEncodings(const std::string& /*connInfo*/)
{
  return std::vector<te::core::EncodingType>();
}

void te::wcs::DataSource::verifyConnectionInfo() const
{
  if (!m_uri.isValid())
    throw Exception(TE_TR("The connection information is invalid!"));

  std::map<std::string, std::string> kvp = te::core::Expand(m_uri.query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  std::map<std::string, std::string>::const_iterator itend = kvp.end();

  if (m_uri.path().empty())
    throw Exception(TE_TR("The connection information is invalid. Missing the path parameter!"));

  it = kvp.find("COVERAGE_NAME");
  if (it == itend || it->second.empty())
    throw Exception(TE_TR("The connection information is invalid. Missing COVERAGE_NAME parameter!"));
}
