/*  Copyright (C) 2011-2012 National Institute For Space Research (INPE) - Brazil.

    This file is part of TerraView - A GIS Application.

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

// TerraLib
#include "../../../maptools/AbstractLayer.h"
#include "../../../maptools/DataSetLayer.h"
#include "../../../maptools/RasterLayer.h"
#include "../../../qt/widgets/se/VisualDockWidget.h"
#include "../../../se/Symbolizer.h"
#include "../events/Event.h"
#include "../events/LayerEvents.h"
#include "../ApplicationController.h"
#include "SymbolizerExplorer.h"

// STL
#include <cassert>

te::qt::af::SymbolizerExplorer::SymbolizerExplorer(te::qt::widgets::VisualDockWidget* explorer, QObject* parent)
  : QObject(parent),
    m_explorer(explorer)
{
  assert(explorer);
}

te::qt::af::SymbolizerExplorer::~SymbolizerExplorer()
{
}

te::qt::widgets::VisualDockWidget* te::qt::af::SymbolizerExplorer::getExplorer() const
{
  return m_explorer;
}

void te::qt::af::SymbolizerExplorer::onApplicationTriggered(te::qt::af::evt::Event* evt)
{
  switch(evt->m_id)
  {
    case te::qt::af::evt::LAYER_SELECTED:
    {
      te::qt::af::evt::LayerSelected* e = static_cast<te::qt::af::evt::LayerSelected*>(evt);

      te::map::AbstractLayer* layer = e->m_layer;
      assert(layer);

      m_explorer->setStyle(layer->getStyle(), layer);
    }
    break;

    case te::qt::af::evt::LAYER_STYLE_SELECTED:
    {
      te::qt::af::evt::LayerStyleSelected* e = static_cast<te::qt::af::evt::LayerStyleSelected*>(evt);

      te::map::AbstractLayer* layer = e->m_layer;
      assert(layer);

      m_explorer->setStyle(layer->getStyle(), layer);

      m_explorer->setVisible(true);
    }
    break;

    default:
      return;
  }
}

void te::qt::af::SymbolizerExplorer::onSymbolizerChanged()
{
}
