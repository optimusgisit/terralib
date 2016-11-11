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
  \file terralib/ws/ogc/wms/dataaccess/DataSource.cpp

  \brief Data Source for WS OGC WMS

  \author Emerson Moraes
*/

#include "DataSource.h"

#include "../../../../core/translator/Translator.h"
#include "../../../../core/uri/URI.h"
#include "../../../../core/uri/Utils.h"
#include "../../../../dataaccess/datasource/DataSourceTransactor.h"
#include "../../../core/Exception.h"
#include "../../../core/CurlWrapper.h"
#include "Transactor.h"


te::da::DataSourceCapabilities te::ws::ogc::wms::da::DataSource::sm_capabilities;


te::ws::ogc::wms::da::DataSource::DataSource(const std::string& connInfo)
  : te::da::DataSource(connInfo),
  m_isOpened(false)
{
}

te::ws::ogc::wms::da::DataSource::DataSource(const te::core::URI& uri)
  : te::da::DataSource(uri),
  m_isOpened(false)
{
}

te::ws::ogc::wms::da::DataSource::~DataSource()
{
}

std::string te::ws::ogc::wms::da::DataSource::getType() const
{
  return TE_OGC_WMS_DRIVER_IDENTIFIER;
}

std::auto_ptr<te::da::DataSourceTransactor> te::ws::ogc::wms::da::DataSource::getTransactor()
{
  if(!m_isOpened)
  {
    throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("The data source is not opened!"));
  }

  return std::auto_ptr<te::da::DataSourceTransactor>(new Transactor(m_wms));
}

void te::ws::ogc::wms::da::DataSource::open()
{
  if(m_isOpened)
    return;

  verifyConnectionInfo();

  try
  {
    std::map<std::string, std::string> kvp = te::core::Expand(m_uri.query());

    m_wms = std::shared_ptr<te::ws::ogc::WMSClient>(new te::ws::ogc::WMSClient(kvp["USERDATADIR"], kvp["URI"], kvp["VERSION"]));

    m_wms->updateCapabilities();
  }
  catch(const te::core::Exception& e)
  {
    throw;
  }
  catch(...)
  {
    throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("Unknow error!"));
  }

  m_isOpened = true;
}

void te::ws::ogc::wms::da::DataSource::close()
{
  m_isOpened = false;
}

bool te::ws::ogc::wms::da::DataSource::isOpened() const
{
  return m_isOpened;
}

bool te::ws::ogc::wms::da::DataSource::isValid() const
{
  if(m_isOpened)
    return true;

  try
  {
    verifyConnectionInfo();

    std::map<std::string, std::string> kvp = te::core::Expand(m_uri.query());

    te::ws::ogc::WMSClient wms(kvp["USERDATADIR"], kvp["URI"], kvp["VERSION"]);

    wms.updateCapabilities();
  }
  catch(...)
  {
    return false;
  }

  return true;
}

const te::da::DataSourceCapabilities& te::ws::ogc::wms::da::DataSource::getCapabilities() const
{
  return sm_capabilities;
}

void te::ws::ogc::wms::da::DataSource::setCapabilities(const te::da::DataSourceCapabilities &capabilities)
{
  sm_capabilities = capabilities;
}

const te::da::SQLDialect* te::ws::ogc::wms::da::DataSource::getDialect() const
{
  return 0;
}


te::ws::ogc::wms::WMSGetMapResponse te::ws::ogc::wms::da::DataSource::getMap(const te::ws::ogc::wms::WMSGetMapRequest &request)
{
  if(!isOpened())
    throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("The WMS DataSource is not opened."));

  return m_wms->getMap(request);
}

void te::ws::ogc::wms::da::DataSource::create(const std::string& /*connInfo*/)
{
  throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("The create() method is not supported by the WMS driver!"));
}

void te::ws::ogc::wms::da::DataSource::drop(const std::string& /*connInfo*/)
{
  throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("The drop() method is not supported by the WMS driver!"));
}

bool te::ws::ogc::wms::da::DataSource::exists(const std::string& connInfo)
{
  if (connInfo.empty())
    return false;

  const te::core::URI aux(connInfo);
  if (!aux.isValid())
    return false;

  std::map<std::string, std::string> kvp = te::core::Expand(m_uri.query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  std::map<std::string, std::string>::const_iterator itend = kvp.end();
  std::string usrDataDir, version, uri;

  it = kvp.find("USERDATADIR");
  if (it == itend || it->second.empty())
    return false;
  else
    usrDataDir = it->second;

  it = kvp.find("VERSION");
  if (it == itend || it->second.empty())
    return false;
  else
    version = it->second;

  it = kvp.find("URI");
  if (it == itend || it->second.empty())
    return false;
  else
    uri = it->second;



  try
  {
    verifyConnectionInfo();

    te::ws::ogc::WMSClient wms(usrDataDir, uri, version);

    wms.updateCapabilities();
  }
  catch(...)
  {
    return false;
  }

  return true;
}

std::vector<std::string> te::ws::ogc::wms::da::DataSource::getDataSourceNames(const std::string& /*connInfo*/)
{
  return std::vector<std::string>();
}

std::vector<te::core::EncodingType> te::ws::ogc::wms::da::DataSource::getEncodings(const std::string& /*connInfo*/)
{
  return std::vector<te::core::EncodingType>();
}

void te::ws::ogc::wms::da::DataSource::verifyConnectionInfo() const
{
  if(!m_uri.isValid())
    throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("The connection information is invalid!"));

  std::map<std::string, std::string> kvp = te::core::Expand(m_uri.query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  std::map<std::string, std::string>::const_iterator itend = kvp.end();

  it = kvp.find("URI");
  if (it == itend || it->second.empty())
    throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("The connection information is invalid. Missing URI parameter!"));

  it = kvp.find("VERSION");
  if (it == itend || it->second.empty())
    throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("The connection information is invalid. Missing VERSION parameter!"));

  it = kvp.find("USERDATADIR");
  if (it == itend || it->second.empty())
    throw te::ws::core::Exception() << te::ErrorDescription(TE_TR("The connection information is invalid. Missing USERDATADIR parameter!"));
}
