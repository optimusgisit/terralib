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
  \file terralib/qt/widgets/charts/ScatterCreatorDialog.cpp

  \brief A widget used to define the basic parameters of a new Scatter chart.
*/

//Terralib

#include "ChartDisplay.h"
#include "ChartDisplayWidget.h"
#include "../../../dataaccess.h"
#include "../../../datatype/Property.h"
#include "../../../se/Graphic.h"
#include "ScatterChart.h"
#include "ScatterDialog.h"
#include "ScatterDataWidget.h"
#include "ScatterStyle.h"
#include "Symbol.h"
#include "ui_ScatterDialogForm.h"

//QT
#include <QtGui/QDockWidget>

te::qt::widgets::ScatterDialog::ScatterDialog(te::da::DataSet* dataSet, QWidget* parent,  Qt::WindowFlags f)
  : QDialog(parent, f),
    m_ui(new Ui::ScatterDialogForm)
{
    m_ui->setupUi(this);

  // Scatter data Widget
  m_scatterDataWidget = new te::qt::widgets::ScatterDataWidget(dataSet, this, 0);

  // Adjusting...
  QGridLayout* layout = new QGridLayout(m_ui->m_dataWidgetFrame);
  layout->addWidget(m_scatterDataWidget);
  this->layout()->setSizeConstraint(QLayout::SetFixedSize);

// connect signal and slots
  connect(m_ui->m_okPushButton, SIGNAL(clicked()), this, SLOT(onOkPushButtonClicked()));
  connect(m_ui->m_helpPushButton, SIGNAL(clicked()), this, SLOT(onHelpPushButtonClicked()));
}

te::qt::widgets::ScatterDialog::~ScatterDialog(){}

void te::qt::widgets::ScatterDialog::onHelpPushButtonClicked(){}

void te::qt::widgets::ScatterDialog::onOkPushButtonClicked()
{
  m_scatterChart = new te::qt::widgets::ScatterChart(m_scatterDataWidget->getScatter());
  m_scatterChart->setScatterStyle(new te::qt::widgets::ScatterStyle());

  //Adjusting the chart Display
  te::qt::widgets::ChartDisplay* chartDisplay = new te::qt::widgets::ChartDisplay(0, QString::fromStdString("Scatter"));;
  m_scatterChart->attach(chartDisplay);
  chartDisplay->show();
  chartDisplay->replot();

  //Adjusting the chart widget
  te::qt::widgets::ChartDisplayWidget* chartWidget = new te::qt::widgets::ChartDisplayWidget(m_scatterChart, te::qt::widgets::SCATTER_CHART, chartDisplay, this->parentWidget());
  chartWidget->setDisplay(chartDisplay);

  // Docking
  QDockWidget* doc = new QDockWidget(this->parentWidget(), Qt::Dialog);
  doc->setWidget(chartWidget);
  doc->setWindowTitle("Chart");
  chartWidget->setParent(doc);

  this->close();
  doc->show();
}
