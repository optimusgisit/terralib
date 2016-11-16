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
  \file terralib/qt/af/connectors/StyleExplorer.h

  \brief A connector for the te::qt::widgets::StyleDockWidget class to the Application Framework.
*/

#ifndef __TERRALIB_QT_AF_CONNECTORS_INTERNAL_STYLEEXPLORER_H
#define __TERRALIB_QT_AF_CONNECTORS_INTERNAL_STYLEEXPLORER_H

// Terralib
#include "../Config.h"

// Qt
#include <QtCore/QObject>

namespace te
{
  namespace map { class AbstractLayer; }

  namespace qt
  {
// Forward declarations
    namespace widgets 
    { 
      class StyleDockWidget;
    }

    namespace af
    {
      class ApplicationController;

      namespace evt
      {
      // Forward declarations
        struct Event;
      }
      /*!
        \class StyleExplorer

        \brief A connector for the te::qt::widgets::StyleDockWidget class to the Application Framework.

        It is used to listen events sent by the application framework.
      */
      class TEQTAFEXPORT StyleExplorer : public QObject
      {
        Q_OBJECT

        public:

          /*!
            \brief Constructor.

            \param explorer te::qt::widgets::StyleDockWidget to be listened.
          */
          StyleExplorer(te::qt::widgets::StyleDockWidget* explorer, te::qt::af::ApplicationController* app, QObject* parent = 0);

          /*! \brief Destructor. */
          ~StyleExplorer();

          te::qt::widgets::StyleDockWidget* getExplorer() const;

        public slots:

          /*!
            \brief Listener to the application framewrork events.

            \param evt An event sent by framework.
          */
          void onApplicationTriggered(te::qt::af::evt::Event* evt);

          void styleChanged(te::map::AbstractLayer* l);

        signals:

          void triggered(te::qt::af::evt::Event* e);

        protected:

          te::qt::af::ApplicationController* m_app;                  //!< Pointer to applicatin controller;

          te::qt::widgets::StyleDockWidget* m_explorer;             //!< Pointer to a component te::qt::widgets::StyleDockWidget.
      };

    } // end namespace af
  }   // end namespace qt
}     // end namespace te

#endif // __TERRALIB_QT_AF_CONNECTORS_INTERNAL_STYLEEXPLORER_H

