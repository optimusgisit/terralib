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
  \file OutsideArea.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "OutsideArea.h"
#include "PropertiesDock.h"
#include "ToolbarDock.h"
#include "ObjectInspectorDock.h"
#include "EditTemplateDock.h"
#include "../../../layout/core/pattern/singleton/Context.h"
#include "../../../layout/qt/outside/PropertiesOutside.h"
#include "../../../layout/qt/outside/ObjectInspectorOutside.h"
#include "../../../layout/qt/outside/ToolbarOutside.h"

// Qt
#include <QMainWindow>
#include <QMenu>
#include <QAction>

te::qt::plugins::layout::OutsideArea::OutsideArea( te::layout::View* view, QWidget* dockParent, QMenu* mnuLayout) :
  m_view(view),
  m_dockParent(dockParent),
  m_parentMenu(mnuLayout),
  m_mainMenu(0),
  m_dockInspector(0),
  m_dockProperties(0),
  m_dockToolbar(0),
  m_dockEditTemplate(0),
  m_optionNew("mnu_main_new"),
  m_optionUpdate("mnu_main_update"),
  m_optionImport("mnu_main_import"),
  m_optionExport("mnu_main_export"),
  m_optionPageConfig("mnu_main_page_config"),
  m_optionPrint("mnu_main_print"),
  m_optionExit("mnu_main_exit"),
  m_optionDockInspector("mnu_dock_inspector"),
  m_optionDockProperties("mnu_dock_properties"),
  m_optionDockToolbar("mnu_dock_toolbar"),
  m_optionDockEditTemplate("mnu_dock_edit_template")
{
  init();
}

te::qt::plugins::layout::OutsideArea::~OutsideArea()
{
  QMainWindow* win = (QMainWindow*)m_dockParent;

  if(!win)
    return;

  if(m_dockProperties)
  {
    win->removeDockWidget(m_dockProperties);
    m_dockProperties->close();
    m_dockProperties->setParent(0);
    delete m_dockProperties;
    m_dockProperties = 0;
  }

  if(m_dockInspector)
  {
    win->removeDockWidget(m_dockInspector);
    m_dockInspector->close();
    m_dockInspector->setParent(0);
    delete m_dockInspector;
    m_dockInspector = 0;
  }

  if(m_dockToolbar)
  {
    win->removeDockWidget(m_dockToolbar);
    m_dockToolbar->close();
    m_dockToolbar->setParent(0);
    delete m_dockToolbar;
    m_dockToolbar = 0;
  }

  if(m_dockEditTemplate)
  {
    win->removeDockWidget(m_dockEditTemplate);
    m_dockEditTemplate->close();
    m_dockEditTemplate->setParent(0);
    delete m_dockEditTemplate;
    m_dockEditTemplate = 0;
  }
}

