set ( CPACK_PACKAGE_DESCRIPTION_SUMMARY "TerraLib / TerraView binary instalation." )
set ( CPACK_PACKAGE_VENDOR "www.dpi.inpe.br" )
set ( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/../README" )
set ( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/../COPYING" )
set ( CPACK_PACKAGE_VERSION_MAJOR "${TE_MAJOR_VERSION}" )
set ( CPACK_PACKAGE_VERSION_MINOR "${TE_MINOR_VERSION}" )
set ( CPACK_PACKAGE_VERSION_PATCH "${TE_PATCH_VERSION}" )
set ( CPACK_PACKAGE_VERSION "${TE_STRING_VERSION}" )
set ( CPACK_PACKAGE_CONTACT "terralib-devel@dpi.inpe.br" )
set ( CPACK_PACKAGE_NAME "terralib" )
set ( CPACK_PACKAGE_INSTALL_DIRECTORY "terralib-${CPACK_PACKAGE_VERSION}" )
set ( CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/../resources/images\\\\terralib_installer_logo_150x57.bmp" )

getPackageName(pname)
set ( CPACK_PACKAGE_FILE_NAME ${pname} )

if(WIN32)
  include (${TE_MODULES_PATH}/NSISPackage.cmake)
else ()
  set (CPACK_STRIP_FILES "")
  set (CPACK_SOURCE_STRIP_FILES "")
  
  if(UNIX AND NOT APPLE)
    include (${TE_MODULES_PATH}/DEBPackage.cmake)
  endif()
endif()

set ( CPACK_COMPONENT_GROUP_TLIB_DESCRIPTION "Terralib files." )
set ( CPACK_COMPONENT_GROUP_TLIB_DISPLAY_NAME "Terralib" )

set ( CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C++ headers" )
set ( CPACK_COMPONENT_HEADERS_DESCRIPTION "Terralib header files." )
set ( CPACK_COMPONENT_HEADERS_GROUP "tlib")
set ( CPACK_COMPONENT_HEADERS_INSTALL_TYPES Developer Full )

set ( CPACK_COMPONENT_BINARIES_DISPLAY_NAME "Binaries" )
set ( CPACK_COMPONENT_BINARIES_DESCRIPTION "Platform-dependent binaries." )
set ( CPACK_COMPONENT_BINARIES_GROUP "tlib")
set ( CPACK_COMPONENT_BINARIES_REQUIRED "1")

set ( CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries" )
set ( CPACK_COMPONENT_LIBRARIES_DESCRIPTION "Platform-dependent libraries." )
set ( CPACK_COMPONENT_LIBRARIES_GROUP "tlib")
set ( CPACK_COMPONENT_LIBRARIES_INSTALL_TYPES Developer Full )

set ( CPACK_COMPONENT_CMAKE_DISPLAY_NAME "CMake files" )
set ( CPACK_COMPONENT_CMAKE_DESCRIPTION "CMake files." )
set ( CPACK_COMPONENT_CMAKE_GROUP "tlib")
set ( CPACK_COMPONENT_CMAKE_INSTALL_TYPES Developer Full )

set ( CPACK_COMPONENT_DOCS_DISPLAY_NAME "API documentation" )
set ( CPACK_COMPONENT_DOCS_DESCRIPTION "API documentation generated by Doxygen." )
set ( CPACK_COMPONENT_DOCS_GROUP "tlib")
set ( CPACK_COMPONENT_DOCS_INSTALL_TYPES Developer Full )

set ( CPACK_COMPONENT_PLUGINS_DISPLAY_NAME "TerraLib Plug-ins" )
set ( CPACK_COMPONENT_PLUGINS_DESCRIPTION "Plug-ins for TerraLib-based applications." )
set ( CPACK_COMPONENT_PLUGINS_GROUP "tlib")
set ( CPACK_COMPONENT_PLUGINS_INSTALL_TYPES Developer Full )

set ( CPACK_COMPONENTS_ALL HEADERS BINARIES LIBRARIES PLUGINS CMAKE DOCS)

include (CPack)
