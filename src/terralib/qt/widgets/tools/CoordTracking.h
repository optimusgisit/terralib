/*  Copyright (C) 2001-2009 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/qt/widgets/tools/CoordTracking.h

  \brief This class implements a concrete tool to geographic coordinate tracking on mouse move operation.
*/

#ifndef __TERRALIB_QT_WIDGETS_INTERNAL_COORDTRACKING_H
#define __TERRALIB_QT_WIDGETS_INTERNAL_COORDTRACKING_H

// TerraLib
#include "../Config.h"
#include "AbstractTool.h"

// Forward declarations
class QPointF;

namespace te
{
  namespace qt
  {
    namespace widgets
    {
// Forward declarations
    class MapDisplay;

      /*!
        \class CoordTracking

        \brief This class implements a concrete tool to geographic coordinate tracking on mouse move operation.
      */
      class TEQTWIDGETSEXPORT CoordTracking : public AbstractTool
      {
        Q_OBJECT

        public:

          /** @name Initializer Methods
           *  Methods related to instantiation and destruction.
           */
          //@{

          /*!
            \brief It constructs a coordinate tracking tool associated with the given map display.

            \param display The map display associated with the tool.
            \param parent The tool's parent.

            \note The tool will NOT take the ownership of the given pointers.
          */
          CoordTracking(MapDisplay* display, QObject* parent = 0);

          /*! \brief Destructor. */
          ~CoordTracking();

          //@}

          /** @name AbstractTool Methods
           *  Methods related with tool behavior.
           */
          //@{

          bool mouseMoveEvent(QMouseEvent* e);

          //@}

        signals:

          /*! This signal is emitted when a coordinate was tracked. */
          void coordTracked(QPointF& coordinate);

        private:

          /** @name Copy Constructor and Assignment Operator
           *  Copy constructor and assignment operator not allowed.
           */
          //@{

          /*!
            \brief Copy constructor not allowed.

            \param rhs The right-hand-side copy that would be used to copy from.
          */
          CoordTracking(const CoordTracking& rhs);

          /*!
            \brief Assignment operator not allowed.

            \param rhs The right-hand-side copy that would be used to copy from.

            \return A reference to this object.
          */
          CoordTracking& operator=(const CoordTracking& rhs);

          //@}
      };

    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_INTERNAL_COORDTRACKING_H
