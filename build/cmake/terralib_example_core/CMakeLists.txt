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
#  Description: Examples for TerraLib Core Runtime Module.
#
#  Author: Vinicius Campanha
#          Matheus Cavassan Zaglia
#          Frederico Augusto Bedê
#          Gilberto Ribeiro de Queiroz
#

include_directories(${TERRALIB_ABSOLUTE_ROOT_DIR}/src)

# Examples of core/encoding

file(GLOB TERRALIB_EXAMPLE_CORE_ENCODING_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/encoding/*.cpp)

source_group("Source Files\\encoding"  FILES ${TERRALIB_EXAMPLE_CORE_ENCODING_SRC_FILES})

add_executable(terralib_example_core_encoding ${TERRALIB_EXAMPLE_CORE_ENCODING_SRC_FILES})

target_link_libraries(terralib_example_core_encoding terralib_mod_core)


#Examples of core/filesystem

file(GLOB TERRALIB_EXAMPLE_CORE_FILESYSTEM_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/filesystem/*.cpp)

source_group("Source Files\\filesystem"  FILES ${TERRALIB_EXAMPLE_CORE_FILESYSTEM_SRC_FILES})

add_executable(terralib_example_core_filesystem ${TERRALIB_EXAMPLE_CORE_FILESYSTEM_SRC_FILES})

target_link_libraries(terralib_example_core_filesystem terralib_mod_core)



#Examples of core/lib

file(GLOB TERRALIB_EXAMPLE_CORE_LIB_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/lib/*.cpp)

source_group("Source Files\\lib"  FILES ${TERRALIB_EXAMPLE_CORE_LIB_SRC_FILES})

add_executable(terralib_example_core_lib ${TERRALIB_EXAMPLE_CORE_LIB_SRC_FILES})

target_link_libraries(terralib_example_core_lib terralib_mod_core
                                                ${Boost_FILESYSTEM_LIBRARY}
                                                ${Boost_SYSTEM_LIBRARY})


#Examples of core/logger

file(GLOB TERRALIB_EXAMPLE_CORE_LOGGER_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/logger/*.cpp)

source_group("Source Files\\logger"  FILES ${TERRALIB_EXAMPLE_CORE_LOGGER_SRC_FILES})

add_executable(terralib_example_core_logger ${TERRALIB_EXAMPLE_CORE_LOGGER_SRC_FILES})

target_link_libraries(terralib_example_core_logger terralib_mod_core)


#Examples of core/translator

file(GLOB TERRALIB_EXAMPLE_CORE_TRANSLATOR_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/translator/*.cpp)

source_group("Source Files\\translator"  FILES ${TERRALIB_EXAMPLE_CORE_TRANSLATOR_SRC_FILES})

add_executable(terralib_example_core_translator ${TERRALIB_EXAMPLE_CORE_TRANSLATOR_SRC_FILES})

target_link_libraries(terralib_example_core_translator terralib_mod_core)

TERRALIB_REGISTER_TRANSLATION("terralib_example_core_translator" "pt_BR"
                              "${TERRALIB_ABSOLUTE_ROOT_DIR}/share/terralib/translations"
                              "${CMAKE_BINARY_DIR}/share/terralib/translations")


#Examples of core/plugin

file(GLOB TERRALIB_EXAMPLE_CORE_PLUGIN_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/plugin/*.cpp)

source_group("Source Files\\plugin"  FILES ${TERRALIB_EXAMPLE_CORE_PLUGIN_SRC_FILES})

add_executable(terralib_example_core_plugin ${TERRALIB_EXAMPLE_CORE_PLUGIN_SRC_FILES})

target_link_libraries(terralib_example_core_plugin terralib_mod_core)

if(WIN32)
  add_definitions(-DTECOREEXEMPDLL )
endif()

#Plugin1 example
file(GLOB TERRALIB_EXAMPLE_CORE_PLUGIN1_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/plugin/te_example_core_plugin1/*.cpp)

add_library(terralib_example_core_plugin1 SHARED ${TERRALIB_EXAMPLE_CORE_PLUGIN1_SRC_FILES})
target_link_libraries(terralib_example_core_plugin1 terralib_mod_core)

file(COPY ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/plugin/te_example_core_plugin1/plugin1.teplg.json
     DESTINATION ${CMAKE_BINARY_DIR}/example/plugins)

#Plugin2 example
file(GLOB TERRALIB_EXAMPLE_CORE_PLUGIN2_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/plugin/te_example_core_plugin2/*.cpp)

add_library(terralib_example_core_plugin2 SHARED ${TERRALIB_EXAMPLE_CORE_PLUGIN2_SRC_FILES})
target_link_libraries(terralib_example_core_plugin2 terralib_mod_core)

file(COPY ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/plugin/te_example_core_plugin2/plugin2.teplg.json
     DESTINATION ${CMAKE_BINARY_DIR}/example/plugins)

#Plugin3 example
file(GLOB TERRALIB_EXAMPLE_CORE_PLUGIN3_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/plugin/te_example_core_plugin3/*.cpp)

add_library(terralib_example_core_plugin3 SHARED ${TERRALIB_EXAMPLE_CORE_PLUGIN3_SRC_FILES})
target_link_libraries(terralib_example_core_plugin3 terralib_mod_core)

file(COPY ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/plugin/te_example_core_plugin3/plugin3.teplg.json
     DESTINATION ${CMAKE_BINARY_DIR}/example/plugins)

#Plugin4 example
file(GLOB TERRALIB_EXAMPLE_CORE_PLUGIN4_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/plugin/te_example_core_plugin4/*.cpp)

add_library(terralib_example_core_plugin4 SHARED ${TERRALIB_EXAMPLE_CORE_PLUGIN4_SRC_FILES})
target_link_libraries(terralib_example_core_plugin4 terralib_mod_core)

file(COPY ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/plugin/te_example_core_plugin4/plugin4.teplg.json
     DESTINATION ${CMAKE_BINARY_DIR}/example/plugins)

add_dependencies(terralib_example_core_plugin terralib_example_core_plugin1
                                              terralib_example_core_plugin2
                                              terralib_example_core_plugin3
                                              terralib_example_core_plugin4)

# Examples of core/uri

file(GLOB TERRALIB_EXAMPLE_CORE_URI_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/uri/*.cpp)

source_group("Source Files\\uri"  FILES ${TERRALIB_EXAMPLE_CORE_URI_SRC_FILES})

add_executable(terralib_example_core_uri ${TERRALIB_EXAMPLE_CORE_URI_SRC_FILES})

target_link_libraries(terralib_example_core_uri terralib_mod_core)

# Library of functions to example lib
if(WIN32)
  add_definitions(-DTECOREEXEMPDLL)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/example)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/example)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/example)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/example)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/example)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/example)

file(GLOB HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/lib/lib_example/*.h)
file(GLOB SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/core/lib/lib_example/*.cpp)

add_library(terralib_example_core_lib_function SHARED ${HDR_FILES} ${SRC_FILES})

set_target_properties(terralib_example_core_lib_function
                      PROPERTIES VERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR}
                                 SOVERSION ${TERRALIB_VERSION_MAJOR}.${TERRALIB_VERSION_MINOR})

add_dependencies(terralib_example_core_lib terralib_example_core_lib_function)


