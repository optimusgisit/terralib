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
#  Description: Build configuration for TerraView.
#
#  Author: Gilberto Ribeiro de Queiroz <gribeiro@dpi.inpe.br>
#          Juan Carlos P. Garrido <juan@dpi.inpe.br>
#          Frederico Augusto T. Bede <frederico.bede@funcate.org.br>
#

configure_file(${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/Config.h.in
               ${CMAKE_BINARY_DIR}/terraview_config.h @ONLY)


include_directories(${Boost_INCLUDE_DIR} ${TERRALIB_ABSOLUTE_ROOT_DIR}/src)

file(GLOB TERRALIB_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/*.cpp)
file(GLOB TERRALIB_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/*.h)
file(GLOB TERRALIB_UI_FILES  ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/ui/*.ui)

source_group("Source Files"  FILES ${TERRALIB_SRC_FILES})
source_group("Header Files"  FILES ${TERRALIB_HDR_FILES})
source_group("UI Files"  FILES ${TERRALIB_UI_FILES})

#events
file(GLOB TERRALIB_EVENTS_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/events/*.h)

source_group("Header Files\\events"  FILES ${TERRALIB_EVENTS_HDR_FILES})

#settings
file(GLOB TERRALIB_SETTINGS_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/settings/*.cpp)
file(GLOB TERRALIB_SETTINGS_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/settings/*.h)
file(GLOB TERRALIB_SETTINGS_UI_FILES  ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/settings/ui/*.ui)

source_group("Source Files\\settings"  FILES ${TERRALIB_SETTINGS_SRC_FILES})
source_group("Header Files\\settings"  FILES ${TERRALIB_SETTINGS_HDR_FILES})
source_group("UI Files\\settings"  FILES ${TERRALIB_SETTINGS_UI_FILES})



if(APPLE)

  find_library(TERRALIB_COREFOUNDATION_LIBRARY
               NAMES CoreFoundation
               PATHS "/")

  if(TERRALIB_BUILD_AS_BUNDLE)
    set(MACOSX_BUNDLE_BUNDLE_NAME "TerraView")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright 2001-2015")
    set(MACOSX_BUNDLE_SIGNATURE "TerraView")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "http://www.terraview.org")
    set(MACOSX_BUNDLE_EXECUTABLE_NAME "TerraView")
    set(MACOSX_BUNDLE_INFO_STRING "System for manipulating geographical data.")
    set(MACOSX_BUNDLE_ICON_FILE "../share/terralib/images/icns/terralib-globe.icns")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${TERRALIB_STRING_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${TERRALIB_PKG_VERSION})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${TERRALIB_STRING_VERSION})

    configure_file(${CMAKE_ROOT}/Modules/MacOSXBundleInfo.plist.in ${CMAKE_BINARY_DIR}/Info.plist)

    set(CPACK_PACKAGE_ICON "${TERRALIB_ABSOLUTE_ROOT_DIR}/share/terralib/images/icns/terralib-globe.icns")
    set(CPACK_BUNDLE_NAME "TerraView")
    set(CPACK_BUNDLE_ICON ${CPACK_PACKAGE_ICON})
    set(CPACK_BUNDLE_PLIST ${CMAKE_BINARY_DIR}/Info.plist)

    set (TERRAVIEW_APP_TYPE MACOSX_BUNDLE)
  endif()
endif()

if(Qt5_FOUND)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

# uic'ing
  QT5_WRAP_UI(TERRALIB_GEN_HDR_FILES ${TERRALIB_UI_FILES})
  QT5_WRAP_UI(TERRALIB_SETTINGS_GEN_HDR_FILES ${TERRALIB_SETTINGS_UI_FILES})

  set(TERRALIB_FILES ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES} ${TERRALIB_GEN_HDR_FILES}
                     ${TERRALIB_EVENTS_HDR_FILES}
                     ${TERRALIB_SETTINGS_SRC_FILES} ${TERRALIB_SETTINGS_HDR_FILES} ${TERRALIB_SETTINGS_GEN_HDR_FILES})


  add_executable(terraview ${TERRAVIEW_APP_TYPE} ${TERRALIB_FILES})
  
  if(WIN32)
    get_target_property(loc terraview LOCATION_Release)

    get_target_property(loc_d terraview LOCATION_Debug)

    get_filename_component(loc_dir ${loc} DIRECTORY)

    get_filename_component(loc_d_dir ${loc_d} DIRECTORY)
  
    set(qt_dir "${Qt5_DIR}/../../..")
  
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/qt.conf.devel "[Paths]\nBinaries = ${qt_dir}/bin\nPlugins = ${qt_dir}/plugins\nTranslations = ../share/terralib/translations")

    add_custom_command(
      TARGET terraview
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/qt.conf.devel" "$<$<CONFIG:Release>:${loc_dir}>$<$<CONFIG:Debug>:${loc_d_dir}>"
      COMMAND ${CMAKE_COMMAND} -E rename "$<$<CONFIG:Release>:${loc_dir}/qt.conf.devel>$<$<CONFIG:Debug>:${loc_d_dir}/qt.conf.devel>" "$<$<CONFIG:Release>:${loc_dir}/qt.conf>$<$<CONFIG:Debug>:${loc_d_dir}/qt.conf>"
      COMMENT "Copying qt.conf file..."
      VERBATIM
    )
  endif()

  if(APPLE)
    target_link_libraries(terraview terralib_mod_qt_apf ${TERRALIB_COREFOUNDATION_LIBRARY})
  else()
    target_link_libraries(terraview terralib_mod_qt_apf)
  endif()

  qt5_use_modules(terraview Widgets)

else()

  include_directories(${CMAKE_CURRENT_BINARY_DIR})

  include(${QT_USE_FILE})

  include_directories(${QT_INCLUDE_DIR})

  add_definitions(${QT_DEFINITIONS})

# uic'ing
  QT4_WRAP_UI(TERRALIB_GEN_HDR_FILES ${TERRALIB_UI_FILES})
  QT4_WRAP_UI(TERRALIB_SETTINGS_GEN_HDR_FILES ${TERRALIB_SETTINGS_UI_FILES})

# moc'ing
  set(TERRALIB_HDRS_TO_MOC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/TerraView.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/ProjectInfoDialog.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/ProjectInfoWidget.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/TerraViewController.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/AboutDialog.h
                                 ${TERRALIB_ABSOLUTE_ROOT_DIR}/src/terraview/settings/ProjectWidget.h)

  QT4_WRAP_CPP(TERRALIB_GEN_SRC_FILES ${TERRALIB_HDRS_TO_MOC_FILES})

  set(TERRALIB_FILES ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES} ${TERRALIB_GEN_HDR_FILES} ${TERRALIB_GEN_SRC_FILES}
                     ${TERRALIB_EVENTS_HDR_FILES}
                     ${TERRALIB_SETTINGS_SRC_FILES} ${TERRALIB_SETTINGS_HDR_FILES} ${TERRALIB_SETTINGS_GEN_HDR_FILES})

  add_executable(terraview ${TERRAVIEW_APP_TYPE} ${TERRALIB_FILES})

  if(APPLE)
    target_link_libraries(terraview terralib_mod_qt_apf ${QT_LIBRARIES} ${TERRALIB_COREFOUNDATION_LIBRARY})
  else()
    target_link_libraries(terraview terralib_mod_qt_apf ${QT_LIBRARIES})
  endif()

endif()

add_dependencies(terraview terralib_qhelp terralib_qtranslation)

if(WIN32 AND MSVC)
  set_target_properties(terraview PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
  set_target_properties(terraview PROPERTIES RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
  set_target_properties(terraview PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  set_target_properties(terraview PROPERTIES MINSIZEREL "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

install(TARGETS terraview
        EXPORT terralib-targets
        RUNTIME DESTINATION ${TERRALIB_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRALIB_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRALIB_DESTINATION_ARCHIVE} COMPONENT runtime
        BUNDLE DESTINATION . COMPONENT runtime)

install(FILES ${TERRALIB_HDR_FILES}
              ${TERRALIB_GEN_HDR_FILES}
              ${CMAKE_BINARY_DIR}/terraview_config.h
        DESTINATION ${TERRALIB_DESTINATION_HEADERS}/terraview COMPONENT devel)

install(DIRECTORY ${TERRALIB_ABSOLUTE_ROOT_DIR}/share/terraview/
        DESTINATION ${TERRALIB_DESTINATION_SHARE}/terraview COMPONENT runtime)

if(TERRALIB_TRACK_3RDPARTY_DEPENDENCIES)
  TeInstallQt5Plugins("qgif;qico;qjpeg;qmng;qtiff;qsvg;qsvgicon;qsqlite;qwindows;qcocoa")        
endif()

if(APPLE)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/qt.conf
       "[Paths]\nBinaries = MacOS\nPlugins = qtplugins\nTranslations = share/terralib/translations")

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qt.conf
          DESTINATION terraview.app/Contents/Resources COMPONENT runtime)

elseif(WIN32)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/qt.conf
       "[Paths]\nBinaries = .\nPlugins = ../qtplugins\nTranslations = ../share/terralib/translations")
         
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qt.conf
          DESTINATION ${TERRALIB_DESTINATION_RUNTIME} COMPONENT runtime)
elseif(UNIX)
  set(_qtDir ${Qt5_DIR}/../../..)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/qt.conf
       "[Paths]\nBinaries = ${_qtDir}/bin\nPlugins = ${_qtDir}/plugins\nTranslations = ../share/terralib/translations")
         
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qt.conf
          DESTINATION ${TERRALIB_DESTINATION_RUNTIME} COMPONENT runtime)
endif()


