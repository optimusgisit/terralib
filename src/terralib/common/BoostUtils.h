/*  Copyright (C) 2001-2009 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/common/BoostUtils.h

  \brief This file contains several utility functions for dealing with Boost containers and algorithms.
*/

#ifndef __TERRALIB_COMMON_INTERNAL_BOOSTUTILS_H
#define __TERRALIB_COMMON_INTERNAL_BOOSTUTILS_H

// TerraLib
#include "Config.h"

// STL
#include <map>
#include <string>

// Boost
#include <boost/unordered_map.hpp>
#include <boost/property_tree/ptree_fwd.hpp>

namespace te
{
  namespace common
  {
    /*!
      \brief This function can be applied to a map of pointers. It will delete each pointer in the map.

      \param m The map of pointers to be cleaned.

      \note It will not clear the map at the end of the cleanup.
    */
    template<class K, class V> inline void FreeContents(boost::unordered_map<K, V*>& m)
    {
      typename boost::unordered_map<K, V*>::const_iterator it = m.begin();
      typename boost::unordered_map<K, V*>::const_iterator itend = m.end();

      while(it != itend)
      {
        delete it->second;
        ++it;
      }
    }

    /*!
      \brief Converts a property tree node into a std::map<std::string, std::string>.

      \param p    A node that can be converted to a std::map<std::string, std::string>.
      \param dict The dictionary to output the key-value pairs.
    */
    TECOMMONEXPORT void Convert(const boost::property_tree::ptree& p, std::map<std::string, std::string>& dict);

  } // end namespace common
}   // end namespace te

#endif  // __TERRALIB_COMMON_INTERNAL_BOOSTUTILS_H
