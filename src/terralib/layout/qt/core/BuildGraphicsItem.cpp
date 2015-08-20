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
  \file BuildGraphicsItem.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "BuildGraphicsItem.h"
#include "Scene.h"
#include "ItemUtils.h"
#include "../../core/pattern/singleton/Context.h"
#include "../../core/property/SharedProperties.h"
#include "../../item/MapModel.h"
#include "../../item/MapController.h"
#include "../item/MapItem.h"
#include "../../item/LegendModel.h"
#include "../../item/LegendController.h"
#include "../item/LegendItem.h"
#include "../../item/PaperModel.h"
#include "../item/PaperItem.h"
#include "../../item/PaperController.h"
#include "../../item/ScaleModel.h"
#include "../item/ScaleItem.h"
#include "../../item/ItemGroupModel.h"
#include "../../item/ItemGroupController.h"
#include "../item/ItemGroup.h"
#include "../../item/ImageModel.h"
#include "../item/ImageItem.h"
#include "../../item/ArrowModel.h"
#include "../item/ArrowItem.h"
#include "../../item/EllipseModel.h"
#include "../item/EllipseItem.h"
#include "../../item/PointModel.h"
#include "../item/PointItem.h"
#include "../../item/TitleModel.h"
#include "../../item/TitleController.h"
#include "../item/TitleItem.h"
#include "../../item/TextGridModel.h"
#include "../../item/TextGridController.h"
#include "../item/TextGridItem.h"
#include "../../core/enum/Enums.h"
#include "../../item/LegendChildModel.h"
#include "../../item/LegendChildController.h"
#include "../item/LegendChildItem.h"
#include "../item/TextItem.h"
#include "../../item/TextModel.h"
#include "../../core/property/Properties.h"
#include "../item/MovingItemGroup.h"
#include "../../item/MovingItemGroupController.h"
#include "../../item/MovingItemGroupModel.h"
#include "../../item/LineModel.h"
#include "../../item/LineController.h"
#include "../item/LineItem.h"
#include "../../item/PolygonModel.h"
#include "../../item/PolygonController.h"
#include "../item/PolygonItem.h"
#include "../../item/BalloonModel.h"
#include "../../item/BalloonController.h"
#include "../item/BalloonItem.h"
#include "../../item/BarCodeModel.h"
#include "../../item/BarCodeController.h"
#include "../item/BarCodeItem.h"
#include "../../item/GridMapModel.h"
#include "../../item/GridMapController.h"
#include "../item/GridMapItem.h"
#include "../../item/GridPlanarModel.h"
#include "../../item/GridPlanarController.h"
#include "../item/GridPlanarItem.h"
#include "../../item/GridGeodesicModel.h"
#include "../../item/GridGeodesicController.h"
#include "../item/GridGeodesicItem.h"
#include "../../item/NorthModel.h"
#include "../../item/NorthController.h"
#include "../item/NorthItem.h"
#include "../../item/MapLocationModel.h"
#include "../../item/MapLocationController.h"
#include "../item/MapLocationItem.h"
#include "../../item/SVGModel.h"
#include "../item/SVGItem.h"
#include "../../core/pattern/mvc/AbstractItemController.h"
#include "../item/TextController1.h"
#include "../item/TitleController1.h"
#include "pattern/factory/item/NewItemFactory.h"
#include "pattern/factory/item/ItemFactoryParamsCreate.h"
#include "../../core/enum/EnumObjectType.h"
#include "../item/MapController1.h"
#include "../item/ItemGroupController1.h"

// Qt
#include <QGraphicsItem>

// STL
#include <sstream>
#include <string> 

te::layout::BuildGraphicsItem::BuildGraphicsItem() 
{
 
}

te::layout::BuildGraphicsItem::~BuildGraphicsItem()
{
 
}

QGraphicsItem* te::layout::BuildGraphicsItem::rebuildItem( te::layout::Properties props, bool draw )
{
  QGraphicsItem* item = 0;

  if(props.getProperties().empty())
    return item;

  clear();

  m_props = props;  
  m_coord = findCoordinate(props);
  m_zValue = findZValue(props);

  EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();

  EnumType* type = props.getTypeObj();

  item = createItem(type, draw);
  
  return item;
}

