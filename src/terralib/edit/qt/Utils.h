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
#include "../../maptools/AbstractLayer.h"
#include "Config.h"

// Qt
#include <QMouseEvent>
#include <QPointF>

// STL
#include <vector>

namespace te
{
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

    TEEDITQTEXPORT te::map::AbstractLayerPtr CreateShapeFileLayer(const te::gm::GeomType& type, const int& srid);
  } // end namespace edit
}   // end namespace te

#endif  // __TERRALIB_EDIT_INTERNAL_QTUTILS_H
