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
#  Description: The C++ plugin example.
#
#  Author: Gilberto Ribeiro de Queiroz <gribeiro@dpi.inpe.br>
#          Juan Carlos P. Garrido <juan@dpi.inpe.br>
#          Frederico Augusto T. Bede <frederico.bede@funcate.org.br>
#


include_directories(${Boost_INCLUDE_DIR} ${TERRALIB_ABSOLUTE_ROOT_DIR}/src)

file(GLOB TERRALIB_SRC_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/cppplugin/helloworld/*.cpp)
file(GLOB TERRALIB_HDR_FILES ${TERRALIB_ABSOLUTE_ROOT_DIR}/examples/cppplugin/helloworld/*.h)

add_library(terralib_example_cppplugin_helloworld SHARED ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES})

target_link_libraries(terralib_example_cppplugin_helloworld terralib_mod_common
                                                            )

install(FILES ${TERRALIB_SRC_FILES} ${TERRALIB_HDR_FILES}
        DESTINATION ${TERRALIB_DESTINATION_EXAMPLES}/cppplugin/helloworld COMPONENT devel)

