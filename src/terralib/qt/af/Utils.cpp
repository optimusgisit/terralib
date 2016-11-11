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
  \file terralib/qt/af/Utils.cpp

  \brief Utility routines for the TerraLib Application Framework module.
*/

// Boost
#include <boost/foreach.hpp> // Boost => don't change this include order, otherwise you may have compiling problems! 

// TerraLib
#include "../../common/BoostUtils.h"
#include "../../common/SystemApplicationSettings.h"
#include "../../common/UserApplicationSettings.h"
#include "../../core/utils/Platform.h"
#include "../../dataaccess/datasource/DataSourceInfo.h"
#include "../../dataaccess/datasource/DataSourceInfoManager.h"
#include "../../dataaccess/serialization/xml/Serializer.h"
#include "../../maptools/AbstractLayer.h"
#include "../../core/plugin/PluginManager.h"
#include "../../maptools/serialization/xml/Layer.h"
#include "../../xml/AbstractWriter.h"
#include "../../xml/AbstractWriterFactory.h"
#include "../../xml/Reader.h"
#include "../../xml/ReaderFactory.h"
#include "../../Version.h"
#include "ApplicationController.h"
#include "Exception.h"
//#include "Project.h"
#include "Utils.h"
#include "XMLFormatter.h"

// STL
#include <cassert>
#include <fstream>
#include <memory>

// Boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

// Qt
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QString>
#include <QTextStream>
#include <QApplication>
#include <QAction>
#include <QMainWindow>
#include <QMessageBox>
#include <QToolBar>

void te::qt::af::UpdateUserSettings()
{
  QSettings user_settings(QSettings::IniFormat,
                          QSettings::UserScope,
                          QApplication::instance()->organizationName(),
                          QApplication::instance()->applicationName());
  
// save enabled plugins
  user_settings.remove("plugins/enabled");
  
  user_settings.beginGroup("plugins");
  user_settings.remove("loaded");
  user_settings.remove("unloaded");

  user_settings.beginWriteArray("loaded");

  int aidx = 0;
  std::vector<te::core::PluginInfo> loaded =
      te::core::PluginManager::instance().getLoadedPlugins();

  for(std::size_t i = 0; i != loaded.size(); ++i)
  {
    user_settings.setArrayIndex(aidx++);

    user_settings.setValue("name", loaded[i].name.c_str());
  }

  user_settings.endArray();

  // save unloaded plugins

  std::vector<te::core::PluginInfo> unloaded =
      te::core::PluginManager::instance().getUnloadedPlugins();

  user_settings.beginWriteArray("unloaded");

  int unloadedidx = 0;

  for(std::size_t i = 0; i != unloaded.size(); ++i)
  {
    user_settings.setArrayIndex(unloadedidx++);

    user_settings.setValue("name", unloaded[i].name.c_str());
  }

  std::vector<te::core::PluginInfo> broken =
      te::core::PluginManager::instance().getBrokenPlugins();

  for(std::size_t i = 0; i != broken.size(); ++i)
  {
    user_settings.setArrayIndex(unloadedidx++);

    user_settings.setValue("name", broken[i].name.c_str());
  }

  user_settings.endArray();
  
  user_settings.endGroup();
}

void te::qt::af::SaveDataSourcesFile(te::qt::af::ApplicationController* appController)
{
  QSettings usettings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
  
  QString fileName = usettings.value("data_sources/data_file").toString();

  if(fileName.isNull())
  {
    const QString& udir = appController->getUserDataDir();
    
    fileName = udir + "/" + QString(TERRALIB_APPLICATION_DATASOURCE_FILE_NAME);
    
    usettings.setValue("data_sources/data_file", QVariant(fileName));
  }

  QFileInfo info(fileName);

  QDir d = info.absoluteDir();

  if(!d.exists())
    d.mkpath(d.path());

  te::qt::af::XMLFormatter::formatDataSourceInfos(true);

  te::serialize::xml::Save(fileName.toUtf8().data());

  te::qt::af::XMLFormatter::formatDataSourceInfos(false);
}


