#
#  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.
#
#  This file is part of the TerraLib - a Framework for building GIS enabled applications.
#
#  TerraLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License,
#  or (at your option) any later version.
#
#  TerraLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with TerraLib. See COPYING. If not, write to
#  TerraLib Team at <terralib-team@terralib.org>.
#
#
#  Description: TerraLib CEditor Runtime Module.
#
#  Author: Frederico Bedê
#

set(TERRALIB_CEDITOR_DIR ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qsci)

include_directories(${Qscintilla_INCLUDE_DIR} ${Boost_INCLUDE_DIR})

if (WIN32)
  add_definitions(-DTECEDITORDLL)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB TERRALIB_CEDITOR_CONFIGHDR_FILE ${TERRALIB_CEDITOR_DIR}/*.h)
file(GLOB TERRALIB_CEDITOR_HDR_FILES ${TERRALIB_CEDITOR_DIR}/ceditor/*.h)
file(GLOB TERRALIB_CEDITOR_SRC_FILES ${TERRALIB_CEDITOR_DIR}/ceditor/*.cpp)

source_group("Header Files"                     FILES ${TERRALIB_CEDITOR_CONFIGHDR_FILES})
source_group("Header Files\\ceditor"            FILES ${TERRALIB_CEDITOR_HDR_FILES})
source_group("Source Files\\ceditor"            FILES ${TERRALIB_CEDITOR_SRC_FILES})

add_library(terralib_mod_ceditor SHARED ${TERRALIB_CEDITOR_SRC_FILES} ${TERRALIB_CEDITOR_CONFIGHDR_FILE} ${TERRALIB_CEDITOR_HDR_FILES})

target_link_libraries(terralib_mod_ceditor terralib_mod_core terralib_mod_vm ${Qscintilla_LIBRARY})

set_target_properties(terralib_mod_ceditor
                      PROPERTIES VERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR}
                                 SOVERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR})

qt5_use_modules(terralib_mod_ceditor Widgets)

install(TARGETS terralib_mod_ceditor
        EXPORT terralib-targets
        RUNTIME DESTINATION ${TERRALIB_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRALIB_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRALIB_DESTINATION_ARCHIVE} COMPONENT runtime)
