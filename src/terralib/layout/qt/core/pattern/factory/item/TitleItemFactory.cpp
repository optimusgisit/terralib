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
  \file terralib/layout/qt/core/pattern/factory/TitleItemFactory.cpp

  \brief This is the concrete factory for title item.
*/

// TerraLib
#include "TitleItemFactory.h"
#include "../../../../../core/enum/Enums.h"
#include "../../../../../item/TitleModel.h"
#include "../../../../../core/pattern/mvc/AbstractItemController.h"

te::layout::AbstractItemView* te::layout::TitleItemFactory::build(ItemFactoryParamsCreate params)
{
  std::string     name = params.getName();
  Properties      props = params.getProperties(); 
  te::gm::Coord2D coord = params.getCoord(); 
  int             zValue = params.getZValue(); 
  int             id = params.getId(); 

  TitleModel* model = new TitleModel();
  if (props.getProperties().empty())
  {
    EnumObjectType* enumObj = Enums::getInstance().getEnumObjectType();

    Property proertyId(0);
    proertyId.setName("id");
    proertyId.setValue(id, Enums::getInstance().getEnumDataType()->getDataTypeInt());
    model->setProperty(proertyId);

    Property propertyName(0);
    propertyName.setName("name");
    propertyName.setValue(name, Enums::getInstance().getEnumDataType()->getDataTypeString());
    model->setProperty(propertyName);
  }

  AbstractItemController* controller = new AbstractItemController(model);
  AbstractItemView* view = controller->getView();

  if (!props.getProperties().empty())
  {
    model->setProperties(props);
  }
  return dynamic_cast<AbstractItemView*>(view);
}

te::layout::TitleItemFactory::TitleItemFactory() :
  NewItemFactory(Enums::getInstance().getEnumObjectType()->getTitleItem()->getName())
{

}

te::layout::TitleItemFactory::~TitleItemFactory()
{

}