void te::qt::plugins::layout::OutsideArea::init()
{
  if(m_view)
  {
    connect(m_view->scene(), SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(m_view->scene(), SIGNAL(addItemFinalized()), this, SLOT(onAddItemFinalized()));
    connect(m_view, SIGNAL(hideView()), this, SLOT(onHideView()));
    connect(m_view, SIGNAL(closeView()), this, SLOT(onCloseView()));
    connect(m_view, SIGNAL(showView()), this, SLOT(onShowView()));
    connect(this, SIGNAL(changeMenuContext(bool)), m_view, SLOT(onMainMenuChangeContext(bool)));
  }

  createPropertiesDock();

  createInspectorDock();
  
  createToolbarDock();

  createMainMenu();

  createEditTemplateDock();

  if(m_dockToolbar)
    connect(m_dockToolbar->getToolbarOutside(), SIGNAL(changeContext(bool)), m_view, SLOT(onToolbarChangeContext(bool)));
}

void te::qt::plugins::layout::OutsideArea::createPropertiesDock()
{
  m_dockProperties = new PropertiesDock;  
  m_dockProperties->setFeatures(QDockWidget::DockWidgetMovable |	
    QDockWidget::DockWidgetFloatable);
}

void te::qt::plugins::layout::OutsideArea::createInspectorDock()
{
  m_dockInspector = new ObjectInspectorDock;
  m_dockInspector->setFeatures(QDockWidget::DockWidgetMovable |	
    QDockWidget::DockWidgetFloatable);
}

void te::qt::plugins::layout::OutsideArea::createToolbarDock()
{
  m_dockToolbar = new ToolbarDock;
  m_dockToolbar->setFeatures(QDockWidget::DockWidgetMovable |	
    QDockWidget::DockWidgetFloatable);
}

void te::qt::plugins::layout::OutsideArea::createEditTemplateDock()
{
  m_dockEditTemplate = new EditTemplateDock;
  m_dockEditTemplate->setFeatures(QDockWidget::NoDockWidgetFeatures);
  m_dockEditTemplate->setVisible(false);
}

void te::qt::plugins::layout::OutsideArea::createMainMenu()
{
  m_mainMenu = new QMenu("Print Model");
  connect(m_mainMenu, SIGNAL(triggered(QAction*)), this, SLOT(onMainMenuTriggered(QAction*)));

  QAction* actionNew = createAction("New", m_optionNew, "layout-new");
  m_mainMenu->addAction(actionNew);

  QAction* actionSave = createAction("Update Template", m_optionUpdate, "layout-save");
  m_mainMenu->addAction(actionSave);

  m_mainMenu->addSeparator();

  QAction* actionImport = createAction("Import Template", m_optionImport, "layout-import");
  m_mainMenu->addAction(actionImport);

  QAction* actionExport = createAction("Export Template", m_optionExport, "layout-export");
  m_mainMenu->addAction(actionExport);

  m_mainMenu->addSeparator();
  
  QAction* actionDockInspector = createAction("Dock Inspector", m_optionDockInspector, "");
  actionDockInspector->setCheckable(true);
  actionDockInspector->setChecked(true);
  m_mainMenu->addAction(actionDockInspector);

  QAction* actionDockProperties = createAction("Dock Properties", m_optionDockProperties, "");
  actionDockProperties->setCheckable(true);
  actionDockProperties->setChecked(true);
  m_mainMenu->addAction(actionDockProperties);

  QAction* actionDockToolbar = createAction("Dock Toolbar", m_optionDockToolbar, "");
  actionDockToolbar->setCheckable(true);
  actionDockToolbar->setChecked(true);
  m_mainMenu->addAction(actionDockToolbar);

  QAction* actionDockEditTemplate = createAction("Dock Edit Template", m_optionDockEditTemplate, "");
  actionDockEditTemplate->setCheckable(true);
  actionDockEditTemplate->setChecked(false);
  m_mainMenu->addAction(actionDockEditTemplate);

  m_mainMenu->addSeparator();

  QAction* actionPageConfig = createAction("Page Config...", m_optionPageConfig, "layout-page-setup");
  m_mainMenu->addAction(actionPageConfig);

  QAction* actionPrint = createAction("Print...", m_optionPrint, "layout-printer");
  m_mainMenu->addAction(actionPrint);

  m_mainMenu->addSeparator();

  QAction* actionExit = createAction("Exit", m_optionExit, "layout-close");
  m_mainMenu->addAction(actionExit);
}

void te::qt::plugins::layout::OutsideArea::onMainMenuTriggered( QAction* action )
{
  if(action->objectName().compare(m_optionNew.c_str()) == 0)
  {
    changeAction(te::layout::TypeNewTemplate);
  }
  else if(action->objectName().compare(m_optionUpdate.c_str()) == 0)
  {
    //changeAction(TypeSaveCurrentTemplate);
  }
  else if(action->objectName().compare(m_optionImport.c_str()) == 0)
  {
    changeAction(te::layout::TypeImportJSONProps);
  }
  else if(action->objectName().compare(m_optionExport.c_str()) == 0)
  {
    changeAction(te::layout::TypeExportPropsJSON);
  }
  else if(action->objectName().compare(m_optionPageConfig.c_str()) == 0)
  {
    changeAction(te::layout::TypePageConfig);
  }
  else if(action->objectName().compare(m_optionPrint.c_str()) == 0)
  {
    changeAction(te::layout::TypePrinter);
  }
  else if(action->objectName().compare(m_optionExit.c_str()) == 0)
  {
    changeAction(te::layout::TypeExit);
  }
  else if(action->objectName().compare(m_optionDockInspector.c_str()) == 0)
  {
    if(m_dockInspector->isVisible())
    {
      m_dockInspector->setVisible(false);
    }
    else
    {
      m_dockInspector->setVisible(true);
    }
  }
  else if(action->objectName().compare(m_optionDockProperties.c_str()) == 0)
  {
    if(m_dockProperties->isVisible())
    {
      m_dockProperties->setVisible(false);
    }
    else
    {
      m_dockProperties->setVisible(true);
    }
  }
  else if(action->objectName().compare(m_optionDockToolbar.c_str()) == 0)
  {
    if(m_dockToolbar->isVisible())
    {
      m_dockToolbar->setVisible(false);
    }
    else
    {
      m_dockToolbar->setVisible(true);
    }
  }
  else if(action->objectName().compare(m_optionDockEditTemplate.c_str()) == 0)
  {
    if(m_dockEditTemplate->isVisible())
    {
      m_dockEditTemplate->setVisible(false);
    }
    else
    {
      m_dockEditTemplate->setVisible(true);
    }
  }
}

QAction* te::qt::plugins::layout::OutsideArea::createAction( std::string text, std::string objName, std::string icon, std::string tooltip )
{
  QAction *actionMenu = new QAction(text.c_str(), this);
  actionMenu->setObjectName(objName.c_str());

  actionMenu->setIcon(QIcon::fromTheme(icon.c_str()));
  actionMenu->setToolTip(tooltip.c_str());

  return actionMenu;
}

void te::qt::plugins::layout::OutsideArea::changeAction( te::layout::LayoutMode mode )
{
  bool result = true;
  te::layout::LayoutMode layoutMode = te::layout::Context::getInstance().getMode();

  if(mode != layoutMode)
  {
    te::layout::Context::getInstance().setMode(mode);
  }
  else
  {
    result = false;
  }

  emit changeMenuContext(result);
}

te::qt::plugins::layout::PropertiesDock* te::qt::plugins::layout::OutsideArea::getPropertiesDock()
{
  return m_dockProperties;
}

te::qt::plugins::layout::ObjectInspectorDock* te::qt::plugins::layout::OutsideArea::getObjectInspectorDock()
{
  return m_dockInspector;
}

te::qt::plugins::layout::ToolbarDock* te::qt::plugins::layout::OutsideArea::getToolbarDock()
{
  return m_dockToolbar;
}

void te::qt::plugins::layout::OutsideArea::openAllDocks()
{
  QMainWindow* win = (QMainWindow*)m_dockParent;

  if(!win)
    return;

  if(m_dockProperties)
  {
    m_dockProperties->setParent(m_dockParent);
    win->addDockWidget(Qt::LeftDockWidgetArea, m_dockProperties);
    m_dockProperties->setVisible(true);
  }
  if(m_dockInspector)
  {
    m_dockInspector->setParent(m_dockParent);
    win->addDockWidget(Qt::LeftDockWidgetArea, m_dockInspector);
    m_dockInspector->setVisible(true);
  }
  if(m_dockToolbar)
  {
    m_dockToolbar->setParent(m_dockParent); 
    win->addDockWidget(Qt::TopDockWidgetArea, m_dockToolbar);
    m_dockToolbar->setVisible(true);
  }
  if(m_dockEditTemplate)
  {
    m_dockEditTemplate->setParent(m_dockParent); 
    win->addDockWidget(Qt::RightDockWidgetArea, m_dockEditTemplate);
    bool visible = m_dockEditTemplate->isVisible();
    m_dockEditTemplate->setVisible(visible);
  }
}

void te::qt::plugins::layout::OutsideArea::closeAllDocks()
{
  QMainWindow* win = (QMainWindow*)m_dockParent;

  if(!win)
    return;

  if(m_dockProperties)
  {
    win->removeDockWidget(m_dockProperties);
    m_dockProperties->close();
  }
  if(m_dockInspector)
  {
    win->removeDockWidget(m_dockInspector);
    m_dockInspector->close();
  }
  if(m_dockToolbar)
  {
    win->removeDockWidget(m_dockToolbar);
    m_dockToolbar->close();
  }
  if(m_dockEditTemplate)
  {
    win->removeDockWidget(m_dockEditTemplate);
    m_dockEditTemplate->close();
  }
}

void te::qt::plugins::layout::OutsideArea::openMainMenu()
{
  if(!m_parentMenu)
    return;

  if(!m_mainMenu)
    return;

  bool exist_menu = false;
  QAction* action = m_mainMenu->menuAction();
  QList<QAction*> acts = m_parentMenu->actions();
  if(action)
  {
    foreach(QAction* act, acts)
    {
      if(act == action)
      {
        exist_menu = true;
      }
    }
  }

  if(!exist_menu)
  {
    m_parentMenu->addSeparator();
    m_parentMenu->addMenu(m_mainMenu);
  }

  if(!acts.empty())
  {
    //m_parentMenu: The first QAction of the QMenu is to open the layout
    QAction* firstOption = acts.first();
    firstOption->setVisible(false);
  }
}

void te::qt::plugins::layout::OutsideArea::closeMainMenu()
{
  if(!m_parentMenu)
    return;

  if(!m_mainMenu)
    return;

  QAction* action = m_mainMenu->menuAction();
  QList<QAction*> acts = m_parentMenu->actions();
  if(action)
  {
    foreach(QAction* act, acts)
    {
      if(act == action)
      {
        m_parentMenu->removeAction(action);
      }
    }
  }

  if(!acts.empty())
  {
    //m_parentMenu: The first QAction of the QMenu is to open the layout
    QAction* firstOption = acts.first();
    firstOption->setVisible(true);
  }

  m_view->closeOutsideWindows();
}

void te::qt::plugins::layout::OutsideArea::onSelectionChanged()
{
  QList<QGraphicsItem*> graphicsItems = m_view->scene()->selectedItems();
  QList<QGraphicsItem*> allItems = m_view->scene()->items();
  //Refresh Property window   
  if(m_dockProperties)
    m_dockProperties->getPropertiesOutside()->itemsSelected(graphicsItems, allItems);
}

void te::qt::plugins::layout::OutsideArea::onAddItemFinalized()
{
  QList<QGraphicsItem*> allItems = m_view->scene()->items();
  //Refresh Inspector Object window
  if(m_dockInspector)
    m_dockInspector->getObjectInspectorOutside()->itemsInspector(allItems);
}

void te::qt::plugins::layout::OutsideArea::onShowView()
{
  openAllDocks();
  openMainMenu();
}

void te::qt::plugins::layout::OutsideArea::onHideView()
{
  closeAllDocks();
  closeMainMenu();
  m_view->closeOutsideWindows();
}

void te::qt::plugins::layout::OutsideArea::onCloseView()
{
  closeAllDocks();
  closeMainMenu();
  m_view->closeOutsideWindows();
}