void AddToolbarAndActions(QToolBar* bar, QSettings& sett)
{
  sett.beginGroup(bar->objectName());

  sett.setValue("name", bar->objectName());

  sett.beginWriteArray("Actions");

  QList<QAction*> acts = bar->actions();

  for(int i=0; i<acts.size(); i++)
  {
    sett.setArrayIndex(i);
    sett.setValue("action", acts.at(i)->objectName());
  }

  sett.endArray();

  sett.endGroup();
}

void te::qt::af::UpdateToolBarsInTheSettings(te::qt::af::ApplicationController* appController)
{
  std::vector<QToolBar*> bars = appController->getToolBars();
  std::vector<QToolBar*>::const_iterator it;
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

  sett.beginGroup("toolbars");

  for (it = bars.begin(); it != bars.end(); ++it)
  {
    QToolBar* bar = *it;

    sett.remove(bar->objectName());

    AddToolbarAndActions(bar, sett);
  }

  sett.endGroup();
}

void te::qt::af::AddToolBarToSettings(QToolBar* bar)
{
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

  sett.beginGroup("toolbars");

  AddToolbarAndActions(bar, sett);

  sett.endGroup();
}

void te::qt::af::RemoveToolBarFromSettings(QToolBar* bar)
{
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

  sett.beginGroup("toolbars");
  sett.remove(bar->objectName());
  sett.endGroup();
}

std::vector<QToolBar*> te::qt::af::ReadToolBarsFromSettings(te::qt::af::ApplicationController* appController, QWidget* barsParent)
{
  std::vector<QToolBar*> bars;

  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

  sett.beginGroup("toolbars");
  QStringList lst = sett.childGroups();

  QStringList::iterator it;

  for(it=lst.begin(); it != lst.end(); ++it)
  {
    QString gr = *it;

    sett.beginGroup(gr);

    QString grName = sett.value("name").toString();

    int size = sett.beginReadArray("Actions");

    QToolBar* toolbar = new QToolBar(barsParent);
    toolbar->setObjectName(grName);
    toolbar->setWindowTitle(grName);

    for(int i=0; i<size; i++)
    {
      sett.setArrayIndex(i);
      QString act = sett.value("action").toString();

      if(act == "")
      {
        toolbar->addSeparator();
      }
      else
      {
        QAction* a = appController->findAction(act);
      
        if(a != 0)
          toolbar->addAction(a);
      }
    }

    sett.endArray();
    sett.endGroup();

    bars.push_back(toolbar);
  }

  sett.endGroup();

  return bars;
}

void te::qt::af::SaveState(QMainWindow* mainWindow)
{
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
  
  sett.beginGroup("mainWindow");
  sett.setValue("geometry", mainWindow->saveGeometry());
  sett.setValue("windowState", mainWindow->saveState());
  sett.endGroup();
}

void te::qt::af::RestoreState(QMainWindow* mainWindow)
{
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

  sett.beginGroup("mainWindow");
  mainWindow->restoreGeometry(sett.value("geometry").toByteArray());
  mainWindow->restoreState(sett.value("windowState").toByteArray());
  sett.endGroup();
}

//void te::qt::af::GetProjectInformationsFromSettings(QString& defaultAuthor, int& maxSaved)
//{
//  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
//
//  sett.beginGroup("projects");
//  defaultAuthor = sett.value("author_name").toString();
//  maxSaved = sett.value("recents_history_size").toInt();
//  sett.endGroup();
//}

//void te::qt::af::SaveProjectInformationsOnSettings(const QString& defaultAuthor, const int& maxSaved)
//{
//  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
//
//  sett.beginGroup("projects");
//  sett.setValue("author_name", defaultAuthor);
//  sett.setValue("recents_history_size", maxSaved);
//  sett.endGroup();
//}

void te::qt::af::SaveLastDatasourceOnSettings(const QString& dsType)
{
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

  sett.setValue("projects/last datasource used", dsType);
}

//void te::qt::af::SaveOpenLastProjectOnSettings(bool openLast)
//{
//  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
//
//  sett.setValue("projects/openLastDataSource", openLast);
//}

