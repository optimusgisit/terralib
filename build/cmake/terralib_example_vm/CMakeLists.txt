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
#  Author: Frederico Bedê
#

include_directories(${TERRALIB_ABSOLUTE_ROOT_DIR}/src)

set (TERRALIB_VM_DIR ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/vm)

file(GLOB TERRALIB_EXAMPLE_VM_SRC_FILES ${TERRALIB_VM_DIR}/*.cpp)

add_executable(terralib_example_vm ${TERRALIB_EXAMPLE_VM_SRC_FILES})

target_link_libraries(terralib_example_vm terralib_mod_vm
                                          terralib_mod_common
                                          terralib_mod_core)

add_dependencies(terralib_example_vm terralib_mod_vm_lua)
