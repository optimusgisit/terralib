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
  \file terralib/unittest/core/uri/TsURI.cpp

  \brief A test suit for the Core URI API.

  \author Vinicius campanha
 */

// STL
#include <iostream>

// TerraLib
#include <terralib/core/uri/URI.h>

// Boost
#include <boost/test/unit_test.hpp>

void printURI(te::core::URI& uri)
{
  std::cout << std::endl;
  std::cout << "URI: " << uri.uri() << std::endl;
  std::cout << "Scheme: " << uri.scheme() << std::endl;
  std::cout << "User Info: " << uri.userInfo() << std::endl;
  std::cout << "Host: " << uri.host() << std::endl;
  std::cout << "Port: " << uri.port() << std::endl;
  std::cout << "Path: " << uri.path() << std::endl;
  std::cout << "Query: " << uri.query() << std::endl;
  std::cout << "Fragment: " << uri.fragment() << std::endl;
}

BOOST_AUTO_TEST_SUITE( test_case )

BOOST_AUTO_TEST_CASE( test_full_address )
{
  std::string address("http://www.dpi.inpe.br:80/terralib5/wiki/doku.php?id=wiki:documentation:devguide#modules");
  te::core::URI uri(address);

  //  BOOST_CHECK(uri.is_absolute());
  //  BOOST_CHECK(!uri.is_opaque());

  printURI(uri);

  BOOST_CHECK(uri.scheme() == "http");
  BOOST_CHECK(uri.userInfo() == "");
  BOOST_CHECK(uri.host() == "www.dpi.inpe.br");
  BOOST_CHECK(uri.port() == "80");
  BOOST_CHECK(uri.path() == "/terralib5/wiki/doku.php");
  BOOST_CHECK(uri.query() == "id=wiki:documentation:devguide");
  BOOST_CHECK(uri.fragment() == "modules");

  return;
}

BOOST_AUTO_TEST_CASE( test_full_address2 )
{
  std::string address("http://sedac.ciesin.columbia.edu/geoserver/wcs?service=WCS&request=GetCapabilities&version=2.0.1");
  te::core::URI uri(address);

  //  BOOST_CHECK(uri.is_absolute());
  //  BOOST_CHECK(!uri.is_opaque());
  printURI(uri);

  BOOST_CHECK(uri.scheme() == "http");
  BOOST_CHECK(uri.userInfo() == "");
  BOOST_CHECK(uri.host() == "sedac.ciesin.columbia.edu");
  BOOST_CHECK(uri.port() == "");
  BOOST_CHECK(uri.path() == "/geoserver/wcs");
  BOOST_CHECK(uri.query() == "service=WCS&request=GetCapabilities&version=2.0.1");
  BOOST_CHECK(uri.fragment() == "");

  return;
}

BOOST_AUTO_TEST_CASE( test_ftp_address )
{
  std::string address("ftp://ftp.ftp.inpe.br:21/path/");
  te::core::URI uri(address);

  //  BOOST_CHECK(uri.is_absolute());
  //  BOOST_CHECK(!uri.is_opaque());
  printURI(uri);

  BOOST_CHECK(uri.scheme() == "ftp");
  BOOST_CHECK(uri.userInfo() == "");
  BOOST_CHECK(uri.host() == "ftp.ftp.inpe.br");
  BOOST_CHECK(uri.port() == "21");
  BOOST_CHECK(uri.path() == "/path/");
  BOOST_CHECK(uri.query() == "");
  BOOST_CHECK(uri.fragment() == "");

  return;
}

BOOST_AUTO_TEST_CASE( test_authority_address )
{
  std::string address("ftp://user:password@ftp.ftp.inpe.br:21/path/");
  te::core::URI uri(address);

  //  BOOST_CHECK(uri.is_absolute());
  //  BOOST_CHECK(!uri.is_opaque());
  printURI(uri);

  BOOST_CHECK(uri.scheme() == "ftp");
  BOOST_CHECK(uri.userInfo() == "user:password");
  BOOST_CHECK(uri.host() == "ftp.ftp.inpe.br");
  BOOST_CHECK(uri.port() == "21");
  BOOST_CHECK(uri.path() == "/path/");
  BOOST_CHECK(uri.query() == "");
  BOOST_CHECK(uri.fragment() == "");

  return;
}

BOOST_AUTO_TEST_CASE( test_opaque_full_address )
{
  std::string address("xmpp:example-node@example.com?message;subject=Hello%20World");
  te::core::URI uri(address);

  //  BOOST_CHECK(uri.is_absolute());
  //  BOOST_CHECK(!uri.is_opaque());
  printURI(uri);

  BOOST_CHECK(uri.scheme() == "xmpp");
  BOOST_CHECK(uri.userInfo() == "");
  BOOST_CHECK(uri.host() == "");
  BOOST_CHECK(uri.port() == "");
  BOOST_CHECK(uri.path() == "example-node@example.com");
  BOOST_CHECK(uri.query() == "message;subject=Hello%20World");
  BOOST_CHECK(uri.fragment() == "");

  return;
}

BOOST_AUTO_TEST_CASE( test_partial_adress )
{
  std::string address("http://www.dpi.inpe.br:80");

  te::core::URI uri(address);

  //  BOOST_CHECK(uri.is_absolute());
  //  BOOST_CHECK(!uri.is_opaque());
  printURI(uri);

  BOOST_CHECK(uri.scheme() == "http");
  BOOST_CHECK(uri.userInfo() == "");
  BOOST_CHECK(uri.host() == "www.dpi.inpe.br");
  BOOST_CHECK(uri.port() == "80");
  BOOST_CHECK(uri.path() == "");
  BOOST_CHECK(uri.query() == "");
  BOOST_CHECK(uri.fragment() == "");
}

