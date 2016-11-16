/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

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
#include "../../../qt/widgets/layer/explorer/LayerItemView.h"
#include "../../../qt/widgets/se/StyleDockWidget.h"
#include "../events/ApplicationEvents.h"
#include "../events/LayerEvents.h"
#include "../ApplicationController.h"
#include "StyleExplorer.h"

// STL
#include <cassert>

te::qt::af::StyleExplorer::StyleExplorer(te::qt::widgets::StyleDockWidget* explorer, te::qt::af::ApplicationController* app, QObject* parent)
  : QObject(parent),
    m_explorer(explorer),
    m_app(app)
{
  assert(explorer);

  connect(m_explorer, SIGNAL(symbolChanged(te::map::AbstractLayer*)), SLOT(styleChanged(te::map::AbstractLayer*)));
}

te::qt::af::StyleExplorer::~StyleExplorer()
{
}

te::qt::widgets::StyleDockWidget* te::qt::af::StyleExplorer::getExplorer() const
{
  return m_explorer;
}

void te::qt::af::StyleExplorer::onApplicationTriggered(te::qt::af::evt::Event* evt)
{
  switch(evt->m_id)
  {
    case te::qt::af::evt::LAYER_SELECTED:
    {
      te::qt::af::evt::LayerSelected* e = static_cast<te::qt::af::evt::LayerSelected*>(evt);

      te::map::AbstractLayerPtr layer = e->m_layer;
      assert(layer);

      if(layer->isValid() && layer->getStyle())
        m_explorer->setLayer(layer.get(), m_app->getSelectionColor().name(QColor::HexRgb).toStdString());
    }
    break;

    case te::qt::af::evt::LAYER_STYLE_SELECTED:
    {
      te::qt::af::evt::LayerStyleSelected* e = static_cast<te::qt::af::evt::LayerStyleSelected*>(evt);

      te::map::AbstractLayerPtr layer = e->m_layer;
      assert(layer);

      m_explorer->setLayer(layer.get(), m_app->getSelectionColor().name(QColor::HexArgb).toStdString());

      m_explorer->setVisible(true);
    }
    break;

    default:
      return;
  }
}

void te::qt::af::StyleExplorer::styleChanged(te::map::AbstractLayer* l)
{
  te::qt::af::evt::GetLayerExplorer e;
  emit triggered(&e);

  e.m_layerExplorer->updateLegend(l);

  te::qt::af::evt::LayerChanged e2(l);
  emit triggered(&e2);
}
