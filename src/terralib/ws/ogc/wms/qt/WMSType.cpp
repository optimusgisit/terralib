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
   \file terralib/ws/ogc/wms/qt/WMSType.cpp

   \brief WMS data source type.

   \author Emerson Moraes
 */

#include "WMSType.h"

#include "../../../../core/translator/Translator.h"
#include "WMSConnector.h"
#include "WMSLayerSelector.h"

te::ws::ogc::wms::qt::WMSType::WMSType()
{
}

te::ws::ogc::wms::qt::WMSType::~WMSType()
{
}

bool te::ws::ogc::wms::qt::WMSType::hasDatabaseSupport() const
{
  return false;
}

bool te::ws::ogc::wms::qt::WMSType::hasFileSupport() const
{
  return false;
}

bool te::ws::ogc::wms::qt::WMSType::hasRasterSupport() const
{
  return true;
}

bool te::ws::ogc::wms::qt::WMSType::hasVectorialSupport() const
{
  return false;
}

std::string te::ws::ogc::wms::qt::WMSType::getName() const
{
  return "WMS2";
}

std::string te::ws::ogc::wms::qt::WMSType::getTitle() const
{
  return TE_TR("Web Map Service");
}

std::string te::ws::ogc::wms::qt::WMSType::getDescription() const
{
  return TE_TR("Access to geospatial through a Web Map Service (WMS)");
}

QWidget* te::ws::ogc::wms::qt::WMSType::getWidget(int widgetType, QWidget* parent, Qt::WindowFlags f) const
{
  switch(widgetType)
  {
    case te::qt::widgets::DataSourceType::WIDGET_DATASOURCE_CONNECTOR:
      return new te::ws::ogc::wms::qt::WMSConnector(parent, f);

    case DataSourceType::WIDGET_LAYER_SELECTOR:
      return new te::ws::ogc::wms::qt::WMSLayerSelector(parent, f);

    default:
      return 0;
  }
}

QIcon te::ws::ogc::wms::qt::WMSType::getIcon(int iconType) const
{
  switch(iconType)
  {
    case te::qt::widgets::DataSourceType::ICON_DATASOURCE_SMALL:
      return QIcon::fromTheme("datasource-wms");

    case te::qt::widgets::DataSourceType::ICON_DATASOURCE_CONNECTOR:
      return QIcon::fromTheme("datasource-wms");

    default:
      return QIcon::fromTheme("unknown-icon");
  }
}
