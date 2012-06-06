
find_package(Boost 1.47 COMPONENTS filesystem system REQUIRED)
set (DEP_LIBS ${Boost_LIBRARIES})
set (DEP_INCLUDES ${Boost_INCLUDE_DIRS})

find_package(GDAL)
if(GDAL_FOUND)
	list (APPEND DEP_LIBS ${GDAL_LIBRARY})
	list (APPEND DEP_INCLUDES ${GDAL_INCLUDE_DIR})
endif()

# Definitions for windows compiling
if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DTEOGRDLL -DBOOST_ALL_NO_LIB -DBOOST_FILESYSTEM_VERSION=3)
endif(WIN32)

list (APPEND DEP_LIBS 
    terralib_common
		terralib_dataaccess
		terralib_datatype
		terralib_geometry
		terralib_memory
		terralib_plugin
    )

# Select the source and header files
file(GLOB SRCS ${SRCDIR}/*.cpp)
file(GLOB HDRS ${SRCDIR}/*.h)

#exporting module information
exportModuleInformation("ogr" "${SRCDIR}" "ogr")