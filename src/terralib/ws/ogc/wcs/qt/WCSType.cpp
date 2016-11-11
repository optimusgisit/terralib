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
   \file terralib/ws/ogc/wcs/qt/WCSType.cpp

   \brief WCS data source type.
 */

#include "WCSType.h"

#include "WCSConnector.h"
#include "WCSLayerSelector.h"
#include "../../../../core/translator/Translator.h"

te::ws::ogc::wcs::qt::WCSType::WCSType()
{
}

te::ws::ogc::wcs::qt::WCSType::~WCSType()
{
}

bool te::ws::ogc::wcs::qt::WCSType::hasDatabaseSupport() const
{
  return false;
}

bool te::ws::ogc::wcs::qt::WCSType::hasFileSupport() const
{
  return false;
}

bool te::ws::ogc::wcs::qt::WCSType::hasRasterSupport() const
{
  return true;
}

bool te::ws::ogc::wcs::qt::WCSType::hasVectorialSupport() const
{
  return false;
}

std::string te::ws::ogc::wcs::qt::WCSType::getName() const
{
  return "WCS2";
}

std::string te::ws::ogc::wcs::qt::WCSType::getTitle() const
{
  return TE_TR("Web Coverage Service");
}

std::string te::ws::ogc::wcs::qt::WCSType::getDescription() const
{
  return TE_TR("Access to geospatial through a Web Coverage Service (WCS)");
}

QWidget* te::ws::ogc::wcs::qt::WCSType::getWidget(int widgetType, QWidget* parent, Qt::WindowFlags f) const
{
  switch(widgetType)
  {
    case te::qt::widgets::DataSourceType::WIDGET_DATASOURCE_CONNECTOR:
      return new WCSConnector(parent, f);

    //TODO Create a WCS layer selection interface
    case DataSourceType::WIDGET_LAYER_SELECTOR:
      return new WCSLayerSelector(parent, f);

    default:
      return 0;
  }
}

QIcon te::ws::ogc::wcs::qt::WCSType::getIcon(int iconType) const
{
  switch(iconType)
  {
    case te::qt::widgets::DataSourceType::ICON_DATASOURCE_SMALL:
      return QIcon::fromTheme("datasource-wcs");

    case te::qt::widgets::DataSourceType::ICON_DATASOURCE_CONNECTOR:
      return QIcon::fromTheme("datasource-wcs");

    default:
      return QIcon::fromTheme("unknown-icon");
  }
}
