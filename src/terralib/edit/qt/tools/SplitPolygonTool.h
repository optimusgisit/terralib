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
  \file terralib/edit/qt/tools/SplitPolygonTool.h

  \brief This class implements a concrete tool to split polygons.
*/

#ifndef __TERRALIB_EDIT_QT_INTERNAL_SPLITPOLYGONTOOL_H
#define __TERRALIB_EDIT_QT_INTERNAL_SPLITPOLYGONTOOL_H

// TerraLib
#include "../../../geometry/Coord2D.h"
#ifndef Q_MOC_RUN
#include "../../../dataaccess/dataset/ObjectIdSet.h"
#include "../../../geometry/GeometryCollection.h"
#include "../../../geometry/GeometryProperty.h"
#include "../../../geometry/MultiPolygon.h"
#include "../../../geometry/Utils.h"
#include "../../../maptools/AbstractLayer.h"
#include "../../../qt/widgets/canvas/Canvas.h"
#include "CreateLineTool.h"
#endif
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
      \class SplitPolygonTool

      \brief This class implements a concrete tool to split polygons.
    */
    class TEEDITQTEXPORT SplitPolygonTool : public CreateLineTool
    {
      Q_OBJECT

      public:

        /** @name Initializer Methods
          *  Methods related to instantiation and destruction.
          */
        //@{

        /*!
          \brief It constructs a split polygon tool associated with the given map display.

          \param display The map display associated with the tool.
          \param parent The tool's parent.

          \note The tool will NOT take the ownership of the given pointers.
        */
        SplitPolygonTool(te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer, const te::edit::MouseEventEdition mouseEventToSave, QObject* parent = 0);

        /*! \brief Destructor. */
        ~SplitPolygonTool();

        //@}

        /** @name AbstractTool Methods
          *  Methods related with tool behavior.
          */
        //@{

        bool mousePressEvent(QMouseEvent* e);

        bool mouseMoveEvent(QMouseEvent* e);

        bool mouseDoubleClickEvent(QMouseEvent* e);

        bool mouseReleaseEvent(QMouseEvent* e);

        void resetVisualizationTool();

        //@}

      private:

        double m_tol;
        te::da::ObjectIdSet* m_oidSet;
        std::vector<te::edit::Feature*> m_vecFeature;
        MouseEventEdition m_mouseEventToSave;

        void draw();
        void pickFeatures();
        void splitPolygon(std::size_t index);
        void startSplit();

    };

  }   // end namespace edit
}     // end namespace te

#endif  // __TERRALIB_EDIT_QT_INTERNAL_SPLITPOLYGONTOOL_H
