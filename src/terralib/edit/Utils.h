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
\file terralib/edit/Utils.h

\brief Utility functions for TerraLib Edit module.
*/

#ifndef __TERRALIB_EDIT_INTERNAL_UTILS_H
#define __TERRALIB_EDIT_INTERNAL_UTILS_H

// TerraLib
#include "../color/RGBAColor.h"
#include "../maptools/AbstractLayer.h"
#include "Config.h"

// STL
#include <string>
#include <vector>

namespace te
{
  // Forward declarations
  namespace da
  {
    class ObjectId;
  }

  namespace gm
  {
    struct Coord2D;
    class Envelope;
    class Geometry;
    class GeometryCollection;
    class LineString;
    class Polygon;
  }

  namespace edit
  {
    // Forward declaration
    class Feature;

    struct VertexIndex
    {
      void setIndex(const std::size_t& line, const std::size_t& pos)
      {
        m_line = line;
        m_pos = pos;
      }

      void makeInvalid()
      {
        m_line = std::string::npos;
        m_pos = std::string::npos;
      }

      bool isValid() const
      {
        return m_line != std::string::npos && m_pos != std::string::npos;
      }

      std::size_t m_line;
      std::size_t m_pos;
    };

    /*!
    \enum FeatureType

    \brief Defines the feature type(Add, Update, Delete ...).
    */
    enum FeatureType
    {
      TO_BLOCKEDIT = 0,   //!< block feature to edition.
      TO_ADD = 1,         //!< To be added.
      TO_UPDATE = 2,      //!< To be updated.
      TO_DELETE = 3       //!< To be removed.
    };

    /*!
    \enum MouseEventEdition

    \brief Defines which mouse event will be used in the tool to complete an operation.
    */
    enum MouseEventEdition 
    {
      mouseDoubleClick = 0,
      mouseReleaseRightClick = 1
    };

    TEEDITEXPORT Feature* PickFeature(const te::map::AbstractLayerPtr& layer, const te::gm::Envelope& env, int srid, FeatureType type);

    TEEDITEXPORT Feature* PickFeature(const te::map::AbstractLayer* layer, const te::gm::Envelope& env, int srid, FeatureType type);

    TEEDITEXPORT void GetLines(te::gm::Geometry* geom, std::vector<te::gm::LineString*>& lines);

    TEEDITEXPORT void GetLines(te::gm::GeometryCollection* gc, std::vector<te::gm::LineString*>& lines);

    TEEDITEXPORT void GetLines(te::gm::Polygon* p, std::vector<te::gm::LineString*>& lines);

    TEEDITEXPORT void GetLines(te::gm::LineString* l, std::vector<te::gm::LineString*>& lines);

    TEEDITEXPORT void MoveVertex(std::vector<te::gm::LineString*>& lines, const VertexIndex& index, const double& x, const double& y);

    TEEDITEXPORT void RemoveVertex(std::vector<te::gm::LineString*>& lines, const VertexIndex& index);

    TEEDITEXPORT void AddVertex(std::vector<te::gm::LineString*>& lines, const double& x, const double& y, const te::gm::Envelope& env, int srid);

    TEEDITEXPORT VertexIndex FindSegment(std::vector<te::gm::LineString*>& lines, const te::gm::Envelope& env, int srid);

    TEEDITEXPORT void MoveGeometry(te::gm::Geometry* geom, const double& deltax, const double& deltay);

    TEEDITEXPORT bool IsSpecialRingVertex(te::gm::LineString* l, const VertexIndex& index);

    TEEDITEXPORT double GetDistance(const te::gm::Coord2D& c1, const te::gm::Coord2D& c2);

    TEEDITEXPORT void GetCoordinates(te::gm::Geometry* geom, std::vector<te::gm::Coord2D>& coords);

    TEEDITEXPORT void TrySnap(te::gm::Coord2D& coord, int srid);

    TEEDITEXPORT te::da::ObjectId* GenerateId();

    TEEDITEXPORT te::gm::Geometry* ConvertGeomType(const te::map::AbstractLayerPtr& layer, te::gm::Geometry* geom);

  } // end namespace edit
}   // end namespace te

#endif  // __TERRALIB_EDIT_INTERNAL_UTILS_H