QString te::qt::af::GetLastDatasourceFromSettings()
{
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

  return sett.value("projects/last datasource used").toString();
}

//bool te::qt::af::GetOpenLastProjectFromSettings()
//{
//  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
//
//  QVariant variant = sett.value("projects/openLastDataSource");
//
//  // If the option was never edited
//  if(variant.isNull() || !variant.isValid())
//    return true;
//
//  return variant.toBool();
//}

QString te::qt::af::UnsavedStar(const QString windowTitle, bool isUnsaved)
{
  QString result(windowTitle);

  if(isUnsaved)
  {
    if(result.at(result.count()-1) != '*')
      result += "*";
  }
  else
  {
    if(result.at(result.count()-1) == '*')
      result.remove((result.count()-1), 1);
  }

  return result;
}

QColor te::qt::af::GetDefaultDisplayColorFromSettings()
{
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
  QString hexColor = sett.value("display/defaultDisplayColor").toString();  
  QColor defaultColor;
  defaultColor.setNamedColor(hexColor);
  if(!defaultColor.isValid())
    return Qt::white;

  return defaultColor;
}

QString te::qt::af::GetStyleSheetFromColors(QColor primaryColor, QColor secondaryColor)
{
  QString sty("alternate-background-color: ");
  sty += "rgb(" + QString::number(secondaryColor.red()) + ", " + QString::number(secondaryColor.green());
  sty += ", " + QString::number(secondaryColor.blue()) + ")";
  sty += ";background-color: rgb(" + QString::number(primaryColor.red()) + ", " + QString::number(primaryColor.green());
  sty += ", " + QString::number(primaryColor.blue()) + ");";

  return sty;
}

QString te::qt::af::GetStyleSheetFromSettings()
{
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
  //bool isChecked = sett.value("table/tableAlternateColors").toBool();
  QColor pColor;
  pColor.setNamedColor(sett.value("table/primaryColor").toString());
  QColor sColor;
  sColor.setNamedColor(sett.value("table/secondaryColor").toString());

  if(!pColor.isValid())
    pColor = Qt::white;
  if(!sColor.isValid())
    sColor = Qt::white;

  return GetStyleSheetFromColors(pColor, sColor);
}

bool te::qt::af::GetAlternateRowColorsFromSettings()
{
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
  bool isChecked = sett.value("table/tableAlternateColors").toBool();

  return isChecked;
}

void te::qt::af::AddActionToCustomToolbars(te::qt::af::ApplicationController* appController, QAction* act)
{
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

  sett.beginGroup("toolbars");
  QStringList lst = sett.childGroups();
  QStringList::iterator it;

  for(it=lst.begin(); it!=lst.end(); ++it)
  {
    int size = sett.beginReadArray(*it+"/Actions");

    for(int i=0; i<size; i++)
    {
      sett.setArrayIndex(i);

      QString v = sett.value("action").toString(); 

      if (!v.isNull() && v.compare(act->objectName()) == 0)
      {
        QToolBar* bar = appController->getToolBar(*it);
        bar->addAction(act);
      
        break;
      }
    }

    sett.endArray();
  }

  sett.endGroup();
}

std::vector<std::string> te::qt::af::GetPluginsFiles()
{
  std::vector<std::string> res;

  QStringList filters;

  filters << "*.teplg.json";

  QDir d(te::core::FindInTerraLibPath("share/terralib/plugins").c_str());

  QFileInfoList files = d.entryInfoList(filters, QDir::Files);

  foreach(QFileInfo file, files)
  {
    res.push_back(file.absoluteFilePath().toUtf8().data());
  }

  return res;
}

std::vector<std::string> te::qt::af::GetDefaultPluginsNames(te::qt::af::ApplicationController* appController)
{
  std::vector<std::string> res;

// Finding the Default plugins file.
  std::string pluginsPath = appController->getAppPluginsPath().toUtf8().data();

  if (pluginsPath == "")
    return res;

// Reading JSON
  boost::property_tree::ptree pt;
  boost::property_tree::json_parser::read_json(pluginsPath, pt);

  BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("plugins"))
  {
    res.push_back(v.second.get<std::string>("plugin"));
  }

  return res;
}

