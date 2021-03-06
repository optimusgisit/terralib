#
#  Copyright (C) 2008-2014 National Institute For Space Research (INPE) - Brazil.
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
#  Description: TerraLib Commmon Runtime Module.
#
#  Author: Gilberto Ribeiro de Queiroz <gribeiro@dpi.inpe.br>
#          Juan Carlos P. Garrido <juan@dpi.inpe.br>
#          Frederico Augusto T. Bede <frederico.bede@funcate.org.br>
#

include_directories(${Boost_INCLUDE_DIR})

if(WIN32)
  add_definitions(-DTECOMMONDLL)
endif()

file(GLOB TERRALIB_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/common/*.cpp)
file(GLOB TERRALIB_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/common/*.h)

file(GLOB TERRALIB_URI_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/common/URI/*.cpp)
file(GLOB TERRALIB_URI_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/common/URI/*.h)

file(GLOB TERRALIB_PROGRESS_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/common/progress/*.cpp)
file(GLOB TERRALIB_PROGRESS_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/common/progress/*.h)

source_group("Source Files"            FILES ${TERRALIB_SRC_FILES})
source_group("Header Files"            FILES ${TERRALIB_HDR_FILES})
source_group("Source Files\\URI"       FILES ${TERRALIB_URI_SRC_FILES})
source_group("Header Files\\URI"       FILES ${TERRALIB_URI_HDR_FILES})
source_group("Source Files\\progress"  FILES ${TERRALIB_PROGRESS_SRC_FILES})
source_group("Header Files\\progress"  FILES ${TERRALIB_PROGRESS_HDR_FILES})

set(TERRALIB_FILES ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES}
                   ${TERRALIB_URI_SRC_FILES} ${TERRALIB_URI_HDR_FILES}
                   ${TERRALIB_PROGRESS_SRC_FILES} ${TERRALIB_PROGRESS_HDR_FILES})

add_library(terralib_mod_common SHARED ${TERRALIB_FILES})

set(TERRALIB_LIBRARIES_DEPENDENCIES ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})

target_link_libraries(terralib_mod_common terralib_mod_core ${TERRALIB_LIBRARIES_DEPENDENCIES})

set_target_properties(terralib_mod_common
                      PROPERTIES VERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR}
                                 SOVERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR})

install(TARGETS terralib_mod_common
        EXPORT terralib-targets
        RUNTIME DESTINATION ${TERRALIB_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRALIB_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRALIB_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRALIB_HDR_FILES}
        DESTINATION ${TERRALIB_DESTINATION_HEADERS}/terralib/common COMPONENT devel)

install(FILES ${TERRALIB_URI_HDR_FILES}
        DESTINATION ${TERRALIB_DESTINATION_HEADERS}/terralib/common/URI COMPONENT devel)

install(FILES ${TERRALIB_PROGRESS_HDR_FILES}
        DESTINATION ${TERRALIB_DESTINATION_HEADERS}/terralib/common/progress COMPONENT devel)

