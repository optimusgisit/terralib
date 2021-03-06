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
  \file terralib/mnt/core/Profile.h

  \brief This file contains a class that represents the profile.

  \reference Adapted from TerraLib4.
*/

#ifndef __TERRALIB_MNT_INTERNAL_PROFILE_H
#define __TERRALIB_MNT_INTERNAL_PROFILE_H

// Terralib Includes
#include "Config.h"
//#include "CreateIsolinesParams.h"

#include "../../../../src/terralib/dataaccess.h"
#include "../../../../src/terralib/dataaccess/datasource/DataSourceFactory.h"
#include "../../../../src/terralib/memory/DataSet.h"
#include "../../../../src/terralib/common/Holder.h"
#include "../../../../src/terralib/statistics/core/Enums.h"

// STL Includes
#include <map>
#include <memory>


namespace te
{
  //forward declarations
  namespace da  { class DataSetType; }
  namespace gm  { class Geometry; }
  namespace map { class AbstractLayer; }
  namespace mem { class DataSet; }
}


namespace te
{
  namespace mnt
  {
    class TEMNTEXPORT Profile
    {
      /*!
      \class CreateIsolines

      \brief Class used to execute the create isolines

      */

    public:

      /*! \brief Default constructor. */
      Profile();

      /*! \brief Virtual destructor. */
      ~Profile();

      /*! \brief Calculate Profile from raster */
      bool runRasterProfile(std::vector<te::gm::LineString*> visadas, std::vector<te::gm::LineString*>& profileSet);

      /*! \brief Calculate Profile from isolines */
      bool runIsolinesProfile(std::vector<te::gm::LineString*> visadas, std::vector<te::gm::LineString*>& profileSet);

      /*! \brief Calculate Profile from TIN */
      bool runTINProfile(std::vector<te::gm::LineString*> visadas, std::vector<te::gm::LineString*>& profileSet);
      
      /*! \brief Reads raster */
      std::auto_ptr<te::rst::Raster> getPrepareRaster();

      /*! \brief Sets input parameters to calculate profile */
      void setInput(te::da::DataSourcePtr inDsrc, std::string inName, std::auto_ptr<te::da::DataSetType> inDsetType, double dummy, std::string zattr);

      /*! \brief Sets SRID */
      void setSRID(int srid) { m_srid = srid; }

    private:
      te::gm::LineString* calculateProfile(std::vector<te::gm::Geometry*> &isolines, te::gm::LineString &trajectory);

      int m_srid;     //!< Attribute with spatial reference information
      double m_dummy; //!< Nodata value
      te::da::DataSourcePtr m_inDsrc; //!< Input Datasource
      std::string m_inName; //!< Input data name
      std::auto_ptr<te::da::DataSetType> m_inDsType; //!< Input datasettype
      std::string m_attrZ;      //!< Z attribute name

    };
  }
}

#endif //__TERRALIB_MNT_INTERNAL_CREATEISOLINES_H