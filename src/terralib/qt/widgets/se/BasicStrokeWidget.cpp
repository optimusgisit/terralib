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
  \file terralib/qt/widgets/se/BasicStrokeWidget.cpp

  \brief A widget used to build a basic stroke element.
*/

// TerraLib
#include "../../../maptools/Utils.h"
#include "../../../maptools/Enums.h"
#include "../../../se/Stroke.h"
#include "../../../se/SvgParameter.h"
#include "BasicStrokeWidget.h"
#include "ui_BasicStrokeWidgetForm.h"

// Qt
#include <QtGui/QColorDialog>
#include <QtGui/QPainter>

// STL
#include <cassert>

te::qt::widgets::BasicStrokeWidget::BasicStrokeWidget(QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f),
    m_ui(new Ui::BasicStrokeWidgetForm),
    m_stroke(new te::se::Stroke)
{
  m_ui->setupUi(this);

  initialize();

  // Signals & slots
  connect(m_ui->m_strokeColorPushButton, SIGNAL(clicked()), SLOT(onStrokeColorPushButtonClicked()));
  connect(m_ui->m_strokeOpacitySlider, SIGNAL(valueChanged(int)), SLOT(onStrokeOpacitySliderValueChanged(int)));
  connect(m_ui->m_strokeWidthDoubleSpinBox, SIGNAL(valueChanged(const QString&)), SLOT(onStrokeWidthDoubleSpinBoxValueChanged(const QString&)));
  connect(m_ui->m_strokeDashComboBox, SIGNAL(currentIndexChanged(int)), SLOT(onStrokeDashComboBoxCurrentIndexChanged(int)));
  connect(m_strokeJoinStyleButtonGroup, SIGNAL(buttonClicked(int)), SLOT(onStrokeJoinStyleChanged(int)));
  connect(m_strokeCapStyleButtonGroup, SIGNAL(buttonClicked(int)), SLOT(onStrokeCapStyleChanged(int)));
}

te::qt::widgets::BasicStrokeWidget::~BasicStrokeWidget()
{
  delete m_stroke;
}

void te::qt::widgets::BasicStrokeWidget::setStroke(const te::se::Stroke* stroke)
{
  assert(stroke);

  delete m_stroke;
  
  m_stroke = stroke->clone();

  updateUi();
}

te::se::Stroke* te::qt::widgets::BasicStrokeWidget::getStroke() const
{
  return m_stroke->clone();
}

void te::qt::widgets::BasicStrokeWidget::initialize()
{
  // Temp: Bulding predefined dash styles
  m_dashes.push_back("");
  m_dashes.push_back("1 2");
  m_dashes.push_back("4 2");
  m_dashes.push_back("4 2 1 2");
  m_dashes.push_back("4 2 1 2 1 2");
  // more?!

  // Default stroke color
  m_color = QColor(TE_SE_DEFAULT_STROKE_BASIC_COLOR);
  updateUiStrokeColor();

  // Width uoms
  m_ui->m_strokeUomComboBox->addItem("px");

  // Dash styles selection
  fillStrokeDashStyleComboBox();

  // Controlling Join Style Buttons
  m_strokeJoinStyleButtonGroup = new QButtonGroup(this);
  m_strokeJoinStyleButtonGroup->addButton(m_ui->m_miterJoinPushButton, te::map::MiterJoin);
  m_strokeJoinStyleButtonGroup->addButton(m_ui->m_roundJoinPushButton, te::map::RoundJoin);
  m_strokeJoinStyleButtonGroup->addButton(m_ui->m_bevelJoinPushButton, te::map::BevelJoin);

  // Controlling Cap Style Buttons
  m_strokeCapStyleButtonGroup = new QButtonGroup(this);
  m_strokeCapStyleButtonGroup->addButton(m_ui->m_buttCapPushButton, te::map::ButtCap);
  m_strokeCapStyleButtonGroup->addButton(m_ui->m_roundCapPushButton, te::map::RoundCap);
  m_strokeCapStyleButtonGroup->addButton(m_ui->m_squareCapPushButton, te::map::SquareCap);

  // For now: hard-coded icon paths.
  m_ui->m_miterJoinPushButton->setIcon(QIcon("D:/terralib5/terralib5/src/terralib/qt/widgets/se/resources/miterjoin.png"));
  m_ui->m_roundJoinPushButton->setIcon(QIcon("D:/terralib5/terralib5/src/terralib/qt/widgets/se/resources/roundjoin.png"));
  m_ui->m_bevelJoinPushButton->setIcon(QIcon("D:/terralib5/terralib5/src/terralib/qt/widgets/se/resources/beveljoin.png"));

  m_ui->m_buttCapPushButton->setIcon(QIcon("D:/terralib5/terralib5/src/terralib/qt/widgets/se/resources/buttcap.png"));
  m_ui->m_roundCapPushButton->setIcon(QIcon("D:/terralib5/terralib5/src/terralib/qt/widgets/se/resources/roundcap.png"));
  m_ui->m_squareCapPushButton->setIcon(QIcon("D:/terralib5/terralib5/src/terralib/qt/widgets/se/resources/squarecap.png"));
}

