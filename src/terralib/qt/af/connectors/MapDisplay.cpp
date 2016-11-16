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

/*! 
  \file terralib/qt/af/connectors/MapDisplay.cpp

  \brief A connector of the te::qt::widgets::MapDisplay class to the Application Framework.
*/

// TerraLib
#include "../../../dataaccess/dataset/DataSet.h"
#include "../../../dataaccess/dataset/ObjectId.h"
#include "../../../dataaccess/dataset/ObjectIdSet.h"
#include "../../../dataaccess/utils/Utils.h"
#include "../../../geometry/Coord2D.h"
#include "../../../geometry/Envelope.h"
#include "../../../geometry/Geometry.h"
#include "../../../geometry/Utils.h"
#include "../../../maptools/CanvasConfigurer.h"
#include "../../../maptools/Utils.h"
#include "../../../se/Rule.h"
#include "../../../se/Style.h"
#include "../../../se/Symbolizer.h"
#include "../../../srs/Config.h"
#include "../../widgets/canvas/Canvas.h"
#include "../../widgets/canvas/EyeBirdMapDisplayWidget.h"
#include "../../widgets/canvas/MapDisplay.h"
#include "../../widgets/canvas/ZoomInMapDisplayWidget.h"
#include "../../widgets/layer/explorer/LayerItemView.h"
#include "../../widgets/tools/AbstractTool.h"
#include "../../widgets/tools/CoordTracking.h"
#include "../../widgets/tools/PanMiddleClick.h"
#include "../../widgets/tools/ZoomWheel.h"
#include "../../widgets/Utils.h"
#include "../events/ApplicationEvents.h"
#include "../events/LayerEvents.h"
#include "../events/MapEvents.h"
//#include "../events/ProjectEvents.h"
#include "../events/ToolEvents.h"
#include "../ApplicationController.h"
//#include "../Project.h"
#include "../Utils.h"
#include "MapDisplay.h"

// Qt
#include <QAction>
#include <QContextMenuEvent>
#include <QMessageBox>

// STL
#include <cassert>
#include <memory>
#include <utility>

te::qt::af::MapDisplay::MapDisplay(te::qt::widgets::MapDisplay* display, te::qt::af::ApplicationController* app)
  : QObject(),
    m_app(app),
    m_display(display),
    m_menu(0),
    m_currentExtentIndex(-1),
    m_extentStackMaxSize(5),
    m_zoomInDisplay(0),
    m_eyeBirdDisplay(0)
{
  // CoordTracking tool
  te::qt::widgets::CoordTracking* coordTracking = new te::qt::widgets::CoordTracking(m_display, this);
  connect(coordTracking, SIGNAL(coordTracked(QPointF&)), SLOT(onCoordTracked(QPointF&)));
  m_display->installEventFilter(coordTracking);

  // Zoom Wheel tool
  m_display->installEventFilter(new te::qt::widgets::ZoomWheel(m_display, 1.5, false, this));

  // Pan Middle Click tool
  m_display->installEventFilter(new te::qt::widgets::PanMiddleClick(m_display, this));

  // Signals & slots
  connect(m_display, SIGNAL(drawLayersFinished(const QMap<QString, QString>&)), SLOT(onDrawLayersFinished(const QMap<QString, QString>&)));
  connect(m_display, SIGNAL(extentChanged()), SLOT(onExtentChanged()));

  // Gets the popup menu
  m_menu = m_app->findMenu("Map");

  // To show popup menu
  m_display->installEventFilter(this);

  // Config the default SRS
  m_display->setSRID(m_app->getDefaultSRID(), false);
  
  // Getting default display color
  m_display->setBackgroundColor(te::qt::af::GetDefaultDisplayColorFromSettings());
}

te::qt::af::MapDisplay::~MapDisplay()
{
  delete m_display;
}

te::qt::widgets::MapDisplay* te::qt::af::MapDisplay::getDisplay()
{
  return m_display;
}

void te::qt::af::MapDisplay::setZoomInDisplay(te::qt::widgets::ZoomInMapDisplayWidget* display)
{
  m_zoomInDisplay = display;
}

void te::qt::af::MapDisplay::setEyeBirdDisplay(te::qt::widgets::EyeBirdMapDisplayWidget* display)
{
  m_eyeBirdDisplay = display;
}

