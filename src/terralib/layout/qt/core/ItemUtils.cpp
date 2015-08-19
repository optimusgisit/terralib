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
  \file ItemUtils.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "ItemUtils.h"
#include "../../core/pattern/mvc/ItemModelObservable.h"
#include "../../core/pattern/mvc/AbstractItemView.h"
#include "../../core/pattern/mvc/AbstractItemController.h"
#include "../../core/pattern/mvc/AbstractItemModel.h"
#include "../../core/pattern/singleton/Context.h"
#include "../../core/enum/Enums.h"
#include "../item/MapItem.h"
#include "../item/LegendItem.h"
#include "../../item/LegendModel.h"
#include "../../item/MapModel.h"
#include "../item/TextItem.h"
#include "../../item/TextModel.h"
#include "../item/LegendChildItem.h"
#include "../../item/LegendChildModel.h"
#include "../../core/pattern/derivativevisitor/VisitorUtils.h"
#include "../../item/GridGeodesicModel.h"
#include "../../item/GridPlanarModel.h"
#include "Scene.h"
#include "../item/GridMapItem.h"
#include "View.h"

// STL
#include <stddef.h>  // defines NULL

// Qt
#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QTextDocument>
#include <QFont>

te::layout::ItemUtils::ItemUtils( QGraphicsScene* scene ) :
  m_scene(scene)
{

}

te::layout::ItemUtils::~ItemUtils()
{

}

std::vector<te::layout::MapItem*> te::layout::ItemUtils::getMapItemList(bool selected)
{
  std::vector<te::layout::MapItem*> list;

  QList<QGraphicsItem*> graphicsItems = getItems(selected);
  foreach( QGraphicsItem *item, graphicsItems) 
  {
    if(!item)
      continue;

    te::layout::ItemObserver* lItem = dynamic_cast<te::layout::ItemObserver*>(item);
    if(!lItem)
      continue;

    te::layout::MapItem* mit = dynamic_cast<te::layout::MapItem*>(lItem);
    if(!mit)
      continue;

    list.push_back(mit);
  }

  return list;
}

te::layout::MapItem* te::layout::ItemUtils::getMapItem( std::string name )
{
  te::layout::MapItem* map = 0;

  te::layout::EnumObjectType* objectType = te::layout::Enums::getInstance().getEnumObjectType();

  std::vector<std::string> strList;

  QList<QGraphicsItem*> graphicsItems = getItems(false);
  foreach( QGraphicsItem *item, graphicsItems) 
  {
    if(!item)
      continue;

    te::layout::AbstractItemView* itemView = dynamic_cast<te::layout::AbstractItemView*>(item);
    if(itemView == 0)
      continue;

    te::layout::AbstractItemModel* model = itemView->getController()->getModel();

    if(model == 0)
      continue;

    if(model->getProperties().getTypeObj() != objectType->getMapItem())
    {
      continue;
    }

    const Property& pName = model->getProperty("name");
    if(pName.getValue().toString().compare(name) != 0)
      continue;

    map = dynamic_cast<te::layout::MapItem*>(itemView);
    break;
  }

  return map;
}

std::vector<std::string> te::layout::ItemUtils::mapNameList(bool selected)
{
  te::layout::EnumObjectType* objectType = te::layout::Enums::getInstance().getEnumObjectType();

  std::vector<std::string> strList;

  QList<QGraphicsItem*> graphicsItems = getItems(selected);
  foreach( QGraphicsItem *item, graphicsItems) 
  {
    if(!item)
      continue;

    te::layout::AbstractItemView* itemView = dynamic_cast<te::layout::AbstractItemView*>(item);
    if(itemView == 0)
      continue;

    te::layout::AbstractItemModel* model = itemView->getController()->getModel();

    if(model == 0)
      continue;

    if(model->getProperties().getTypeObj() != objectType->getMapItem())
    {
      continue;
    }

    const Property& pName = model->getProperty("name");
    strList.push_back(pName.getValue().toString());
  }

  return strList;
}

int te::layout::ItemUtils::countType( te::layout::EnumType* type )
{
  int count = 0;

  QList<QGraphicsItem*> graphicsItems = getItems();
  foreach( QGraphicsItem *item, graphicsItems) 
  {
    if(!item)
      continue;

    te::layout::AbstractItemView* absItem = dynamic_cast<te::layout::AbstractItemView*>(item);
    if(absItem == 0)
      continue;

    te::layout::AbstractItemModel* model = absItem->getController()->getModel();
    if(model == 0)
      continue;

    if(model->getProperties().getTypeObj() == type)
    {
      count+=1;
    }
  }

  return count;
}

