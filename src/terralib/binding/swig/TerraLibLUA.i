/* 
* File: TerraLibLUA.i 
* 
*/ 
 
%module terralib_mod_binding_lua 

#define TECOMMONEXPORT

%import "terralib/common/Singleton.h"

%include "terralib/common/Singleton.h"

namespace te {
  namespace common {
    %template(TeSingleton) Singleton < TerraLib >;
  }
}

typedef te::common::Singleton< TerraLib > TeSingleton;

%include stl.i
%include typemaps.i

%apply unsigned int *INOUT {std::size_t& size}
%apply const string& { const std::string& }

%include "terralib/common/Enums.h"  
%include "terralib/common/TerraLib.h"

%{  
#include <terralib/Config.h>
#include <terralib/common/Enums.h>  
#include <terralib/common/TerraLib.h>
%}

/* Include SRS module to the bind. */
%include common/SRS.i

/* Include Data Type module to the bind. */
%include common/DataType.i

/* Include Geometry module to the bind. */   
%include common/Geometry.i

/* Include Data Access module to the bind. */
%include common/DataAccess.i 