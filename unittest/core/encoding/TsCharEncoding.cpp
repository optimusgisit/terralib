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
  \file terralib/unittest/core/encoding/TsCharEncoding.cpp

  \brief A test suit for the TerraLib Core Encoding Module.

  \author Matheus Cavassan Zaglia.
 */

// STL
#include <string>
#include <iostream>

// TerraLib
#include <terralib/core/encoding/CharEncoding.h>

// Boost
#include <boost/test/unit_test.hpp>

#include <cstring>

#if TE_PLATFORM == TE_PLATFORMCODE_MSWINDOWS
#include <Windows.h>
#endif

BOOST_AUTO_TEST_CASE(encoding_test)
{
  std::string utf8string = "\x4C\xC3\xB5\x72\xC3\xA9\x6D\x20\x69\x70\x73\x75\x6D\x20\x64\x6F\x6C\xC3\xB5\x72\x20\x73\xC3\xAD\x74\x20\xC3\xA3\x6D\x65\x74\x2C\x20\xC3\xA1\x65\x71\x75\x65\x20\x65\x73\x73\x65\x6E\x74\x20\x76\x69\x74\x75\x70\x65\x72\x61\x74\x61\x20\x6E\x65\x20\x68\xC3\xA2\x73\x2C\x20\x6D\xC3\xAA\x6C\x20\x69\x6E\x20\x69\x75\x64\x69\x63\xC3\xA3\x62\x69\x74\x20\x69\x72\xC3\xA0\x63\x75\x6E\x64\x69\xC3\xA0\x2C\x20\xC3\xA7\x75\x20\x6D\x65\x6E\x61\x6E\x64\x72\x69\x20\x64\x65\x73\xC3\xA9\x72\x75\x69\x73\x73\x65\x20\x71\x75\x6F\x21\x20\x56\x65\x6C\x20\x65\x74\x20\x63\x69\x76\x69\x62\x75\x73\x20\xC3\xA1\x72\x67\x75\x6D\x65\x6E\x74\x75\x6D\x2E\x20\x42\xC3\xB5\x6E\x6F\x72\x75\x6D\x20\x73\x65\x6E\x73\xC3\xA9\x72\x69\x74\x20\xC3\xA9\x66\x66\xC3\xAD\x63\x69\x65\x6E\x64\x69\x20\x6E\xC3\xA0\x6D\x20\x63\xC3\xBA\x2E\x20\x50\xC3\xB5\x73\x73\x65\x20\x76\x69\x76\xC3\xAA\x6E\x64\xC3\xBA\x6D\x20\xC3\xA1\x6E\x20\x76\x69\x6D\x2C\x20\x64\x75\x6F\x20\x64\x6F\x6C\xC3\xB5\x72\x20\x64\x65\x63\x6F\x72\x65\x20\x64\x69\x73\x70\xC3\xBA\x74\xC3\xA0\x6E\x64\xC3\xB4\x20\x65\x74\x2E\x20\x48\xC3\xAD\x73\x20\x63\x75\x20\xC3\xA3\x67\xC3\xA3\x6D\x20\x66\x75\x67\x69\x74\x20\x65\x72\x69\x70\xC3\xBA\x69\x74\x2C\x20\x74\x69\x6D\x65\xC3\xA0\x6D\x20\x72\x61\x74\x69\xC3\xB5\x6E\x69\x62\x75\x73\x20\x73\x69\x74\x20\xC3\xA3\x74\x2C\x20\xC3\xAA\x74\x20\xC3\xA9\x75\x6D\x20\x70\x72\x6F\x64\x65\x73\x73\x65\x74\x20\xC3\xA3\x62\x68\x6F\x72\x72\xC3\xA9\x61\x6E\x74\x20\x63\xC3\xB5\x6E\x73\x74\x69\x74\xC3\xBA\x61\x6D\x3F\x20\x45\x70\x69\x63\x75\x72\x69\x20\x70\xC3\xA9\x72\x74\x69\x6E\xC3\xA2\x78\x20\x73\xC3\xAA\x64\x20\xC3\xA2\x74\x2C\x20\xC3\xAA\x78\x20\x76\x69\x78\x20\x74\xC3\xAD\x6E\x63\x69\x64\x75\x6E\x74\x20\x64\xC3\xAD\x73\x73\x65\x6E\x74\xC3\xAD\x61\x73\x20\x76\x6F\x6C\x75\x70\x74\x61\x74\x69\x62\x75\x73\x2E";



  BOOST_CHECK(utf8string != te::core::CharEncoding::fromUTF8(utf8string, te::core::EncodingType::LATIN1));

  BOOST_CHECK(utf8string.size() != te::core::CharEncoding::fromUTF8(utf8string, te::core::EncodingType::LATIN1).size());

  BOOST_CHECK(te::core::EncodingType::UTF8 == te::core::CharEncoding::getEncodingType("UTF8"));

  BOOST_CHECK("UTF8" == te::core::CharEncoding::getEncodingName(te::core::EncodingType::UTF8));

  return ;
}
