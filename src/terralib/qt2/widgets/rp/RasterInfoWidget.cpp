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
  \file terralib/qt/widgets/rp/RasterInfoWidget.cpp

  \brief This file has the RasterInfoWidget class.
*/

// TerraLib
#include "ParameterTableWidget.h"
#include "RasterInfoWidget.h"
#include "ui_RasterInfoWidgetForm.h"
#include "ui_ParameterTableWidgetForm.h"

// QT
#include <QFileDialog>

te::qt::widgets::RasterInfoWidget::RasterInfoWidget(QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f),
    m_ui(new Ui::RasterInfoWidgetForm),
    m_dir("")
{
  m_ui->setupUi(this);

  QGridLayout* layout = new QGridLayout(m_ui->m_widget);
  m_table.reset( new te::qt::widgets::ParameterTableWidget(m_ui->m_widget));
  layout->addWidget(m_table.get());
  layout->setContentsMargins(0,0,0,0);

  m_table->getForm()->m_parameterTitle->setText(tr("Extra parameters"));

  //connects
  connect(m_ui->m_openFileDlgToolButton, SIGNAL(clicked()), this, SLOT(onOpenFileDlgToolButtonClicked()));
}

te::qt::widgets::RasterInfoWidget::~RasterInfoWidget()
{
}

Ui::RasterInfoWidgetForm* te::qt::widgets::RasterInfoWidget::getForm() const
{
  return m_ui.get();
}

std::string te::qt::widgets::RasterInfoWidget::getType() const
{
  if(m_ui->m_fileRadioButton->isChecked())
  {
    return "GDAL";
  }
  else if(m_ui->m_memRadioButton->isChecked())
  {
    return "MEM";
  }
  else if(m_ui->m_dsRadioButton->isChecked())
  {
    return "";
  }

  return "";
}

std::map<std::string, std::string> te::qt::widgets::RasterInfoWidget::getInfo() const
{
  std::map<std::string, std::string> rinfo;

  std::string name = "";

  if(m_ui->m_nameLineEdit->text().isEmpty() == false)
  {
    name = m_ui->m_nameLineEdit->text().toStdString();
  }

  if(m_ui->m_fileRadioButton->isChecked())
  {
    std::string fileName = m_dir + "/" + name + ".tif";

    rinfo["URI"] = fileName;
  }
  else if(m_ui->m_memRadioButton->isChecked())
  {

  }
  else if(m_ui->m_dsRadioButton->isChecked())
  {

  }

  if(m_ui->m_overightRadioButton->isChecked() == false)
  {
    //get extra parameters
    std::map<std::string, std::string> extra = m_table->getMap();

    rinfo.insert(extra.begin(), extra.end());
  }

  return rinfo;
}

std::string te::qt::widgets::RasterInfoWidget::getName() const
{
  std::string name = "";

  if(m_ui->m_nameLineEdit->text().isEmpty() == false)
  {
    name = m_ui->m_nameLineEdit->text().toStdString();
  }

  if(m_ui->m_fileRadioButton->isChecked())
  {
     name += ".tif";
  }

  return name;
}

bool te::qt::widgets::RasterInfoWidget::overight() const
{
  return m_ui->m_overightRadioButton->isChecked();
}

void te::qt::widgets::RasterInfoWidget::onOpenFileDlgToolButtonClicked()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if(dir.isEmpty() == false)
  {
    m_dir = dir.replace(QRegExp("\\\\"), "/").toStdString();

    m_ui->m_fileNameLineEdit->setText(m_dir.c_str());
  }
}