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
#  Description: Examples for TerraLib Code Editor Module.
#
#  Author: Frederico Augusto Bedê
#

include_directories(${TERRALIB_ABSOLUTE_ROOT_DIR}/src ${Boost_INCLUDE_DIR} ${Qscintilla_INCLUDE_DIR})

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set (TERRALIB_CEDITOR_DIR ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/ceditor)

file(GLOB TERRALIB_EXAMPLE_CEDITOR_HDR_FILES ${TERRALIB_CEDITOR_DIR}/*.h)
file(GLOB TERRALIB_EXAMPLE_CEDITOR_SRC_FILES ${TERRALIB_CEDITOR_DIR}/*.cpp)
file(GLOB TERRALIB_EXAMPLE_CEDITOR_UI_FILES ${TERRALIB_CEDITOR_DIR}/ui/*.ui)

QT5_WRAP_UI(TERRALIB_EXAMPLE_CEDITOR_GEN_HDR_FILES ${TERRALIB_EXAMPLE_CEDITOR_UI_FILES})

source_group("UI Files"  FILES ${TERRALIB_EXAMPLE_CEDITOR_UI_FILES})

source_group("Generated Files"  FILES ${TERRALIB_EXAMPLE_CEDITOR_GEN_HDR_FILES})

add_executable(terralib_example_ceditor ${TERRALIB_EXAMPLE_CEDITOR_HDR_FILES} ${TERRALIB_EXAMPLE_CEDITOR_SRC_FILES}
  ${TERRALIB_EXAMPLE_CEDITOR_GEN_HDR_FILES} ${TERRALIB_EXAMPLE_CEDITOR_UI_FILES})

target_link_libraries(terralib_example_ceditor terralib_mod_ceditor 
											   terralib_mod_common
											   ${Qscintilla_LIBRARY})

qt5_use_modules(terralib_example_ceditor Widgets)

add_dependencies(terralib_example_ceditor terralib_mod_vm_lua)
