/*  Copyright (C) 2001-2009 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled applications.

    TerraLib is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraLib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TerraLib. See COPYING. If not, write to
    TerraLib Team at <terralib-team@terralib.org>.
 */

/*!
  \file srs.h
  
  \brief This file contains include headers for TerraLib Spatial Reference System module.
 */
#ifndef __TERRALIB_INTERNAL_SRS_H
#define __TERRALIB_INTERNAL_SRS_H

// TerraLib
#include "srs/Config.h"
#include "srs/Converter.h"
#include "srs/SpatialReferenceSystem.h"
#include "srs/Datum.h"
#include "srs/Ellipsoid.h"
#include "srs/Exception.h"
#include "srs/GeographicCoordinateSystem.h"
#include "srs/ProjectedCoordinateSystem.h"
#include "srs/SpatialReferenceSystemManager.h"

namespace te
{
  /*!
    \brief This is the namespace for the Spatial Reference System module module.

    This module can be used to manage the concept of spatial reference systems.
  
   */
  namespace srs
  {
  } // end namespace srs
}   // end namespace te

#endif  // __TERRALIB_INTERNAL_SRS_H
