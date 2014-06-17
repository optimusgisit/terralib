/*  Copyright (C) 2001-2014 National Institute For Space Research (INPE) - Brazil.

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
  \file ToolbarOutside.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "ToolbarOutside.h"
#include "Context.h"
#include "AbstractScene.h"
#include "Scene.h"
#include "OutsideModelObservable.h"
#include "ItemObserver.h"
#include "OutsideObserver.h"
#include "OutsideController.h"
#include "../../../../geometry/Envelope.h"
#include "EnumMode.h"
#include "LayoutConfig.h" /*Layout Images*/

//STL
#include <string>

// Qt
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QMenuBar>
#include <QAction>
#include <QPushButton>

te::layout::ToolbarOutside::ToolbarOutside( OutsideController* controller, Observable* o ) :
	QDockWidget("", 0, 0),
	OutsideObserver(controller, o),
  m_optionMapDefault("map_default"),
  m_optionMapGrid("map_grid"),
  m_optionLegendDefault("legend_default"),
  m_optionRectangle("geometry_rectangle"),
  m_optionViewPan("view_pan"),
  m_optionViewZoomIn("view_zoom_in"),
  m_optionViewZoomOut("view_zoom_out"),
  m_optionScale("scale_object"),
  m_optionMapPan("map_pan"),
  m_optionMapZoomIn("map_zoom_in"),
  m_optionMapZoomOut("map_zoom_out"),
  m_optionGroup("items_group"),
  m_optionUngroup("items_ungroup"),
  m_optionLineIntersectionMouse("items_intersection_mouse"),
  m_optionSceneZoom("scene_zoom"),
  m_toolbar(0),
  m_btnMap(0)
{
	te::gm::Envelope box = m_model->getBox();	

	setVisible(false);
	setWindowTitle("Layout - Toolbar");
  setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
  setMinimumSize(200, 10);

  createToolbar();
  setWidget(m_toolbar);
}

te::layout::ToolbarOutside::~ToolbarOutside()
{

}

void te::layout::ToolbarOutside::updateObserver( ContextItem context )
{
	setVisible(context.getShow());
	if(context.getShow() == true)
		show();
	else
		hide();
}

void te::layout::ToolbarOutside::setPosition( const double& x, const double& y )
{
	move(x,y);
	refresh();
}

te::gm::Coord2D te::layout::ToolbarOutside::getPosition()
{
  QPointF posF = pos();
  qreal valuex = posF.x();
  qreal valuey = posF.y();

  te::gm::Coord2D coordinate;
  coordinate.x = valuex;
  coordinate.y = valuey;

  return coordinate;
}

void te::layout::ToolbarOutside::createToolbar()
{
  m_toolbar = new QToolBar;

  createArrowCursorButton();
  m_toolbar->addSeparator();

  createLineIntersectionToolButton();
  m_toolbar->addSeparator();

  createViewAreaToolButton();
  m_toolbar->addSeparator();

  createMapToolsToolButton();
  m_toolbar->addSeparator();

  createMapToolButton();
  m_toolbar->addSeparator();
  
  createGeometryToolButton();
  m_toolbar->addSeparator();

  createItemTools();
  m_toolbar->addSeparator();

  createBringToFrontToolButton();
  createSendToBackToolButton();
  m_toolbar->addSeparator();

  /*createSceneZoomCombobox();
  m_toolbar->addSeparator();*/
}

void te::layout::ToolbarOutside::createMapToolButton()
{
  QMenu* menu = new QMenu();

  QAction* actionDefaultMenu = createAction("Default Map Object", m_optionMapDefault, "layout-default-map");
  menu->addAction(actionDefaultMenu);
  
  QAction* actionMapGrid = createAction("Map Grid", m_optionMapGrid, "layout-map-grid");
  menu->addAction(actionMapGrid);

  QAction* actionLegend = createAction("Default Legend", m_optionLegendDefault, "layout-default-legend");
  menu->addAction(actionLegend);

  QAction* actionScale = createAction("Scale Object", m_optionScale, "layout-scale");
  menu->addAction(actionScale);
  
  QToolButton *btnMap = createToolButton("Map", "Map Objects", "layout-default-map");
  btnMap->setMenu(menu);
  btnMap->setPopupMode(QToolButton::InstantPopup);

  connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(onMapTriggered(QAction*)));
  m_toolbar->addWidget(btnMap);
}

