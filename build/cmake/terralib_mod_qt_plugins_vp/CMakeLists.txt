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
#  Description: Build configuration for the Vector Processing Qt plugin.
#
#  Author: Gilberto Ribeiro de Queiroz <gribeiro@dpi.inpe.br>
#          Juan Carlos P. Garrido <juan@dpi.inpe.br>
#          Frederico Augusto T. Bede <frederico.bede@funcate.org.br>
#

include_directories(${Boost_INCLUDE_DIR})

if(WIN32)
  add_definitions(-DTEQTPLUGINVPDLL)
endif()

file(GLOB TERRALIB_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/*.cpp)
file(GLOB TERRALIB_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/*.h)

if(Qt5_FOUND)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  set(TERRALIB_FILES ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES})

  add_library(terralib_mod_qt_plugins_vp SHARED ${TERRALIB_FILES})

  target_link_libraries(terralib_mod_qt_plugins_vp terralib_mod_vp_qt
                                                   
                                                   terralib_mod_qt_apf)

  qt5_use_modules(terralib_mod_qt_plugins_vp Widgets)

else()
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include(${QT_USE_FILE})
  include_directories(${QT_INCLUDE_DIR})
  add_definitions(${QT_DEFINITIONS})

# moc'ing
  set(TERRALIB_HDRS_TO_MOC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/AbstractAction.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/AggregationAction.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/BufferAction.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/DifferenceAction.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/GeometricOpAction.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/IntersectionAction.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/LineToPolygonAction.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/MergeAction.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/MultipartToSinglepartAction.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/PolygonToLineAction.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/Plugin.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/SummarizationAction.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/TransformationAction.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terralib/qt/plugins/vp/UnionAction.h)


  QT4_WRAP_CPP(TERRALIB_GEN_SRC_FILES ${TERRALIB_HDRS_TO_MOC_FILES})

  set(TERRALIB_FILES ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES} ${TERRALIB_GEN_SRC_FILES})

  add_library(terralib_mod_qt_plugins_vp SHARED ${TERRALIB_FILES})

  target_link_libraries(terralib_mod_qt_plugins_vp terralib_mod_vp_qt
                                                   
                                                   terralib_mod_qt_apf
                                                   ${QT_LIBRARIES})

endif()

set_target_properties(terralib_mod_qt_plugins_vp
                      PROPERTIES VERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR}
                                 SOVERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR})

install(TARGETS terralib_mod_qt_plugins_vp
        EXPORT terralib-targets
        RUNTIME DESTINATION ${TERRALIB_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRALIB_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRALIB_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRALIB_HDR_FILES}
        DESTINATION ${TERRALIB_DESTINATION_HEADERS}/terralib/qt/plugins/vp COMPONENT devel)

install(FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/share/terralib/plugins/te.qt.vp.teplg.json
        DESTINATION ${TERRALIB_DESTINATION_PLUGINS} COMPONENT runtime)

