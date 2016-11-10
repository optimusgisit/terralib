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
  \file terralib/wcs/Module.h

  \brief The TerraLib WCS driver as a plugin.
*/

#ifndef __TERRALIB_WCS_INTERNAL_MODULE_H
#define __TERRALIB_WCS_INTERNAL_MODULE_H

// TerraLib
#include "../core/plugin/CppPlugin.h"
#include "Config.h"

namespace te
{
  namespace wcs
  {
    /*!
      \class Module
     
      brief The TerraLib WCS driver as a plugin.
    */
    class Module : public te::core::CppPlugin
    {
    public:
      
      /*!
       \brief Plugin constructor.

       \param pInfo Basic information provided to initialize this module through the plugin API.
      */
      Module(const te::core::PluginInfo& pluginInfo);
      
      /*! \brief Destructor. */
      ~Module();
      
      //@}
      
      /** @name Re-Implmentation from Plugin Interface
       *  Re-Implementation from Plugin Interface.
       */
      //@{
      
      /*!
        \brief This method will be called by TerraLib to startup some plugin's functionality.
       
        \exception Exception It throws an exception if the plugin can not be started.
      */
      void startup();
      
      /*!
        \brief This method will be called by TerraLib to shutdown plugin's functionality.

        \exception Exception It throws an exception if the plugin can not be shutdown.
      */
      void shutdown();

      //@}
    };

  } // end namespace wcs
}   // end namespace te

#endif  // __TERRALIB_WCS_INTERNAL_MODULE_H
