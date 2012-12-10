/*  Copyright (C) 2001-2010 National Institute For Space Research (INPE) - Brazil.

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
  \defgroup appframework Application Framework
  \brief Defines a framework to be used on TerraLib-based applications
  \details The framework consists of three groups of classes:
  <ol>
    <li>Core: The CoreApplication class is the main class. This class manages the configurations of the application and is responsible for broad cast events
    to framework listeners.</li>
    <li>Connectors: These classes just makes the interface between TerraLib components and the application framework. This way we can simple connect
    signals of the components and dispatches the ocurrencies generated by them. On the other side, it can receive events from the framework and
    react on the component.</li>
    <li>Events: These classes are reponsible for communicating ocurrencies between classes in the framework. Any object can broadcast events using
    framework, but only the registered ones will react to them.</li>
  </ol>
*/

/*!
  \defgroup afconnector Application Framework Connectors
  \ingroup appframework
  \brief Connectors for application framework.
  \details These classes are used to connect the TerraLib qt-based components to the application framework. The implementation only contains
  the slot \a onApplicationTriggered(te::qt::af::evt::Event*) and a pointer to the componet to be connected to the framework.
  This way, it is possible to enable communication between components without inheritance, just intercepting the events sent by
  application framework and can act on the component.
  \note NO ONE of the connectors gets the ownership of the assisted components.
*/

/*! 
  \file af/CoreApplication.h
  \brief 
  \details 
  \version 5.0
  \author Frederico Augusto Bed&ecirc; &lt;frederico.bede@funcate.org.br&gt;
  \date 2001-2012
  \copyright GNU Lesser General Public License.
  \TODO Add comments to brief and details of the class te::qt::af::CoreApplication.
 */

#ifndef __TERRALIB_QT_AF_INTERNAL_COREAPPLICATION_H
#define __TERRALIB_QT_AF_INTERNAL_COREAPPLICATION_H

//! Terralib include files
#include "Config.h"
#include <terralib/common/Singleton.h>

//! Qt include files
#include <QObject>

//! STL include files
#include <map>
#include <set>
#include <vector>

//! Forward declarations
class QToolBar;
class QMenu;

namespace te
{
  namespace qt
  {
    namespace af
    {
      //! Forward declarations
      class Event;

      /*!
        \class CoreApplication
        \copydoc af/CoreApplication.h
        \ingroup appframework
      */

      class TEQTAFEXPORT CoreApplication : public QObject
      {
        Q_OBJECT

      public:

        /*!
          \brief Constructor.
          \param parent QObject parent.
        */
        CoreApplication(QObject* parent=0);

        /*!
          \brief Destructor.
        */
        ~CoreApplication();

        /*!
          \brief Updates the QToolBar set.
          \note Do NOT get the pointer ownership.
          \note Will send an event to append toolbar in some GUI.
          \exception Exception It throws an exception if a toolbar with the given \a barId already exists.
        */
        void addToolBar(const QString& barId, QToolBar* bar);

        /*!
          \brief Updates the QToolBar set.
          \note Do NOT get the pointer ownership.
          \note Do NOT send event to insert toolbar.
          \exception Exception It throws an exception if a toolbar with the given \a barId already exists.
        */
        void registerToolBar(const QString& barId, QToolBar* bar);

        /*!
          \brief Returns the QToolBar identified by \a barId or \a NULL if no one was found.
          \param barId
          \return
        */
        QToolBar* getToolBar(const QString& barId);

        /*!
          \brief Inserts an object that wil listen to framework events.
          \param obj
          \note The \a obj MUST HAVE AN IMPLEMENTATION of the slot \a onApplicationTriggered(Event*). If dont has, a warning will be sent at run-time.
          \note If the \a obj was previously registered, nothing will be done.
        */
        void addListener(QObject* obj);

        /*!
          \brief Removes the \a obj from listeners of the framework events.
          \param obj Object to be removed.
          \note If the \a obj was not previously registered, nothing will be done.
        */
        void removeListener(QObject* obj);

        /*!
          \brief
          \param
        */
        void setApplicationInfo(const std::vector< std::pair<std::string, std::string> >& info);

        /*!
          \brief
          \return
        */
        const std::vector< std::pair<std::string, std::string> >* getApplicationInfo() const;

        /*!
          \brief
          \param
        */
        void setApplicationPlgInfo(const std::vector< std::pair<std::string, std::string> >& info);

        /*!
          \brief
          \return
        */
        const std::vector< std::pair<std::string, std::string> >* getApplicationPlgInfo() const;

        /*!
          \brief
          \param
        */
        void setUserInfo(const std::vector< std::pair<std::string, std::string> >& info);

        /*!
          \brief
          \return
        */
        const std::vector< std::pair<std::string, std::string> >* getUserInfo() const;

        /*!
          \brief
          \param fileName
          \param start
          \param connect2AF
        */
        void addPlugin(const std::string fileName, const bool& start=false, const bool& connect2AF=false);

        /*!
          \brief Initializes the application framework.
          \details Initializes the application framework, reading configurations files and updating the applications. This also initializes the available terralib modules.
          This means that applications that uses the application framework, does not need to initialize terralib modules, by calling TerraLib::getinstance().initialize() and 
          consequently TerraLib::getinstance().finalize() methods.
          \note If a client of this class has objects which want to know about the initialization events - like any information that a plug-in can send in its loading process 
          for example, make sure that these objects are connected to the framework, BEFORE calling this method.
          \note Make sure that the application calls this method BEFORE it uses any terralib modules or functions.
          \note Make sure that the application calls the method setApplicationInfo() BEFORE calling initialize. This method saves the application configuration file if
          it not exists. See sections of Configuring Applications Using the application framework for details about the configurations available.
        */
        void initialize();

        /*!
          \brief Finalizes the application framework.
          \details This method will unload all terralib modules, plug-ins, and dynamic libraries that are loaded.
          \note Make sure that the application calls this method BEFORE it finishes.
        */
        void finalize();

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

      protected:

        std::map<QString, QToolBar*> m_toolbars;                                                //!< Toolbars registered.
        std::set<QObject*> m_reg_coms;                                                          //!< Registered objects.
        std::vector< std::pair<std::string, std::string> > m_app_info;                          //!< Configurations.
        std::vector< std::pair<std::string, std::string> > m_user_info;                         //!< User configurations.
        std::vector< std::pair<std::string, std::string> > m_app_plg_info;                      //!< Application Plug-insConfigurations.
        std::map<std::string, std::string> m_plg_files;
        bool m_initialized;
      };

      /*!
        \class Application
        \brief A singleton over the te::qt::af::CoreApplication class.
      */
      class TEQTAFEXPORT Application : public te::common::Singleton<CoreApplication>
      {
        friend class te::common::Singleton<CoreApplication>;

        private:

          /*!
            \name Construtor and copy methods not allowed.
          */
          //! @{

          /*!
            \brief Default constructor.
          */
          Application();

          /*!
            \brief Copy constructor.
          */
          Application(const Application&);
          
          /*!
            \brief Destructor.
          */
          ~Application();

          /*!
            \brief Copy operator.
          */
          Application& operator=(const Application&);
          //! @}
      };
    }
  }
}

#endif // __TERRALIB_QT_AF_INTERNAL_COREAPPLICATION_H
