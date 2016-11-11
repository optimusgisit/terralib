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
  \file

  \brief
*/

#ifndef __TERRALIB_EDIT_QT_INTERNAL_GEOMETRIESUPDATETOOL_H
#define __TERRALIB_EDIT_QT_INTERNAL_GEOMETRIESUPDATETOOL_H

#include "../Config.h"
#include "../../../qt/widgets/tools/AbstractTool.h"


namespace te
{
  namespace gm
  {
    class Geometry;
  }

  namespace map
  {
    class AbstractLayer;
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
    class Feature;

    class TEEDITQTEXPORT GeometriesUpdateTool : public te::qt::widgets::AbstractTool
    {
      Q_OBJECT

    public:

      GeometriesUpdateTool(te::qt::widgets::MapDisplay* display, te::map::AbstractLayer* layer, QObject* parent = 0);

      virtual ~GeometriesUpdateTool();

      void setLayer(te::map::AbstractLayer* layer);

      bool isInUse() const;

      void setInUse(const bool& status);

      virtual void resetVisualizationTool();

      virtual void storeUndoCommand();

      std::string getLayerId();

    Q_SIGNALS:

      void geometriesEdited();

      void toolDeleted();

      void endSelectCell(const std::string&, const std::size_t&, std::size_t&);

    private slots:

      void onUndoFeedback();

    protected:

      virtual void saveGeometry(te::gm::Geometry* gm, const int& op);

      virtual void storeFeature();

      bool m_isInUse;

      Feature*                m_feature;
      te::map::AbstractLayer* m_layer;

    };
  }
}


#endif //__TERRALIB_EDIT_QT_INTERNAL_GEOMETRIESUPDATETOOL_H