int te::layout::ItemUtils::maxTypeId( te::layout::EnumType* type )
{
  int id = -1;

  QList<QGraphicsItem*> graphicsItems = getItems();
  foreach( QGraphicsItem *item, graphicsItems) 
  {
    if(!item)
      continue;

    te::layout::AbstractItemView* absItem = dynamic_cast<te::layout::AbstractItemView*>(item);
    if(absItem == 0)
      continue;

    te::layout::AbstractItemModel* model = absItem->getController()->getModel();
    if(model == 0)
      continue;

    int currentId = model->getProperty("id").getValue().toInt();

    if(model->getProperties().getTypeObj() == type)
    {
      if(id == -1)
      {
        id = currentId;
      }
      else
      {
        if(currentId > id)
        {
          id = currentId;
        }
      }
    }
  }

  return id;
}

bool te::layout::ItemUtils::isCurrentMapTools()
{
  bool result = false;

  AbstractScene* abScene = Context::getInstance().getScene();
  if(!abScene)
  {
    return result;
  }

  Scene* sc = dynamic_cast<Scene*>(abScene);
  if(!sc)
  {
    return result;
  }

  ContextObject context = sc->getContext();

  te::layout::EnumType* mode = context.getCurrentMode();
  te::layout::EnumModeType* type = te::layout::Enums::getInstance().getEnumModeType();

  if(mode == type->getModeMapPan())
  {
    result = true;
  }
  else if(mode == type->getModeMapZoomIn())
  {
    result = true;
  }
  else if(mode == type->getModeMapZoomOut())
  {
    result = true;
  }
  return result;
}

QList<QGraphicsItem*> te::layout::ItemUtils::getItems( bool selected )
{
  QList<QGraphicsItem*> graphicsItems;

  if(selected)
  {
    graphicsItems = m_scene->selectedItems();
  }
  else
  {
    graphicsItems = m_scene->items();
  }

  return graphicsItems;
}

void te::layout::ItemUtils::setCurrentToolInSelectedMapItems( EnumType* mode )
{
  /*
  if(!mode)
    return;

  EnumModeType* type = Enums::getInstance().getEnumModeType();

  if(mode == type->getModeNone())
    return;

  if(!isCurrentMapTools())
    return;

  QList<QGraphicsItem*> graphicsItems = getItems(true);
  foreach(QGraphicsItem *item, graphicsItems) 
  {
    if(!item)
      continue;

    te::layout::ItemObserver* lItem = dynamic_cast<te::layout::ItemObserver*>(item);
    if(!lItem)
      continue;

    te::layout::MapItem* mit = dynamic_cast<te::layout::MapItem*>(lItem);
    if(!mit)
      continue;

    mit->changeCurrentTool(mode);
  }
  */
}

void te::layout::ItemUtils::createTextGridAsObject()
{
  //do nothing;
}

void te::layout::ItemUtils::createTextMapAsObject()
{
  /*
  QGraphicsItem *item = m_scene->selectedItems().first();
  if(item)
  {
    ItemObserver* it = dynamic_cast<ItemObserver*>(item);
    if(it)
    {
      MapItem* mt = dynamic_cast<MapItem*>(it);
      if(mt)
      {
        MapModel* model = dynamic_cast<MapModel*>(mt->getModel());
        std::map<te::gm::Point*, std::string> map = model->getTextMapAsObjectInfo();
        createTextItemFromObject(map);
      }
    }
  }
  */
}

void te::layout::ItemUtils::createTextItemFromObject( std::map<te::gm::Point*, std::string> map, QFont* ft )
{
  /*
  Scene* scne = dynamic_cast<Scene*>(m_scene);

  if(!scne)
    return;

  EnumModeType* mode = Enums::getInstance().getEnumModeType();

  std::map<te::gm::Point*, std::string>::iterator it;
  
  for (it = map.begin(); it != map.end(); ++it) 
  {
    te::gm::Point* pt = it->first;
    std::string text = it->second;

    QGraphicsItem* item = 0;
    
    te::gm::Coord2D coord(pt->getX(), pt->getY());
    item = scne->createItem(coord);
    if(!item)
      continue;

    TextItem* txtItem = dynamic_cast<TextItem*>(item);
    if(txtItem)
    {
      TextModel* model = dynamic_cast<TextModel*>(txtItem->getModel());
      if(model)
      {
        if(ft)
        {
          txtItem->setFont(*ft);
          Font fnt = model->getFont();
          fnt.setFamily(ft->family().toStdString());
          fnt.setPointSize(ft->pointSize());
          model->setFont(fnt);
        }        
        model->setText(text);
        txtItem->document()->setPlainText(text.c_str());
      }
    }
  }
  */
}

