
find_package(Boost ${_Boost_VERSION} REQUIRED)
if(Boost_FOUND)
  set (DEP_INCLUDES ${Boost_INCLUDE_DIRS})
endif()

find_package(Xerces ${_Xerces_VERSION} REQUIRED)
if(Xerces_FOUND)
  list (APPEND DEP_INCLUDES ${XERCES_INCLUDE_DIR})
  set (DEP_LIBS ${XERCES_LIBRARY})  
endif()

#Definitions for windows compiling
if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DTEXERCESDLL -DBOOST_ALL_NO_LIB)
endif(WIN32)

list (APPEND DEP_LIBS 
    terralib_common
		terralib_plugin
		terralib_xml
    )

# Select the source and header files
file(GLOB SRCS ${SRCDIR}/*.cpp)
file(GLOB HDRS ${SRCDIR}/*.h)
 
#exporting module information
exportModuleInformation("xerces" "${SRCDIR}" "xerces")