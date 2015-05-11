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
  \file BalloonItem.h
   
  \brief 

  \ingroup layout
*/

#ifndef __TERRALIB_LAYOUT_INTERNAL_BALLOON_ITEM_H 
#define __TERRALIB_LAYOUT_INTERNAL_BALLOON_ITEM_H

// TerraLib
#include "ObjectItem.h"
#include "../../core/Config.h"

// STL
#include <vector>

// Qt
#include <QGraphicsSceneMouseEvent>
#include <QPointF>

namespace te
{
  namespace layout
  {
    class Observable;

    class TELAYOUTEXPORT BalloonItem : public ObjectItem
    {
      public:

        BalloonItem( ItemController* controller, Observable* o );

        virtual ~BalloonItem();

        virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
        
      protected:

        virtual void drawBalloon(QPainter* painter);

        virtual void	mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
        
        virtual void  mousePressEvent ( QGraphicsSceneMouseEvent * event );

        virtual void	mouseMoveEvent ( QGraphicsSceneMouseEvent * event );

        QPointF     m_initPoint;
        QPointF     m_endPoint;
    };
  }
}

#endif



