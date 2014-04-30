/*  Copyright (C) 2014-2014 National Institute For Space Research (INPE) - Brazil.

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
  \file MapItem.h
   
  \brief 

  \ingroup layout
*/

#ifndef __TERRALIB_LAYOUT_INTERNAL_MAP_ITEM_H 
#define __TERRALIB_LAYOUT_INTERNAL_MAP_ITEM_H

// Qt
#include <QGraphicsProxyWidget>
#include <QVariant>

// TerraLib
#include "ItemObserver.h"
#include "../../../../qt/widgets/canvas/MultiThreadMapDisplay.h"
#include "../../../../maptools/AbstractLayer.h"

class QGraphicsSceneMouseEvent;

namespace te
{
  namespace layout
  {
    class Observable;
    class ItemController;

    class MapItem : public QGraphicsProxyWidget, public ItemObserver
    {
      Q_OBJECT //for slots/signals

      public:

        MapItem( ItemController* controller, Observable* o );
        virtual ~MapItem();
        
        virtual void updateObserver(ContextItem context);

        void setPixmap( const QPixmap& pixmap );

        virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem* option, QWidget * widget = 0 );

        //Mandatory implementation methods
        virtual void setRect(QRectF rect);
                        
    protected:
      
      virtual void	dropEvent ( QGraphicsSceneDragDropEvent * event );

      //Override method for using moves, selected, etc., 
      //which are implemented by default in QGraphicsItem
      virtual void	mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
      virtual void	mousePressEvent ( QGraphicsSceneMouseEvent * event );
      virtual void	mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
      virtual void	dragEnterEvent ( QGraphicsSceneDragDropEvent * event );
      virtual void	dragLeaveEvent ( QGraphicsSceneDragDropEvent * event );
      virtual void	dragMoveEvent ( QGraphicsSceneDragDropEvent * event );
      virtual void resizeEvent ( QGraphicsSceneResizeEvent * event );
      
      virtual te::gm::Coord2D getPosition();

      virtual void setPosition( const double& x, const double& y );
      
      virtual void drawSelection( QPainter* painter );

    protected:

      QPixmap m_pixmap;
      QRectF  m_rect;//In local coordinate
      
      te::qt::widgets::MultiThreadMapDisplay* m_mapDisplay;
      bool m_grabbedByWidget;
    };
  }
}

#endif