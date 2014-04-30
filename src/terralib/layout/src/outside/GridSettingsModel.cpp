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
  \file GridSettingsModel.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "GridSettingsModel.h"
#include "Property.h"
#include "Properties.h"

te::layout::GridSettingsModel::GridSettingsModel() 
{
  m_box = te::gm::Envelope(0., 0., 200., 200.);
  m_type = TPGridSettings;
}

te::layout::GridSettingsModel::~GridSettingsModel()
{

}

te::layout::Properties* te::layout::GridSettingsModel::getProperties() const
{
  m_properties->clear();

  Property pro_name;
  pro_name.setName("GridSettings");

  m_properties->addProperty(pro_name);

  m_properties->setTypeObj(m_type);
  return m_properties;
}

void te::layout::GridSettingsModel::updateProperties( te::layout::Properties* properties )
{

}

void te::layout::GridSettingsModel::setOutsideProperty( Property property )
{
  m_property.clear();
  m_property = property;
}

te::layout::Property te::layout::GridSettingsModel::getOutsideProperty()
{
  return m_property;
}

te::layout::Property te::layout::GridSettingsModel::containsOutsideSubProperty( std::string name )
{
  return m_property.containsSubProperty(name);
}

void te::layout::GridSettingsModel::updateOutsideSubProperty( Property property )
{
  m_property.removeSubProperty(property);
  m_property.addSubProperty(property);
}
