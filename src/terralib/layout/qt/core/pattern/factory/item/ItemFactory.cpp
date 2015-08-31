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
  \file ItemFactory.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "ItemFactory.h"
#include "../../../../../core/enum/EnumType.h"
#include "../../../../../core/pattern/factory/ItemParamsCreate.h"
#include "../../../../../core/enum/Enums.h"
#include "../../../../item/ItemGroup.h"
#include "../../../../item/PaperItem.h"
#include "../../../../item/MapItem.h"
#include "../../../../item/GridMapItem.h"
#include "../../../../item/MovingItemGroup.h"
#include "../../../../item/GridPlanarItem.h"
#include "../../../../item/GridGeodesicItem.h"
#include "../../../../item/MapLocationItem.h"
#include "../../../../item/RectangleItem.h"
#include "../../../../item/EllipseItem.h"
#include "../../../../item/ArrowItem.h"
#include "../../../../item/ScaleItem.h"
#include "../../../../item/PointItem.h"
#include "../../../../item/TextItem.h"
#include "../../../../item/TitleItem.h"
#include "../../../../item/TextGridItem.h"
#include "../../../../item/BarCodeItem.h"
#include "../../../../item/NorthItem.h"
#include "../../../../item/LineItem.h"
#include "../../../../item/PolygonItem.h"
#include "../../../../item/ImageItem.h"
#include "../../../../item/LegendItem.h"
#include "../../../../item/LegendChildItem.h"
#include "../../../../item/SVGItem.h"
#include "../../../../../core/enum/EnumObjectType.h"

te::layout::ItemFactory::ItemFactory()
{

}

te::layout::ItemFactory::~ItemFactory()
{

}

te::layout::AbstractItemView* te::layout::ItemFactory::make(EnumType* type, ItemParamsCreate params)
{
  AbstractItemView* item = 0;

  EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();

  if(type == enumObj->getPaperItem())
  {
    item = new PaperItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getRectangleItem())
  {
    item = new RectangleItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getEllipseItem())
  {
    item = new EllipseItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getArrowItem())
  {
    item = new ArrowItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getScaleItem())
  {
    item = new ScaleItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getPointItem())
  {
    item = new PointItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getTextItem())
  {
    item = new TextItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getTitleItem())
  {
    item = new TitleItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getTextGridItem())
  {
    item = new TextGridItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getBarCodeItem())
  {
    item = new BarCodeItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getNorthItem())
  {
    item = new NorthItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getLineItem())
  {
    item = new LineItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getPolygonItem())
  {
    item = new PolygonItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getMapItem())
  {
    item = new MapItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getMapLocationItem())
  {
    item = new MapLocationItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getGridMapItem())
  {
    item = new GridMapItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getGridPlanarItem())
  {
    item = new GridPlanarItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getGridGeodesicItem())
  {
    item = new GridGeodesicItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getImageItem())
  {
    item = new ImageItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getLegendItem())
  {
    item = new LegendItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getLegendChildItem())
  {
    item = new LegendChildItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getSVGItem())
  {
    item = new SVGItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getBalloonItem())
  {
    item = new BarCodeItem(params.getController(), params.getModel());
  }
  else if(type == enumObj->getItemGroup())
  {
    item = new ItemGroup(params.getController(), params.getModel());
  }
  else if (type == enumObj->getMovingItemGroup())
  {
    item = new MovingItemGroup(params.getController(), params.getModel());
  }
  
  return item;
}

