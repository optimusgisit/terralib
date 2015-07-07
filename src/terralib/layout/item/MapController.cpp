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
  \file MapController.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "MapController.h"
#include "../core/ContextItem.h"
#include "MapModel.h"
#include "../core/pattern/factory/AbstractItemFactory.h"
#include "../core/pattern/singleton/Context.h"
#include "../core/pattern/mvc/ItemModelObservable.h"
#include "../core/pattern/factory/ItemParamsCreate.h"
#include "../core/pattern/mvc/ItemObserver.h"
#include "../core/enum/Enums.h"
#include <QList>
#include <QGraphicsView>
#include "../qt/core/View.h"
#include "../qt/core/Scene.h"

te::layout::MapController::MapController( Observable* o ) :
  ItemController(o)
{
  
}

te::layout::MapController::~MapController()
{
  
}

void te::layout::MapController::refreshAllProperties()
{
  Scene* scene = dynamic_cast<Scene*>(Context::getInstance().getScene());
  QList<QGraphicsView *> views = scene->views();
  if(views.size()==0)
  {
    return;
  }
  View* view = dynamic_cast<View*>(views[0]);
  view->refreshAllProperties();
}





