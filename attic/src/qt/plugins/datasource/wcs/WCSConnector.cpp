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
  \file terralib/qt/plugins/datasource/wcs/WCSConnector.cpp

  \brief OGC Web Coverage Service (WCS) connector implementation for the Qt data source widget.
*/

// TerraLib
#include "../../../../dataaccess/datasource/DataSource.h"
#include "../../../../dataaccess/datasource/DataSourceInfoManager.h"
#include "../../../../dataaccess/datasource/DataSourceManager.h"
#include "WCSConnector.h"
#include "WCSConnectorDialog.h"

// Boost
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>

// Qt
#include <QFileDialog>
#include <QMessageBox>

te::qt::plugins::wcs::WCSConnector::WCSConnector(QWidget* parent, Qt::WindowFlags f)
  : te::qt::widgets::AbstractDataSourceConnector(parent, f)
{
}

te::qt::plugins::wcs::WCSConnector::~WCSConnector()
{
}

void te::qt::plugins::wcs::WCSConnector::connect(std::list<te::da::DataSourceInfoPtr>& datasources)
{
  std::auto_ptr<WCSConnectorDialog> cdialog(new WCSConnectorDialog(static_cast<QWidget*>(parent())));

  cdialog->exec();

  te::da::DataSourceInfoPtr ds = cdialog->getDataSource();

  if(ds.get() != 0)
  {
    if(te::da::DataSourceInfoManager::getInstance().add(ds))
      datasources.push_back(ds);
  }
}

void te::qt::plugins::wcs::WCSConnector::create(std::list<te::da::DataSourceInfoPtr>& datasources)
{
  QMessageBox::warning(this,
                       tr("TerraLib Qt Components"),
                       tr("Not implemented yet!\nWe will provide it soon!"));
}

void te::qt::plugins::wcs::WCSConnector::update(std::list<te::da::DataSourceInfoPtr>& /*datasources*/)
{
  QMessageBox::warning(this,
                       tr("TerraLib Qt Components"),
                       tr("Not implemented yet!\nWe will provide it soon!"));
}

void te::qt::plugins::wcs::WCSConnector::remove(std::list<te::da::DataSourceInfoPtr>& /*datasources*/)
{
  QMessageBox::warning(this,
                       tr("TerraLib Qt Components"),
                       tr("Not implemented yet!\nWe will provide it soon!"));
}

