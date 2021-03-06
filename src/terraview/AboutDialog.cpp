/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of TerraView - A Free and Open Source GIS Application.

    TerraView is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraView is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TerraLib Code Editor. See COPYING. If not, write to
    TerraLib Team at <terralib-team@dpi.inpe.br>.
 */

/*!
  \file terraview/AboutDialog.cpp

  \brief A Qt dialog showing TerraView about window.
*/

// TerraView
#include "AboutDialog.h"
#include "Config.h"
#include "ui_AboutDialogForm.h"

// TerraLib
#include <terralib/common/Version.h>
#include <terralib/qt/af/ApplicationController.h>
#include <terralib/Version.h>

// Qt
#include <QtGui/QPixmap>

AboutDialog::AboutDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f),
    m_ui(new Ui::AboutDialogForm)
{
  m_ui->setupUi(this);

  std::string copyrightStr = tr("<p>Copyright &copy; 2010 INPE<BR>").toUtf8().data();
  m_ui->m_copyrightLabel->setText(copyrightStr.c_str());

  std::string terraviewVersionStr = std::string(tr("TerraView Version: ").toUtf8().data()) + TERRALIB_VERSION_STRING;
  m_ui->m_terraviewVersionLabel->setText(terraviewVersionStr.c_str());

  std::string terralibVersionStr = std::string(tr("TerraLib Version: ").toUtf8().data()) + TERRALIB_VERSION_STRING;
  m_ui->m_terralibVersionLabel->setText(terralibVersionStr.c_str());

  std::string buildDateStr = std::string(tr("Build Date: ").toUtf8().data()) + te::common::Version::buildDate();
  m_ui->m_buildDateLabel->setText(buildDateStr.c_str());
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::setTerraViewLogoFilePath(std::string path)
{
  QPixmap pixmapTVLarge(path.c_str());

  m_ui->m_applicationLargeLogo->setPixmap(pixmapTVLarge);
}

void AboutDialog::setTerraLibLogoFilePath(std::string path)
{
  QPixmap pixmapTE(path.c_str());

  pixmapTE = pixmapTE.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  m_ui->m_terralibLogo->setPixmap(pixmapTE);
}
