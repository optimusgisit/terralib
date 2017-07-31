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
 \file terralib/core/utils/Platform.h
 
 \brief This file is a wrapper around platform specific include files.
 
 \author Matheus Cavassan Zaglia
 \author Gilberto Ribeiro de Queiroz
 */

#ifndef __TERRALIB_CORE_UTILS_PLATFORM_H__
#define __TERRALIB_CORE_UTILS_PLATFORM_H__

// TerraLib
#include "../Config.h"

// STL
#include <string>

namespace te
{
    namespace core
    {
        
        /*!
         \brief Returns the path relative to a directory or file in the context of TerraLib.
         
         \param path A path to be searched in the TerraLib context.
         
         \return A complete path to the file or directory if it is found, otherwise returns an empty string.
         */
        TECOREEXPORT std::string FindInTerraLibPath(const std::string& path);
        
        /*!
         \brief Returns the system user home dir path.
         
         \return Returns the system user home dir path.
         
         \note An empty string will be returned on errors.
         */
        TECOREEXPORT std::string GetUserDirectory();
        
        /*!
         \brief It returns the writable folder location to store per user data.
         
         \return The folder location to store per user data.
         */
        TECOREEXPORT std::string GetAppLocalDataLocation();
        
        /*!
         \brief It returns the writable folder location to store application data applied to all users.
         
         \return The folder location to store application data applied to all users.
         */
        TECOREEXPORT std::string GetAppDataLocation();
        
    }  // end namespace core
}    // end namespace te

#endif //__TERRALIB_CORE_UTILS_PLATFORM_H__
