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
  \file terralib/wms/Config.h

  \brief Configuration flags for the TerraLib WMS module.
*/

#ifndef __TERRALIB_WMS_INTERNAL_CONFIG_H
#define __TERRALIB_WMS_INTERNAL_CONFIG_H

#define TE_WMS_MODULE_NAME "te.wms"

/*!
  \def TE_WMSL_DRIVER_IDENTIFIER

  \brief The WMS driver identifier string.
*/
#define TE_WMS_DRIVER_IDENTIFIER "WMS"

/*!
  \def TE_WMS_DEFAULT_WIDTH

  \brief It specifies the default width used on GetMap request.
*/
#define TE_WMS_DEFAULT_WIDTH "512"

/*!
  \def TE_WMS_DEFAULT_HEIGHT

  \brief It specifies the default height used on GetMap request.
*/
#define TE_WMS_DEFAULT_HEIGHT "512"

/*!
  \def TE_WMS_DEFAULT_IMAGE_FORMAT

  \brief It specifies the default image format used on GetMap request.
*/
#define TE_WMS_DEFAULT_IMAGE_FORMAT "image/png"

/*!
  \def TE_WMS_DEFAULT_SRS

  \brief It specifies the default SRS used on GetMap request.
*/
#define TE_WMS_DEFAULT_SRS "EPSG:4326"

/** @name Internationalization Defines
 *  Flags for TerraLib code internationalization.
 */
//@{

/*!
  \def TE_WMS_TEXT_DOMAIN

  \brief It contains the name of the text domain used in the translation of messages in TerraLib WMS module.
*/
#define TE_WMS_TEXT_DOMAIN "tewms"

/*!
  \def TE_WMS_TEXT_DOMAIN_DIR

  \brief It contains the translation catalog directory.
*/
#define TE_WMS_TEXT_DOMAIN_DIR "locale"

/*!
  \def TR_WMS

  \brief It marks a string in order to get translated. This is a special mark used in the TerraLib WMS module.
*/
#define TR_WMS(message) TR(message, TE_WMS_TEXT_DOMAIN)

/*!
  \def TR_WMS_PLURAL

  \brief This mark can be used when you have a plural phrase to be translated. This is a special mark used in the TerraLib WMS module.
*/
#define TR_WMS_PLURAL(message1, message2, n) TR_PLURAL(TE_WMS_TEXT_DOMAIN, message1, message2, n)

//@}

/** @name DLL/LIB Module
 *  Flags for building TerraLib as a DLL or as a Static Library
 */
//@{

/*!
  \def TEWMSEXPORT

  \brief You can use this macro in order to export/import classes and functions from this module.

  \note If you want to compile TerraLib as DLL in Windows, remember to insert TEWMSDLL into the project's list of defines.

  \note If you want to compile TerraLib as an Static Library under Windows, remember to insert the TEWMSSTATIC flag into the project list of defines.
*/

#ifdef WIN32

  #ifdef _MSC_VER
    #pragma warning( disable : 4251 )
    #pragma warning( disable : 4275 )
    #pragma warning( disable : 4290 )
  #endif

  #ifdef TEWMSSTATIC
    #define TEWMSEXPORT                          // Don't need to export/import... it is a static library
  #elif TEWMSDLL
    #define TEWMSEXPORT  __declspec(dllexport)   // export DLL information
  #else
    #define TEWMSEXPORT  __declspec(dllimport)   // import DLL information
  #endif 
#else
  #define TEWMSEXPORT
#endif

//@}

#endif  // __TERRALIB_WMS_INTERNAL_CONFIG_H
