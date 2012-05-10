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
  \file Utils.h

  \brief Utility functions for MapTools module.
*/

#ifndef __TERRALIB_MAPTOOLS_INTERNAL_UTILS_H
#define __TERRALIB_MAPTOOLS_INTERNAL_UTILS_H

// TerraLib
#include "../color/RGBAColor.h"
#include "Config.h"

// STL
#include <vector>

namespace te
{
// Forward declarations
  namespace se
  {
    class ParameterValue;
  }

  namespace map
  {
    /*!
      \brief Gets the parameter value as RGBA color.

      \param param The Symbology Enconding Parameter Value element.

      \return The RGBA color.

      \note Warning: It consider that the paramater value is enconded using a Literal OGC Filter expression.
    */
    TEMAPEXPORT te::color::RGBAColor GetColor(const te::se::ParameterValue* param);

    /*!
      \brief Gets the parameter value as integer.

      \param param The Symbology Enconding Parameter Value element.

      \return The integer value.

      \note Warning: It consider that the paramater value is enconded using a Literal OGC Filter expression.
    */
    TEMAPEXPORT int GetInt(const te::se::ParameterValue* param);

    /*!
      \brief Gets the parameter value as double.

      \param param The Symbology Enconding Parameter Value element.

      \return The double value.

      \note Warning: It consider that the paramater value is enconded using a Literal OGC Filter expression.
    */
    TEMAPEXPORT double GetDouble(const te::se::ParameterValue* param);

    /*!
      \brief Gets the parameter value as string.

      \param param The Symbology Enconding Parameter Value element.

      \return The string value.

      \note Warning: It consider that the paramater value is enconded using a Literal OGC Filter expression.
    */
    TEMAPEXPORT std::string GetString(const te::se::ParameterValue* param);

    /*!
      \brief Converts a dasharray pattern coded by a string to a vector of double.

      \param dasharray The dasharray pattern coded by a string.
      \param style     The vector of double that will be filled with the values extracted from dasharray string.

      \note The dasharray should be specified as an even number of positive values separated by blank space. i.e "10 5 3 4"
            Here, the values "10" and "3" (in uom) are dashes and the values "5" and "4" are spaces (in uom).
            The given vector style will be filled with the values: style[0] = 10; style[1] = 5; style[2] = 3; style[3] = 4.
    */
    TEMAPEXPORT void GetDashStyle(const std::string& dasharray, std::vector<double>& style);

  } // end namespace map
}   // end namespace te

#endif // __TERRALIB_MAPTOOLS_INTERNAL_UTILS_H