void te::layout::ToolbarOutside::createMapToolsToolButton()
{
  QMenu* menu = new QMenu();

  QAction* actionPan = createAction("Pan Tool", m_optionMapPan, "layout-map-pan");
  menu->addAction(actionPan);

  QAction* actionZoomIn = createAction("Zoom In", m_optionMapZoomIn, "layout-map-zoom-in");
  menu->addAction(actionZoomIn);

  QAction* actionZoomOut = createAction("Zoom Out", m_optionMapZoomOut, "layout-map-zoom-out");
  menu->addAction(actionZoomOut);
  
  QToolButton *btnMapTools = createToolButton("Map Tools", "Map Tools", "layout-map-pan");
  btnMapTools->setMenu(menu);
  btnMapTools->setPopupMode(QToolButton::InstantPopup);

  connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(onMapToolsTriggered(QAction*)));
  m_toolbar->addWidget(btnMapTools);
}

void te::layout::ToolbarOutside::createGeometryToolButton()
{
  QMenu* menu = new QMenu();

  QAction* actionRectagle = createAction("Rectangle Object", m_optionRectangle, "layout-square");
  menu->addAction(actionRectagle);

  QToolButton *btnGeometry = createToolButton("Geometry Tools", "Geometries Objects", "layout-square");
  btnGeometry->setMenu(menu);
  btnGeometry->setPopupMode(QToolButton::InstantPopup);

  connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(onGeometryTriggered(QAction*)));
  m_toolbar->addWidget(btnGeometry);
}

void te::layout::ToolbarOutside::createViewAreaToolButton()
{
  QMenu* menu = new QMenu();

  QAction* actionPan = createAction("Pan Tool", m_optionViewPan, "layout-paper-pan");
  menu->addAction(actionPan);
  
  QAction* actionZoomIn = createAction("Zoom In", m_optionViewZoomIn, "layout-paper-zoom-in");
  menu->addAction(actionZoomIn);
  
  QAction* actionZoomOut = createAction("Zoom Out", m_optionRectangle, "layout-paper-zoom-out");
  menu->addAction(actionZoomOut);

  QToolButton *btnViewArea = createToolButton("Paper Tools", "Paper Tools", "layout-paper-pan");
  btnViewArea->setMenu(menu);
  btnViewArea->setPopupMode(QToolButton::InstantPopup);

  connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(onViewAreaTriggered(QAction*)));
  m_toolbar->addWidget(btnViewArea);
}

void te::layout::ToolbarOutside::createArrowCursorButton()
{
  QToolButton *btnArrowCursor = createToolButton("Arrow Cursor", "Arrow Cursor", "layout-default-cursor");
  btnArrowCursor->setCheckable(false);
  connect(btnArrowCursor, SIGNAL(clicked(bool)), this, SLOT(onArrowCursorClicked(bool)));

  m_toolbar->addWidget(btnArrowCursor);
}

void te::layout::ToolbarOutside::createItemTools()
{
  QMenu* menu = new QMenu();

  QAction* actionGroup = createAction("Group", m_optionGroup, "layout-group");
  menu->addAction(actionGroup);

  QAction* actionUngroup = createAction("Ungroup", m_optionUngroup, "layout-ungroup");
  menu->addAction(actionUngroup);
  
  QToolButton *btnTools = createToolButton("Item Tools", "Item Tools", "layout-group");
  btnTools->setMenu(menu);
  btnTools->setPopupMode(QToolButton::InstantPopup);

  connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(onItemToolsTriggered(QAction*)));
  m_toolbar->addWidget(btnTools);
}

