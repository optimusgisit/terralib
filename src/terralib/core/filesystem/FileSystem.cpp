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
 \file terralib/core/filesystem/FileSystem.cpp
 
 \brief A class for handling system files and paths.
 
 \author Matheus Cavassan Zaglia
 */

// TerraLib
#include "FileSystem.h"
#include "../Exception.h"
#include "../encoding/CharEncoding.h"

// Boost
#include <boost/filesystem.hpp>

// System dependent includes
#if TE_PLATFORMCODE_MSWINDOWS == TE_PLATFORM
#  include <windows.h>
#elif TE_PLATFORMCODE_LINUX == TE_PLATFORM
#  include <unistd.h>
#  include <limits.h>
#elif TE_PLATFORMCODE_APPLE == TE_PLATFORM
#  include <mach-o/dyld.h>
#endif

std::string te::core::FileSystem::currentPath()
{
    return te::core::CharEncoding::toUTF8(
                                          boost::filesystem::current_path().string());
}

std::string te::core::FileSystem::systemCompletePath(const std::string& path)
{
    std::string complete =
    boost::filesystem::system_complete(te::core::CharEncoding::fromUTF8(path))
    .string();
    return te::core::CharEncoding::toUTF8(complete);
}

std::string te::core::FileSystem::tempDirectoryPath()
{
    return te::core::CharEncoding::toUTF8(
                                          boost::filesystem::temp_directory_path().string());
}

std::string te::core::FileSystem::uniquePath(const std::string& format)
{
    return te::core::CharEncoding::toUTF8(
                                          boost::filesystem::unique_path(format).string());
}

std::string te::core::FileSystem::absolutePath(const std::string& path)
{
    return te::core::CharEncoding::toUTF8(
                                          boost::filesystem::absolute(path).string());
}

std::string te::core::FileSystem::extension(const std::string& path)
{
    return boost::filesystem::extension(path);
}

bool te::core::FileSystem::isDirectory(const std::string& path)
{
    return boost::filesystem::is_directory(
                                           te::core::CharEncoding::fromUTF8(path));
}

bool te::core::FileSystem::isEmpty(const std::string& path)
{
    return boost::filesystem::is_empty(te::core::CharEncoding::fromUTF8(path));
}

bool te::core::FileSystem::isRegularFile(const std::string& path)
{
    return boost::filesystem::is_regular_file(
                                              te::core::CharEncoding::fromUTF8(path));
}

std::string te::core::FileSystem::executableDirectory()
{
#if TE_PLATFORMCODE_MSWINDOWS == TE_PLATFORM
    char exePath[1024];
    if (GetModuleFileName(NULL, exePath, sizeof(exePath)) == 0)
        exePath[0] = '\0';
#elif TE_PLATFORMCODE_SOLARIS == TE_PLATFORM
    char exePath[PATH_MAX];
    if(realpath(getexecname(), exePath) == NULL)
        exePath[0] = '\0';
#elif TE_PLATFORMCODE_LINUX == TE_PLATFORM
    char exePath[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", exePath, sizeof(exePath));
    if(len == -1 || len == sizeof(exePath))
        len = 0;
    exePath[len] = '\0';
#elif TE_PLATFORMCODE_APPLE == TE_PLATFORM
    char exePath[PATH_MAX];
    uint32_t len = sizeof(exePath);
    if(_NSGetExecutablePath(exePath, &len) != 0)
    {
        exePath[0] = '\0';  // buffer too small (!)
    }
    else
    {
        char* canonicalPath = realpath(exePath, NULL);
        if(canonicalPath != NULL)
        {
            strncpy(exePath, canonicalPath, len);
            free(canonicalPath);
        }
    }
#endif
    
    boost::filesystem::path p(exePath);
    return te::core::CharEncoding::toUTF8(p.parent_path().string());
}

bool te::core::FileSystem::exists(const std::string& path)
{
    return boost::filesystem::exists(te::core::CharEncoding::fromUTF8(path));
}

bool te::core::FileSystem::createDirectory(const std::string& path)
{
    return boost::filesystem::create_directory(
                                               te::core::CharEncoding::fromUTF8(path));
}

bool te::core::FileSystem::createDirectories(const std::string& path)
{
    return boost::filesystem::create_directories(
                                                 te::core::CharEncoding::fromUTF8(path));
}

void te::core::FileSystem::copyFile(const std::string& from,
                                    const std::string& to)
{
    boost::filesystem::copy_file(te::core::CharEncoding::fromUTF8(from),
                                 te::core::CharEncoding::fromUTF8(to));
}

bool te::core::FileSystem::remove(const std::string& path)
{
    return boost::filesystem::remove(te::core::CharEncoding::fromUTF8(path));
}

void te::core::FileSystem::rename(const std::string& old_p,
                                  const std::string& new_p)
{
    boost::filesystem::rename(te::core::CharEncoding::fromUTF8(old_p),
                              te::core::CharEncoding::fromUTF8(new_p));
}

std::vector<std::string> te::core::FileSystem::listDirectory(
                                                             const std::string& path)
{
    if (!isDirectory(path))
        throw InvalidArgumentException()
        << ErrorDescription("The given path is not a directory.");
    
    std::vector<std::string> result;
    
    boost::filesystem::path p(te::core::CharEncoding::fromUTF8(path));
    
    for (boost::filesystem::directory_iterator it(p), it_end; it != it_end; ++it)
        result.push_back(te::core::CharEncoding::toUTF8(it->path().string()));
    
    return result;
}
