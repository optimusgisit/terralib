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
  \file terralib/edit/qt/tools/CreateContinuosPolygonTool.h

  \brief This class implements a concrete tool to create polygons.
*/

#ifndef __TERRALIB_EDIT_QT_INTERNAL_CREATECONTINUOSPOLYGONTOOL_H
#define __TERRALIB_EDIT_QT_INTERNAL_CREATECONTINUOSPOLYGONTOOL_H

// TerraLib
#include "../../../geometry/Coord2D.h"
#include "../../../maptools/AbstractLayer.h"
#include "../Config.h"
#include "GeometriesUpdateTool.h"

// STL
#include <vector>

namespace te
{
  namespace gm
  {
    class Geometry;
  }

  namespace qt
  {
    namespace widgets
    {
      class MapDisplay;
    }
  }

  namespace edit
  {
    /*!
      \class CreateContinuosPolygonTool

      \brief This class implements a concrete tool to create polygons.
    */
    class TEEDITQTEXPORT CreateContinuosPolygonTool : public GeometriesUpdateTool
    {
      Q_OBJECT

      public:

        /** @name Initializer Methods
          *  Methods related to instantiation and destruction.
          */
        //@{

        /*!
          \brief It constructs a create polygon tool associated with the given map display.

          \param display The map display associated with the tool.
          \param parent The tool's parent.

          \note The tool will NOT take the ownership of the given pointers.
        */
        CreateContinuosPolygonTool(te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer, const QCursor& cursor, QObject* parent = 0);

        /*! \brief Destructor. */
        ~CreateContinuosPolygonTool();

        //@}

        /** @name AbstractTool Methods
          *  Methods related with tool behavior.
          */
        //@{

        bool mousePressEvent(QMouseEvent* e);

        bool mouseMoveEvent(QMouseEvent* e);

        bool mouseReleaseEvent(QMouseEvent* e);

        //@}

        void cancelEditionTool();

      private:

        void draw();

        void drawPolygon();

        void drawLine();

        void clear();

        te::gm::Geometry* buildPolygon();

        te::gm::Geometry* buildLine();

        void storeNewGeometry();

        void storeUndoCommand();

      private slots:

        void onExtentChanged();

      Q_SIGNALS:

        void closedPolygon();

      protected:

        std::vector<te::gm::Coord2D> m_coords;  //!< The coord list managed by this tool.
        te::gm::Coord2D m_lastPos;              //!< The last position captured on mouse move event.
        bool m_continuousMode;                  //!< A flag that indicates if the tool is working in 'continuous mode'. i.e. the coordinates will be acquired  from each mouseMove.
        bool m_isFinished;                      //!< A flag that indicates if the operations was finished.
        std::vector<Feature*> m_addWatches;

        int m_aux;
        int m_currentIndex;

    };

  }   // end namespace edit
}     // end namespace te

#endif  // __TERRALIB_EDIT_QT_INTERNAL_CreateContinuosPolygonTool_H