void te::layout::ToolbarOutside::createLineIntersectionToolButton()
{
  QToolButton *btnLineMouse = createToolButton("Draw Line Intersection Mouse", "Draw Line Intersection Mouse", "layout-draw-line-mouse-intersection");
  connect(btnLineMouse, SIGNAL(toggled(bool)), this, SLOT(onLineIntersectionMouse(bool)));

  m_toolbar->addWidget(btnLineMouse);
}

void te::layout::ToolbarOutside::createSceneZoomCombobox()
{
  m_comboSceneZoom = new QComboBox();
  m_comboSceneZoom->setObjectName(m_optionSceneZoom.c_str());
  //m_comboSceneZoom->setVisible(false);

  m_comboSceneZoom->addItem("50%", 0.5);
  m_comboSceneZoom->addItem("70%", 0.7);
  m_comboSceneZoom->addItem("100%", 1.);
  m_comboSceneZoom->addItem("150%", 1.5);
  m_comboSceneZoom->addItem("200%", 2.);
  m_comboSceneZoom->addItem("300%", 3.);
  m_comboSceneZoom->addItem("400%", 4.); 

  connect(m_comboSceneZoom, SIGNAL(currentIndexChanged(int)), this, SLOT(onSceneZoomCurrentIndexChanged(int)));
  m_comboSceneZoom->setCurrentIndex(1);
  
  m_toolbar->addWidget(m_comboSceneZoom);
}

void te::layout::ToolbarOutside::createBringToFrontToolButton()
{
  QToolButton *btn = createToolButton("Bring to front", "Bring to front", "layout-in-front");
  btn->setCheckable(false);
  connect(btn, SIGNAL(clicked(bool)), this, SLOT(onBringToFrontClicked(bool)));

  m_toolbar->addWidget(btn);
}

void te::layout::ToolbarOutside::createSendToBackToolButton()
{
  QToolButton *btn = createToolButton("Send to back", "Send to back", "layout-to-back");
  btn->setCheckable(false);
  connect(btn, SIGNAL(clicked(bool)), this, SLOT(onSendToBackClicked(bool)));

  m_toolbar->addWidget(btn);
}

void te::layout::ToolbarOutside::onMapTriggered( QAction* action )
{
  if(action->objectName().compare(m_optionMapDefault.c_str()) == 0)
  {
    changeAction(TypeCreateMap);
  }
  if(action->objectName().compare(m_optionMapGrid.c_str()) == 0)
  {
    changeAction(TypeCreateMapGrid);
  }
  if(action->objectName().compare(m_optionLegendDefault.c_str()) == 0)
  {
    changeAction(TypeCreateLegend);
  }
  if(action->objectName().compare(m_optionScale.c_str()) == 0)
  {
    changeAction(TypeCreateScale);
  }
}

void te::layout::ToolbarOutside::onMapToolsTriggered( QAction* action )
{
  if(action->objectName().compare(m_optionMapPan.c_str()) == 0)
  {
    changeAction(TypeMapPan);
  }
  if(action->objectName().compare(m_optionMapZoomIn.c_str()) == 0)
  {
    changeAction(TypeMapZoomIn);
  }
  if(action->objectName().compare(m_optionMapZoomOut.c_str()) == 0)
  {
    changeAction(TypeMapZoomOut);
  }
}

void te::layout::ToolbarOutside::onGeometryTriggered( QAction* action )
{
  if(action->objectName().compare(m_optionRectangle.c_str()) == 0)
  {
    changeAction(TypeCreateRectangle);
  }
}

void te::layout::ToolbarOutside::onViewAreaTriggered( QAction* action )
{
  if(action->objectName().compare(m_optionViewPan.c_str()) == 0)
  {
    changeAction(TypePan);
  }
  if(action->objectName().compare(m_optionViewZoomIn.c_str()) == 0)
  {
    changeAction(TypeZoomIn);
  }
  if(action->objectName().compare(m_optionViewZoomOut.c_str()) == 0)
  {
    changeAction(TypeZoomOut);
  }
}

