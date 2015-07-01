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
  \file SVGItem.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "SVGItem.h"
#include "../../core/pattern/mvc/ItemController.h"
#include "../../core/AbstractScene.h"
#include "../../core/pattern/mvc/Observable.h"
#include "../../../color/RGBAColor.h"
#include "../../../qt/widgets/Utils.h"
#include "../../../geometry/Envelope.h"
#include "../../../common/STLUtils.h"
#include "../../item/PointModel.h"
#include "../../core/enum/EnumPointType.h"
#include "../../item/SVGModel.h"

// STL
#include <cmath>

// Qt
#include <QColor>
#include <QPen>
#include <QPolygonF>
#include <QPainterPath>

te::layout::SVGItem::SVGItem( ItemController* controller, Observable* o ) :
  ObjectItem(controller, o)
{
  m_nameClass = std::string(this->metaObject()->className());
}

te::layout::SVGItem::~SVGItem()
{

}

void te::layout::SVGItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0 */ )
{
  Q_UNUSED( option );
  Q_UNUSED( widget );
  if ( !painter )
  {
    return;
  }

  drawBackground(painter);

  drawSVG(painter);
  
  //Draw Selection
  if (option->state & QStyle::State_Selected)
  {
    drawSelection(painter);
  }
}

void te::layout::SVGItem::drawSVG( QPainter * painter )
{
  SVGModel* model = dynamic_cast<SVGModel*>(m_model);
  if(!model)
  {
    return;
  }

  painter->save();

  painter->drawRect(boundingRect());
  painter->drawText(QPointF(boundingRect().center().x(), boundingRect().center().y()), "SVG Item");
  
  painter->restore();
}