std::vector<std::string> te::qt::af::GetPluginsNames(const std::vector<std::string>& plgFiles)
{
  std::vector<std::string> res;
  std::vector<std::string>::const_iterator it;

  for(it=plgFiles.begin(); it!=plgFiles.end(); ++it)
  {
    boost::property_tree::ptree p;
    boost::property_tree::read_xml(*it, p, boost::property_tree::xml_parser::trim_whitespace);

    res.push_back(p.get<std::string>("PluginInfo.Name"));
  }

  return res;
}

QString te::qt::af::GetDateTime()
{
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

  return sett.value("configuration/generation").toString();
}

void te::qt::af::SetDateTime(const QString& dateTime)
{
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

  sett.setValue("configuration/generation", dateTime);
}

QString te::qt::af::GetDefaultConfigFileOutputDir()
{
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

  QFileInfo info(sett.fileName());

  return info.absolutePath();
}

void te::qt::af::UpdateUserSettingsFile(const QString& fileName, const bool& removeOlder)
{
  QFileInfo info(fileName);
  te::common::UserApplicationSettings& usrSett = te::common::UserApplicationSettings::getInstance();
  te::common::SystemApplicationSettings& appSett = te::common::SystemApplicationSettings::getInstance();

  if(info.exists())
    info.dir().remove(info.fileName());

  std::string olderFile = appSett.getValue("Application.UserSettingsFile.<xmlattr>.xlink:href");

  appSett.setValue("Application.UserSettingsFile.<xmlattr>.xlink:href", fileName.toUtf8().data());

  if(removeOlder)
  {
    info.setFile(olderFile.c_str());
    info.dir().remove(info.fileName());
  }

  info.setFile(fileName);

  if(!info.exists())
  {
#if BOOST_VERSION > 105600
    boost::property_tree::xml_writer_settings<std::string> settings('\t', 1);
#else
    boost::property_tree::xml_writer_settings<char> settings('\t', 1);
#endif
    boost::property_tree::write_xml(fileName.toUtf8().data(), usrSett.getAllSettings(), std::locale(), settings);
  }

  usrSett.load(fileName.toUtf8().data());
}

//void te::qt::af::WriteDefaultProjectFile(const QString& fileName)
//{
//  boost::property_tree::ptree p;
//
//  std::string schema_location = te::core::FindInTerraLibPath("share/terralib/schemas/terralib/qt/af/project.xsd");
//
//  //Header
//  p.add("Project.<xmlattr>.xmlns:xsd", "http://www.w3.org/2001/XMLSchema-instance");
//  p.add("Project.<xmlattr>.xmlns:te_map", "http://www.terralib.org/schemas/maptools");
//  p.add("Project.<xmlattr>.xmlns:te_qt_af", "http://www.terralib.org/schemas/qt/af");
//  p.add("Project.<xmlattr>.xmlns", "http://www.terralib.org/schemas/qt/af");
//  p.add("Project.<xmlattr>.xsd:schemaLocation", "http://www.terralib.org/schemas/qt/af " + schema_location);
//  p.add("Project.<xmlattr>.version", TERRALIB_VERSION_STRING);
//
//  //Contents
//  p.add("Project.Title", "Default project");
//  p.add("Project.Author", "");
//  p.add("Project.ComponentList", "");
//  p.add("Project.te_map:LayerList", "");
//
//  //Store file
//#if BOOST_VERSION > 105600
//  boost::property_tree::xml_writer_settings<std::string> settings('\t', 1);
//#else
//  boost::property_tree::xml_writer_settings<char> settings('\t', 1);
//#endif
//  boost::property_tree::write_xml(fileName.toUtf8().data(), p, std::locale(), settings);
//}

QString te::qt::af::GetGenerationDate()
{
  QString fileName = qApp->applicationDirPath() + "/../.generated";

  QFile f(fileName);
  if (!f.open(QFile::ReadOnly | QFile::Text))
    return "";

  QTextStream in(&f);
  QString s = in.readAll();

  f.close();

  return s;
}
