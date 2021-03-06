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
  \file terralib/terralib4/Globals.h
   
  \brief An static class with global definitions.
*/

#ifndef __TERRALIB_TERRALIB4_INTERNAL_GLOBALS_H
#define __TERRALIB_TERRALIB4_INTERNAL_GLOBALS_H

// TerraLib
#include "../common/Static.h"
#include "Config.h"

// STL
#include <string>

namespace te
{
  // Forward declaration
  namespace da
  { 
    class DataSourceCapabilities;
    class SQLDialect;
  }
}   // end namespace te

namespace terralib4
{
  /*!
    \class Globals

    \brief An static class with global definitions.
  */
  class TETERRALIB4EXPORT Globals : public te::common::Static
  {
    public:

      static const std::string sm_driverIdentifier;  //!< The Terralib 4 driver identifier.

      static te::da::SQLDialect* sm_queryDialect;                //!< The query dialect supported by Terralib 4 driver.
      static te::da::DataSourceCapabilities* sm_capabilities;    //!< The query dialect supported by Terralib 4 driver.

  };
} // end namespace terralib4

#endif  // __TERRALIB_TERRALIB4_INTERNAL_GLOBALS_H

