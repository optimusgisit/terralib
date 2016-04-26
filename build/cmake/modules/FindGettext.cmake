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
#  Description: Find Gettext tools and macros for creating translations and templates
#
#  Author: Matheus Cavassan Zaglia
#
#  Notes:
#
#    There are three files created with the macros provided in this module:
#
#    POT File -> Template for translations, do not translate in this file.
#    PO  File -> Created from POT file, translations are made in this file.
#    MO  File -> Binary file of the PO file, this file will be used to translate your application.
#
#    The following macros use the GNU Gettext.
#    The GNU Gettext is available for Windows, Linux and MAC OS X at: https://www.gnu.org/software/gettext/
#
#  Usage:
#
#    add_dependencies(module translations translations_pot)
#
#    GETTEXT_CREATE_TEMPLATE(module                                         <--- Name of the POT file
#                            "TE_TR"                                        <--- Keyword that will be searched
#                            "${TERRALIB_ABSOLUTE_ROOT_DIR}/unittest/core/" <--- Where the macro will search for the keyword
#                            "${CMAKE_CURRENT_BINARY_DIR}")                 <--- Destination for the POT file
#
#    GETTEXT_CREATE_TRANSLATION(module                                      <--- Name of the POT file
#                               "pt_BR.UTF-8"                               <--- Translation language
#                               "${CMAKE_CURRENT_BINARY_DIR}")              <--- Destination for the translations files

FIND_PROGRAM(GETTEXT_MSGMERGE_EXECUTABLE msgmerge)
FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)
FIND_PROGRAM(GETTEXT_MSGINIT_EXECUTABLE msginit)
FIND_PROGRAM(XGETTEXT_EXECUTABLE xgettext)

IF (GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_MSGFMT_EXECUTABLE AND GETTEXT_MSGINIT_EXECUTABLE)
  SET(GETTEXT_FOUND TRUE)
ELSE (GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_MSGFMT_EXECUTABLE AND GETTEXT_MSGINIT_EXECUTABLE)
  SET(GETTEXT_FOUND FALSE)
  IF (GetText_REQUIRED)
    MESSAGE(FATAL_ERROR "GetText not found")
  ENDIF (GetText_REQUIRED)
ENDIF (GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_MSGFMT_EXECUTABLE AND GETTEXT_MSGINIT_EXECUTABLE)
IF(XGETTEXT_EXECUTABLE)
  SET(XGETTEXT_FOUND TRUE)
ELSE(XGETTEXT_EXECUTABLE)
  MESSAGE(STATUS "xgettext not found.")
  SET(XGETTTEXT_FOUND FALSE)
ENDIF(XGETTEXT_EXECUTABLE)
IF(XGETTEXT_FOUND)

IF (WIN32)
   STRING(REGEX REPLACE "/" "\\\\" XGETTEXT_EXECUTABLE ${XGETTEXT_EXECUTABLE})
   STRING(REGEX REPLACE "/" "\\\\" GETTEXT_MSGFMT_EXECUTABLE ${GETTEXT_MSGFMT_EXECUTABLE})
   STRING(REGEX REPLACE "/" "\\\\" GETTEXT_MSGMERGE_EXECUTABLE ${GETTEXT_MSGMERGE_EXECUTABLE})
   STRING(REGEX REPLACE "/" "\\\\" GETTEXT_MSGINIT_EXECUTABLE ${GETTEXT_MSGINIT_EXECUTABLE})
ENDIF (WIN32)