bool te::qt::af::MapDisplay::eventFilter(QObject* /*watched*/, QEvent* e)
{
  switch(e->type())
  {
    case QEvent::ContextMenu:
    {
      if (m_menu)
      {
        if (m_menu->objectName() == "Map")
          m_menu->exec(static_cast<QContextMenuEvent*>(e)->globalPos());
      }
    }
    break;
          
    default:
    break;
  }

  return false;
}

void te::qt::af::MapDisplay::draw(const std::list<te::map::AbstractLayerPtr>& layers)
{
  if(layers.empty())
  {
    clear();
    return;
  }

  std::list<te::map::AbstractLayerPtr> visibleLayers;
  te::map::GetVisibleLayers(layers, visibleLayers);

  configSRS(visibleLayers);

  if(!m_display->getExtent().isValid())
  {
    te::gm::Envelope displayExtent = te::map::GetExtent(layers, m_display->getSRID(), true);
    m_display->setExtent(displayExtent, false);
  }
  
  m_display->setLayerList(layers);

  m_display->refresh();

  if(m_zoomInDisplay)
    m_zoomInDisplay->setList(visibleLayers, m_display->getSRID());

  if(m_eyeBirdDisplay)
    m_eyeBirdDisplay->setList(visibleLayers, m_display->getSRID());
  
}

void te::qt::af::MapDisplay::clear()
{
  std::list<te::map::AbstractLayerPtr> empty;
  m_display->setLayerList(empty);
  m_display->refresh();
}

void te::qt::af::MapDisplay::nextExtent()
{
  if(m_extentStack.empty())
    return;

  if(m_currentExtentIndex < static_cast<int>(m_extentStack.size() - 1))
  {
    m_currentExtentIndex += 1;
    m_display->setExtent(m_extentStack[m_currentExtentIndex]);
  }

  emit hasNextExtent(m_currentExtentIndex < static_cast<int>(m_extentStack.size() - 1));
  emit hasPreviousExtent(m_currentExtentIndex > 0);
}

void te::qt::af::MapDisplay::previousExtent()
{
  if(m_extentStack.empty())
    return;

  if(m_currentExtentIndex > 0)
  {
    m_currentExtentIndex -= 1;
    m_display->setExtent(m_extentStack[m_currentExtentIndex]);
  }

  emit hasNextExtent(m_currentExtentIndex < static_cast<int>(m_extentStack.size() - 1));
  emit hasPreviousExtent(m_currentExtentIndex > 0);
}

void te::qt::af::MapDisplay::fit(const std::list<te::map::AbstractLayerPtr>& layers)
{
  std::list<te::map::AbstractLayerPtr> visibleLayers;
  te::map::GetVisibleLayers(layers, visibleLayers);

  configSRS(visibleLayers);

  te::gm::Envelope displayExtent = te::map::GetExtent(layers, m_display->getSRID(), true);

  m_display->setExtent(displayExtent, false);

  m_display->setLayerList(layers);

  m_display->refresh(true);

  if(m_zoomInDisplay)
    m_zoomInDisplay->setList(visibleLayers, m_display->getSRID());

  if(m_eyeBirdDisplay)
    m_eyeBirdDisplay->setList(visibleLayers, m_display->getSRID());
}

void te::qt::af::MapDisplay::onCoordTracked(QPointF& coordinate)
{
  te::qt::af::evt::CoordinateTracked e(coordinate.x(), coordinate.y());
  emit triggered(&e);

  if(m_zoomInDisplay)
    m_zoomInDisplay->drawCursorPosition(static_cast<double>(coordinate.x()), static_cast<double>(coordinate.ry()));
}

void te::qt::af::MapDisplay::onDrawLayersFinished(const QMap<QString, QString>& /*errors*/)
{
  // Stores the clean pixmap!
  m_lastDisplayContent = QPixmap(*m_display->getDisplayPixmap());

  // Draw the layers selection
  std::list<te::map::AbstractLayerPtr> ls = getSelectedLayer();

  if(!ls.empty())
    drawLayersSelection(ls);

  // Informs the end of drawing
  te::qt::af::evt::DrawingFinished drawingFinished(this);
  emit triggered(&drawingFinished);
}

