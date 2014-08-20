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
  \file ImageModel.h
   
  \brief 

  \ingroup layout
*/

#ifndef __TERRALIB_LAYOUT_INTERNAL_IMAGE_MODEL_H
#define __TERRALIB_LAYOUT_INTERNAL_IMAGE_MODEL_H

// TerraLib
#include "../core/pattern/mvc/ItemModelObservable.h"
#include "../core/ContextItem.h"
#include "../../maptools/Enums.h"

// STL
#include <string>

namespace te
{
  namespace layout
  {
    class ImageModel : public ItemModelObservable
    {
      public:

        ImageModel();
        virtual ~ImageModel();

        virtual te::layout::Properties* getProperties() const;

        virtual void updateProperties(te::layout::Properties* properties);   

        virtual void draw( ContextItem context );

        virtual void setFileName(std::string fileName);

        virtual std::string getFileName();
        
        virtual std::string getFileExtension();

        virtual te::map::ImageType getFileType();

      protected:
        
        std::string m_fileName;
        std::string m_fileExtension;
        te::map::ImageType m_imgType;
    };
  }
}

#endif //__TERRALIB_LAYOUT_INTERNAL_IMAGE_MODEL_H