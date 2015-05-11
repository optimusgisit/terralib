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
  \file GridMapItem.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "GridMapItem.h"
#include "../../core/pattern/mvc/ItemController.h"
#include "../../core/AbstractScene.h"
#include "../../core/pattern/mvc/Observable.h"
#include "../../../color/RGBAColor.h"
#include "../../../qt/widgets/Utils.h"
#include "../../../geometry/Envelope.h"
#include "../../../common/STLUtils.h"
#include "../../item/GridMapModel.h"
#include "MapItem.h"

//Qt
#include <QStyleOptionGraphicsItem>
#include "../../core/WorldTransformer.h"

te::layout::GridMapItem::GridMapItem( ItemController* controller, Observable* o ) :
  ObjectItem(controller, o),
  m_maxWidthTextMM(0),
  m_maxHeigthTextMM(0),
  m_onePointMM(0.3527777778),
  m_changeSize(false)
{  
  m_nameClass = std::string(this->metaObject()->className());
}

te::layout::GridMapItem::~GridMapItem()
{

}

void te::layout::GridMapItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0 */ )
{
  ObjectItem::paint(painter, option, widget);

  Q_UNUSED( option );
  Q_UNUSED( widget );
  if ( !painter )
  {
    return;
  }
  
  drawBackground(painter);
  
  drawGrid(painter);

  //Draw Selection
  if (option->state & QStyle::State_Selected)
  {
    drawSelection(painter);
  }
}

void te::layout::GridMapItem::drawGrid( QPainter* painter )
{
  GridMapModel* model = dynamic_cast<GridMapModel*>(m_model);
  if(!model)
  {
    return;
  }

  painter->save();

  QRectF parentBound = boundingRect();

  if(parentItem())
  {
    parentBound = parentItem()->boundingRect();
  }

  QPainterPath gridMapPath;
  gridMapPath.setFillRule(Qt::WindingFill);

  int heightRect = (int)parentBound.height();
  int widgetRect = (int)parentBound.width();

  te::color::RGBAColor rgbColor = model->getLineColor();
  QColor cLine(rgbColor.getRed(), rgbColor.getGreen(), rgbColor.getBlue(), rgbColor.getAlpha());

  painter->setPen(QPen(cLine, 0, Qt::SolidLine));

  QFont ft(model->getFontFamily().c_str(), model->getTextPointSize());

  painter->setFont(ft);

  m_maxHeigthTextMM = m_onePointMM * ft.pointSize();

  //QString text = "A";

  for (int i = 0; i <= heightRect; i+=10)
  {
    QLineF lineOne = QLineF(parentBound.topLeft().x(), parentBound.topLeft().y() + i, parentBound.topRight().x(), parentBound.topRight().y() + i);

    QPointF pointInit(parentBound.topLeft().x(), parentBound.topLeft().y() + i - (m_maxHeigthTextMM/2)); //left
    //drawText(pointInit, painter, text.toStdString(), true);
    QPointF pointFinal(parentBound.topRight().x(), parentBound.topRight().y() + i  - (m_maxHeigthTextMM/2)); //right
    //drawText(pointFinal, painter, text.toStdString());

    painter->drawLine(lineOne);

    for (int j = 0; j <= widgetRect; j+=10)
    {
      QLineF lineTwo = QLineF(parentBound.topLeft().x() + j, parentBound.topLeft().y(), parentBound.bottomLeft().x() + j, parentBound.bottomLeft().y());

      QPointF pointInit(parentBound.topLeft().x() + j + (m_maxWidthTextMM/2), boundingRect().topLeft().y()); //upper
      //drawText(pointInit, painter, text.toStdString(), true);
      QPointF pointFinal(parentBound.bottomLeft().x() + j  - (m_maxWidthTextMM/2), parentBound.bottomLeft().y()); //lower
      //drawText(pointFinal, painter, text.toStdString());

      painter->drawLine(lineTwo);
    }    
  }
    
  painter->restore();
}

void te::layout::GridMapItem::drawText( QPointF point, QPainter* painter, std::string text, bool displacementLeft /*= false*/, bool displacementRight /*= false*/ )
{
  painter->save();

  QTransform t = painter->transform();
  QPointF p = t.map(point);

  double zoomFactor = Context::getInstance().getZoomFactor();

  QFont ft = painter->font();
  ft.setPointSize(ft.pointSize() * zoomFactor);
  painter->setFont(ft);

  QFontMetrics fm(ft);
  int width = fm.width(text.c_str());

  QPointF newPoint (p);

  if(displacementLeft)
  {
    newPoint.setX(newPoint.x() - width);    
  }

  if(displacementRight)
  {
    newPoint.setX(newPoint.x() + width);
  }

  QTransform copyT = painter->transform().inverted();
  QPointF copyP = copyT.map(newPoint);
  double widthMM = point.x() - copyP.x();

  if(widthMM > m_maxWidthTextMM)
  {
    m_maxWidthTextMM = widthMM;
    m_changeSize = true;
  }

  //Keeps the size of the text.(Aspect)
  painter->setMatrixEnabled(false);
  painter->drawText(newPoint, text.c_str());
  painter->setMatrixEnabled(true);

  painter->restore();
}

QRectF te::layout::GridMapItem::boundingRect() const
{
  if(parentItem())
  {
    return parentItem()->boundingRect();
  }
  return m_rect;
}

void te::layout::GridMapItem::recalculateBoundingRect()
{
  if(!m_changeSize)
    return;

  if(parentItem())
  {    
    QRectF parentBoundRect = parentItem()->boundingRect();
    if(parentBoundRect.isValid())
    {
      QRectF boundRect = boundingRect();
      double w = parentBoundRect.width() + (m_maxWidthTextMM*2);
      double h = parentBoundRect.height() + (m_maxHeigthTextMM*2);
      if(boundRect.width() != w || boundRect.height() != h)
      {
        prepareGeometryChange();
        QRectF rect(0., 0., w, h);
        setRect(rect);
        
        //update model
        te::gm::Envelope box(m_model->getBox());
        box.m_urx = box.m_llx + w;
        box.m_ury = box.m_lly + h;
        m_controller->setBox(box);
      }
    } 
  }
  m_changeSize = false;
}

QVariant te::layout::GridMapItem::itemChange( QGraphicsItem::GraphicsItemChange change, const QVariant & value )
{
  if(change == QGraphicsItem::ItemParentHasChanged)
  {
    GridMapModel* model = dynamic_cast<GridMapModel*>(m_model);
    if(model)
    {
      if(parentItem())
      {
        MapItem* mapItem = dynamic_cast<MapItem*>(parentItem());
        if(mapItem)
        {
          if(mapItem->getModel())
          {
            model->setMapName(mapItem->getModel()->getName());
          }
        }
      }
    }
  }
  return QGraphicsItem::itemChange(change, value);
}

















