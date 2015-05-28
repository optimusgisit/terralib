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
  \file StarItem.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "StarItem.h"
#include "../../core/pattern/mvc/ItemController.h"
#include "../../core/AbstractScene.h"
#include "../../core/pattern/mvc/Observable.h"
#include "../../../color/RGBAColor.h"
#include "../../../qt/widgets/Utils.h"
#include "../../../geometry/Envelope.h"
#include "../../../common/STLUtils.h"
#include "../../item/StarModel.h"
#include "geos/platform.h"

te::layout::StarItem::StarItem( ItemController* controller, Observable* o ) :
  ObjectItem(controller, o)
{  
  m_nameClass = std::string(this->metaObject()->className());
}

te::layout::StarItem::~StarItem()
{

}

void te::layout::StarItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0 */ )
{
	Q_UNUSED( option );
	Q_UNUSED( widget );
	if ( !painter )
	{
		return;
	}

	if(m_resizeMode)
	{
		ObjectItem::paint(painter, option, widget);
		return;
	}

	drawBackground(painter);

	StarModel* model = dynamic_cast<StarModel*>(m_model);

	if(model)
	{
		EnumStarType* enumScale = model->getEnumStarType();

		if(model->getCurrentStarType() == enumScale->getStar1Type())
		{
			drawStar1(painter);
		}
		if(model->getCurrentStarType() == enumScale->getStar2Type())
		{
			drawStar2(painter);
		}
		if(model->getCurrentStarType() == enumScale->getStar3Type())
		{
			drawStar3(painter);
		}
		if(model->getCurrentStarType() == enumScale->getStar4Type())
		{
			drawStar4(painter);
		}
	}

	drawBorder(painter);

	//Draw Selection
	if (option->state & QStyle::State_Selected)
	{
		drawSelection(painter);
	}
}

void te::layout::StarItem::drawStar1( QPainter * painter )
{
	StarModel* model = dynamic_cast<StarModel*>(m_model);
	if(!model)
	{
		return;
	}

	painter->save();

	double halfW = boundingRect().width() / 4.;
	double w = boundingRect().width() / 2.;
	QPolygonF star;
	qreal const c = halfW;
	qreal const d = w;
	bool inner = true;
	QPointF center;
	for ( qreal i = 0 ; i < 2*M_PI; i += M_PI/5.0, inner=!inner ) {
		qreal const f = inner ? c : d;
		star << QPointF( f * std::cos(i), f * std::sin(i) );
		if(i == 0)
		{
			center = QPointF( f * std::cos(i), f * std::sin(i) );
		}
	}
	star << center;
	star.translate(boundingRect().center());
	painter->drawPolygon(star);
	painter->restore();
}

void te::layout::StarItem::drawStar2(QPainter * painter)
{
	StarModel* model = dynamic_cast<StarModel*>(m_model);
	if(!model)
	{
		return;
	}

	painter->save();

	double halfW = boundingRect().width() / 4.;
	double w = boundingRect().width() / 2.;
	QPolygonF star;
	qreal const c = halfW;
	qreal const d = w;
	bool inner = true;
	QPointF center;
	for ( qreal i = 0 ; i < 2*M_PI; i += M_PI/6.0, inner=!inner ) {
		qreal const f = inner ? c : d;
		star << QPointF( f * std::cos(i), f * std::sin(i) );
		if(i == 0)
		{
			center = QPointF( f * std::cos(i), f * std::sin(i) );
		}
	}
	star << center;
	star.translate(boundingRect().center());
	painter->drawPolygon(star);
	painter->restore();
}

void te::layout::StarItem::drawStar3(QPainter * painter)
{
	StarModel* model = dynamic_cast<StarModel*>(m_model);
	if(!model)
	{
		return;
	}

	painter->save();

	QPolygonF star;
	double halfW = boundingRect().width() / 4.;
	double w = boundingRect().width() / 2.;
	qreal const c = halfW;
	qreal const d = w;
	bool inner = true;
	for ( qreal i=0; i<2*M_PI; i+=M_PI/7.0, inner=!inner ) {
		qreal const f = inner ? c : d;
		star << QPointF( f * cos(i), f * sin(i) );
	}
	star.translate(boundingRect().center());
	painter->drawPolygon(star);
	painter->restore();
}

void te::layout::StarItem::drawStar4(QPainter * painter)
{
	StarModel* model = dynamic_cast<StarModel*>(m_model);
	if(!model)
	{
		return;
	}

	painter->save();

	QPolygonF star;
	double halfW = boundingRect().width() / 4.;
	double w = boundingRect().width() / 2.;
	qreal const c = halfW;
	qreal const d = w;
	bool inner = true;
	for ( qreal i=0; i<2*M_PI; i+=M_PI/8.0, inner=!inner ) {
		qreal const f = inner ? c : d;
		star << QPointF( f * cos(i), f * sin(i) );
	}
	star.translate(boundingRect().center());
	painter->drawPolygon(star);
	painter->restore();
}
