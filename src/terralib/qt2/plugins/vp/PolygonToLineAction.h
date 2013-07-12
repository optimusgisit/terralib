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
  \file terralib/qt/plugins/vp/PolygonToLineAction.h

  \brief This file defines the PolygonToLine class
*/

#ifndef __TE_QT_PLUGINS_VP_INTERNAL_POLYGONTOLINEACTION_H
#define __TE_QT_PLUGINS_VP_INTERNAL_POLYGONTOLINEACTION_H

// TerraLib
#include "Config.h"
#include "AbstractAction.h"

namespace te
{
  namespace qt
  {
    namespace plugins
    {
      namespace vp
      {
        /*!
          \class PolygonToLineAction

          \brief This class register the contrast action into VP Plugin.

        */
        class PolygonToLineAction : public te::qt::plugins::vp::AbstractAction
        {
          Q_OBJECT

          public:

            PolygonToLineAction(QMenu* menu);

            virtual ~PolygonToLineAction();

          protected slots:

            virtual void onActionActivated(bool checked);
        };

      } // end namespace vp
    }   // end namespace plugins
  }     // end namespace qt
}       // end namespace te

#endif //__TE_QT_PLUGINS_VP_INTERNAL_POLYGONTOLINEACTION_H