void te::qt::af::MapDisplay::onApplicationTriggered(te::qt::af::evt::Event* e)
{
  switch(e->m_id)
  {
    case te::qt::af::evt::PROJECT_ADDED:
      clear();
    break;

    case te::qt::af::evt::LAYER_SELECTED_OBJECTS_CHANGED:
    {
      te::qt::af::evt::LayerSelectedObjectsChanged* evt = static_cast<te::qt::af::evt::LayerSelectedObjectsChanged*> (e);

      QPixmap* content = m_display->getDisplayPixmap();
      content->fill(Qt::transparent);

      QPainter painter(content);
      painter.drawPixmap(0, 0, m_lastDisplayContent);
      painter.end();

      // Fred: revisar
     drawLayersSelection(getSelectedLayer());
    }
    break;

    case te::qt::af::evt::HIGHLIGHT_LAYER_OBJECTS:
    {
      te::qt::af::evt::HighlightLayerObjects* highlightEvent = static_cast<te::qt::af::evt::HighlightLayerObjects*>(e);
      drawDataSet(highlightEvent->m_dataset, highlightEvent->m_layer->getGeomPropertyName(), highlightEvent->m_layer->getSRID(), highlightEvent->m_color);
      m_display->repaint();
    }
    break;

    case te::qt::af::evt::LAYER_CHANGED:
    {
      te::qt::af::evt::LayerChanged* lEvent = static_cast<te::qt::af::evt::LayerChanged*>(e);
      m_display->updateLayer(lEvent->m_layer, false);
    }
    break;

    case te::qt::af::evt::ITEM_OF_LAYER_REMOVED:
//      draw(getSelectedLayer());
    break;

    case te::qt::af::evt::LAYER_REMOVED:
    {
      m_display->setLayerList(getVisibleLayers());
      m_display->refresh();
    }
    break;

    case te::qt::af::evt::MAP_COLOR_CHANGED:
    {
      te::qt::af::evt::MapColorChanged* mapColorChanged = static_cast<te::qt::af::evt::MapColorChanged*>(e);
      m_display->setBackgroundColor(mapColorChanged->m_color);
      m_display->refresh();
    }
    break;

    case te::qt::af::evt::GET_MAPDISPLAY:
    {
      te::qt::af::evt::GetMapDisplay* getDisplay = static_cast<te::qt::af::evt::GetMapDisplay*>(e);
      getDisplay->m_display = this;
    }
    break;

    default:
      return;
  }
}

bool te::qt::af::MapDisplay::isPanToSelectedEnabled()
{
  return m_pantoSelectedAction->isChecked();
}

void te::qt::af::MapDisplay::drawLayersSelection(const std::list<te::map::AbstractLayerPtr>& layers)
{
  if(layers.empty())
    return;

  std::list<te::map::AbstractLayerPtr>::const_iterator it;
  for(it = layers.begin(); it != layers.end(); ++it)
      drawLayerSelection(*it);

  m_display->repaint();
}