QGraphicsItem* te::layout::BuildGraphicsItem::createItem( te::layout::EnumType* mode, const te::gm::Coord2D& coordinate, bool draw )
{
  QGraphicsItem* item = 0;

  m_coord = coordinate;
  clear();

  EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
  EnumModeType* enumMode = Enums::getInstance().getEnumModeType();

  if(mode == enumMode->getModeCreateMap())
  {
    item = createItem(enumObj->getMapItem(), draw);
  }
  else if(mode == enumMode->getModeCreateGridMap())
  {
    item = createItem(enumObj->getGridMapItem(), draw);
  }
  else if(mode == enumMode->getModeCreateText())
  {
    item = createItem(enumObj->getTextItem(), draw);
  }
  else if(mode == enumMode->getModeCreateImage())
  {
    item = createItem(enumObj->getImageItem(), draw);
  }
  else if(mode == enumMode->getModeCreateRectangle())
  {
    item = createItem(enumObj->getRectangleItem(), draw);
  }
  else if(mode == enumMode->getModeCreateLegend())
  {
    item = createItem(enumObj->getLegendItem(), draw);
  }
  else if(mode == enumMode->getModeCreateScale())
  {
    item = createItem(enumObj->getScaleItem(), draw);
  }
  else if(mode == enumMode->getModeCreateItemGroup())
  {
    item = createItem(enumObj->getItemGroup(), draw);
  }
  else if(mode == enumMode->getModeCreatePoint())
  {
    item = createItem(enumObj->getPointItem(), draw);
  }
  else if(mode == enumMode->getModeCreateArrow())
  {
    item = createItem(enumObj->getArrowItem(), draw);
  }
  else if(mode == enumMode->getModeCreateEllipse())
  {
    item = createItem(enumObj->getEllipseItem(), draw);
  }
  else if(mode == enumMode->getModeCreateTitle())
  {
    item = createItem(enumObj->getTitleItem(), draw);
  }
  else if(mode == enumMode->getModeCreateTextGrid())
  {
    item = createItem(enumObj->getTextGridItem(), draw);
  }
  else if(mode == enumMode->getModeCreateLegendChild())
  {
    item = createItem(enumObj->getLegendChildItem(), draw);
  }
  else if (mode == enumMode->getModeCreateLineItem()) 
  {
    item = createItem(enumObj->getLineItem(), draw);
  }
  else if (mode == enumMode->getModeCreatePolygonItem()) 
  {
    item = createItem(enumObj->getPolygonItem(), draw);
  }
  else if (mode == enumMode->getModeCreateSVG()) 
  {
    item = createItem(enumObj->getSVGItem(), draw);
  }
  else if (mode == enumMode->getModeCreateBalloon()) 
  {
    item = createItem(enumObj->getBalloonItem(), draw);
  }
  else if (mode == enumMode->getModeCreateBarCode())
  {
    item = createItem(enumObj->getBarCodeItem(), draw);
  }
  else if (mode == enumMode->getModeCreateGridPlanar())
  {
    item = createItem(enumObj->getGridPlanarItem(), draw);
  }
  else if (mode == enumMode->getModeCreateGridGeodesic())
  {
    item = createItem(enumObj->getGridGeodesicItem(), draw);
  }
  else if (mode == enumMode->getModeCreateNorth())
  {
    item = createItem(enumObj->getNorthItem(), draw);
  }
  else if (mode == enumMode->getModeCreateMapLocation())
  {
    item = createItem(enumObj->getMapLocationItem(), draw);
  }

  return item;
}

