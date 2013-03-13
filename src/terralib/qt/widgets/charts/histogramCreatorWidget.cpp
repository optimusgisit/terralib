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
  \file terralib/qt/widgets/charts/chartStyleWidget.cpp

  \brief A widget created to customize the style parameters of a chart
*/

#include "ui_histogramCreatorWidget.h"
#include "HistogramCreatorWidget.h"

te::qt::widgets::HistogramCreatorWidget::HistogramCreatorWidget(QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f),
    m_ui(new Ui::histogramWidget)
{
    m_ui->setupUi(this);
}

te::qt::widgets::HistogramCreatorWidget::~HistogramCreatorWidget(){}

void te::qt::widgets::HistogramCreatorWidget::onStylePushButtonClicked(){}
void te::qt::widgets::HistogramCreatorWidget::onBarStylePushButtonClicked(){}
void te::qt::widgets::HistogramCreatorWidget::onOkPushButtonClicked(){}
void te::qt::widgets::HistogramCreatorWidget::onCancelPushButtonClicked(){}
void te::qt::widgets::HistogramCreatorWidget::onHelpPushButtonClicked(){}