void te::qt::af::MapDisplay::drawLayerSelection(te::map::AbstractLayerPtr layer)
{
  assert(layer.get());

  if(layer.get() == 0 || layer->getVisibility() != te::map::VISIBLE)
    return;

  std::auto_ptr<te::da::DataSetType> dsType = layer->getSchema();

  if(!dsType.get() || !dsType->hasGeom())
    return;

  const te::da::ObjectIdSet* oids = layer->getSelected();

  if(oids == 0 || oids->size() == 0)
    return;

  try
  {
    std::size_t maxOids = 4000;

    if(oids->size() > maxOids)
    {
      std::auto_ptr<te::da::ObjectIdSet> oidsBatch(new te::da::ObjectIdSet(*oids, false));

      // Count the all oids
      std::size_t nOids = 0;

      // Count the processed oids
      std::size_t nProcessedOids = 0;

      std::set<te::da::ObjectId*, te::common::LessCmp<te::da::ObjectId*> >::const_iterator it;
      for(it = oids->begin(); it != oids->end(); ++it)
      {
  
        oidsBatch->add((*it)->clone());

        ++nOids;
        ++nProcessedOids;

        if(nProcessedOids == maxOids || nOids == oids->size())
        {
          // Try to retrieve the layer selection batch
          std::auto_ptr<te::da::DataSet> selected(layer->getData(oidsBatch.get()));

          if (layer->getSelectionStyle())
            drawDataSet(selected.get(), layer->getGeomPropertyName(), layer->getSRID(), layer->getSelectionStyle(), te::da::HasLinkedTable(layer->getSchema().get()));
          else
            drawDataSet(selected.get(), layer->getGeomPropertyName(), layer->getSRID(), m_app->getSelectionColor(), te::da::HasLinkedTable(layer->getSchema().get()));

          // Prepares to next batch
          oidsBatch->clear();
          nProcessedOids = 0;
        }
      }
    }
    else
    {
      std::auto_ptr<te::da::DataSet> selected(layer->getData(oids->getExpression()));

      if (layer->getSelectionStyle())
        drawDataSet(selected.get(), layer->getGeomPropertyName(), layer->getSRID(), layer->getSelectionStyle(), te::da::HasLinkedTable(layer->getSchema().get()));
      else
        drawDataSet(selected.get(), layer->getGeomPropertyName(), layer->getSRID(), m_app->getSelectionColor(), te::da::HasLinkedTable(layer->getSchema().get()));
    }
  }
  catch(std::exception& e)
  {
    QMessageBox::critical(m_display, tr("Error"), QString(tr("The layer selection cannot be drawn. Details:") + " %1.").arg(e.what()));
    return;
  }

  return;
}

void te::qt::af::MapDisplay::drawDataSet(te::da::DataSet* dataset, const std::string& geomPropertyName, int srid, const QColor& color, bool isLinked)
{
  assert(dataset);
  assert(color.isValid());

  if(srid == TE_UNKNOWN_SRS && m_display->getSRID() != TE_UNKNOWN_SRS)
    return;

  bool needRemap = false;

  if((srid != TE_UNKNOWN_SRS) && (m_display->getSRID() != TE_UNKNOWN_SRS) && (srid != m_display->getSRID()))
    needRemap = true;

  std::size_t gpos = std::string::npos;
  geomPropertyName.empty() ? gpos = te::da::GetFirstPropertyPos(dataset, te::dt::GEOMETRY_TYPE): gpos = te::da::GetPropertyPos(dataset, geomPropertyName);

  assert(gpos != std::string::npos);

  QPixmap* content = m_display->getDisplayPixmap();

  const te::gm::Envelope& displayExtent = m_display->getExtent();

  te::qt::widgets::Canvas canvas(content);
  canvas.setWindow(displayExtent.m_llx, displayExtent.m_lly, displayExtent.m_urx, displayExtent.m_ury);

  te::gm::GeomType currentGeomType = te::gm::UnknownGeometryType;

  dataset->moveBeforeFirst();

  std::set<std::string> highlightedGeoms;

  while(dataset->moveNext())
  {
    std::auto_ptr<te::gm::Geometry> g(dataset->getGeometry(gpos));
    if(needRemap)
    {
      g->setSRID(srid);
      g->transform(m_display->getSRID());
    }

    if(currentGeomType != g->getGeomTypeId())
    {
      currentGeomType = g->getGeomTypeId();
      te::qt::widgets::Config2DrawLayerSelection(&canvas, color, currentGeomType);
    }

    if(isLinked)
    {
      if(highlightedGeoms.insert(g->asText()).second)
      {
        canvas.draw(g.get());
      }
    }
    else
      canvas.draw(g.get());
  }
}