QGraphicsItem* te::layout::BuildGraphicsItem::createItem( te::layout::EnumType* type, bool draw )
{
  QGraphicsItem* item = 0;

  EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();

  if(type == enumObj->getMovingItemGroup())
  {
    item = createMovingItemGroup();
  }
  else if(type == enumObj->getMapItem())
  {
    item = createMap();
  }
  else if(type == enumObj->getGridMapItem())
  {
    item = createGridMap();
  }
  else if(type == enumObj->getTextItem())
  {
    item = createText();
  }
  else if(type == enumObj->getImageItem())
  {
    item = createImage();
  }
  else if(type == enumObj->getRectangleItem())
  {
    item = createRectangle();
  }
  else if(type == enumObj->getLegendItem())
  {
    item = createLegend();
  }
  else if(type == enumObj->getPaperItem())
  {
    item = createPaper();
  }
  else if(type == enumObj->getScaleItem())
  {
    item = createScale();
  }
  else if(type == enumObj->getItemGroup())
  {
    item = createItemGroup();
  }
  else if(type == enumObj->getArrowItem())
  {
    item = createArrow();
  }
  else if(type == enumObj->getEllipseItem())
  {
    item = createEllipse();
  }
  else if(type == enumObj->getPointItem())
  {
    item = createPoint();
  }
  else if(type == enumObj->getTextGridItem())
  {
    item = createTextGrid();
  }
  else if(type == enumObj->getTitleItem())
  {
    item = createTitle();
  }
  else if(type == enumObj->getLegendChildItem())
  {
    item = createLegendChild();
  }
  else if(type == enumObj->getLineItem())
  {
    item = createLineItem();
  }
  else if(type == enumObj->getPolygonItem())
  {
    item = createPolygonItem();
  }
  else if(type == enumObj->getSVGItem())
  {
    item = createSVG();
  }
  else if(type == enumObj->getBalloonItem())
  {
    item = createBalloon();
  }
  else if(type == enumObj->getBarCodeItem())
  {
    item = createBarCode();
  }
  else if(type == enumObj->getGridPlanarItem())
  {
    item = createGridPlanar();
  }
  else if(type == enumObj->getGridGeodesicItem())
  {
    item = createGridGeodesic();
  }
  else if(type == enumObj->getNorthItem())
  {
    item = createNorth();
  }
  else if(type == enumObj->getMapLocationItem())
  {
    item = createMapLocation();
  }

  AbstractScene* abstScene = Context::getInstance().getScene();
  if(abstScene)
  {
    Scene* sc = dynamic_cast<Scene*>(abstScene);
    if(sc)
    {
      sc->insertItem(item);
    }
  }

  afterBuild(item, draw);

  return item;
}

std::string te::layout::BuildGraphicsItem::nameItem( te::layout::EnumType* type )
{
  std::string name = "";

  AbstractScene* abstScene = Context::getInstance().getScene();

  QList<QGraphicsItem*> graphicsItems;
  std::stringstream ss;//create a stringstream

  if(abstScene)
  {
    Scene* sc = dynamic_cast<Scene*>(abstScene);
    if(sc)
    {
      graphicsItems = sc->items();
    }
  }  

  ItemUtils* iUtils = Context::getInstance().getItemUtils();
  if(!iUtils)
  {
    return name;
  }

  m_id = iUtils->maxTypeId(type);

  if(m_id < 0)
    m_id = 0;
  else
    m_id+= 1;

  ss << m_id;

  name = type->getName();
  name+="_";
  name+=ss.str();

  return name;
}

bool te::layout::BuildGraphicsItem::addChild( QGraphicsItem* child, int x, int y )
{
  bool result = false;

  if(!child)
  {
    return result;
  }

  ItemUtils* iUtils = Context::getInstance().getItemUtils();
  if(!iUtils)
  {
    return result;
  }

  QGraphicsItem* parentItem = iUtils->intersectionSelectionItem(x, y);

  if(!parentItem)
  {
    return result;
  }
  
  // No add child in unselected items
  if(!parentItem->isSelected())
  {
    return result;
  }

  ItemObserver* itemOb = dynamic_cast<ItemObserver*>(parentItem);
  if(!itemOb)
  {
    return result;
  }

  if(!itemOb->getModel())
  {
    return result;
  }

  ItemModelObservable* model = dynamic_cast<ItemModelObservable*>(itemOb->getModel());  
  if(!model)
  {
    return result;
  }

  if(!model->isEnableChildren())
  {
    return result;
  }

  ItemObserver* itemChild = dynamic_cast<ItemObserver*>(child);
  if(!itemChild)
  {
    return result;
  }

  if(!itemOb->canBeChild(itemChild))
  {
    return result;
  }

  if(child->scene() == parentItem->scene())
  {
    QGraphicsScene* scene = child->scene();
    if(scene)
    {
      scene->removeItem(child);
    }
  }
  //implicitly adds this graphics item to the scene of the parent.
  child->setParentItem(parentItem);
  result = true;

  emit addChildFinalized(parentItem, child);

  return result;
}

