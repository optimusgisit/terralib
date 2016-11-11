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
  \file terralib/wms/WMSLayerRenderer.h

  \brief It renders the data associated to a WMS layer.
*/

#ifndef __TERRALIB_WMS_INTERNAL_WMSLAYERRENDERER_H
#define __TERRALIB_WMS_INTERNAL_WMSLAYERRENDERER_H

// TerraLib
#include "../dataaccess/datasource/DataSource.h"
#include "../maptools/AbstractRenderer.h"
#include "Config.h"

namespace te
{
// Forward declarations
  namespace rst
  {
    class Raster;
  }

  namespace se
  {
    class CoverageStyle;
  }

  namespace wms
  {
// Forward declaration
    class WMSLayer;

    /*!
      \class WMSLayerRenderer

      \brief It renders the data associated to a WMS layer.
    */
    class TEWMSEXPORT WMSLayerRenderer : public te::map::AbstractRenderer
    {
      public:

        WMSLayerRenderer();

        ~WMSLayerRenderer();

        void draw(te::map::AbstractLayer* layer, te::map::Canvas* canvas, const te::gm::Envelope& bbox, int srid, const double& scale, bool* cancel);

      private:

        void draw(WMSLayer* layer, te::map::Canvas* canvas, const te::gm::Envelope& bbox, int srid);

        void drawRaster(const std::string& layerTitle, te::rst::Raster* raster, te::map::Canvas* canvas, const te::gm::Envelope& bbox, int bboxSRID,
                        const te::gm::Envelope& visibleArea, int srid, te::se::CoverageStyle* style);
    };

  } // end namespace wms
}   // end namespace te

#endif  // __TERRALIB_WMS_INTERNAL_WMSLAYERRENDERER_H
