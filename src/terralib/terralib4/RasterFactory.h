/*  Copyright (C) 2008-2011 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/terralib4/RasterFactory.h

  \brief This is the concrete factory for TerraLib 4.x rasters.
*/

#ifndef __TERRALIB_TERRALIB4_INTERNAL_RASTERFACTORY_H
#define __TERRALIB_TERRALIB4_INTERNAL_RASTERFACTORY_H

// TerraLib
#include "../raster/RasterFactory.h"

namespace terralib4
{
  class RasterFactory : public te::rst::RasterFactory
  {
    public:

      ~RasterFactory() {}

      const std::string& getType() const;

      void getCreationalParameters(std::vector< std::pair<std::string, std::string> >& params) const;

      static void initialize();

      static void finalize();

      std::map<std::string, std::string> getCapabilities() const;

    protected:

      RasterFactory();

      te::rst::Raster* create(te::rst::Grid* g, const std::vector<te::rst::BandProperty*> bands, const std::map<std::string, std::string>& rinfo, void* h = 0, void (*deleter)(void*) = 0);

      te::rst::Raster* build();

    private:

      static RasterFactory* sm_factory;
  };

}   // end namespace terralib4

#endif  // __TERRALIB_TERRALIB4_INTERNAL_RASTERFACTORY_H