void te::layout::ToolbarOutside::onArrowCursorClicked(bool checked)
{
  changeAction(TypeArrowCursor);
}

void te::layout::ToolbarOutside::onItemToolsTriggered( QAction* action )
{
  if(action->objectName().compare(m_optionGroup.c_str()) == 0)
  {
    changeAction(TypeGroup);
  }
  if(action->objectName().compare(m_optionUngroup.c_str()) == 0)
  {
    changeAction(TypeUngroup);
  }
}

void te::layout::ToolbarOutside::onLineIntersectionMouse( bool checked )
{
  bool result = false;
  LayoutMode mouseMode = Context::getInstance()->getLineIntersectionMouseMode();

  if(checked)
  {
    if(TypeActiveLinesIntersectionMouse != mouseMode)
    {
      Context::getInstance()->setLineIntersectionMouseMode(TypeActiveLinesIntersectionMouse);
    }
  }
  else
  {
    if(TypeOffLinesIntersectionMouse != mouseMode)
    {
      Context::getInstance()->setLineIntersectionMouseMode(TypeOffLinesIntersectionMouse);
    }
  }

  emit changeContext(result);
}

void te::layout::ToolbarOutside::onSceneZoomCurrentIndexChanged( int index )
{
  QVariant variant = m_comboSceneZoom->itemData(index);
  double zoomFactor = Context::getInstance()->getZoomFactor();
  if(variant.toDouble() != zoomFactor)
  {
    double oldZoom = Context::getInstance()->getZoomFactor();
    Context::getInstance()->setZoomFactor(variant.toDouble());
    Context::getInstance()->setOldZoomFactor(oldZoom);
    changeAction(TypeSceneZoom);
  }
}

void te::layout::ToolbarOutside::onBringToFrontClicked( bool checked )
{
  changeAction(TypeBringToFront);
}

void te::layout::ToolbarOutside::onSendToBackClicked( bool checked )
{
  changeAction(TypeSendToBack);
}

void te::layout::ToolbarOutside::changeAction( LayoutMode mode )
{
  bool result = true;
  LayoutMode layoutMode = Context::getInstance()->getMode();

  if(mode != layoutMode)
  {
    Context::getInstance()->setMode(mode);
  }
  else
  {
    result = false;
  }

  emit changeContext(result);
}

QToolButton* te::layout::ToolbarOutside::createToolButton( std::string text, std::string tooltip, std::string icon )
{
  QToolButton *btn = new QToolButton;
  btn->setText(text.c_str());
  btn->setGeometry(0,0,10,10);
  btn->setCheckable(true);
  btn->setToolTip(tooltip.c_str());
  
  std::string icon_path = LAYOUT_IMAGES_PNG"/" + icon;
  btn->setIcon(QIcon::fromTheme(icon_path.c_str()));

  return btn;
}

QPushButton* te::layout::ToolbarOutside::createPushButton( std::string text, std::string tooltip, std::string icon )
{
  QPushButton *btn = new QPushButton;
  btn->setText(text.c_str());
  btn->setGeometry(0,0,10,10);
  btn->setCheckable(true);
  btn->setToolTip(tooltip.c_str());

  std::string icon_path = LAYOUT_IMAGES_PNG"/" + icon;
  btn->setIcon(QIcon::fromTheme(icon_path.c_str()));

  return btn;
}

QAction* te::layout::ToolbarOutside::createAction( std::string text, std::string objName, std::string icon, std::string tooltip )
{
  QAction *actionMenu = new QAction(text.c_str(), this);
  actionMenu->setObjectName(objName.c_str());

  std::string icon_path = LAYOUT_IMAGES_PNG"/" + icon;
  actionMenu->setIcon(QIcon::fromTheme(icon_path.c_str()));
  actionMenu->setToolTip(tooltip.c_str());

  return actionMenu;
}