void te::layout::ItemUtils::createLegendChildItemFromLegend( std::map<te::gm::Point*, std::string> map, te::layout::MapModel* visitable )
{
  /*
  Scene* scne = dynamic_cast<Scene*>(m_scene);

  if(!scne)
    return;

  if(!visitable)
    return;

  EnumModeType* mode = Enums::getInstance().getEnumModeType();

  std::map<te::gm::Point*, std::string>::iterator it;

  for (it = map.begin(); it != map.end(); ++it) 
  {
    te::gm::Point* pt = it->first;
    std::string text = it->second;
    
    QGraphicsItem* item = 0;
    te::gm::Coord2D coord(pt->getX(), pt->getY());
    item = scne->createItem(coord);
    if(!item)
      continue;

    LegendChildItem* lgItem = dynamic_cast<LegendChildItem*>(item);
    if(lgItem)
    {
      QList<QGraphicsItem*> legends;
      legends.push_back(lgItem);
      te::layout::VisitorUtils::getInstance().changeMapVisitable(legends, visitable);
    }
  }
  */
}

std::vector<te::layout::Properties*> te::layout::ItemUtils::getGridMapProperties()
{
  std::vector<te::layout::Properties*> props;

  /*
  std::vector<te::layout::GridMapItem*> gridMapItems = getMapChildren();

  std::vector<te::layout::GridMapItem*>::iterator it = gridMapItems.begin();
  for( ; it != gridMapItems.end() ; ++it)
  {
    if(!(*it))
    {
      continue;
    }

    if(!(*it)->getModel())
    {
      continue;;
    }

    Properties* prop = (*it)->getModel()->getProperties();
    props.push_back(prop);
  }
  */

  return props;
}

std::vector<te::layout::GridMapItem*> te::layout::ItemUtils::getMapChildren()
{
  std::vector<te::layout::GridMapItem*> gridMapItems;

  QGraphicsItem *item = m_scene->selectedItems().first();
  if(!item)
  {
    return gridMapItems;
  }
  
  MapItem* map = dynamic_cast<MapItem*>(item);
  if(!map)
  {
    return gridMapItems;
  }

  QList<QGraphicsItem*> graphicsItems = map->childItems();
  foreach(QGraphicsItem *item, graphicsItems) 
  {
    if(!item)
      continue;

    GridMapItem* grid = dynamic_cast<GridMapItem*>(item);
    if(!grid)
    {
      continue;
    }

    gridMapItems.push_back(grid);
  }
  
  return gridMapItems;
}

QGraphicsItem* te::layout::ItemUtils::intersectionSelectionItem( int x, int y )
{
  QGraphicsItem* intersectionItem = 0;

  AbstractScene* abstScene = Context::getInstance().getScene();

  if(!abstScene)
  {
    return intersectionItem;
  }

  Scene* sc = dynamic_cast<Scene*>(abstScene);
  if(!sc)
  {
    return intersectionItem;
  }

  QList<QGraphicsItem*> items = sc->selectedItems();

  QPointF pt(x, y);

  bool intersection = false;

  foreach (QGraphicsItem *item, items) 
  {
    if(item)
    {
      bool intersection = item->contains(pt);
      if(intersection)
      {
        intersectionItem = item;
        break;
      }
    }
  }

  return intersectionItem;
}

void te::layout::ItemUtils::getTextBoundary( QFont ft, double& w, double& h, std::string txt)
{
  AbstractScene* abScene = Context::getInstance().getScene();

  if(!abScene)
  {
    return;
  }

  Scene* scene = dynamic_cast<Scene*>(abScene);

  QTransform matrix = scene->sceneTransform();

  QString qtx(txt.c_str());
  QFontMetrics fm(ft);  

  int widthWithBearing = 0;

  QChar *data = qtx.data();
  while (!data->isNull()) 
  {
    QChar caracter(data->unicode());
    widthWithBearing+= fm.width(caracter); // logical width of a character in pixels without bearing
    widthWithBearing+= fm.leftBearing(caracter);
    widthWithBearing+= fm.rightBearing(caracter);
    ++data;
  }
  
  //QRectF rec(0, 0, widthWithBearing, fm.boundingRect(qtx).height() - 1);
  QRectF rec(fm.boundingRect(qtx));
  QRectF wrec(matrix.inverted().mapRect(rec));
  
  w = wrec.width();
  h = wrec.height();
}

void te::layout::ItemUtils::changeViewMode( EnumType* mode )
{
  AbstractScene* abScene = Context::getInstance().getScene();

  if(!abScene)
  {
    return;
  }

  Scene* scene = dynamic_cast<Scene*>(abScene);
  if(!scene)
  {
    return;
  }

  View* view = scene->getView();
  if(!view)
  {
    return;
  }

  view->changeMode(mode);
}




