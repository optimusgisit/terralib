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
  \file terralib/rp/MixtureModelStrategyFactory.h

  \brief Raster mixture model strategy factory base class.
 */

#ifndef __TERRALIB_RP_INTERNAL_MIXTUREMODELSTRATEGYFACTORY_H
#define __TERRALIB_RP_INTERNAL_MIXTUREMODELSTRATEGYFACTORY_H

// TerraLib
#include "../common/AbstractFactory.h"
#include "MixtureModelStrategy.h"
#include "Config.h"

// STL
#include <string>

namespace te
{
  namespace rp
  {
    /*!
      \class MixtureModelStrategyFactory

      \brief Raster Mixture model strategy factory base class.
    */
    class TERPEXPORT MixtureModelStrategyFactory : public te::common::AbstractFactory<te::rp::MixtureModelStrategy, std::string, std::less<std::string> >
    {
      public:

        virtual ~MixtureModelStrategyFactory();

      protected:

        MixtureModelStrategyFactory(const std::string& factoryKey);
    };

  } // end namespace rp
}   // end namespace te

#endif  // __TERRALIB_RP_INTERNAL_MIXTUREMODELSTRATEGYFACTORY_H

