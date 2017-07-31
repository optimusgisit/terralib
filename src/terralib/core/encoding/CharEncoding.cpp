/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled
   applications.

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
  \file terralib/core/encoding/CharEncoding.cpp

  \brief A class for handling character enconding/decoding.

  \author Matheus Cavassan Zaglia
  \author Gilberto Ribeiro de Queiroz
*/

// TerraLib
#include "CharEncoding.h"
#include "../logger/Logger.h"
#include "../Exception.h"
#include "../translator/Translator.h"

// Boost
#include <boost/assign/list_of.hpp>
#include <boost/format.hpp>
#include <boost/locale.hpp>

const std::map<te::core::EncodingType, std::string> EncodingString =
    boost::assign::map_list_of(te::core::EncodingType::UTF8, "UTF-8")
                              (te::core::EncodingType::CP1250, "CP1250")
                              (te::core::EncodingType::CP1251, "CP1251")
                              (te::core::EncodingType::CP1252, "CP1252")
                              (te::core::EncodingType::CP1253, "CP1253")
                              (te::core::EncodingType::CP1254, "CP1254")
                              (te::core::EncodingType::CP1257, "CP1257")
                              (te::core::EncodingType::LATIN1, "LATIN1");

std::string te::core::CharEncoding::toUTF8(const std::string &src,
                                           EncodingType from)
{
  return boost::locale::conv::to_utf<char>(src, EncodingString.at(from));
}

std::string te::core::CharEncoding::toUTF8(const std::string &src)
{
  boost::locale::generator g;
  g.locale_cache_enabled(true);
  try
  {
    std::locale loc = g(boost::locale::util::get_system_locale(true));
    return boost::locale::conv::to_utf<char>(src, loc);
  }
  catch(std::exception &e)
  {
    boost::format err_msg(TE_TR(
        "Could not get the system locale in order to convert fromUTF8 to the "
        "system encoding."));
    TE_LOG_ERROR(err_msg.str());
    throw Exception() << ErrorDescription(err_msg.str());
  }
}

std::string te::core::CharEncoding::fromUTF8(const std::string &src)
{
  boost::locale::generator g;
  g.locale_cache_enabled(true);
  try
  {
    std::locale loc = g(boost::locale::util::get_system_locale(true));
    return boost::locale::conv::from_utf<char>(src, loc);
  }
  catch(std::exception &e)
  {
    boost::format err_msg(TE_TR(
        "Could not get the system locale in order to convert fromUTF8 to the "
        "system encoding."));
    TE_LOG_ERROR(err_msg.str());
    throw Exception() << ErrorDescription(err_msg.str());
  }
}

std::string te::core::CharEncoding::fromUTF8(const std::string &src,
                                             EncodingType to)
{
  return boost::locale::conv::from_utf<char>(src, EncodingString.at(to));
}

std::string te::core::CharEncoding::convert(const std::string &src,
                                            EncodingType from, EncodingType to)
{
  return boost::locale::conv::between(src, EncodingString.at(to),
                                      EncodingString.at(from));
}

std::string te::core::CharEncoding::toASCII(const std::string &src)
{
  std::string decomposed;
  boost::locale::generator g;

  try
  {
    g.locale_cache_enabled(true);
    std::locale loc = g.generate("");
    decomposed = boost::locale::normalize(src, boost::locale::norm_nfd, loc);
  }
  catch(std::exception &e)
  {
    boost::format err_msg(TE_TR(
        "Could not decompose the given string to extract the ASCII "
        "characters."));
    TE_LOG_ERROR(err_msg.str());
    throw Exception() << ErrorDescription(err_msg.str());
  }

  std::string ascii_str;

  for(size_t i = 0; decomposed.size() > i; ++i)
    if(decomposed[i] >= 0 && decomposed[i] <= 127) ascii_str += decomposed[i];

  return ascii_str;
}

std::string te::core::CharEncoding::getEncodingName(EncodingType et)
{
  return EncodingString.at(et);
}

te::core::EncodingType te::core::CharEncoding::getEncodingType(
    const std::string &name)
{
  std::map<EncodingType, std::string>::const_iterator it;
  for(it = EncodingString.begin(); it != EncodingString.end(); ++it)
  {
    if(it->second == name) return it->first;
  }

  boost::format err_msg(TE_TR("There is no encoding type with the name: %1%."));
  throw Exception() << te::ErrorDescription((err_msg % name).str());
}

std::vector<std::string> te::core::CharEncoding::getEncodingList()
{
  std::vector<std::string> vec;

  std::map<EncodingType, std::string>::const_iterator it;

  for(it = EncodingString.begin(); it != EncodingString.end(); ++it)
    vec.push_back(it->second);

  return vec;
}
