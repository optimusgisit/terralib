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
  \file HorizontalRulerItem.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "HorizontalRulerItem.h"
#include "ItemController.h"
#include "Context.h"
#include "AbstractScene.h"
#include "ItemModelObservable.h"
#include "../../../color/RGBAColor.h"
#include "../../../../qt/widgets/Utils.h"
#include "../../../../geometry/Envelope.h"
#include "../../../../common/STLUtils.h"

te::layout::HorizontalRulerItem::HorizontalRulerItem( ItemController* controller, Observable* o ) :
  ObjectItem(controller, o)
{
  m_printable = false;
}

te::layout::HorizontalRulerItem::~HorizontalRulerItem()
{

}

void te::layout::HorizontalRulerItem::updateObserver( ContextItem context )
{
  te::color::RGBAColor** rgba = context.getPixmap();

  if(!rgba)
    return;

  Utils* utils = Context::getInstance()->getUtils();

  te::gm::Envelope box = utils->viewportBox(m_model->getBox());

  QPixmap pixmap;
  QImage* img = 0;

  if(rgba)
  {
    img = te::qt::widgets::GetImage(rgba, box.getWidth(), box.getHeight());
    pixmap.fill(Qt::transparent);
    pixmap = QPixmap::fromImage(*img);
  }

  te::common::Free(rgba, box.getHeight());
  if(img)
    delete img;

  setPixmap(pixmap);
  update();
}