void te::layout::BuildGraphicsItem::afterBuild( QGraphicsItem* item, bool draw )
{
  if(!item)
  {
    return;
  }
  
  bool result = addChild(item, m_coord.x, m_coord.y);
  if(!result)
  {
    double width = item->boundingRect().width();
    double height = item->boundingRect().height();

    QPointF pointInSceneCS(m_coord.x, m_coord.y);
    QPointF pointInItemCS = item->mapFromScene(pointInSceneCS);
    pointInItemCS.setX(pointInItemCS.x() - (width /2.));
    pointInItemCS.setY(pointInItemCS.y() - (height / 2.));
    pointInSceneCS = item->mapToScene(pointInItemCS);

    item->setPos(pointInSceneCS);
  }
  
  if(!m_props.getProperties().empty())

  {
    item->setZValue(m_zValue);
  }


  ItemObserver* itemObs = dynamic_cast<ItemObserver*>(item);
  if(itemObs && draw)
  {
    itemObs->redraw();
  }
}

QGraphicsItem* te::layout::BuildGraphicsItem::createPaper()
{
  AbstractScene* abScene = Context::getInstance().getScene();
  if(!abScene)
  {
    return 0;
  }

  Scene* sc = dynamic_cast<Scene*>(abScene);
  if(!sc)
  {
    return 0;
  }

  PaperConfig* pConfig = sc->getPaperConfig();
  if(!pConfig)
  {
    return 0;
  }
  PaperModel* model = new PaperModel(pConfig);
  if(m_props.getProperties().empty())
  {
    model->setId(m_id);
    
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string name = nameItem(enumObj->getPaperItem());
    model->setName(name);
  }

  PaperController* controllerMap = new PaperController(model);
  ItemObserver* itemObs = (ItemObserver*)controllerMap->getView();

  PaperItem* view = dynamic_cast<PaperItem*>(itemObs);
  if((!m_props.getProperties().empty()) && view)
  {
    Properties* props = new Properties(m_props);
    model->updateProperties(props);
    delete props;
  }  
  return view;
}