void te::qt::af::MapDisplay::drawDataSet(te::da::DataSet* dataset, const std::string& geomPropertyName, int srid, te::se::Style* style, bool isLinked)
{
  assert(dataset);
  assert(style);

  if (srid == TE_UNKNOWN_SRS && m_display->getSRID() != TE_UNKNOWN_SRS)
    return;

  bool needRemap = false;

  if ((srid != TE_UNKNOWN_SRS) && (m_display->getSRID() != TE_UNKNOWN_SRS) && (srid != m_display->getSRID()))
    needRemap = true;

  std::size_t gpos = std::string::npos;
  geomPropertyName.empty() ? gpos = te::da::GetFirstPropertyPos(dataset, te::dt::GEOMETRY_TYPE) : gpos = te::da::GetPropertyPos(dataset, geomPropertyName);

  assert(gpos != std::string::npos);

  QPixmap* content = m_display->getDisplayPixmap();

  const te::gm::Envelope& displayExtent = m_display->getExtent();

  te::qt::widgets::Canvas canvas(content);
  canvas.setWindow(displayExtent.m_llx, displayExtent.m_lly, displayExtent.m_urx, displayExtent.m_ury);

  //configure canvas style
  te::map::CanvasConfigurer cc(&canvas);


  if (style->getRules().empty())
    return;

  te::se::Rule* rule = style->getRule(0);

  const std::vector<te::se::Symbolizer*>& symbolizers = rule->getSymbolizers();

  if (symbolizers.empty())
    return;

  te::se::Symbolizer* symb = symbolizers[0];

  cc.config(symb);


  dataset->moveBeforeFirst();

  std::set<std::string> highlightedGeoms;

  while (dataset->moveNext())
  {
    std::auto_ptr<te::gm::Geometry> g(dataset->getGeometry(gpos));
    if (needRemap)
    {
      g->setSRID(srid);
      g->transform(m_display->getSRID());
    }

    if (isLinked)
    {
      if (highlightedGeoms.insert(g->asText()).second)
      {
        canvas.draw(g.get());
      }
    }
    else
      canvas.draw(g.get());
  }
}

void te::qt::af::MapDisplay::onExtentChanged()
{
  if(!m_extentStack.empty() && m_display->getExtent().equals(m_extentStack[m_currentExtentIndex]))
    return;

  if(m_currentExtentIndex != m_extentStackMaxSize)
  {
    m_extentStack.push_back(m_display->getExtent());
    m_currentExtentIndex += 1;
  }
  else
  {
    m_extentStack.erase(m_extentStack.begin());
    m_extentStack.push_back(m_display->getExtent());
    m_currentExtentIndex = m_extentStackMaxSize;
  }

  emit hasNextExtent(m_currentExtentIndex < static_cast<int>(m_extentStack.size() - 1));
  emit hasPreviousExtent(m_currentExtentIndex > 0);
}

void te::qt::af::MapDisplay::configSRS(const std::list<te::map::AbstractLayerPtr>& layers)
{
  if(layers.size() == 1 && (*layers.begin())->getSRID() ==  TE_UNKNOWN_SRS && m_display->getSRID() != TE_UNKNOWN_SRS)
  {
    const te::map::AbstractLayerPtr& layer = *layers.begin();

    m_display->setSRID(TE_UNKNOWN_SRS, false);

    std::pair<int, std::string> srid(layer->getSRID(), "EPSG");
    te::qt::af::evt::MapSRIDChanged mapSRIDChanged(srid);
    emit triggered(&mapSRIDChanged);
  }
  else if(m_display->getSRID() == TE_UNKNOWN_SRS)
  {
    std::list<te::map::AbstractLayerPtr>::const_iterator it;

    for(it = layers.begin(); it != layers.end(); ++it)
    {
      const te::map::AbstractLayerPtr& layer = *it;

      if(layer->getSRID() == TE_UNKNOWN_SRS)
        continue;

      m_display->setSRID(layer->getSRID(), false);

      std::pair<int, std::string> srid(layer->getSRID(), "EPSG");

      te::qt::af::evt::MapSRIDChanged mapSRIDChanged(srid);
      emit triggered(&mapSRIDChanged);

      break;
    }
  }
}

std::list<te::map::AbstractLayerPtr> te::qt::af::MapDisplay::getSelectedLayer()
{
  te::qt::af::evt::GetLayerSelected evt;
  emit triggered(&evt);

  std::list<te::map::AbstractLayerPtr> lst;

  if(evt.m_layer != 0)
    lst.push_back(evt.m_layer);

  return lst;
}

std::list<te::map::AbstractLayerPtr> te::qt::af::MapDisplay::getVisibleLayers()
{
  std::list<te::map::AbstractLayerPtr> layers;

  te::qt::af::evt::GetLayerExplorer evt;
  emit triggered(&evt);

  if(evt.m_layerExplorer == 0)
    return layers;

  te::qt::widgets::LayerItemView* view = evt.m_layerExplorer;

  te::qt::widgets::GetValidLayers(view->model(), QModelIndex(), layers);

  return layers;
}
