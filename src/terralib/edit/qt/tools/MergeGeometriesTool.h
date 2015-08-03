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

\brief This class implements a concrete tool to create lines.
*/

#ifndef __TERRALIB_EDIT_QT_INTERNAL_MERGEGEOMETRIESTOOL_H
#define __TERRALIB_EDIT_QT_INTERNAL_MERGEGEOMETRIESTOOL_H

// TerraLib
#include "../../../edit/qt/core/EditionManager.h"
#include "../../../geometry/Coord2D.h"
#include "../../../dataaccess/dataset/ObjectIdSet.h"
#include "../../../maptools/AbstractLayer.h"
#include "../../../qt/widgets/tools/AbstractTool.h"
#include "../Config.h"

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

    class TEEDITQTEXPORT MergeGeometriesTool : public te::qt::widgets::AbstractTool
    {
      Q_OBJECT

    public:

      MergeGeometriesTool(te::edit::EditionManager* editionManager, te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer, const QCursor& cursor, QObject* parent = 0);

      /*! \brief Destructor. */
      ~MergeGeometriesTool();

      bool mousePressEvent(QMouseEvent* e);

      bool mouseMoveEvent(QMouseEvent* e);

      bool mouseReleaseEvent(QMouseEvent* e);

      bool mouseDoubleClickEvent(QMouseEvent* e);


    private:

      te::da::ObjectId* m_refOid;
      void draw();
      void storeMergedFeature();
      void mergeGeometries();
      bool spatialRelationDisjoint(te::gm::GeometryCollection* gc);

      te::gm::Geometry* Union(te::gm::Geometry* g1, te::gm::Geometry* g2);
      const te::gm::Envelope* getRefEnvelope(te::da::DataSet* ds, te::da::ObjectId* oid, te::gm::GeometryProperty* geomProp);
      te::da::ObjectId* getBaseOID(const te::da::ObjectIdSet* objSet, QString msg);
      te::da::ObjectIdSet* m_oidRemoved;

    private slots:

      void onExtentChanged();

    protected:

      te::map::AbstractLayerPtr m_layer;
      Feature* m_feature;
      te::edit::EditionManager* m_editionManager;

    };

  }   // end namespace edit
}     // end namespace te

#endif  // __TERRALIB_EDIT_QT_INTERNAL_MERGEGEOMETRIESTOOL_H