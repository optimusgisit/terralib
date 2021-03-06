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
  \file terralib/antigrain/Config.h

  \brief Configuration flags for the TerraLib AGG.
 */

#ifndef __TERRALIB_AGG_INTERNAL_CONFIG_H
#define __TERRALIB_AGG_INTERNAL_CONFIG_H

/** @name Internationalization Defines
 *  Flags for TerraLib code internationalization.
 */
//@{

/*!
  \def TE_AGG_TEXT_DOMAIN

  \brief It contains the name of the text domain used in the translation of messages in TerraLib AGG module.
 */
#define TE_AGG_TEXT_DOMAIN "teagg"

/*!
  \def TE_AGG_TEXT_DOMAIN_DIR

  \brief It contains the translation catalog directory.
 */
#define TE_AGG_TEXT_DOMAIN_DIR "locale"

/*!
  \def TR_AGG

  \brief It marks a string in order to get translated. This is a special mark used in the TerraLib AGG module. 
 */
#define TR_AGG(message) TR(message, TE_AGG_TEXT_DOMAIN)

/*!
  \def TR_PLURAL_AGG

  \brief This mark can be used when you have a plural phrase to be translated. This is a special mark used in the AGG module of TerraLib. 
 */
#define TR_PLURAL_AGG(message1, message2, n) TR_PLURAL(TE_AGG_TEXT_DOMAIN, message1, message2, n)

//@}

/** @name Logger Defines
 *  Flags for building TerraLib with Log support
 */
//@{

/*!
  \def TE_AGG_LOGGER_NAME
  
  \brief This is the fully qualified TerraLib AGG Model logger name.
  
  \author Gilberto Ribeiro de Queiroz <gribeiro@dpi.inpe.br> 

  \note Any doubts, please, send us an e-mail to help improve the documentation quality!  
 */
#define TE_AGG_LOGGER_NAME "te.agg"

//@}

/** @name DLL/LIB Module
 *  Flags for building TerraLib as a DLL or as a Static Library
 */
//@{

/*!
  \def TEAGGEXPORT

  \brief You can use this macro in order to export/import classes and functions from this module.

  \note If you want to compile TerraLib as DLL in Windows, remember to insert TEAGGDLL into the project's list of defines.

  \note If you want to compile TerraLib as an Static Library under Windows, remember to insert the TEAGGSTATIC flag into the project list of defines.
 */
#ifdef WIN32

  #ifdef _MSC_VER
    #pragma warning( disable : 4251 )
    //#pragma warning( disable : 4275 )
  #endif


  #ifdef TEAGGSTATIC
    #define TEAGGEXPORT                          // Don't need to export/import... it is a static library
  #elif TEAGGDLL
    #define TEAGGEXPORT  __declspec(dllexport)   // export DLL information
  #else
    #define TEAGGEXPORT  __declspec(dllimport)   // import DLL information
  #endif 
#else
  #define TEAGGEXPORT
#endif

//@}        

#endif  // __TERRALIB_AGG_INTERNAL_CONFIG_H

