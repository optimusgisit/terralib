/*  Copyright (C) 2011-2012 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/qt/widgets/datasource/connector/wcs/WCSType.cpp

  \brief WCS data source type.
*/

// TerraLib
#include <terralib/common/Translator.h>
//#include "../../../layer/selector/core/DataSetLayerSelector.h"
#include "WCSConnector.h"
#include "WCSType.h"

// STL
#include <cassert>

namespace plg_wcs = qt_af::plugin::wcs;
namespace te_qt = te::qt::widgets;

plg_wcs::WCSType::WCSType()
{
}

plg_wcs::WCSType::~WCSType()
{
}

std::string plg_wcs::WCSType::getName() const
{
  return "WCS";
}

std::string plg_wcs::WCSType::getTitle() const
{
  return AF_TR_WCS("Web Coverage Service");
}

std::string plg_wcs::WCSType::getDescription() const
{
  return AF_TR_WCS("Access to geospatial through a Web Coverage Service (WCS)");
}

QWidget* plg_wcs::WCSType::getWidget(int widgetType, QWidget* parent, Qt::WindowFlags f) const
{
  switch(widgetType)
  {
    case te_qt::DataSourceType::WIDGET_DATASOURCE_CONNECTOR:
      return new WCSConnector(parent, f);

    //case DataSourceType::WIDGET_LAYER_SELECTOR:
      //return new DataSetLayerSelector(parent, f);

    default:
      return 0;
  }
}

QIcon plg_wcs::WCSType::getIcon(int iconType) const
{
  switch(iconType)
  {
    case te_qt::DataSourceType::ICON_DATASOURCE_SMALL:
      return QIcon::fromTheme("datasource-wcs");

    case te_qt::DataSourceType::ICON_DATASOURCE_CONNECTOR:
      return QIcon::fromTheme("datasource-wcs");

    default:
      return QIcon::fromTheme("unknown-icon");
  }
}