macro(GETTEXT_CREATE_TEMPLATE pot_file keyword directory output_path)

  get_filename_component(_absDir ${directory} ABSOLUTE)
  set(_absPot "${output_path}/${pot_file}")
  if (UNIX)
    add_custom_command(OUTPUT "${pot_file}.pot"
                       COMMAND test -d ${_absPot}|| mkdir -p ${_absPot}
                       COMMAND test -e ${_absPot}/${pot_file}.pot || touch ${_absPot}/${pot_file}.pot
                       COMMAND find `pwd` -name '*.cpp' -print > ${_absPot}/${pot_file}.txt
                       COMMAND ${XGETTEXT_EXECUTABLE} --from-code=UTF-8 --keyword=${keyword} -C -j -f ${_absPot}/${pot_file}.txt -o ${_absPot}/${pot_file}.pot
                       COMMAND rm ${_absPot}/${pot_file}.txt
                       COMMENT "Generating POT file..."
                       WORKING_DIRECTORY "${_absDir}"
                       )
  endif(UNIX)

#  if(WIN32)
#    add_custom_command(OUTPUT "${pot_file}.pot"
#                       COMMAND if not exist ${_absPot} mkdir -p ${_absPot}
#                       COMMAND if not exist ${_absPot}/${pot_file}.pot echo. > ${_absPot}/${pot_file}.pot
#                       COMMAND dir *.cpp /s /b > ${_absPot}/${pot_file}.txt
#                       COMMAND ${XGETTEXT_EXECUTABLE} --from-code=UTF-8ls
#                       --keyword=${keyword} -C -j -f ${_absPot}/${pot_file}.txt -o ${_absPot}/${pot_file}.pot
#                       COMMENT "Generating POT file..."
#                       WORKING_DIRECTORY "${_absDir}"
#                       )
#  endif(WIN32)
  add_custom_target(translations_pot
                    DEPENDS "${pot_file}.pot")
endmacro(GETTEXT_CREATE_TEMPLATE)


macro(GETTEXT_CREATE_TRANSLATION pot_file locale pot_path output_path)
  get_filename_component(_absPot ${pot_path} ABSOLUTE)
  set(_absPot "${_absPot}/${pot_file}")
  if(UNIX)
    add_custom_command(OUTPUT "${pot_file}_${locale}.po" "${pot_file}.mo"
                        COMMAND test -e ${_absPot}/${pot_file}_${locale}.po || ${GETTEXT_MSGINIT_EXECUTABLE} -l ${locale} -i ${_absPot}/${pot_file}.pot  -o ${_absPot}/${pot_file}_${locale}.po
                        COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} -U --backup=none -q --lang=${locale} ${_absPot}/${pot_file}_${locale}.po ${_absPot}/${pot_file}.pot
                        COMMAND test -d ${CMAKE_BINARY_DIR}/translations/${locale}/LC_MESSAGES/ || mkdir -p ${CMAKE_BINARY_DIR}/translations/${locale}/LC_MESSAGES/
                        COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${output_path}/${locale}/LC_MESSAGES/${pot_file}.mo ${_absPot}/${pot_file}_${locale}.po
                        COMMENT "Generating translations files..."
                        WORKING_DIRECTORY ${_absDir}
                        )
  endif(UNIX)

#  if(WIN32)
#   add_custom_command(OUTPUT "${pot_file}_${locale}.po" "${pot_file}.mo"
#                       COMMAND if not exist ${_absPot}/${pot_file}_${locale}.po ${GETTEXT_MSGINIT_EXECUTABLE} --no-translator -l ${locale} -i ${_absPot}/${pot_file}.pot  -o ${_absPot}/${pot_file}_${locale}.po
#                       COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} -U --backup=none -q --lang=${locale} ${_absPot}/${pot_file}_${locale}.po ${_absPot}/${pot_file}.pot
#                       COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${pot_file}.mo ${_absPot}/${pot_file}_${locale}.po
#                       COMMENT "Generating translations files..."
#                       WORKING_DIRECTORY ${_absDir}
#                       )
#  endif(WIN32)
  add_custom_target(translations DEPENDS "${pot_file}.mo" "${pot_file}_${locale}.po" )

endmacro(GETTEXT_CREATE_TRANSLATION)

ENDIF(XGETTEXT_FOUND)
