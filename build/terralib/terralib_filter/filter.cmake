find_package(Boost ${_Boost_VERSION} REQUIRED)
if(Boost_FOUND)
  list( APPEND TE_DEP_INCLUDES ${Boost_INCLUDE_DIRS})
endif()

#Definitions for windows compiling
if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DTEFEDLL -DBOOST_ALL_NO_LIB)
endif(WIN32)

set(TE_DEP_LIBS 
    terralib_common
		terralib_dataaccess
		terralib_datatype
		terralib_geometry
		terralib_gml
		terralib_srs
    )

# Select the source and header files
file(GLOB SRCS ${SRCDIR}/*.cpp)
file(GLOB HDRS ${SRCDIR}/*.h)
 
#exporting module information
exportModuleInformation("filter" "${SRCDIR}" "fe")