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
  \file terralib/qt/widgets/charts/chartStyleDialog.cpp

  \brief A widget created to customize the style parameters of a chart
*/

#include "ChartStyleDialog.h"
#include "ChartDisplay.h"
#include "ChartStyle.h"
#include "../utils/ColorPickerToolButton.h"
#include "ui_chartStyleDialog.h"

te::qt::widgets::ChartStyleDialog::ChartStyleDialog(QWidget* parent, Qt::WindowFlags f, QString title, QString PropertyX, QString PropertyY)
  : QDialog(parent, f),
    m_ui(new Ui::ChartStyleDialog)
{
  m_ui->setupUi(this);
  m_ui->m_labelStylePushButton->setEnabled(false);
  m_ui->m_titleStylePushButton->setEnabled(false);
  m_ui->m_chartTitleLineEdit->setText(title);
  m_ui->m_labelXlLineEdit->setText(PropertyX);
  m_ui->m_labelYlLineEdit->setText(PropertyY);

  // Color Picker
  m_colorPicker = new te::qt::widgets::ColorPickerToolButton(this);
  m_colorPicker->setFixedSize(107, 24);

  // Adjusting the color picker
  QGridLayout* layout = new QGridLayout(m_ui->m_colorPickerFrame);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSizeConstraint(QLayout::SetFixedSize);
  layout->addWidget(m_colorPicker);

  m_chartStyle = new te::qt::widgets::ChartStyle();

  connect(m_ui->m_chartTitleLineEdit, SIGNAL(editingFinished()), this, SLOT(onTitleLineEditFinish()));
  connect(m_ui->m_okPushButton, SIGNAL(clicked()), this, SLOT(onOkPushButtonClicked()));
  connect(m_ui->m_helpPushButton, SIGNAL(clicked()), this, SLOT(onHelpPushButtonClicked()));
  connect(m_colorPicker, SIGNAL(colorChanged(const QColor&)), SLOT(onColorChanged(const QColor&)));
}

te::qt::widgets::ChartStyleDialog::~ChartStyleDialog(){}

Ui::ChartStyleDialog* te::qt::widgets::ChartStyleDialog::getForm() const
{
  return m_ui.get();
}

void te::qt::widgets::ChartStyleDialog::onTitleStylePushButtonClicked(){}
void te::qt::widgets::ChartStyleDialog::onLabelStylePushButtonClicked(){}
void te::qt::widgets::ChartStyleDialog::onGridCheckBoxToggled(){}

void te::qt::widgets::ChartStyleDialog::onOkPushButtonClicked()
{
  
}

void te::qt::widgets::ChartStyleDialog::onHelpPushButtonClicked(){}

void te::qt::widgets::ChartStyleDialog::onColorChanged(const QColor& color)
{
  // The new fill color
  m_chartStyle->setColor(color);

  m_colorPicker->setColor(m_chartStyle->getColor());
}

void te::qt::widgets::ChartStyleDialog::onTitleLineEditFinish()
{
  m_chartStyle->setTitle(m_ui->m_chartTitleLineEdit->text().toStdString());
}