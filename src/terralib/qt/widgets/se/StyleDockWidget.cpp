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
  \file terralib/qt/widgets/se/StyleDockWidget.cpp

  \brief A dock widget used control the geographic data style using SE elements and a property browser to show its properties.
*/

// TerraLib
#include "../../../dataaccess/utils/Utils.h"
#include "../../../maptools/AbstractLayer.h"
#include "../../../raster/RasterProperty.h"
#include "../../../raster/BandProperty.h"
#include "PolygonSymbolizerProperty.h"
#include "LineSymbolizerProperty.h"
#include "PointSymbolizerProperty.h"
#include "RasterSymbolizerWidget.h"
#include "TextSymbolizerProperty.h"
#include "RuleProperty.h"
#include "StyleControllerWidget.h"
#include "StyleExplorer.h"
#include "StyleDockWidget.h"

// Qt
#include <QLayout>
#include <QScrollArea>

te::qt::widgets::StyleDockWidget::StyleDockWidget(QWidget* parent, Qt::WindowFlags flags)
  : QDockWidget("Style Explorer", parent, flags),
    m_currentLayer(0),
    m_selColor("")
{
  // Build form
  this->setWidget(buildUi());
}

te::qt::widgets::StyleDockWidget::~StyleDockWidget()
{
}

void te::qt::widgets::StyleDockWidget::setLayer(te::map::AbstractLayer* layer, std::string selColor)
{
  m_selColor = selColor;
  m_currentLayer = layer;
  m_styleController->setLayer(layer, selColor);
}

void te::qt::widgets::StyleDockWidget::setTabStatus(bool status)
{
  m_tabWidget->setTabEnabled(0, status); // Rules
  m_tabWidget->setTabEnabled(1, status); // Polygons
  m_tabWidget->setTabEnabled(2, status); // Lines
  m_tabWidget->setTabEnabled(3, status); // Points
  m_tabWidget->setTabEnabled(4, status); // Raster
  m_tabWidget->setTabEnabled(5, status); // Text
}

void te::qt::widgets::StyleDockWidget::updateUi()
{
}

QWidget* te::qt::widgets::StyleDockWidget::buildUi()
{
  // The result widget
  QWidget* w = new QWidget(this);

  QGridLayout* layout = new QGridLayout(w);

  // StyleExplorer
  m_styleController = new te::qt::widgets::StyleControllerWidget(w);
  layout->addWidget(m_styleController, 0, 0);
  m_styleController->setMaximumHeight(300);

  connect(m_styleController, SIGNAL(mapRefresh()), this, SLOT(onMapRefresh()));
  connect(m_styleController->getStyleExplorer(), SIGNAL(symbolizerClicked(te::se::Symbolizer*)), this, SLOT(onSymbolizerSelected(te::se::Symbolizer*)));
  connect(m_styleController->getStyleExplorer(), SIGNAL(ruleClicked(te::se::Rule*)), this, SLOT(onRuleSelected(te::se::Rule*)));
  connect(m_styleController->getStyleExplorer(), SIGNAL(styleImported(te::se::Style*, bool)), this, SLOT(onStyleImported(te::se::Style*, bool)));
  connect(this, SIGNAL(symbolizerChanged(te::se::Symbolizer*)), m_styleController->getStyleExplorer(), SLOT(onSymbolizerChanged(te::se::Symbolizer*)));

  // Tab widget
  m_tabWidget = new QTabWidget(w);
  m_tabWidget->setTabPosition(QTabWidget::South);
  layout->addWidget(m_tabWidget, 1, 0);
  m_tabWidget->setMinimumWidth(360);

  //Rule Property
  m_ruleWidget = new te::qt::widgets::RuleProperty(m_tabWidget);
  m_tabWidget->addTab(m_ruleWidget, tr("Rule"));

  // PolygonSymbolizer Property
  m_polyWidget = new te::qt::widgets::PolygonSymbolizerProperty(m_tabWidget);
  m_tabWidget->addTab(m_polyWidget, tr("Polygons"));
  connect(m_polyWidget, SIGNAL(symbolizerChanged()), this, SLOT(onPolygonSymbolizerChanged()));

  // LineSymbolizer Property
  m_lineWidget = new te::qt::widgets::LineSymbolizerProperty(m_tabWidget);
  m_tabWidget->addTab(m_lineWidget, tr("Lines"));
  connect(m_lineWidget, SIGNAL(symbolizerChanged()), this, SLOT(onLineSymbolizerChanged()));

  // Point Symbolizer Widget
  m_pointWidget = new te::qt::widgets::PointSymbolizerProperty(m_tabWidget);
  m_tabWidget->addTab(m_pointWidget, tr("Points"));
  connect(m_pointWidget, SIGNAL(symbolizerChanged()), this, SLOT(onPointSymbolizerChanged()));

  // RasterSymbolizer Property Widget
  m_rasterWidget = new te::qt::widgets::RasterSymbolizerWidget(m_tabWidget);
  m_tabWidget->addTab(m_rasterWidget, tr("Raster"));
  connect(m_rasterWidget, SIGNAL(symbolizerChanged()), this, SLOT(onRasterSymbolizerChanged()));

  // Text Symbolizer Widget
  m_textWidget = new te::qt::widgets::TextSymbolizerProperty(m_tabWidget);
  m_tabWidget->addTab(m_textWidget, tr("Text"));
  connect(m_textWidget, SIGNAL(symbolizerChanged()), this, SLOT(onTextSymbolizerChanged()));

  setTabStatus(false);

  return w;
}

