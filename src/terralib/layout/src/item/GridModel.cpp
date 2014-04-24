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
  \file GridModel.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "GridModel.h"
#include "ContextItem.h"
#include "Context.h"
#include "../../../../../geometry/Envelope.h"
#include "../../../../../geometry/Coord2D.h"
#include "Property.h"

te::layout::GridModel::GridModel() :
  m_visible(false),
  m_lneVrtDisplacement(0), 
  m_lneHrzDisplacement(0),
  m_initialGridPointX(0),
  m_initialGridPointY(0),
  m_name("GRID_MODEL")
{
  m_properties = new te::layout::Properties(m_name, TPGrid);
}

te::layout::GridModel::~GridModel()
{

}

te::layout::Properties* te::layout::GridModel::getProperties() const
{
  m_properties->clear();

  Property pro_name;
  pro_name.setName("name");
  pro_name.setId("unknown");
  pro_name.setValue(m_name, DataTypeString);

  Property pro_visible;
  pro_visible.setName("grid_visible");
  pro_visible.setId("unknown");
  pro_visible.setValue(m_visible, DataTypeBool);  

  m_properties->addProperty(pro_name);
  m_properties->addProperty(pro_visible);

  m_properties->setObjectName(m_name);
  m_properties->setTypeObj(TPGrid);
  return m_properties;
}

void te::layout::GridModel::updateProperties( te::layout::Properties* properties )
{
  Property pro_name = properties->contains(m_name);

  if(!pro_name.isNull())
  {
    m_name = pro_name.getValue().toString();

    Property pro_visible = pro_name.containsSubProperty("grid_visible");

    if(!pro_visible.isNull())
    {
      m_visible = pro_visible.getValue().toBool();
    }
  }
}

std::string te::layout::GridModel::getName()
{
  return m_name;
}

void te::layout::GridModel::setName( std::string name )
{
  m_name = name;
}

bool te::layout::GridModel::isVisible()
{
  return m_visible;
}

void te::layout::GridModel::setVisible( bool visible )
{
  m_visible = visible;
}



