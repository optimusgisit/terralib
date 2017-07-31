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
 \file terralib/core/utils/Platform.cpp
 
 \brief This file contains several utility functions when dealing with Linux specific API.
 
 \author Matheus Cavassan Zaglia
 \author Gilberto Ribeiro de Queiroz
 */

// TerraLib
#include "Platform.h"
#include "../filesystem/FileSystem.h"
#include "../../BuildConfig.h"
#include "../../Defines.h"

// Boost
#include <boost/filesystem.hpp>

#if TE_PLATFORM == TE_PLATFORMCODE_MSWINDOWS
// Windows
#include <Shlobj.h>
#include <comutil.h>
#include <winerror.h>
#pragma comment(lib, "comsuppw")
#endif

std::string te::core::FindInTerraLibPath(const std::string& path)
{
    // 1st: look in the neighborhood of the executable
    boost::filesystem::path tl_path = te::core::FileSystem::executableDirectory();
    
    boost::filesystem::path eval_path = tl_path / path;
    
    if(te::core::FileSystem::exists(eval_path.string()))
        return eval_path.string();
    
    // check for one path above
    tl_path /= "..";
    eval_path = tl_path / path;
    
    if(te::core::FileSystem::exists(eval_path.string()))
        return eval_path.string();
    
#ifdef TERRALIB_BUILD_AS_DEV
    // look into the codebase path
    tl_path = TERRALIB_CODEBASE_PATH;
    
    eval_path = tl_path / path;
    
    if(te::core::FileSystem::exists(eval_path.string()))
        return eval_path.string();
    
    // look into the build path
    tl_path = TERRALIB_BUILD_PATH;
    
    eval_path = tl_path / path;
    
    if(te::core::FileSystem::exists(eval_path.string()))
        return eval_path.string();
    
#else
    
#if !defined(TERRALIB_BUILD_AS_BUNDLE) || (TE_PLATFORM == TE_PLATFORMCODE_LINUX)
    // look into the install path
    tl_path = TERRALIB_INSTALL_PREFIX_PATH;
    
    eval_path = tl_path / path;
    
    if(te::core::FileSystem::exists(eval_path.string()))
        return eval_path.string();
    
#endif
    
    // look for an environment variable defined by macro TERRALIB_DIR_VAR_NAME
    const char* te_env = getenv(TERRALIB_DIR_VAR_NAME);
    
    if(te_env != 0)
    {
        tl_path = te_env;
        
        eval_path = tl_path / path;
        
        if(te::core::FileSystem::exists(eval_path.string()))
            return eval_path.string();
    }
#endif
    
    return "";
}

std::string te::core::GetUserDirectory()
{
    std::string returnString;
    
#if (TE_PLATFORM == TE_PLATFORMCODE_LINUX) || (TE_PLATFORM == TE_PLATFORMCODE_APPLE)
    char* homePtr = getenv( "HOME" );
    if( homePtr )
    {
        returnString = std::string( homePtr );
    }
#elif TE_PLATFORM == TE_PLATFORMCODE_MSWINDOWS
    char* homePathPtr = getenv( "HOMEPATH" );
    char* homeDrivePtr = getenv( "HOMEDRIVE" );
    
    if( ( homePathPtr!= 0 ) && ( homeDrivePtr != 0 ) )
    {
        returnString = std::string( homeDrivePtr ) +
        std::string( homePathPtr );
    }
#else
#error "Platform not supported! Contact TerraLib Team"
#endif
    
    return returnString;
}


std::string te::core::GetAppLocalDataLocation()
{
    std::string dataPath;
#if TE_PLATFORM == TE_PLATFORMCODE_MSWINDOWS
    LPWSTR wszPath = NULL;
    HRESULT hr;
    
    hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &wszPath);
    
    if (SUCCEEDED(hr))
    {
        _bstr_t bstrPath(wszPath);
        dataPath = ((char*)bstrPath);
    }
    
    CoTaskMemFree(wszPath);
    
#elif (TE_PLATFORM == TE_PLATFORMCODE_LINUX)
    
    char* homePtr = getenv( "HOME" );
    if( homePtr && te::core::FileSystem::isDirectory(homePtr))
    {
        boost::filesystem::path path(homePtr);
        path /=".local/share";
        
        dataPath = path.string();
    }
#elif (TE_PLATFORM == TE_PLATFORMCODE_APPLE)
    
    char* homePtr = getenv( "HOME" );
    if( homePtr && te::core::FileSystem::isDirectory(homePtr))
    {
        boost::filesystem::path path(homePtr);
        path /="Library/Application Support";
        
        dataPath = path.string();
    }
#else
#error "Platform not supported! Contact TerraLib Team"
#endif
    return dataPath;
}

std::string te::core::GetAppDataLocation()
{
    std::string dataPath;
#if TE_PLATFORM == TE_PLATFORMCODE_MSWINDOWS
    LPWSTR wszPath = NULL;
    HRESULT hr;
    
    hr = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &wszPath);
    
    if (SUCCEEDED(hr))
    {
        _bstr_t bstrPath(wszPath);
        dataPath = ((char*)bstrPath);
    }
    
    CoTaskMemFree(wszPath);
    
#elif (TE_PLATFORM == TE_PLATFORMCODE_LINUX)
    
    dataPath = GetAppLocalDataLocation();
    
#elif (TE_PLATFORM == TE_PLATFORMCODE_APPLE)
    
    dataPath = GetAppLocalDataLocation();
    
#else
#error "Platform not supported! Contact TerraLib Team"
#endif
    return dataPath;
}
