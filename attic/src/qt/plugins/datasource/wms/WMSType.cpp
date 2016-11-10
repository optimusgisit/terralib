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
  \file terralib/qt/plugins/datasource/wms/WMSType.cpp

  \brief WMS data source type.
*/

// TerraLib
#include "../../../../core/translator/Translator.h"
#include "../../../../wms/qt/WMSLayerSelector.h"
#include "WMSConnector.h"
#include "WMSType.h"

// STL
#include <cassert>

te::qt::plugins::wms::WMSType::WMSType()
{
}

te::qt::plugins::wms::WMSType::~WMSType()
{
}

bool te::qt::plugins::wms::WMSType::hasDatabaseSupport() const
{
  return false;
}

bool te::qt::plugins::wms::WMSType::hasFileSupport() const
{
  return false;
}

bool te::qt::plugins::wms::WMSType::hasRasterSupport() const
{
  return false;
}

bool te::qt::plugins::wms::WMSType::hasVectorialSupport() const
{
  return false;
}

std::string te::qt::plugins::wms::WMSType::getName() const
{
  return "WMS";
}

std::string te::qt::plugins::wms::WMSType::getTitle() const
{
  return TE_TR("Web Map Service");
}

std::string te::qt::plugins::wms::WMSType::getDescription() const
{
  return TE_TR("Access to geospatial through a Web Map Service (WMS)");
}

QWidget* te::qt::plugins::wms::WMSType::getWidget(int widgetType, QWidget* parent, Qt::WindowFlags f) const
{
  switch(widgetType)
  {
    case te::qt::widgets::DataSourceType::WIDGET_DATASOURCE_CONNECTOR:
      return new WMSConnector(parent, f);

    case DataSourceType::WIDGET_LAYER_SELECTOR:
      return new te::wms::WMSLayerSelector(parent, f);

    default:
      return 0;
  }
}

QIcon te::qt::plugins::wms::WMSType::getIcon(int iconType) const
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