void te::qt::widgets::BasicStrokeWidget::updateUi()
{
  // Color
  te::color::RGBAColor rgba(TE_SE_DEFAULT_STROKE_BASIC_COLOR, TE_OPAQUE);
  te::map::GetColor(m_stroke, rgba);
  m_color = QColor(rgba.getRgba());
  m_color.setAlpha(rgba.getAlpha());
  updateUiStrokeColor();

  // Opacity
  m_ui->m_strokeOpacitySlider->setValue(m_color.alphaF() * 100);

  // Width
  const te::se::SvgParameter* width = m_stroke->getWidth();
  if(width)
    m_ui->m_strokeWidthDoubleSpinBox->setValue(te::map::GetDouble(width));

  // Dash style
  const te::se::SvgParameter* dasharray = m_stroke->getDashArray();
  if(dasharray)
    updateUiDashStyle(te::map::GetString(dasharray));

  // Join Style
  const te::se::SvgParameter* join = m_stroke->getLineJoin();
  if(join)
    updateUiJoinStyle(te::map::GetString(join));

  // Cap Style
  const te::se::SvgParameter* cap = m_stroke->getLineCap();
  if(cap)
    updateUiCapStyle(te::map::GetString(cap));
}

void te::qt::widgets::BasicStrokeWidget::updateUiStrokeColor()
{
  QPalette pallete;
  pallete.setColor(QPalette::Background, m_color);
  m_ui->m_strokeColorLabel->setPalette(pallete);
}

void te::qt::widgets::BasicStrokeWidget::updateUiDashStyle(const std::string& pattern)
{
  int i = 0;
  std::vector<std::string>::iterator it;
  for(it = m_dashes.begin(); it != m_dashes.end(); ++it) // Try find the given pattern
  {
    if(pattern == *it)
    {
      m_ui->m_strokeDashComboBox->setCurrentIndex(i);
      return;
    }
    ++i;
  }

  if(it == m_dashes.end()) // Not found
  {
    m_dashes.push_back(pattern); // Adding on dash styles...
    fillStrokeDashStyleComboBox(); // Updating dash combo box...
    m_ui->m_strokeDashComboBox->setCurrentIndex(i); // Select it!
  }
}

void te::qt::widgets::BasicStrokeWidget::updateUiJoinStyle(const std::string& style)
{
  // Default TE_SE_MITRE
  if(style == TE_SE_ROUND_JOIN)
    m_ui->m_roundJoinPushButton->setChecked(true);
  else if(style == TE_SE_BEVEL_JOIN)
    m_ui->m_bevelJoinPushButton->setChecked(true);
}

