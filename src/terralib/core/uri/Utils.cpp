/*
  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/core/uri/Utils.cpp

  \brief  This file contains utility functions used to manipulate data from a URI.

  \author Andre Gomes de Oliveira
  \author Gilberto Ribeiro de Queiroz
*/

//Terralib
#include "../Exception.h"
#include "../translator/Translator.h"
#include "../utils/HexUtils.h"
#include "Utils.h"

// STL
#include <iterator>

// Boost
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>

std::map<std::string, std::string>
te::core::Expand(const std::string& query_str)
{
  std::map<std::string, std::string> result;

  boost::char_separator<char> kvp_sep("&");

  boost::tokenizer<boost::char_separator<char> > key_value_pairs(query_str, kvp_sep);

  boost::tokenizer<boost::char_separator<char> >::iterator ikv_pair = key_value_pairs.begin();

  while(ikv_pair != key_value_pairs.end())
  {
    std::string kv = *ikv_pair;

    boost::char_separator<char> kv_sep("=");

    boost::tokenizer<boost::char_separator<char> > key_value_pair(kv, kv_sep);

    boost::tokenizer<boost::char_separator<char> >::iterator ikv = key_value_pair.begin();

    if(ikv == key_value_pair.end())
    {
      boost::format err_msg(TE_TR("Invalid key-value pair format in query string!"));
      throw Exception() << te::ErrorDescription((err_msg).str());
    }

    std::string k = *ikv;

    ++ikv;

    if (ikv == key_value_pair.end())
    {
      boost::format err_msg(TE_TR("Invalid key-value pair format in query string!"));
      throw Exception() << te::ErrorDescription((err_msg).str());
    }

    std::string v = *ikv;

    std::string decodedValue = te::core::URIDecode(v);

    result[k] = decodedValue;

    ++ikv_pair;
  }

  return result;
}

std::string te::core::URIDecode(const std::string &srcUri)
{

  const unsigned char * pSrc = (const unsigned char *)srcUri.c_str();
  const int SRC_LEN = srcUri.length();
  const unsigned char * const SRC_END = pSrc + SRC_LEN;
  const unsigned char * const SRC_LAST_DEC = SRC_END - 2;   // last decodable '%'

  char * const pStart = new char[SRC_LEN];
  char * pEnd = pStart;

  while (pSrc < SRC_LAST_DEC)
  {
    if (*pSrc == '%')
    {
        char * hex = new char[2];
        hex[0] = *(pSrc + 1);
        hex[1] = *(pSrc + 2);

        char c = te::core::Hex2Char(hex);

        *pEnd++ = c;
        pSrc += 3;

        delete [] hex;
        continue;
    }

    *pEnd++ = *pSrc++;
  }

  // the last 2- chars
  while (pSrc < SRC_END)
    *pEnd++ = *pSrc++;

  std::string sResult(pStart, pEnd);
  delete [] pStart;
  return sResult;
}


std::string te::core::URIEncode(const std::string &srcUri)
{
  const unsigned char * pSrc = (const unsigned char *)srcUri.c_str();
  const int SRC_LEN = srcUri.length();
  unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
  unsigned char * pEnd = pStart;
  const unsigned char * const SRC_END = pSrc + SRC_LEN;

  for (; pSrc < SRC_END; ++pSrc)
  {
    const char& c = *pSrc;
    if ((48 <= c && c <= 57) ||//0-9
        (65 <= c && c <= 90) ||//abc...xyz
        (97 <= c && c <= 122) || //ABC...XYZ
        (c=='-' || c=='_' || c=='.' || c=='~'))
    {
      *pEnd++ = c;
    }
    else
    {
      // escape this char
      *pEnd++ = '%';
      char* res = new char[2];

      te::core::Char2Hex(c, res);

      *pEnd++ = res[0];
      *pEnd++ = res[1];

      delete [] res;
    }
  }

  std::string sResult((char *)pStart, (char *)pEnd);
  delete [] pStart;
  return sResult;
}
