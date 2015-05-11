/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

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
  \file PaperItem.h
   
   \brief Class that represents a graphic sheet of paper. 
   Its coordinate system is the same of scene (millimeters). 
   This is also son of ItemObserver and ObjectItem, so it can become observer of a model (Observable). 

  \ingroup layout
*/

#ifndef __TERRALIB_LAYOUT_INTERNAL_PAPER_ITEM_H
#define __TERRALIB_LAYOUT_INTERNAL_PAPER_ITEM_H

// TerraLib
#include "ObjectItem.h"
#include "../../core/Config.h"

namespace te
{
  namespace layout
  {
    class Observable;

    /*!
    \brief Class that represents a graphic sheet of paper. 
    Its coordinate system is the same of scene (millimeters). 
    This is also son of ItemObserver and ObjectItem, so it can become observer of a model (Observable). 
	  
	  \ingroup layout

    \sa te::layout::ObjectItem
	  */
    class TELAYOUTEXPORT PaperItem : public ObjectItem
    {
      public:

        /*!
          \brief Constructor

          \param controller "Controller" part of MVC component
          \param o "Model" part of MVC component
        */ 
        PaperItem( ItemController* controller, Observable* o );

        /*!
          \brief Destructor
         */
        virtual ~PaperItem();
        
        /*!
          \brief Mandatory implementation from QGraphicsItem
         */
        virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

      protected:

        virtual void drawPaper(QPainter * painter);
    };
  }
}

#endif
