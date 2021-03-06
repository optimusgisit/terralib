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
#  Description: Build configuration for the PostGIS Module.
#
#  Author: Gilberto Ribeiro de Queiroz <gribeiro@dpi.inpe.br>
#          Juan Carlos P. Garrido <juan@dpi.inpe.br>
#          Frederico Augusto T. Bede <frederico.bede@funcate.org.br>
#

include_directories(${Boost_INCLUDE_DIR})

if(PostgreSQL_INCLUDE_DIRS)
  include_directories(${PostgreSQL_INCLUDE_DIRS})
else()
  include_directories(${PostgreSQL_INCLUDE_DIR})
endif()

if(NOT PostgreSQL_LIBRARIES)
  set(PostgreSQL_LIBRARIES PostgreSQL_LIBRARY)
endif()

if(WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -DTEPGISDLL)
endif()

file(GLOB TERRALIB_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/postgis/*.cpp)
file(GLOB TERRALIB_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/postgis/*.h)

add_library(terralib_mod_postgis SHARED ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES})

target_link_libraries(terralib_mod_postgis terralib_mod_dataaccess
                                           terralib_mod_geometry
                                           terralib_mod_datatype
                                           
                                           terralib_mod_xml
                                           terralib_mod_common
                                           terralib_mod_core
                                           ${PostgreSQL_LIBRARIES}
                                           ${Boost_THREAD_LIBRARY}
                                           ${Boost_DATE_TIME_LIBRARY}
                                           ${Boost_SYSTEM_LIBRARY})

set_target_properties(terralib_mod_postgis
                      PROPERTIES VERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR}
                                 SOVERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR})

install(TARGETS terralib_mod_postgis
        EXPORT terralib-targets
        RUNTIME DESTINATION ${TERRALIB_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRALIB_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRALIB_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRALIB_HDR_FILES}
        DESTINATION ${TERRALIB_DESTINATION_HEADERS}/terralib/postgis COMPONENT devel)

install(FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/share/terralib/plugins/te.da.pgis.teplg.json
              ${TERRALIB_ABSOLUTE_ROOT_DIR}/share/terralib/plugins/postgis-capabilities.xml
        DESTINATION ${TERRALIB_DESTINATION_PLUGINS} COMPONENT runtime)