QGraphicsItem* te::layout::BuildGraphicsItem::createMap()
{
  MapModel* model = new MapModel();
  if(m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getMapItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  MapController1* controller = new MapController1(model);
  AbstractItemView* view = controller->getView();

  if ((!m_props.getProperties().empty()) && view)
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createGridMap()
{
  /*
  GridMapModel* model = new GridMapModel();
  if (m_props.getProperties().empty())
  {
    model->setId(m_id);
    
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string name = nameItem(enumObj->getGridMapItem());
    model->setName(name);
  }

  GridMapController* controller = new GridMapController(model);
  ItemObserver* itemObs = (ItemObserver*)controller->getView();

  GridMapItem* view = dynamic_cast<GridMapItem*>(itemObs);
  if ((!m_props.getProperties().empty()) && view)
  {
    Properties* props = new Properties(m_props);
    model->updateProperties(props);
    delete props;
  }
  return view;
  */
  return 0;
}

QGraphicsItem* te::layout::BuildGraphicsItem::createText()
{
  TextModel* model = new TextModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getRectangleItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new TextController1(model);
  AbstractItemView* view = controller->getView();

  if (!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createRectangle()
{
  te::layout::EnumObjectType* obj = Enums::getInstance().getEnumObjectType();

  std::string strName = nameItem(obj->getRectangleItem());

  ItemFactoryParamsCreate params(strName, m_zValue, m_id, m_coord);

  std::string name = obj->getRectangleItem()->getName();

  AbstractItemView* abstractItem = te::layout::NewItemFactory::make(name, params);
  return dynamic_cast<QGraphicsItem*>(abstractItem);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createLegend()
{
  LegendModel* model = new LegendModel();  
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getRectangleItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if(!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createScale()
{
  ScaleModel* model = new ScaleModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getScaleItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if (!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createItemGroup()
{
  ItemGroupModel* model = new ItemGroupModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getItemGroup());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new ItemGroupController1(model);
  AbstractItemView* view = controller->getView();

  if(!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createMovingItemGroup()
{
  MovingItemGroupModel* model = new MovingItemGroupModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getMovingItemGroup());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if(!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createImage()
{
  ImageModel* model = new ImageModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getRectangleItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if(!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createArrow()
{
  ArrowModel* model = new ArrowModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getArrowItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if (!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createEllipse()
{
  EllipseModel* model = new EllipseModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getEllipseItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if (!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createPoint()
{
  PointModel* model = new PointModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getPointItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if (!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createTextGrid()
{
  /*
  TextGridModel* model = new TextGridModel();  
  if(!m_props)
  {
    model->setId(m_id);
    
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string name = nameItem(enumObj->getTextGridItem());
    model->setName(name);
  }

  TextGridController* controller = new TextGridController(model);
  ItemObserver* itemObs = (ItemObserver*)controller->getView();

  TextGridItem* view = dynamic_cast<TextGridItem*>(itemObs); 
  if(m_props && view)
  {
    model->updateProperties(m_props);
  }
  return view;
  */
  return 0;
}

QGraphicsItem* te::layout::BuildGraphicsItem::createTitle()
{
  TitleModel* model = new TitleModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getTitleItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new TitleController1(model);
  AbstractItemView* view = controller->getView();

  if (!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createLegendChild()
{
  LegendChildModel* model = new LegendChildModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getRectangleItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if (!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createLineItem() 
{
  LineModel* model = new LineModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getLineItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if (!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createPolygonItem() 
{
  PolygonModel* model = new PolygonModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getPolygonItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if (!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createBalloon() 
{
  BalloonModel* model = new BalloonModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getPolygonItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if (!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createBarCode()
{
  /*
  BarCodeModel* model = new BarCodeModel;
  if(m_props)
  {
    model->updateProperties(m_props);
  }
  else
  {
    model->setId(m_id);
    
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string name = nameItem(enumObj->getBarCodeItem());
    model->setName(name);
  }

  BarCodeController* controller = new BarCodeController(model);
  ItemObserver* itemObs = (ItemObserver*)controller->getView();

  BarCodeItem* view = dynamic_cast<BarCodeItem*>(itemObs);
  if(m_props && view)
  {
    model->updateProperties(m_props);
  }  
  return view;
  */
  return 0;
}

QGraphicsItem* te::layout::BuildGraphicsItem::createGridPlanar()
{
  GridPlanarModel* model = new GridPlanarModel();
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getGridPlanarItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if ((!m_props.getProperties().empty()) && view)
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createGridGeodesic()
{
  GridGeodesicModel* model = new GridGeodesicModel();  
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getGridGeodesicItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if ((!m_props.getProperties().empty()) && view)
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createNorth()
{
  NorthModel* model = new NorthModel;
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getNorthItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if (!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}

QGraphicsItem* te::layout::BuildGraphicsItem::createMapLocation()
{
  /*
  MapLocationModel* model = new MapLocationModel;
  if (m_props.getProperties().empty())
  {
    model->setId(m_id);

    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string name = nameItem(enumObj->getGridGeodesicItem());
    model->setName(name);
  }
  MapLocationController* controller = new MapLocationController(model);
  ItemObserver* itemObs = (ItemObserver*)controller->getView();

  MapLocationItem* view = dynamic_cast<MapLocationItem*>(itemObs);
  if ((!m_props.getProperties().empty()) && view)
  {
    Properties* props = new Properties(m_props);
    model->updateProperties(props);
    delete props;
  }
  return view;
  */
  return 0;
}

QGraphicsItem* te::layout::BuildGraphicsItem::createSVG()
{
  SVGModel* model = new SVGModel;
  if (m_props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();
    std::string strName = nameItem(enumObj->getRectangleItem());

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(m_id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(strName, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if (!m_props.getProperties().empty())
  {
    model->setProperties(m_props);
  }
  return dynamic_cast<QGraphicsItem*>(view);
}
