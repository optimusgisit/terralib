/*  Copyright (C) 2001-2014 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled applications.

    TerraLib is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraLib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with TerraLib. See COPYING. If not, write to
    TerraLib Team at <terralib-team@terralib.org>.
 */

/*!
  \file AbstractBuildGraphicsOutside.h
   
  \brief Abstract class for build MVC widgets. 
    
  \ingroup layout
*/

#ifndef __TERRALIB_LAYOUT_INTERNAL_ABSTRACT_BUILD_GRAPHICS_OUTSIDE_H
#define __TERRALIB_LAYOUT_INTERNAL_ABSTRACT_BUILD_GRAPHICS_OUTSIDE_H

// TerraLib
#include "Config.h"

namespace te
{
  namespace layout
  {

	/*!
      \class AbstractBuildGraphicsOutside

      \brief Abstract class for build MVC widgets.  

      \ingroup layout
     */

    class TELAYOUTEXPORT AbstractBuildGraphicsOutside
    {
      public:
	  
	      /*!
          \brief Constructor
        */       
        AbstractBuildGraphicsOutside();
		
		    /*!
          \brief Destructor
        */
        virtual ~AbstractBuildGraphicsOutside();
    };
  }
}

#endif