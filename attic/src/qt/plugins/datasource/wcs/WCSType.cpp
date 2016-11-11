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
  \file terralib/qt/plugins/datasource/wcs/WCSType.cpp

  \brief WCS data source type.
*/

// TerraLib
#include "../../../../core/translator/Translator.h"
#include "../../../widgets/layer/selector/DataSetLayerSelector.h"
#include "WCSConnector.h"
#include "WCSType.h"

// STL
#include <cassert>

te::qt::plugins::wcs::WCSType::WCSType()
{
}

te::qt::plugins::wcs::WCSType::~WCSType()
{
}

bool te::qt::plugins::wcs::WCSType::hasDatabaseSupport() const
{
  return false;
}

bool te::qt::plugins::wcs::WCSType::hasFileSupport() const
{
  return false;
}

bool te::qt::plugins::wcs::WCSType::hasRasterSupport() const
{
  return true;
}

bool te::qt::plugins::wcs::WCSType::hasVectorialSupport() const
{
  return false;
}

std::string te::qt::plugins::wcs::WCSType::getName() const
{
  return "WCS";
}

std::string te::qt::plugins::wcs::WCSType::getTitle() const
{
  return TE_TR("Web Coverage Service");
}

std::string te::qt::plugins::wcs::WCSType::getDescription() const
{
  return TE_TR("Access to geospatial through a Web Coverage Service (WCS)");
}

QWidget* te::qt::plugins::wcs::WCSType::getWidget(int widgetType, QWidget* parent, Qt::WindowFlags f) const
{
  switch(widgetType)
  {
    case te::qt::widgets::DataSourceType::WIDGET_DATASOURCE_CONNECTOR:
      return new WCSConnector(parent, f);

    case DataSourceType::WIDGET_LAYER_SELECTOR:
      return new te::qt::widgets::DataSetLayerSelector(parent, f);

    default:
      return 0;
  }
}

QIcon te::qt::plugins::wcs::WCSType::getIcon(int iconType) const
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
