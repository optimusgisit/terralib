/*  Copyright (C) 2010-2013 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/qt/widgets/charts/scatterStyle.h

  \brief A class used to define the style of a scatter's chart
*/

//TerraLib
#include "../../../se/Fill.h"
#include "../../../se/ParameterValue.h"
#include "../../../se/Graphic.h"
#include "../../../se/Mark.h"
#include "../../../se/Stroke.h"
#include"MarkSymbol.h"
#include "ScatterStyle.h"
#include "Utils.h"

te::qt::widgets::ScatterStyle::ScatterStyle ()
{
  //COnfiguring a defulat mark
  te::se::Mark* mark = new te::se::Mark();
  std::string* markName =  new std::string("circle");
  mark->setFill(new te::se::Fill());
  mark->setStroke(new te::se::Stroke());
  mark->setWellKnownName(markName);

//Configuring a default graphic
  m_graphic  = new te::se::Graphic();
  te::se::ParameterValue* grSize = new te::se::ParameterValue("8");
  m_graphic->add(mark);
  m_graphic->setSize(grSize);
}

te::qt::widgets::ScatterStyle::~ScatterStyle()
{
  delete m_graphic;
}

te::qt::widgets::ScatterStyle* te::qt::widgets::ScatterStyle::clone()
{
  te::qt::widgets::ScatterStyle* style = new te::qt::widgets::ScatterStyle();
  style->setGraphic(this->getGraphic()->clone());
  return style;
}

te::se::Graphic* te::qt::widgets::ScatterStyle::getGraphic()
{
  return m_graphic;
}

void te::qt::widgets::ScatterStyle::setGraphic(te::se::Graphic* newGraphic)
{
  delete m_graphic;
  m_graphic = newGraphic;
}

QwtSymbol* te::qt::widgets::ScatterStyle::getSymbol()
{
  return te::qt::widgets::Terralib2Qwt(m_graphic); 
}