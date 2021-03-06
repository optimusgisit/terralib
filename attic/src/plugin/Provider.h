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
  \file terralib/plugin/Provider.h

  \brief This struct can be used to describe a given plugin provider.
*/

#ifndef __TERRALIB_PLUGIN_INTERNAL_PROVIDER_H
#define __TERRALIB_PLUGIN_INTERNAL_PROVIDER_H

// STL
#include <string>

namespace te
{
  namespace plugin
  {
    /*!
      \struct Provider

      \brief This struct can be used to describe a given plugin provider.

      \ingroup plugin

      \sa Plugin
    */
    struct Provider
    {
      std::string m_name;   //!< Provider name: may be a person or a company.
      std::string m_site;   //!< The provider home page.
      std::string m_email;  //!< The provider contact e-mail.
    };

  } // end namespace plugin
}   // end namespace te

#endif  // __TERRALIB_PLUGIN_INTERNAL_PROVIDER_H

