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
  \file PropertiesController.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "PropertiesController.h"
#include "../core/pattern/factory/OutsideParamsCreate.h"
#include "../core/pattern/mvc/OutsideModelObservable.h"
#include "../core/pattern/factory/AbstractOutsideFactory.h"
#include "../core/pattern/singleton/Context.h"
#include "../core/pattern/mvc/Observer.h"
#include "../core/enum/Enums.h"

te::layout::PropertiesController::PropertiesController( Observable* o ) :
	OutsideController(o)
{
  create();
}

te::layout::PropertiesController::PropertiesController( Observable* o, EnumType* type ):
  OutsideController(o, type)
{

}

te::layout::PropertiesController::~PropertiesController()
{

}

void te::layout::PropertiesController::setPosition( const double& x, const double& y )
{
  if(m_model)
  {
    OutsideModelObservable* model = dynamic_cast<OutsideModelObservable*>(m_model);
    if(model)
      return model->setPosition(x, y);
  }
}

void te::layout::PropertiesController::create()
{
  EnumType* type = Enums::getInstance().getEnumObjectType()->getPropertiesWindow();
  m_model->setType(type);

  AbstractOutsideFactory* factory = Context::getInstance().getOutsideFactory(); 
  OutsideParamsCreate params(this, m_model);
  if(factory)
    m_view = (Observer*)factory->make(m_model->getType(), params);
}
