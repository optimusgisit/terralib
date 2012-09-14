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
  \file WKTActions.h

  \brief A class that implements the semanthic actions to grammar rules for well known text (WKT) format for Spatial Reference Systems.
         Basically, the class is responsible to generate a SRS element.

  \warning Do not use this class. It is for TerraLib internal use. Try the te::srs::WKTReader instead.
 */

#ifndef __TERRALIB_SRS_INTERNAL_WKTACTIONS_H
#define __TERRALIB_SRS_INTERNAL_WKTACTIONS_H

// STL
#include <map>
#include <string>
#include <vector>

namespace te
{
  namespace srs
  {
// Forward declarations
    class SpatialReferenceSystem;
    class Datum;
    class Ellipsoid;
    class GeographicCoordinateSystem;
    class ProjectedCoordinateSystem;

    /*!
      \class WKTActions
      
      \brief A class that implements the Grammar Rules for well known text (WKT) format for Spatial Reference Systems.
     */
    class WKTActions
    {
      public:

        /** @name Constructor
         *  Initilizer methods.
         */
        //@{

        /*! \brief Default constructor. */
        WKTActions();

        /*! \brief Destructor. */
        ~WKTActions();

        //@}

        /*! 
          \brief This method resets the Action class to original state.

          \note Should be called case the parser processing faill.
          \note Basically, it is responsable to free the memory.
         */
        void reset();

        /** @name Actions methods.
          * Actions methods to WKT SRS parser.
          */
        //@{

        void createProjectedCoordinateSystem();
        void createGeographicCoordinateSystem();
        void createDatum();
        void createSpheroid();

        void setName(const std::string& name);

        void setPrimeMeridian(const double& primem);
        
        void setUnitName(const std::string& name);
        void setConversionFactor(const double& f);

        void setDatumName(const std::string& name);

        void setDx(const double& dx);
        void setDy(const double& dy);
        void setDz(const double& dz);
        void setEx(const double& ex);
        void setEy(const double& ey);
        void setEz(const double& ez);
        void setPPM(const double& ppm);
        void endToWGS84();

        void setAuthorityName(const std::string& name);
        void setAuthorityCode(const std::string& code);

        void setSpheroidName(const std::string& name);
        void setSemiMajorAxis(const double& rad);
        void setInverseFlattening(const double& invflat);

        void setProjectionName(const std::string& name);

        void setParameter(const std::string& name);
        void setParameterValue(const double& value);

        void setAxisName(const std::string& name);
        void setAxisValue(const std::string& value);

        void endSpheroid();
        void endDatum();
        void endGeographicCoordinateSystem();
        void endProjectedCoordinateSystem();

        //@}

        /** @name Access method.
        * Method to access the SRS generated.
        */
        //@{

        /*!
          \brief It returns the SRS generated by the parser process.

          \note The caller of this method will take the ownership of the SRS.
         */
        SpatialReferenceSystem* getSRS();

        //@}

      private:

        SpatialReferenceSystem* m_srs;                //<! Result SRS generated by the parser process.

        ProjectedCoordinateSystem*  m_projCS;         //<! Projected Coordinate System that can be generated during the parser process.
        GeographicCoordinateSystem* m_geoCS;          //<! Geographic Coordinate System that can be generated during the parser process.
        
        Datum*     m_datum;                           //<! Datum generated during the parser process.
        Ellipsoid* m_ellps;                           //<! Ellipsoid generated during the parser process.

        std::vector<double> m_sevenParams;            //<! toWGS84 seven parameters that can be read during the parser process.

        std::pair<std::string, double> m_parameter;   //<! A pair that represents a projection parameter.
        std::map<std::string, double>  m_params;      //<! The set of projection parameters.

        std::pair<std::string, std::string> m_axis;   //<! A pair that represents a axis.
        std::map<std::string, std::string>  m_axes;   //<! The set of axes.

        std::string m_authorityName;                  //<! Authority names that can be read during the parser process.
        std::string m_authorityCode;                  //<! Authority codes that can be read during the parser process.
    };

  } // namespace srs
}   // namespace te

#endif  // __TERRALIB_SRS_INTERNAL_WKTACTIONS_H
