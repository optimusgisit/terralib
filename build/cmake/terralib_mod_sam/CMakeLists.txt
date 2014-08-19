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
#  Description: TerraLib Spatial Access Methods (SAM).
#
#  Author: Gilberto Ribeiro de Queiroz <gribeiro@dpi.inpe.br>
#          Juan Carlos P. Garrido <juan@dpi.inpe.br>
#          Emiliano Ferreira Castejon <castejon@dpi.inpe.br>
#

include_directories(${Boost_INCLUDE_DIR})

if(WIN32)
  add_definitions(-DTESAMDLL)
endif()

#file(GLOB TERRALIB_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/sam/rtree/*.cpp)
file(GLOB TERRALIB_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/sam/rtree/*.h)

#add_library(terralib_mod_sam SHARED ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES})
add_library(terralib_mod_sam STATIC EXCLUDE_FROM_ALL ${TERRALIB_HDR_FILES})

#target_link_libraries(terralib_mod_sam terralib_mod_geometry)

set_target_properties(terralib_mod_sam PROPERTIES LINKER_LANGUAGE CXX)

#set_target_properties(terralib_mod_sam
#                      PROPERTIES VERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR}
#                                 SOVERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR}
#                                 INSTALL_NAME_DIR "@executable_path/../lib")

#install(TARGETS terralib_mod_sam
#        EXPORT terralib-targets
#        RUNTIME DESTINATION ${TERRALIB_DESTINATION_RUNTIME} COMPONENT runtime
#        LIBRARY DESTINATION ${TERRALIB_DESTINATION_LIBRARY} COMPONENT runtime
#        ARCHIVE DESTINATION ${TERRALIB_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/sam/rtree.h
        DESTINATION ${TERRALIB_DESTINATION_HEADERS}/terralib/sam COMPONENT devel)

install(FILES ${TERRALIB_HDR_FILES}
        DESTINATION ${TERRALIB_DESTINATION_HEADERS}/terralib/sam/rtree COMPONENT devel)

#export(TARGETS terralib_mod_sam APPEND FILE ${CMAKE_BINARY_DIR}/terralib-exports.cmake)