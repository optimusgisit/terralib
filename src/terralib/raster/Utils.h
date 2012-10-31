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
  \file terralib/raster/Utils.h
 
  \brief Utility functions for the raster module.
*/

#ifndef __TERRALIB_RASTER_INTERNAL_UTILS_H
#define __TERRALIB_RASTER_INTERNAL_UTILS_H

// TerraLib
#include "Band.h"
#include "BandProperty.h"
#include "Config.h"
#include "Grid.h"
#include "Raster.h"

// STL
#include <map>
#include <vector>

// Boost
#include <boost/shared_ptr.hpp>

namespace te
{
  namespace rst
  {
// Forward declaration
    class Band;
    class BandProperty;
    class Grid;
    class Raster;

    /*! \brief Returns the byte size of a given datatype. */
    TERASTEREXPORT int GetPixelSize(int datatype);

    /*!
      \brief Returns a vector of band properties, based on a given raster info.

      \param rinfo The map of raster informations.

      \return A vector of band properties.

      \note The caller will take the ownership of the returned pointer.

      \warning The variable rinfo must have the properties NBANDS and BANDSTYPE defined.
    */
    TERASTEREXPORT std::vector<BandProperty*> GetBandProperties(const std::map<std::string, std::string>& rinfo);

    /*!
      \brief Returns a grid based on a given raster info.

      \param rinfo The map of raster informations.

      \return A grid info.

      \note The caller will take the ownership of the returned pointer.

      \warning The parameter rinfo must have the properties NCOLS and NROWS defined.
      \warning Optional properties in parameter rinfo are RESX, RESY, SRID, ULX and ULY.
    */
    TERASTEREXPORT Grid* GetGrid(const std::map<std::string, std::string>& rinfo);

    /*!
      \brief Copies the pixel values from one raster to another.

      \param rin   The input raster.
      \param rout  The output raster.

      \warning Both rasters must have the same properties of grid and number/type of bands.
    */
    TERASTEREXPORT void Copy(const Raster& rin, Raster& rout);

    /*!
      \brief Copies the pixel values from one band to another.

      \param bin   The input band.
      \param bout  The output band.

      \warning Both bands must have the same properties of type and grid.
    */
    TERASTEREXPORT void Copy(const Band& bin, Band& bout);

    /*!
      \brief Copy a subset of the raster, given a box.

      \param method      The method of interpolation. \sa te::rst::Interpolator
      \param drow        The starting row to make a subset of the image.
      \param dcolumn     The starting column to make a subset of the image.
      \param height      The height of the subset.
      \param width       The width of the subset.
      \param rin         Pointer to valid output raster.
      \param rout        Pointer to valid output raster.

    */
     TERASTEREXPORT void Copy(unsigned int drow, unsigned int dcolumn, unsigned int height, unsigned int width, const Raster& rin, Raster& rout);

    /*!
      \brief Round a double value to a integer value.

      \param val        The double value.

      \return A integer value.
    */

     TERASTEREXPORT int Round(double val);
     
    /*!
      \brief Create a new raster from existing one.

      \param rin The input raster.
      
      \param uri The output raster URI.
      
      \param rType The name of the specific driver to create the raster. 
      
      \return a valid pointer to the created raster or a null (invalid) pointer if an error ocurred.
    */
    TERASTEREXPORT boost::shared_ptr< te::rst::Raster > CreateCopy(
      const te::rst::Raster& rin, const std::string& uri, 
      const std::string& rType = std::string( "GDAL" ) );     

  } // end namespace rst
}   // end namespace te

#endif  // __TERRALIB_RASTER_INTERNAL_UTILS_H
