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
  \file TableEvents.h

  \brief Contains a list of the table events.
*/

#ifndef __TERRALIB_QT_AF_EVENTS_INTERNAL_TABLEEVENTS_H
#define __TERRALIB_QT_AF_EVENTS_INTERNAL_TABLEEVENTS_H

#include "../../../maptools/Enums.h"
#include "../../../maptools/AbstractLayer.h"
#include "Event.h"
#include "Enums.h"

namespace te
{
  namespace qt
  {
    namespace af
    {
      namespace evt
      {
        /*!
          \struct TableAlternatingColorsChanged

          \brief This event signals that table has alternating row colors.

          \ingroup afevents
         */
        struct TableAlternatingColorsChanged : public Event
        {
          /*!
            \brief Constructor.

            \param isAlternating If table has alternating row colors.
          */
          TableAlternatingColorsChanged(bool isAlternating) : 
            Event(TABLE_ALTERNATE_COLORS_CHANGED),
            m_isAlternating(isAlternating)
          {
          }

          bool m_isAlternating;
        };
      }
    }
  }
}

#endif //__TERRALIB_QT_AF_EVENTS_INTERNAL_TABLEEVENTS_H
