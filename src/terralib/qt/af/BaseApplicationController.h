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
  \file terralib/qt/af/BaseApplicationController.h

  \brief The base API for controllers of TerraLib applications.
*/

#ifndef __TERRALIB_QT_AF_INTERNAL_BASEAPPLICATIONCONTROLLER_H
#define __TERRALIB_QT_AF_INTERNAL_BASEAPPLICATIONCONTROLLER_H

// Terralib
#include "Config.h"

// STL
#include <map>
#include <set>
#include <vector>

// Qt
#include <QtCore/QObject>
#include <QtCore/QString>

// Forward declarations
class QMenu;
class QMenuBar;
class QToolBar;
class QWidget;

namespace te
{
  namespace qt
  {
    namespace af
    {
// Forward declarations
      class Event;

      /*!
        \class BaseApplicationController

        \brief The base API for TerraLib applications.
      */
      class TEQTAFEXPORT BaseApplicationController : public QObject
      {
        Q_OBJECT

        public:

          /*!
            \brief Constructor.

            \param parent The parent object.
          */
          BaseApplicationController(QObject* parent = 0);

          /*! \brief Destructor. */
          virtual ~BaseApplicationController();

          /*!
            \brief Tells wich configuration file to be used by the controller during its initialization.

            \param configFileName The configuration file name with full path.
          */
          virtual void setConfigFile(const std::string& configFileName);

          /*!
            \brief Tells the widget to be used as the parent of messages showned in a QMessageBox.

            \param w The widget to be used as the parent of messages showned in a QMessageBox.
          */
          virtual void setMsgBoxParentWidget(QWidget* w);

          /*!
            \brief Register the toolbar in the list of the known toolbars and dispatch an event.

            \param id  The toolbar identification string.
            \param bar The toolbar to be registered. The controller will not take the \a bar ownership.

            \pos An event will be broadcasted to indicate that the toolbar was appended.

            \exception Exception It throws an exception if a toolbar with the given \a id already exists.
          */
          void addToolBar(const QString& id, QToolBar* bar);

          /*!
            \brief Register the toolbar in the list of the known toolbars.

            \param id  The toolbar identification string.
            \param bar The toolbar to be registered. The controller will not take the \a bar ownership.

            \exception Exception It throws an exception if a toolbar with the given \a id already exists.

            \todo Rever se este metodo eh necessario ou nao! Talvez possamos viver somente com o addToolBar!
          */
          void registerToolBar(const QString& id, QToolBar* bar);

          /*!
            \brief Return the toolbar identified by \a id or \a NULL if none is found.

            \param id The toolbar identification string.

            \return A toolbar identified by \a id or \a NULL if none is found.
          */
          QToolBar* getToolBar(const QString& id) const;

          /*! 
            \name Menus and Menubars Management.

            \brief Register and recover menus and menu bars.

            The methods \a find will returns a valid object only if it exists in some menu, otherwise the return will be a NULL pointer.
            The methods \a get ALWAYS returns a valid pointer. If the requested menu or menubar does not exists these funtions will create,
            register and return the object.

            \note The identifiers of the menus are the texts presented by menus.
            \note The search for menus will occurs in ALL OF THE REGISTERED menus and menu bars, including submenus. The correct sintaxe
            for searchies submenus may be found in the documentation of the findMenu(QString mnuText, QMenu* mnu) method.
          */

          //@{
          /*!
            \brief Register the \a mnu.

            \param mnu Pointer to the menu to be registered.
          */
          void registerMenu(QMenu* mnu);

          /*!
            \brief Returns the menu registered with key \a id

            \param id Indentifier of the menu.

            \return Menu registered or NULL with there is not one registered with the key \a id.
          */
          QMenu* findMenu(const QString& id) const;

          /*!
            \brief Returns a menu registered with key \a id.

            \param id Identifier of menu to be searched.

            \return The menu registered with the \a id identifier.

            \note If the menu does not exists, then this function will create it.
          */
          QMenu* getMenu(const QString& id) const;

