set ( CPACK_PACKAGE_DESCRIPTION_SUMMARY "TerraLib / TerraView binary instalation." )
set ( CPACK_PACKAGE_VENDOR "www.dpi.inpe.br" )
set ( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/../../README" )
set ( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/../../COPYING" )
set ( CPACK_PACKAGE_VERSION "5.0.0" )
set ( CPACK_PACKAGE_VERSION_MAJOR "5" )
set ( CPACK_PACKAGE_VERSION_MINOR "0" )
set ( CPACK_PACKAGE_VERSION_PATCH "0" )
set ( CPACK_PACKAGE_CONTACT "terralib-devel@dpi.inpe.br" )
set ( CPACK_PACKAGE_NAME "terralib-${CPACK_PACKAGE_VERSION}" )
set ( CPACK_PACKAGE_INSTALL_DIRECTORY "terralib-${CPACK_PACKAGE_VERSION}" )
set ( CPACK_PACKAGE_FILE_NAME "terralib-${CPACK_PACKAGE_VERSION}" )
set ( CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/../../images/bmp/logo\\\\terralib_installer_logo_150x57.bmp")

if(WIN32)
  include (${T5_MODULES_PATH}/NSISPackage.cmake)
else ()
  set (CPACK_STRIP_FILES "")
  set (CPACK_SOURCE_STRIP_FILES "")
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

set ( CPACK_COMPONENTS_ALL HEADERS BINARIES LIBRARIES CMAKE)

include (CPack)