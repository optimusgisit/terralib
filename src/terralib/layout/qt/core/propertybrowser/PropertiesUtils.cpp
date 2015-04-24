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
  \file PropertiesUtils.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "PropertiesUtils.h"
#include "../../../core/property/Properties.h"
#include "../../../core/pattern/mvc/ItemObserver.h"
#include "../../../core/property/SharedProperties.h"
#include "../../../core/enum/Enums.h"
#include "../../../core/pattern/singleton/Context.h"
#include "../ItemUtils.h"

// Qt
#include <QGraphicsItem>

te::layout::PropertiesUtils::PropertiesUtils() 
{

}

te::layout::PropertiesUtils::~PropertiesUtils()
{
  
}

te::layout::Properties* te::layout::PropertiesUtils::intersection( QList<QGraphicsItem*> graphicsItems, bool& window )
{
  Properties* props = 0;

  if(graphicsItems.size() == 1)
  {
    QGraphicsItem* item = graphicsItems.first();
    if (item)
    {			
      ItemObserver* lItem = dynamic_cast<ItemObserver*>(item);
      if(lItem)
      {
        props = const_cast<Properties*>(lItem->getProperties());
        window = props->hasWindows();
      }
    }
  }
  else
  {
    props = sameProperties(graphicsItems, window);
  }

  return props;
}

te::layout::Properties* te::layout::PropertiesUtils::sameProperties( QList<QGraphicsItem*> graphicsItems, bool& window )
{
  Properties* props = 0;
  std::vector<Properties*> propsVec = getAllProperties(graphicsItems, window);

  QGraphicsItem* firstItem = graphicsItems.first();
  ItemObserver* lItem = dynamic_cast<ItemObserver*>(firstItem);

  if(!lItem)
  {
    return props;
  }

  Properties* firstProps = const_cast<Properties*>(lItem->getProperties());
  if(!firstProps)
  {
    return props;
  }

  std::vector<Properties*>::iterator it = propsVec.begin();
  std::vector<Properties*>::iterator itend = propsVec.end();
  bool result = false;
  foreach( Property prop, firstProps->getProperties()) 
  {
    contains(itend, it, prop.getName(), result);
    if(result)
    {
      if(!props)
      {
        props = new Properties("");
      }
      props->addProperty(prop);
    }
  }  

  return props;
}

void te::layout::PropertiesUtils::contains( std::vector<Properties*>::iterator itend, std::vector<Properties*>::iterator it, std::string name, bool& result )
{
  Property prop = (*it)->contains(name);
  if(prop.isNull())
  {
    result = false;
    return;
  }
  else
  {
    ++it;
    result = true;
    if(it != itend)
    {
      contains(itend, it, name, result);
    }
  }
}

std::vector<te::layout::Properties*> te::layout::PropertiesUtils::getAllProperties( QList<QGraphicsItem*> graphicsItems, bool& window )
{
  std::vector<Properties*> propsVec;
  bool result = true;

  foreach( QGraphicsItem *item, graphicsItems) 
  {
    if (item)
    {			
      ItemObserver* lItem = dynamic_cast<ItemObserver*>(item);
      if(lItem)
      {
        Properties* propsItem = const_cast<Properties*>(lItem->getProperties());
        if(propsItem)
        {
          propsVec.push_back(propsItem);
          if(result)
          {
            result = propsItem->hasWindows();
          }
        }
      }
    }
  }

  window = result;
  return propsVec;
}

void te::layout::PropertiesUtils::addDynamicOptions( Property& property, std::vector<std::string> list )
{
  EnumDataType* dataType = Enums::getInstance().getEnumDataType();

  foreach(std::string str, list) 
  {
    Variant v;
    v.setValue(str, dataType->getDataTypeString());
    property.addOption(v);
  }
}

void te::layout::PropertiesUtils::checkDynamicProperty( Property& property, QList<QGraphicsItem*> graphicsItems )
{
  SharedProperties* sharedProps = new SharedProperties;
  
  if(property.getName().compare(sharedProps->getMapName()) == 0)
  {
    mapNameDynamicProperty(property, graphicsItems);
  }

  delete sharedProps;
  sharedProps = 0;
}

void te::layout::PropertiesUtils::mapNameDynamicProperty( Property& property, QList<QGraphicsItem*> graphicsItems )
{
  std::string currentName = property.getValue().toString();

  if(currentName.compare("") == 0)
  {
    currentName = property.getOptionByCurrentChoice().toString();
  }

  ItemUtils* iUtils = Context::getInstance().getItemUtils();
  std::vector<std::string> strList = iUtils->mapNameList();

  if(std::find(strList.begin(), strList.end(), currentName) != strList.end())
  {
    std::vector<std::string>::iterator it = std::find(strList.begin(), strList.end(), currentName);
    strList.erase(it);
  }

  addDynamicOptions(property, strList);
}