          /*!
            \brief Register the \a bar.

            \param bar Pointer to the menu bar to be registered.
          */
          void registerMenuBar(QMenuBar* bar);

          /*!
            \brief Returns the menu bar registered with key \a id

            \param id Indentifier of the menu bar.

            \return Menu bar registered or NULL with there is not one registered with the key \a id.
          */
          QMenuBar* findMenuBar(const QString& id) const;

          /*!
            \brief Returns a menu bar registered with key \a id.

            \param id Identifier of menu bar to be searched.

            \return The menu bar registered with the \a id identifier.

            \note If the menu bar does not exists, then this function will create it.
          */
          QMenuBar* getMenuBar(const QString& id) const;
          //@}

          /*!
            \brief Insert an application item that will listen to framework events.

            \param obj The application item that will listen to framework events.

            \note The \a obj MUST HAVE AN IMPLEMENTATION of the slot \a onApplicationTriggered(Event*). If it hasn't, a warning will be sent at run-time.

            \note If the \a obj was previously registered, nothing will be done.
          */
          void addListener(QObject* obj);

          /*!
            \brief Remove the \a obj from the list of event listeners.

            \param obj The application item to be removed.

            \note If the \a obj was not previously registered, nothing will be done.
          */
          void removeListener(QObject* obj);

          /*!
            \brief Initializes the application framework.

            The initialization will read the following configurations files (in order):
            <ul>
            <li>A main config file, generally called config.xml</li>
            <li>A user settings file, generally called user_settings.xml<li>
            </ul>

            It will start by initializing the available TerraLib modules.
            This means that applications that uses the application framework, 
            does not need to call TerraLib initialization (TerraLib::getInstance().initialize()) and 
            finalization methods (TerraLib::getInstance().finalize()).

            \exception Exception It throws an exception if the initialization fails.

            \note Make sure that the application calls this method BEFORE it uses any TerraLib modules or functions.
          */
          virtual void initialize();

          /*!
            \brief Load the plugin list and initialize the plugins enabled by the user.

            The plugin initialization will read a file containing a list of plugins with their addresses.
            This file is generally called application_plugins.xml.

            \pre The application must assure that any resource needed by their plugins are ready for use. This
                 means that initialize should be called before initializing the plugins.
          */
          virtual void initializePlugins();

          /*!
            \brief Finalize the application framework.

            \details This method will unload all TerraLib modules, plugins, and dynamic libraries that are loaded.

            \note Make sure that the application calls this method BEFORE it finishes.
          */
          virtual void finalize();

        public slots:

          /*!
            \brief Send events in broadcast for all registered components.

            \param evt Event to be sent.
          */
          void broadcast(te::qt::af::Event* evt);

        signals:

          /*!
            \brief Signal emmited for the aplication framework listeners.
          */
          void triggered(te::qt::af::Event*);

        public:

          const QString& getAppTitle() const;

          const QString& getAppIconName() const;

        protected:

          std::map<QString, QToolBar*> m_toolbars;    //!< Toolbars registered.
          std::vector<QMenu*> m_menus;                //!< Menus registered.
          std::vector<QMenuBar*> m_menuBars;          //!< Menu bars registered.
          std::set<QObject*> m_applicationItems;      //!< The list of registered application items.
          QWidget* m_msgBoxParentWidget;              //!<
          std::string m_appConfigFile;                //!< The application framework configuration file.
          QString m_appOrganization;
          QString m_appName;
          QString m_appTitle;
          QString m_appIconName;
          std::string m_appUserSettingsFile;
          QString m_appPluginsFile;
          QString m_appHelpFile;
          QString m_appIconThemeDir;
          QString m_appDefaultIconTheme;
          QString m_appToolBarDefaultIconSize;
          std::string m_appDatasourcesFile;
          bool m_initialized;                         //!< A flag indicating if the controller is initialized.
      };

    } // end namespace af
  }   // end namespace qt
}     // end namespace te

#endif // __TERRALIB_QT_AF_INTERNAL_BASEAPPLICATIONCONTROLLER_H
