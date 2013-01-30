/*  Copyright (C) 2011-2013 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/qt/af/settings/ProjectFrame.cpp

  \brief A frame for setting project options.
*/

#include "ui_ProjectFrameForm.h"
#include "ProjectFrame.h"

te::qt::af::ProjectFrame::ProjectFrame(QWidget* parent)
  : ComponentWidget(parent),
    m_ui(new Ui::ProjectFrameForm)
{
  m_ui->setupUi(this);
  m_label = "Project";
}

te::qt::af::ProjectFrame::~ProjectFrame()
{

}