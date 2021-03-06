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
  \file terralib/qt/plugins/rp/ClassifierAction.h

  \brief This file defines the Classifier class
*/

#ifndef __TE_QT_PLUGINS_RP_INTERNAL_CLASSIFIERACTION_H
#define __TE_QT_PLUGINS_RP_INTERNAL_CLASSIFIERACTION_H

// TerraLib
#include "Config.h"
#include "AbstractAction.h"
#include "../../../qt/widgets/rp/ClassifierWizard.h"

namespace te
{
  namespace qt
  {
    namespace plugins
    {
      namespace rp
      {
        /*!
          \class ClassifierAction

          \brief This class register the classifier action into RP Plugin.

        */
        class ClassifierAction : public te::qt::plugins::rp::AbstractAction
        {
          Q_OBJECT

          public:

            ClassifierAction(QMenu* menu, QMenu* popupMenu);

            virtual ~ClassifierAction();

          protected slots:

            virtual void onActionActivated(bool checked);

            void addLayerSlot(te::map::AbstractLayerPtr layer);

        private:

          te::qt::widgets::ClassifierWizard* m_classifierWizard;

        };

      } // end namespace rp
    }   // end namespace plugins
  }     // end namespace qt
}       // end namespace te

#endif //__TE_QT_PLUGINS_RP_INTERNAL_CLASSIFIERACTION_H