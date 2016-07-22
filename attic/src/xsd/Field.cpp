/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled applications.

    TerraLib is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraLib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TerraLib. See COPYING. If not, write to
    TerraLib Team at <terralib-team@terralib.org>.
 */

/*!
  \file Field.cpp

  \brief It models the field element of an XML Schema.
*/

// TerraLib
#include "Field.h"

// STL
#include <cassert>

te::xsd::Field::Field(std::string* xpath, Annotation* ann, std::string* id)
  : Identifiable(id),
    Annotated(ann),
    m_xpath(xpath)
{
}

te::xsd::Field::Field(const Field& rhs)
  : Identifiable(rhs),
    Annotated(rhs),
    m_xpath(0)
{
}

te::xsd::Field::~Field()
{
  delete m_xpath;
}

te::xsd::Field& te::xsd::Field::operator=(const Field& rhs)
{
  return *this;
}

std::string* te::xsd::Field::getXPath() const
{
  return m_xpath;
}

void te::xsd::Field::setXPath(std::string* xpath)
{
  delete m_xpath;
  m_xpath = xpath;
}