void te::qt::widgets::BasicStrokeWidget::updateUiCapStyle(const std::string& style)
{
  // Default TE_SE_BUTT
  if(style == TE_SE_ROUND_CAP)
    m_ui->m_roundCapPushButton->setChecked(true);
  else if(style == TE_SE_SQUARE_CAP)
    m_ui->m_squareCapPushButton->setChecked(true);
}

void te::qt::widgets::BasicStrokeWidget::fillStrokeDashStyleComboBox()
{
  // Cleaning...
  m_ui->m_strokeDashComboBox->clear();

  // Dash graphical representation size
  QSize size(200, 5);

  // Adjusts the dash combo box to comport the graphical representation
  m_ui->m_strokeDashComboBox->setIconSize(size);

  // Line that will be draw with the dashe styles
  QLine line(0, size.height() * 0.5, size.width(), size.height() * 0.5);

  // Setuping...
  QPen pen;
  pen.setWidth(2);

  // Draw!
  std::vector<std::string>::iterator it = m_dashes.begin();
  for(it = m_dashes.begin(); it != m_dashes.end(); ++it) // for each dash style
  {
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    std::string pattern = *it;
    std::vector<double> dasharray;
    te::map::GetDashStyle(pattern, dasharray);
      
    QVector<qreal> qdasharray = QVector<qreal>::fromStdVector(dasharray);

    pen.setDashPattern(qdasharray);
    painter.setPen(pen);
    painter.drawLine(line);
    
    m_ui->m_strokeDashComboBox->addItem(pixmap, pattern.c_str());
  }
}

void te::qt::widgets::BasicStrokeWidget::onStrokeColorPushButtonClicked()
{
  QColor color = QColorDialog::getColor(m_color, this);
  if(!color.isValid())
    return;

  // The new stroke color
  m_color.setRgb(color.red(), color.green(), color.blue(), m_color.alpha());

  // Updating stroke color
  m_stroke->setColor(m_color.name().toStdString());
  emit strokeChanged();

  updateUiStrokeColor();
}

void te::qt::widgets::BasicStrokeWidget::onStrokeOpacitySliderValueChanged(int value)
{
  double opacity = value / 100.0;

  // Updating stroke opacity
  m_stroke->setOpacity(QString::number(opacity, 'g', 2).toStdString());
  emit strokeChanged();

  m_color.setAlpha(opacity * 255);

  updateUiStrokeColor();
}

void te::qt::widgets::BasicStrokeWidget::onStrokeWidthDoubleSpinBoxValueChanged(const QString& text)
{
  m_stroke->setWidth(text.toStdString());
  emit strokeChanged();
}

void te::qt::widgets::BasicStrokeWidget::onStrokeDashComboBoxCurrentIndexChanged(int index)
{
  std::string pattern = m_dashes[index];
  m_stroke->setDashArray(pattern);
  emit strokeChanged();
}

void te::qt::widgets::BasicStrokeWidget::onStrokeJoinStyleChanged(int style)
{
  switch(style)
  {
    case te::map::MiterJoin:
      m_stroke->setLineJoin(TE_SE_MITRE_JOIN);
    break;

    case te::map::RoundJoin:
      m_stroke->setLineJoin(TE_SE_ROUND_JOIN);
    break;

    case te::map::BevelJoin:
      m_stroke->setLineJoin(TE_SE_BEVEL_JOIN);
    break;
  }

  emit strokeChanged();
}

void te::qt::widgets::BasicStrokeWidget::onStrokeCapStyleChanged(int style)
{
  switch(style)
  {
    case te::map::ButtCap:
      m_stroke->setLineCap(TE_SE_BUTT_CAP);
    break;

    case te::map::RoundCap:
      m_stroke->setLineCap(TE_SE_ROUND_CAP);
    break;

    case te::map::SquareCap:
      m_stroke->setLineCap(TE_SE_SQUARE_CAP);
    break;
  }

  emit strokeChanged();
}
