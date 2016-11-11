/* 
* File: TerraLibLUA.i 
* 
*/ 
 
%module terralib_mod_binding_lua

%include stl.i
%include std_string.i
%include std_vector.i
%include std_map.i
%include lua/typemaps.i
%include typemaps.i
%include std_except.i

%catches(std::exception);

#define TECOMMONEXPORT

%include "terralib/common/Singleton.h"

namespace te 
{
  namespace common 
  {
    %template(TeSingleton) Singleton < TerraLib >;
  }
}

typedef te::common::Singleton< TerraLib > TeSingleton;

%apply unsigned int *INOUT {std::size_t& size}
%apply const string& { const std::string& }

%include "terralib/common/Enums.h"  
%include "terralib/common/TerraLib.h"

%{  
/* #include <terralib/Config.h> */
#include <terralib/common/Enums.h>  
#include <terralib/common/Exception.h>
#include <terralib/common/TerraLib.h>

// Boost
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

using te::common::Exception;

/* Defining a function for generating randomic ids. */
static std::string GetRandomicId()
{
  //Generates a randon id to the data source
  boost::uuids::basic_random_generator<boost::mt19937> gen;
  boost::uuids::uuid u = gen();
  return boost::uuids::to_string(u);
}

%}

/* Include Common module to the bind. */
%include common/Common.i

/* Include SRS module to the bind. */
%include common/SRS.i

/* Include Data Type module to the bind. */
%include common/DataType.i

/* Include Geometry module to the bind. */   
%include common/Geometry.i

/* Include Raster module to the bind. */   
%include common/Raster.i

/* Include Data Access module to the bind. */
%include common/DataAccess.i 
%include lua/DataAccess.i

/* Include Plugin module to the bind. 
%include common/Plugin.i */

// Wrap function
std::string GetRandomicId();
