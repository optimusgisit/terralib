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
#  Description: Build configuration for Qt Components for OGC WMS.
#
#  Author: Emerson Moraes
#

include_directories(${Boost_INCLUDE_DIR})

if(WIN32)
  add_definitions(-DTEWSWMSQTDLL)
endif()

file(GLOB TERRALIB_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/ws/ogc/wms/qt/*.cpp)
file(GLOB TERRALIB_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/ws/ogc/wms/qt/*.h)
file(GLOB TERRALIB_UI_FILES  ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/ws/ogc/wms/qt/ui/*.ui)


source_group("Source Files"  FILES ${TERRALIB_SRC_FILES})
source_group("Header Files"  FILES ${TERRALIB_HDR_FILES})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(Qt5_FOUND)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  # uic'ing
  QT5_WRAP_UI(TERRALIB_GEN_HDR_FILES ${TERRALIB_UI_FILES})

  set(TERRALIB_FILES ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES} ${TERRALIB_GEN_HDR_FILES})

  add_library(terralib_mod_ws_ogc_wms_qt SHARED ${TERRALIB_FILES})

  target_link_libraries(terralib_mod_ws_ogc_wms_qt terralib_mod_qt_widgets
                                                   terralib_mod_qt_apf
                                                   terralib_mod_ws_ogc_wms_dataaccess)

  qt5_use_modules(terralib_mod_ws_ogc_wms_qt Widgets)

else()
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include(${QT_USE_FILE})
  include_directories(${QT_INCLUDE_DIR})
  add_definitions(${QT_DEFINITIONS})

  set(TERRALIB_HDRS_TO_MOC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/ws/ogc/wms/qt/WMSConnectDialog.h)

  QT4_WRAP_CPP(TERRALIB_GEN_SRC_FILES ${TERRALIB_HDRS_TO_MOC_FILES})

  set(TERRALIB_FILES ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES} ${TERRALIB_GEN_SRC_FILES})

  add_library(terralib_mod_ws_ogc_wms_qt SHARED ${TERRALIB_FILES})

  target_link_libraries(terralib_mod_ws_ogc_wms_qt terralib_mod_qt_widgets
                                                     terralib_mod_qt_apf
                                                     terralib_mod_ws_ogc_wms_dataaccess
                                                     ${QT_LIBRARIES})
endif()

set_target_properties(terralib_mod_ws_ogc_wms_qt
                      PROPERTIES VERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR}
                                 SOVERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR})

install(TARGETS terralib_mod_ws_ogc_wms_qt
                EXPORT terralib-targets
                RUNTIME DESTINATION ${TERRALIB_DESTINATION_RUNTIME} COMPONENT runtime
                LIBRARY DESTINATION ${TERRALIB_DESTINATION_LIBRARY} COMPONENT runtime
                ARCHIVE DESTINATION ${TERRALIB_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRALIB_HDR_FILES} ${TERRALIB_GEN_HDR_FILES}
        DESTINATION ${TERRALIB_DESTINATION_HEADERS}/terralib/ws/ogc/wms/qt COMPONENT devel)

install(FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/share/terralib/plugins/te.ogc.wms.teplg.json
        DESTINATION ${TERRALIB_DESTINATION_PLUGINS} COMPONENT runtime)
