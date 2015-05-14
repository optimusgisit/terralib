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
  \file GridMapItem.h
   
   \brief Class that represents a graphic GridMap. 
      Its coordinate system is the same of scene (millimeters). 
      This is also son of ItemObserver and ObjectItem, so it can become observer of a model (Observable). 

  \ingroup layout
*/

#ifndef __TERRALIB_LAYOUT_INTERNAL_GRIDMAP_ITEM_H 
#define __TERRALIB_LAYOUT_INTERNAL_GRIDMAP_ITEM_H

// TerraLib
#include "ObjectItem.h"
#include "../../core/Config.h"

namespace te
{
  namespace layout
  {
    class Observable;

    /*!
    \brief Class that represents a graphic GridMap. 
        Its coordinate system is the same of scene (millimeters). 
        He is also the son of ItemObserver and ObjectItem, so it can become observer of a model (Observable). 
	  
	    \ingroup layout

	    \sa te::layout::ObjectItem
	  */
    class TELAYOUTEXPORT GridMapItem : public ObjectItem
    {
      public:

        /*!
          \brief Constructor

          \param controller "Controller" part of MVC component
          \param o "Model" part of MVC component
        */ 
        GridMapItem( ItemController* controller, Observable* o );

        /*!
          \brief Destructor
         */
        virtual ~GridMapItem();
        
        virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

        /*!
          \brief Reimplemented from ParentItem
         */
        virtual QRectF boundingRect() const;
                        
      protected:

        /*!
          \brief Reimplemented from QGraphicsItem
         */
        virtual QVariant	itemChange ( QGraphicsItem::GraphicsItemChange change, const QVariant & value );
                        
        virtual void drawGrid(QPainter* painter);

        virtual void drawVerticalLines();

        virtual void drawHorizontalLines();
                
        virtual void drawText( QPointF point, QPainter* painter, std::string text, bool displacementLeft = false, bool displacementRight = false);

        virtual void recalculateBoundingRect();

        double m_maxWidthTextMM;
        double m_maxHeigthTextMM;
        double m_onePointMM;
        bool   m_changeSize;
    };
  }
}

#endif

