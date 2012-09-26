# List of versions of 3rd-parties dependencies
set (_Boost_VERSION 1.46)
set (_GDAL_VERSION 1.8.1)
set (_Geos_VERSION 3.2)
set (_PostgreSQL_VERSION 9)
set (_Qwt_VERSION 6.0)
set (_Proj4_VERSION 4.7)
set (_Xerces_VERSION 3.1)
set (_Qt4_VERSION 4.6)

set (PGIS_ENABLED FALSE)

if(UNIX AND NOT APPLE)
  set (_Boost_LIBS libboost_system libboost_filesystem libboost_date_time libboost_thread)
  set (_GDAL_LIBS libgdal)
  set (_Geos_LIBS libgeos)
  set (_PostgreSQL_LIBS libpq)
  set (_Qwt_LIBS libqwt)
  set (_Proj4_LIBS libproj)
  set (_Xerces_LIBS libxerces-c)
  set (_Qt4_LIBS libQtCore libQtGui)
  set (_DL_LIBS libdl)
  set (_Grib_LIBS libgrib_api)
  set (_Log4Cxx_LIBS liblog4cxx)
endif()

# Macro
# for Linux search codes: tested on Ubuntu 11.04
MACRO (getLibrariesNames packName libs)
  if(_${packName}_VERSION)
    list (LENGTH _${packName}_LIBS numLibs)
    set (ret "")
    set (count 0)
    math (EXPR lim ${numLibs}-1)
    
    foreach(lib ${_${packName}_LIBS})
      set (ret "${ret} ${lib} (>= ${_${packName}_VERSION})")
      
      if(count LESS lim)
        set (ret ${ret},)
        math (EXPR count ${count}+1)
      endif()
    endforeach()
  else()
    set (ret " ${_${packName}_LIBS}")
  endif()
  
  set (${libs} ${ret}) 
ENDMACRO (getLibrariesNames)
