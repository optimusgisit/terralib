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
  \file terralib/wms/WMSLayerInfo.h
  
  \brief Informations about WMS Layers.
 */

#ifndef __TERRALIB_WMS_WMSLAYERINFO_H
#define __TERRALIB_WMS_WMSLAYERINFO_H

// TerraLib
#include "Config.h"

// STL
#include <string>

namespace te
{
  namespace wms
  {
    /*!
      \struct WMSLayerInfo
      
      \brief Informations about WMS Layers.
    */
    struct TEWMSEXPORT WMSLayerInfo
    {
      std::string m_name;                /*!< The layer name.                  */
      std::string m_title;               /*!< The layer title      .           */
      std::string m_pseudoGetMapRequest; /*!< The layer pseudo GetMap request. */
      std::string m_bbox;                /*!< The layer BBOX.                  */
      std::string m_srs;                 /*!< The layer SRS.                   */
      std::string m_version;             /*!< The layer version.               */
    };

  }   // end namespace wms
}     // end namespace te

#endif  // __TERRALIB_WMS_WMSLAYERINFO_H
