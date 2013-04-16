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
  \file terralib/qt/widgets/charts/GraphicSettings.cpp

  \brief A dialog used to customize a graphic's parameters, weather it is about it's data or it's visual style
*/

#include "ui_GraphicSettingsDialog.h"
#include "GraphicWidget.h"
#include "GraphicWidgetFactory.h"
//#include "HistogramFrameFactory.h"
//#include "ScatterFrameFactory.h"
#include "GraphicSettings.h"

te::qt::widgets::GraphicSettings::GraphicSettings(QWidget* parent)
  : QDialog(parent),
    m_ui(new Ui::GraphicSettingsDialog),
    m_curComp(0)
{
  m_ui->setupUi(this);

  //init factories
//  HistogramFrameFactory::initialize();
//  ScatterFrameFactory::initialize();

  std::vector<std::string> vec;
  const te::qt::widgets::GraphicWidgetFactory::dictionary_type& d = te::qt::widgets::GraphicWidgetFactory::getDictionary();
  te::qt::widgets::GraphicWidgetFactory::dictionary_type::const_iterator it = d.begin();

  while(it != d.end())
  {
    vec.push_back(it->first);
    ++it;
  }

  for(size_t i = 0; i < vec.size(); i++)
  {
    m_ui->m_componentsListWidget->addItem(vec[i].c_str());
  }

  connect(m_ui->m_componentsListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(on_itemClicked(QListWidgetItem*)));

}

te::qt::widgets::GraphicSettings::~GraphicSettings()
{
  delete m_curComp;
}

void te::qt::widgets::GraphicSettings::on_itemClicked(QListWidgetItem * current) 
{
  std::string value = current->text().toStdString();
  delete m_curComp;
  m_curComp = te::qt::widgets::GraphicWidgetFactory::make(value, m_ui->m_scrollArea);
  m_ui->m_scrollArea->setWidget(m_curComp);
  m_curComp->show();
}