void te::qt::widgets::StyleDockWidget::onRuleSelected(te::se::Rule* r)
{
  setTabStatus(false);

  m_tabWidget->setCurrentIndex(0);
  m_tabWidget->setTabEnabled(0, true);

  m_ruleWidget->setRule(r);
}

void te::qt::widgets::StyleDockWidget::onSymbolizerSelected(te::se::Symbolizer* s)
{
  setTabStatus(false);

  if(s->getType() == "PolygonSymbolizer")
  {
    m_tabWidget->setCurrentWidget(m_polyWidget);
    m_tabWidget->setTabEnabled(1, true);
    m_polyWidget->setSymbolizer(dynamic_cast<te::se::PolygonSymbolizer*>(s));
  }
  else if(s->getType() == "LineSymbolizer")
  {
    m_tabWidget->setCurrentWidget(m_lineWidget);
    m_tabWidget->setTabEnabled(2, true);
    m_lineWidget->setSymbolizer(dynamic_cast<te::se::LineSymbolizer*>(s));
  }
  else if(s->getType() == "PointSymbolizer")
  {
    m_tabWidget->setCurrentWidget(m_pointWidget);
    m_tabWidget->setTabEnabled(3, true);
    m_pointWidget->setSymbolizer(dynamic_cast<te::se::PointSymbolizer*>(s));
  }
  else if(s->getType() == "RasterSymbolizer")
  {
    m_tabWidget->setCurrentIndex(4);
    m_tabWidget->setTabEnabled(4, true);

    std::auto_ptr<te::map::LayerSchema> lschema = m_currentLayer->getSchema();

    te::rst::RasterProperty* prop = te::da::GetFirstRasterProperty(lschema.get());

    const std::vector<te::rst::BandProperty*>& bprops = prop->getBandProperties();
    
    m_rasterWidget->setBandProperty(bprops);

    m_rasterWidget->setRasterSymbolizer(dynamic_cast<te::se::RasterSymbolizer*>(s));
  }
  else if (s->getType() == "TextSymbolizer")
  {
    std::vector<std::string> propNames;

    std::auto_ptr<te::da::DataSetType> dsType = m_currentLayer->getSchema();

    for (std::size_t t = 0; t < dsType->getProperties().size(); ++t)
    {
      if (dsType->getProperty(t)->getType() != te::dt::GEOMETRY_TYPE)
        propNames.push_back(dsType->getProperty(t)->getName());
    }
    
    m_tabWidget->setCurrentWidget(m_textWidget);
    m_tabWidget->setTabEnabled(5, true);

    m_textWidget->setLabels(propNames);

    te::se::TextSymbolizer* symbText = dynamic_cast<te::se::TextSymbolizer*>(s);

    if (symbText)
    {
      if (!symbText->getLabel() || te::se::GetString(symbText->getLabel()) == "")
      {
        if (!propNames.empty())
          symbText->setLabel(new te::se::ParameterValue(propNames[0]));
      }

      m_textWidget->setSymbolizer(symbText);
    }
  }
}

void te::qt::widgets::StyleDockWidget::onStyleImported(te::se::Style* style, bool isVisual)
{
  if (isVisual)
    m_currentLayer->setStyle(style);
  else
    m_currentLayer->setSelectionStyle(style);

  m_styleController->setLayer(m_currentLayer, m_selColor);

  emit symbolChanged(m_currentLayer);
}

void te::qt::widgets::StyleDockWidget::onPolygonSymbolizerChanged()
{
  te::se::Symbolizer* s = m_polyWidget->getSymbolizer();
  emit symbolizerChanged(s);

  emit symbolChanged(m_currentLayer);
}

void te::qt::widgets::StyleDockWidget::onLineSymbolizerChanged()
{
  te::se::Symbolizer* s = m_lineWidget->getSymbolizer();
  emit symbolizerChanged(s);

  emit symbolChanged(m_currentLayer);
}

void te::qt::widgets::StyleDockWidget::onPointSymbolizerChanged()
{
  te::se::Symbolizer* s = m_pointWidget->getSymbolizer();
  emit symbolizerChanged(s);

  emit symbolChanged(m_currentLayer);
}

void te::qt::widgets::StyleDockWidget::onRasterSymbolizerChanged()
{
  te::se::Symbolizer* s = m_rasterWidget->getRasterSymbolizer();
  emit symbolizerChanged(s);

  emit symbolChanged(m_currentLayer);
}

void te::qt::widgets::StyleDockWidget::onTextSymbolizerChanged()
{
  te::se::Symbolizer* s = m_textWidget->getSymbolizer();
  emit symbolizerChanged(s);

  emit symbolChanged(m_currentLayer);
}

void te::qt::widgets::StyleDockWidget::onMapRefresh()
{
  emit repaintMapDisplay();
}
