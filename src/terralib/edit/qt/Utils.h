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
  \file terralib/edit/qt/Utils.h
   
  \brief Utility Qt functions for TerraLib Edit module.
*/

#ifndef __TERRALIB_EDIT_INTERNAL_QTUTILS_H
#define __TERRALIB_EDIT_INTERNAL_QTUTILS_H

// TerraLib
#include "../../geometry/Coord2D.h"
#include "../Config.h"

// Qt
#include <QMouseEvent>
#include <QPointF>

// STL
#include <vector>
//----------------------------------------

// TerraLib
#include "../../maptools/AbstractLayer.h"
#include "Config.h"

// STL
#include <string>
#include <vector>

namespace te
{
  // Forward declaration
  class Feature;

  // Forward declarations
  namespace da
  {
    class ObjectId;
  }

// Forward declarations
  namespace gm
  {
    class Geometry;
    class LineString;
    struct Coord2D;
    class Envelope;
    class GeometryCollection;
    class Polygon;
  }

  namespace qt
  {
    namespace widgets
    {
      class Canvas;
    }
  }

  namespace edit
  {
    TEEDITQTEXPORT QPointF GetPosition(QMouseEvent* e);

    TEEDITQTEXPORT te::gm::Geometry* convertGeomType(const te::map::AbstractLayerPtr& layer, te::gm::Geometry* geom);

  } // end namespace edit
}   // end namespace te

#endif  // __TERRALIB_EDIT_INTERNAL_QTUTILS_H
