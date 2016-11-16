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
#  Description: Build configuration for the Geometry Module.
#
#  Author: Gilberto Ribeiro de Queiroz <gribeiro@dpi.inpe.br>
#          Juan Carlos P. Garrido <juan@dpi.inpe.br>
#          Frederico Augusto T. Bede <frederico.bede@funcate.org.br>
#

include_directories(${Boost_INCLUDE_DIR})

if(TERRALIB_GEOS_ENABLED)
  include_directories(${GEOS_INCLUDE_DIR} ${GEOS_INCLUDE_DIR}/..)
endif()

if(WIN32)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS -DTEGEOMDLL)
endif()

file(GLOB TERRALIB_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/geometry/*.cpp)
file(GLOB TERRALIB_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/geometry/*.h)

source_group("Source Files"  FILES ${TERRALIB_SRC_FILES})
source_group("Header Files"  FILES ${TERRALIB_HDR_FILES})

if(TERRALIB_MOD_XML_ENABLED)

  file(GLOB TERRALIB_XML_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/geometry/serialization/xml/*.cpp)
  file(GLOB TERRALIB_XML_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/geometry/serialization/xml/*.h)

  source_group("Source Files\\serialization\\xml"  FILES ${TERRALIB_XML_SRC_FILES})
  source_group("Header Files\\serialization\\xml"  FILES ${TERRALIB_XML_HDR_FILES})

  add_library(terralib_mod_geometry SHARED ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES} ${TERRALIB_XML_SRC_FILES} ${TERRALIB_XML_HDR_FILES})

else()

  add_library(terralib_mod_geometry SHARED ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES})

endif()

set(TERRALIB_LIBRARIES_DEPENDENCIES "terralib_mod_common")

list(APPEND TERRALIB_LIBRARIES_DEPENDENCIES "terralib_mod_core")

list(APPEND TERRALIB_LIBRARIES_DEPENDENCIES "terralib_mod_datatype")

if(TERRALIB_MOD_XML_ENABLED)
  list(APPEND TERRALIB_LIBRARIES_DEPENDENCIES "terralib_mod_xml")
endif()

if(TERRALIB_MOD_SRS_ENABLED)
  list(APPEND TERRALIB_LIBRARIES_DEPENDENCIES "terralib_mod_srs")
endif()

if(TERRALIB_GEOS_ENABLED)
  list(APPEND TERRALIB_LIBRARIES_DEPENDENCIES ${GEOS_LIBRARY})
endif()

list(APPEND TERRALIB_LIBRARIES_DEPENDENCIES ${Boost_THREAD_LIBRARY})

target_link_libraries(terralib_mod_geometry ${TERRALIB_LIBRARIES_DEPENDENCIES})

set_target_properties(terralib_mod_geometry
                      PROPERTIES VERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR}
                                 SOVERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR})

install(TARGETS terralib_mod_geometry
        EXPORT terralib-targets
        RUNTIME DESTINATION ${TERRALIB_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRALIB_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRALIB_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRALIB_HDR_FILES}
        DESTINATION ${TERRALIB_DESTINATION_HEADERS}/terralib/geometry COMPONENT devel)

if(TERRALIB_MOD_XML_ENABLED)
  install(FILES ${TERRALIB_XML_HDR_FILES}
          DESTINATION ${TERRALIB_DESTINATION_HEADERS}/terralib/geometry/serialization/xml COMPONENT devel)
endif()

export(TARGETS terralib_mod_geometry APPEND FILE ${CMAKE_BINARY_DIR}/terralib-exports.cmake)
