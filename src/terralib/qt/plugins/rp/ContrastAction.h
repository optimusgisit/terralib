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
  \file terralib/qt/plugins/rp/ContrastAction.h

  \brief This file defines the Contrast class
*/

#ifndef __TE_QT_PLUGINS_RP_INTERNAL_CONTRASTACTION_H
#define __TE_QT_PLUGINS_RP_INTERNAL_CONTRASTACTION_H

// TerraLib
#include "../../../qt/widgets/rp/ContrastDialogForm.h"
#include "Config.h"
#include "AbstractAction.h"

namespace te
{
  namespace qt
  {
    namespace plugins
    {
      namespace rp
      {
        /*!
          \class ContrastAction

          \brief This class register the contrast action into RP Plugin.

        */
        class ContrastAction : public te::qt::plugins::rp::AbstractAction
        {
          Q_OBJECT

          public:

            ContrastAction(QMenu* menu, QMenu* popupMenu);

            virtual ~ContrastAction();

          protected slots:

            virtual void onActionActivated(bool checked);

            virtual void onPopUpActionActivated(bool checked);

            void onApplicationTriggered(te::qt::af::evt::Event* e);
            
            void addLayer(te::map::AbstractLayerPtr outputLayer);

            void closeTool();

          private:
            te::qt::widgets::ContrastDialogForm* m_contrastDlg;
        };

      } // end namespace rp
    }   // end namespace plugins
  }     // end namespace qt
}       // end namespace te

#endif //__TE_QT_PLUGINS_RP_INTERNAL_CONTRASTACTION_H