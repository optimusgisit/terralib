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
#  Description: Build configuration OGC WMS Data Access Driver.
#
#  Author: Emerson Moraes
#

include_directories(${Boost_INCLUDE_DIR}
                    ${GDAL_INCLUDE_DIR})

if(WIN32)
  add_definitions(-DTEWSWMSDATAACCESSDLL)
endif()

file(GLOB TERRALIB_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/ws/ogc/wms/dataaccess/*.cpp)
file(GLOB TERRALIB_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/ws/ogc/wms/dataaccess/*.h)

source_group("Source Files"  FILES ${TERRALIB_SRC_FILES})
source_group("Header Files"  FILES ${TERRALIB_HDR_FILES})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(TERRALIB_MOD_XML_ENABLED)

  file(GLOB TERRALIB_XML_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/ws/ogc/wms/dataaccess/serialization/xml/*.cpp)
  file(GLOB TERRALIB_XML_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/ws/ogc/wms/dataaccess/serialization/xml/*.h)

  source_group("Source Files\\serialization\\xml"  FILES ${TERRALIB_XML_SRC_FILES})
  source_group("Header Files\\serialization\\xml"  FILES ${TERRALIB_XML_HDR_FILES})

  set(TERRALIB_FILES ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES} ${TERRALIB_XML_SRC_FILES} ${TERRALIB_XML_HDR_FILES})

else()

  set(TERRALIB_FILES ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES})

endif()

add_library(terralib_mod_ws_ogc_wms_dataaccess SHARED ${TERRALIB_FILES})

if(TERRALIB_MOD_XML_ENABLED)
  target_link_libraries(terralib_mod_ws_ogc_wms_dataaccess terralib_mod_common
                                                           terralib_mod_dataaccess
                                                           terralib_mod_datatype
                                                           terralib_mod_maptools
                                                           terralib_mod_ws_core
                                                           terralib_mod_ws_ogc_wms_client
                                                           terralib_mod_xml)
else()
  target_link_libraries(terralib_mod_ws_ogc_wms_dataaccess terralib_mod_common
                                                           terralib_mod_dataaccess
                                                           terralib_mod_datatype
                                                           terralib_mod_maptools
                                                           terralib_mod_ws_core
                                                           terralib_mod_ws_ogc_wms_client)
endif()

set_target_properties(terralib_mod_ws_ogc_wms_dataaccess
                      PROPERTIES VERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR}
                                 SOVERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR})

install(TARGETS terralib_mod_ws_ogc_wms_dataaccess
        EXPORT terralib-targets
        RUNTIME DESTINATION ${TERRALIB_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRALIB_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRALIB_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRALIB_HDR_FILES}
        DESTINATION ${TERRALIB_DESTINATION_HEADERS}/terralib/ws/ogc/wms/da COMPONENT devel)

if(TERRALIB_MOD_XML_ENABLED)
  install(FILES ${TERRALIB_XML_HDR_FILES}
          DESTINATION ${TERRALIB_DESTINATION_HEADERS}/terralib/ws/ogc/wms/da/serialization/xml COMPONENT devel)
endif()

install(FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/share/terralib/plugins/te.ogc.wms.teplg.json
        DESTINATION ${TERRALIB_DESTINATION_PLUGINS} COMPONENT runtime)
