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
  \file terralib/qt/plugins/mnt/Plugin.cpp

  \brief Plugin implementation for the MNT Qt Plugin widget.
*/

// TerraLib
#include "../../../BuildConfig.h"
#include "../../../common/Config.h"
#include "../../../core/translator/Translator.h"
#include "../../../core/logger/Logger.h"
#include "../../af/ApplicationController.h"
#include "../../af/Utils.h"
#include "CreateIsolinesAction.h"
#include "ImageGenerationAction.h"
#include "MNTGenerationAction.h"
#include "ShowValuesAction.h"
#include "SlopeAction.h"
#include "SmoothAction.h"
#include "TINGenerationAction.h"
#include "VolumeAction.h"
#include "ProfileAction.h"

#include "Plugin.h"

// QT
#include <QMenu>
#include <QMenuBar>
#include <QString>

// STL
#include <string>


te::qt::plugins::mnt::Plugin::Plugin(const te::core::PluginInfo& pluginInfo)
: te::core::CppPlugin(pluginInfo),
m_mntMenu(0),
m_ISOGeneration(0),
m_MNTGeneration(0),
m_ImageGeneration(0),
m_ShowValue(0),
m_Slope(0),
m_Smooth(0),
m_TINGeneration(0),
m_Volume(0),
m_Profile(0)
{
}

te::qt::plugins::mnt::Plugin::~Plugin() 
{
}

void te::qt::plugins::mnt::Plugin::startup()
{
  if(m_initialized)
    return;

// it initializes the Translator support for the TerraLib MNT Qt Plugin
   TE_LOG_TRACE(TE_TR("TerraLib Qt DTM Plugin startup!"));

// add plugin menu
  QMenu* pluginMenu = te::qt::af::AppCtrlSingleton::getInstance().getMenu("Processing");
  m_mntMenu = new QMenu(pluginMenu);
  m_mntMenu->setIcon(QIcon::fromTheme("mnt-processing-icon"));

  // Insert action before plugin manager action
  QAction* pluginsSeparator = te::qt::af::AppCtrlSingleton::getInstance().findAction("ManagePluginsSeparator");

  pluginMenu->insertMenu(pluginsSeparator, m_mntMenu);

  m_mntMenu->setTitle(TE_TR("DTM Processing"));

// register actions
  registerActions();

  m_initialized = true;

  te::qt::af::AppCtrlSingleton::getInstance().addListener(this, te::qt::af::SENDER);
}

void te::qt::plugins::mnt::Plugin::shutdown()
{
  if(!m_initialized)
    return;

// remove menu
  delete m_mntMenu;

// unregister actions
  unRegisterActions();

  TE_LOG_TRACE(TE_TR("TerraLib Qt DTM Plugin shutdown!"));

  m_initialized = false;

  te::qt::af::AppCtrlSingleton::getInstance().removeListener(this);
}

void te::qt::plugins::mnt::Plugin::registerActions()
{
  m_TINGeneration = new te::qt::plugins::mnt::TINGenerationAction(m_mntMenu);
  connect(m_TINGeneration, SIGNAL(triggered(te::qt::af::evt::Event*)), SIGNAL(triggered(te::qt::af::evt::Event*)));
  te::qt::af::AddActionToCustomToolbars(&te::qt::af::AppCtrlSingleton::getInstance(), m_TINGeneration->getAction());

  m_MNTGeneration = new te::qt::plugins::mnt::MNTGenerationAction(m_mntMenu);
  connect(m_MNTGeneration, SIGNAL(triggered(te::qt::af::evt::Event*)), SIGNAL(triggered(te::qt::af::evt::Event*)));
  te::qt::af::AddActionToCustomToolbars(&te::qt::af::AppCtrlSingleton::getInstance(), m_MNTGeneration->getAction());

  m_ISOGeneration = new te::qt::plugins::mnt::CreateIsolinesAction(m_mntMenu);
  connect(m_ISOGeneration, SIGNAL(triggered(te::qt::af::evt::Event*)), SIGNAL(triggered(te::qt::af::evt::Event*)));
  te::qt::af::AddActionToCustomToolbars(&te::qt::af::AppCtrlSingleton::getInstance(), m_ISOGeneration->getAction());

  m_Smooth = new te::qt::plugins::mnt::SmoothAction(m_mntMenu);
  connect(m_Smooth, SIGNAL(triggered(te::qt::af::evt::Event*)), SIGNAL(triggered(te::qt::af::evt::Event*)));
  te::qt::af::AddActionToCustomToolbars(&te::qt::af::AppCtrlSingleton::getInstance(), m_Smooth->getAction());

  m_Slope = new te::qt::plugins::mnt::SlopeAction(m_mntMenu);
  connect(m_Slope, SIGNAL(triggered(te::qt::af::evt::Event*)), SIGNAL(triggered(te::qt::af::evt::Event*)));
  te::qt::af::AddActionToCustomToolbars(&te::qt::af::AppCtrlSingleton::getInstance(), m_Slope->getAction());

  m_ImageGeneration = new te::qt::plugins::mnt::ImageGenerationAction(m_mntMenu);
  connect(m_ImageGeneration, SIGNAL(triggered(te::qt::af::evt::Event*)), SIGNAL(triggered(te::qt::af::evt::Event*)));
  te::qt::af::AddActionToCustomToolbars(&te::qt::af::AppCtrlSingleton::getInstance(), m_ImageGeneration->getAction());

  m_Volume = new te::qt::plugins::mnt::VolumeAction(m_mntMenu);
  connect(m_Volume, SIGNAL(triggered(te::qt::af::evt::Event*)), SIGNAL(triggered(te::qt::af::evt::Event*)));
  te::qt::af::AddActionToCustomToolbars(&te::qt::af::AppCtrlSingleton::getInstance(), m_Volume->getAction());

  m_Profile = new te::qt::plugins::mnt::ProfileAction(m_mntMenu);
  connect(m_Profile, SIGNAL(triggered(te::qt::af::evt::Event*)), SIGNAL(triggered(te::qt::af::evt::Event*)));
  te::qt::af::AddActionToCustomToolbars(&te::qt::af::AppCtrlSingleton::getInstance(), m_Profile->getAction());

  m_ShowValue = new te::qt::plugins::mnt::ShowValuesAction(m_mntMenu);
  connect(m_ShowValue, SIGNAL(triggered(te::qt::af::evt::Event*)), SIGNAL(triggered(te::qt::af::evt::Event*)));
  te::qt::af::AddActionToCustomToolbars(&te::qt::af::AppCtrlSingleton::getInstance(), m_ShowValue->getAction());
}

void  te::qt::plugins::mnt::Plugin::unRegisterActions()
{
  delete m_TINGeneration;
  delete m_MNTGeneration;
  delete m_ISOGeneration;
  delete m_Slope;
  delete m_Smooth;
  delete m_ShowValue;
  delete m_Volume;
  delete m_Profile;
}

TERRALIB_PLUGIN_CALL_BACK_IMPL(te::qt::plugins::mnt::Plugin)

