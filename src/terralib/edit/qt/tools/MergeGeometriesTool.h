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
  \file terralib/edit/qt/tools/MergeGeometriesTool.h

  \brief This class implements a concrete tool to merge polygons.
*/

#ifndef __TERRALIB_EDIT_QT_INTERNAL_MERGEGEOMETRIESTOOL_H
#define __TERRALIB_EDIT_QT_INTERNAL_MERGEGEOMETRIESTOOL_H

// TerraLib
#include "../../../geometry/Coord2D.h"
#ifndef Q_MOC_RUN
#include "../../../geometry/Geometry.h"
#include "../../../geometry/GeometryCollection.h"
#include "../../../geometry/GeometryProperty.h"
#include "../../../dataaccess/dataset/ObjectId.h"
#include "../../../dataaccess/dataset/ObjectIdSet.h"
#include "../../../maptools/AbstractLayer.h"
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

    class TEEDITQTEXPORT MergeGeometriesTool : public GeometriesUpdateTool
    {
      Q_OBJECT

    public:

      MergeGeometriesTool(te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer, const QCursor& cursor, QObject* parent = 0);

      /*! \brief Destructor. */
      ~MergeGeometriesTool();

    private:

      void draw();

      void pickFeature(const te::map::AbstractLayerPtr& layer, const QPointF& pos);

      te::gm::Envelope buildEnvelope(const QPointF& pos);

      void storeFeature();

      void mergeGeometries();

      bool spatialRelationDisjoint(te::gm::GeometryCollection& gc);

      const te::gm::Envelope* getRefEnvelope(te::da::DataSet& ds, te::gm::GeometryProperty& geomProp);

      void getBaseOID(const te::da::ObjectIdSet& objSet, QString msg);

    protected:

      std::string m_chosenOid;
      te::gm::GeometryCollection* m_geocollection;
      te::da::ObjectIdSet* m_oidsMerged;

    };

  }   // end namespace edit
}     // end namespace te

#endif  // __TERRALIB_EDIT_QT_INTERNAL_MERGEGEOMETRIESTOOL_H