BOOST_AUTO_TEST_CASE( test_partial_adress2 )
{
  std::string address("http://www.dpi.inpe.br");

  te::core::URI uri(address);

  //  BOOST_CHECK(uri.is_absolute());
  //  BOOST_CHECK(!uri.is_opaque());
  printURI(uri);

  BOOST_CHECK(uri.scheme() == "http");
  BOOST_CHECK(uri.userInfo() == "");
  BOOST_CHECK(uri.host() == "www.dpi.inpe.br");
  BOOST_CHECK(uri.port() == "");
  BOOST_CHECK(uri.path() == "");
  BOOST_CHECK(uri.query() == "");
  BOOST_CHECK(uri.fragment() == "");
}

BOOST_AUTO_TEST_CASE( test_pgsql )
{
  std::string address("pgsql://gribeiro:secreto@atlas.dpi.inpe.br:5433/bdqueimadas?client_encoding=UTF8max_connections=20");

  te::core::URI uri(address);

  //  BOOST_CHECK(uri.is_absolute());
  //  BOOST_CHECK(!uri.is_opaque());
  printURI(uri);

  BOOST_CHECK(uri.scheme() == "pgsql");
  BOOST_CHECK(uri.userInfo() == "gribeiro:secreto");
  BOOST_CHECK(uri.host() == "atlas.dpi.inpe.br");
  BOOST_CHECK(uri.port() == "5433");
  BOOST_CHECK(uri.path() == "/bdqueimadas");
  BOOST_CHECK(uri.query() == "client_encoding=UTF8max_connections=20");
  BOOST_CHECK(uri.fragment() == "");
}

BOOST_AUTO_TEST_CASE( test_2 )
{
  std::string address("http://chronos.dpi.inpe.br/wfs");

  te::core::URI uri(address);

  //  BOOST_CHECK(uri.is_absolute());
  //  BOOST_CHECK(!uri.is_opaque());
  printURI(uri);

  BOOST_CHECK(uri.scheme() == "http");
  BOOST_CHECK(uri.userInfo() == "");
  BOOST_CHECK(uri.host() == "chronos.dpi.inpe.br");
  BOOST_CHECK(uri.port() == "");
  BOOST_CHECK(uri.path() == "/wfs");
  BOOST_CHECK(uri.query() == "");
  BOOST_CHECK(uri.fragment() == "");
}

BOOST_AUTO_TEST_CASE( test_3 )
{
  std::string address("http://chronos.dpi.inpe.br/wms");

  te::core::URI uri(address);

  //  BOOST_CHECK(uri.is_absolute());
  //  BOOST_CHECK(!uri.is_opaque());
  printURI(uri);

  BOOST_CHECK(uri.scheme() == "http");
  BOOST_CHECK(uri.userInfo() == "");
  BOOST_CHECK(uri.host() == "chronos.dpi.inpe.br");
  BOOST_CHECK(uri.port() == "");
  BOOST_CHECK(uri.path() == "/wms");
  BOOST_CHECK(uri.query() == "");
  BOOST_CHECK(uri.fragment() == "");
}

BOOST_AUTO_TEST_CASE( test_4 )
{
  std::string address("http://chronos.dpi.inpe.br/wfs");

  te::core::URI uri(address);

  //  BOOST_CHECK(uri.is_absolute());
  //  BOOST_CHECK(!uri.is_opaque());
  printURI(uri);

  BOOST_CHECK(uri.scheme() == "http");
  BOOST_CHECK(uri.userInfo() == "");
  BOOST_CHECK(uri.host() == "chronos.dpi.inpe.br");
  BOOST_CHECK(uri.port() == "");
  BOOST_CHECK(uri.path() == "/wfs");
  BOOST_CHECK(uri.query() == "");
  BOOST_CHECK(uri.fragment() == "");
}

BOOST_AUTO_TEST_CASE( test_5 )
{
  std::string address("file:///home/gribeiro/data");

  te::core::URI uri(address);

  //  BOOST_CHECK(uri.is_absolute());
  //  BOOST_CHECK(!uri.is_opaque());
  printURI(uri);

  BOOST_CHECK(uri.scheme() == "file");
  BOOST_CHECK(uri.userInfo() == "");
  BOOST_CHECK(uri.host() == "");
  BOOST_CHECK(uri.port() == "");
  BOOST_CHECK(uri.path() == "/home/gribeiro/data");
  BOOST_CHECK(uri.query() == "");
  BOOST_CHECK(uri.fragment() == "");
}

BOOST_AUTO_TEST_CASE( test_6 )
{
  std::string address("file:///home/gribeiro/data/arq.shp");

  te::core::URI uri(address);

  //  BOOST_CHECK(uri.is_absolute());
  //  BOOST_CHECK(!uri.is_opaque());
  printURI(uri);

  BOOST_CHECK(uri.scheme() == "file");
  BOOST_CHECK(uri.userInfo() == "");
  BOOST_CHECK(uri.host() == "");
  BOOST_CHECK(uri.port() == "");
  BOOST_CHECK(uri.path() == "/home/gribeiro/data/arq.shp");
  BOOST_CHECK(uri.query() == "");
  BOOST_CHECK(uri.fragment() == "");
}

BOOST_AUTO_TEST_SUITE_END()
