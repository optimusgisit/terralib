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
  \file ItemFactory.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "ItemFactory.h"
#include "../../../../core/pattern/mvc/Observer.h"
#include "../../../../core/enum/Enums.h"
#include "../../../item/RectangleItem.h"
#include "../../../item/ItemGroup.h"
#include "../../../item/PaperItem.h"
#include "../../../item/MapItem.h"
#include "../../../item/GridMapItem.h"
#include "../../../item/LegendItem.h"
#include "../../../item/ScaleItem.h"
#include "../../../item/ImageItem.h"
#include "../../../item/PointItem.h"
#include "../../../item/ArrowItem.h"
#include "../../../item/EllipseItem.h"
#include "../../../item/TitleItem.h"
#include "../../../item/TextGridItem.h"
#include "../../../item/LegendChildItem.h"
#include "../../../item/TextItem.h"
#include "../../../item/MovingItemGroup.h"
#include "../../../item/LineItem.h"
#include "../../../item/PolygonItem.h"
#include "../../../item/BalloonItem.h"
#include "../../../item/BarCodeItem.h"
#include "../../../item/GridPlanarItem.h"
#include "../../../item/GridGeodesicItem.h"

te::layout::ItemFactory::ItemFactory()
{

}

te::layout::ItemFactory::~ItemFactory()
{

}

te::layout::Observer* te::layout::ItemFactory::make( EnumType* type, ItemParamsCreate params /*= LayoutItemParamsCreate()*/ )
{
  Observer* item = 0;

  EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();

  if(type == enumObj->getRectangleItem())
  {
    RectangleItem* rectangle = new RectangleItem(params.getController(), params.getModel());
    item = (Observer*)rectangle;
  }
  else if(type == enumObj->getLegendItem())
  {
    LegendItem* legend = new LegendItem(params.getController(), params.getModel());
    item = (Observer*)legend;
  }
  else if(type == enumObj->getTextItem())
  {
    TextItem* txt = new TextItem(params.getController(), params.getModel());
    item = (Observer*)txt;
  }
  else if(type == enumObj->getItemGroup())
  {
    ItemGroup* group = new ItemGroup(params.getController(), params.getModel());
    item = (Observer*)group;
  }
  else if(type == enumObj->getPaperItem())
  {
    PaperItem* paper = new PaperItem(params.getController(), params.getModel());
    item = (Observer*)paper;
  }
  else if(type == enumObj->getMapItem())
  {
    MapItem* map = new MapItem(params.getController(), params.getModel());
    item = (Observer*)map;
  }
  else if(type == enumObj->getGridMapItem())
  {
    GridMapItem* gridMap = new GridMapItem(params.getController(), params.getModel());
    item = (Observer*)gridMap;
  }
  else if(type == enumObj->getScaleItem())
  {
    ScaleItem* scale = new ScaleItem(params.getController(), params.getModel());
    item = (Observer*)scale;
  }
  else if(type == enumObj->getImageItem())
  {
    ImageItem* scale = new ImageItem(params.getController(), params.getModel());
    item = (Observer*)scale;
  }
  else if(type == enumObj->getPointItem())
  {
    PointItem* point = new PointItem(params.getController(), params.getModel());
    item = (Observer*)point;
  }
  else if(type == enumObj->getArrowItem())
  {
    ArrowItem* arrow = new ArrowItem(params.getController(), params.getModel());
    item = (Observer*)arrow;
  }
  else if(type == enumObj->getEllipseItem())
  {
    EllipseItem* ellipse = new EllipseItem(params.getController(), params.getModel());
    item = (Observer*)ellipse;
  }
  else if(type == enumObj->getTitleItem())
  {
    TitleItem* title = new TitleItem(params.getController(), params.getModel());
    item = (Observer*)title;
  }
  else if(type == enumObj->getTextGridItem())
  {
    TextGridItem* txtGrid = new TextGridItem(params.getController(), params.getModel());
    item = (Observer*)txtGrid;
  }
  else if(type == enumObj->getLegendChildItem())
  {
    LegendChildItem* mText = new LegendChildItem(params.getController(), params.getModel());
    item = (Observer*)mText;
  }
  else if (type == enumObj->getMovingItemGroup())
  {
    MovingItemGroup* mItemGroup = new MovingItemGroup(params.getController(), params.getModel());
    item = (Observer*) mItemGroup;
  }
  else if(type == enumObj->getLineItem())
  {
    LineItem* line = new LineItem(params.getController(), params.getModel());
    item = (Observer*)line;
  }
  else if(type == enumObj->getPolygonItem())
  {
    PolygonItem* polygon = new PolygonItem(params.getController(), params.getModel());
    item = (Observer*)polygon;
  }
  else if(type == enumObj->getBalloonItem())
  {
    BalloonItem* balloon = new BalloonItem(params.getController(), params.getModel());
    item = (Observer*)balloon;
  }
  else if(type == enumObj->getBarCodeItem())
  {
    BarCodeItem* barCode = new BarCodeItem(params.getController(), params.getModel());
    item = (Observer*)barCode;
  }
  else if(type == enumObj->getGridPlanarItem())
  {
    GridPlanarItem* gridPlanar = new GridPlanarItem(params.getController(), params.getModel());
    item = (Observer*)gridPlanar;
  }
  else if(type == enumObj->getGridMapItem())
  {
    GridGeodesicItem* gridGeodesic = new GridGeodesicItem(params.getController(), params.getModel());
    item = (Observer*)gridGeodesic;
  }

  return item;
}


