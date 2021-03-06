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
  \file STDataLoader.h

  \brief  This file contains a class that contains a set of static methods
          to load spatiotemporal data from data sources. 
*/

#ifndef __TERRALIB_ST_INTERNAL_STDATALOADER_H
#define __TERRALIB_ST_INTERNAL_STDATALOADER_H

//TerraLib
#include "../../common/Enums.h"
#include "../../geometry/Enums.h"
#include "../../geometry/Envelope.h"
#include "../../datatype/Enums.h"

//ST
#include "../Config.h"
#include "../core/coverage/CoverageSeriesDataSetInfo.h"

//STL
#include <memory>

//Boost
#include <boost/ptr_container/ptr_vector.hpp>

// Forward declarations
namespace te { namespace gm { class Geometry; } }
namespace te { namespace dt { class DateTime; class DateTimePeriod; } }

namespace te
{
  namespace st
  {    
    // Forward declarations
    class ObservationDataSet;
    class TrajectoryDataSet;
    class TimeSeriesDataSet;
    class ObservationDataSetInfo;
    class TrajectoryDataSetInfo;
    class TimeSeriesDataSetInfo;
    class CoverageSeriesDataSetInfo;
    class PointCoverageDataSetInfo;
    class RasterCoverageDataSetInfo;
    class Trajectory;
    class TimeSeries;
    class CoverageSeries;
    class PointCoverage;
    class AbstractPointCoverageInterp;
    class RasterCoverage;

     /*!
      \class STDataLoader

      \brief  A class that contains a set of static methods to load spatiotemporal 
              data from data sources.
                
      \ingroup st

      \note Before using the methods of this class, certify that the Data Source exists in the DataSourceManager  
      \note The methods of this class can throw an Exception when internal errors occur.
      
      \sa ObservationDataSet ObservationDataSetInfo 
      \sa TrajectoryDataSet TrajectoryDataSetInfo 
      \sa CoverageSeriesDataSet CoverageSeriesDataSetInfo
    */
    class TESTEXPORT STDataLoader
    {
      public:

        /*! \name Operations for loading ObservationDataSet */
        //@{
        
        /*!
          \brief It returns a ObservationDataSet, that is, a DataSet that contains observations. 

          \param info Information about the DataSource which the observation are from and 
                      the DataSet which contains the observations.
          \param travType The traverse type associated to the returned dataset.  
          
          \return A pointer to a new ObservationDataSet. 

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<ObservationDataSet> getDataSet(const ObservationDataSetInfo& info, 
                                 te::common::TraverseType travType = te::common::FORWARDONLY);

        /*!
          \brief It returns a data set with observations whose observed geometries satisfy a
                 given spatial relation. 

          \param info Information about the DataSource which the observation are from and 
                      the DataSet which contains the observations.
          \param e        A given envelope.
          \param r        A given spatial relation.
          \param travType The traverse type associated to the returned dataset.  
          
          \return A pointer to a new ObservationDataSet. 

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<ObservationDataSet> getDataSet(const ObservationDataSetInfo& info, const te::gm::Envelope& e,
                                 te::gm::SpatialRelation r = te::gm::INTERSECTS,
                                 te::common::TraverseType travType = te::common::FORWARDONLY);
        
        /*!
          \brief It returns a data set with observations whose observed geometries satisfy a
                 given spatial relation. 

           The possible spatial relations implemented here are: INTERSECTS

          \param info Information about the DataSource which the observation are from and 
                      the DataSet which contains the observations.
          \param geom     A given geometry.
          \param r        A given spatial relation.
          \param travType The traverse type associated to the returned dataset. 
          
          \return A pointer to a new ObservationDataSet. 

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<ObservationDataSet> getDataSet(const ObservationDataSetInfo& info, const te::gm::Geometry& geom, 
                                 te::gm::SpatialRelation r = te::gm::INTERSECTS,
                                 te::common::TraverseType travType = te::common::FORWARDONLY);
        
        /*!
          \brief It returns a data set with observations whose phenomenon times satisfy a
                 given temporal relation. 

          The possible temporal relations are:
          1. BEFORE; 2. AFTER; 3. DURING;  4. EQUALS;
          5. MEETS; 6. OVERLAPS 7 - STARTS; 8 -FINISHES;

          \param info Information about the DataSource which the observation are from and 
                      the DataSet which contains the observations.
          \param  dt  A given datetime.
          \param  r   A given temporal relation.
          \param travType The traverse type associated to the returned dataset. 
          
          \return A pointer to a new ObservationDataSet. 

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note When the temporal relation is DURING, dt must be a time period.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<ObservationDataSet> getDataSet(const ObservationDataSetInfo& info, 
                                 const te::dt::DateTime& dt, te::dt::TemporalRelation r = te::dt::DURING,
                                 te::common::TraverseType travType = te::common::FORWARDONLY);

         /*!
          \brief It returns a data set with observations whose observed geometries 
                 satisfy a given spatial relation and phenomenon times satisfy a given temporal relation. 

          The possible temporal relations are:
          1. BEFORE; 2. AFTER; 3. DURING;  4. EQUALS;
          5. MEETS; 6. OVERLAPS 7 - STARTS; 8 -FINISHES;

          \param info Information about the DataSource which the observation of trajectpries 
                      are from and the DataSet which contains the observations.
          \param  dt  A given datetime.
          \param  tr  A given temporal relation.
          \param  e   A given envelope.
          \param  sr  A given spatial relation.
          \param travType The traverse type associated to the returned dataset. 
          
          \return A pointer to a new ObservationDataSet. 

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note When the temporal relation is DURING, dt must be a time period.
          \note It can throw an Exception when internal errors occur.
        */

        static std::auto_ptr<ObservationDataSet> getDataSet(const ObservationDataSetInfo& info,
                                                            const te::dt::DateTime& dt, 
                                                            te::dt::TemporalRelation tr,
                                                            const te::gm::Envelope& e, 
                                                            te::gm::SpatialRelation sr = te::gm::INTERSECTS,
                                                            te::common::TraverseType travType = te::common::FORWARDONLY);

        /*!
          \brief It returns a data set with observations whose observed geometries satisfy a
                 given spatial relation and phenomenon times satisfy a given temporal relation. 

          The possible spatial relations are: INTERSECTS
          The possible temporal relations are:
          1. BEFORE; 2. AFTER; 3. DURING;  4. EQUALS;
          5. MEETS; 6. OVERLAPS 7 - STARTS; 8 -FINISHES;

          \param info Information about the DataSource which the observation are from and 
                      the DataSet which contains the observations.
          \param geom A given geometry.
          \param sr   A given spatial relation.
          \param dt   A given datetime.
          \param tr   A given temporal relation.
          \param travType The traverse type associated to the returned dataset. 
          
          \return A pointer to a new ObservationDataSet. 

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It will NOT take the ownership of the given pointers.
          \note When the temporal relation is DURING, dt must be a time period.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<ObservationDataSet> getDataSet(const ObservationDataSetInfo& info, 
                                 const te::gm::Geometry& geom, te::gm::SpatialRelation sr,
                                 const te::dt::DateTime& dt, te::dt::TemporalRelation tr = te::dt::DURING,
                                 te::common::TraverseType travType = te::common::FORWARDONLY);
        //@}

        /*! \name Operations for loading TrajectoryDataSet */
        //@{
        
        /*!
          \brief It returns the information about all trajectories that exist in a single DataSet.
          
          The information about the DataSource and DataSet that contains trajectories is defined
          as a  TrajectoryDataSetInfo (input parameter).

          The information about each trajectory (its correct id) is returned as a 
          TrajectoryDataSetInfo of the parameter output. 
          These results can be used to create TrajectoryDataSetLayers.

          \param input    The information about the DataSource and DataSet that contains trajectories
          \param output   The information about each trajectory (its correct id).
          
          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note It can throw an Exception when internal errors occur.
        */
        static void getInfo( const TrajectoryDataSetInfo& input, 
                             std::vector<TrajectoryDataSetInfo>& output);
        
        /*!
          \brief It returns a data set with observations of trajectories. 

          When the DataSet contains more than one trajectory, the info parameter
          (TrajectoryDataSetInfo) must contain the id of the desire trajectory that
          must be returned as a TrajectoryDataSet.

          \param info Information about the DataSource which the observations of a trajectory 
                      are from and the DataSet which contains these observations.
          \param travType The traverse type associated to the returned datasets. 
          \param rwRole   The read and write permission associated to the returned datasets.
          \return The returned TrajectoryDataSet.
          
          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<TrajectoryDataSet> getDataSet(const TrajectoryDataSetInfo& info, 
                              te::common::TraverseType travType = te::common::FORWARDONLY);

        /*!
          \brief  It returns patches of a trajectory whose geometries 
                  satisfy a given spatial relation. 

          When the DataSet contains more than one trajectory, the info parameter
          (TrajectoryDataSetInfo) must contain the id of the desire trajectory that
          must be returned as a TrajectoryDataSet.

          The possible spatial relations implemented here are: INTERSECTS

          \param info Information about the DataSource which the observations of a trajectory 
                      are from and the DataSet which contains these observations.
          \param geom A given geometry.
          \param r    A given spatial relation.
          \param travType The traverse type associated to the returned dataset. 

          \return The returned trajectoy data set.

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<TrajectoryDataSet> getDataSet(const TrajectoryDataSetInfo& info,  
                             const te::gm::Geometry& geom, te::gm::SpatialRelation r,
                             te::common::TraverseType travType = te::common::FORWARDONLY);

        /*!
          \brief  It returns patches of a trajectory whose geometries 
                  satisfy a given spatial relation. 

          When the DataSet contains more than one trajectory, the info parameter
          (TrajectoryDataSetInfo) must contain the id of the desire trajectory that
          must be returned as a TrajectoryDataSet.

          \param info Information about the DataSource which the observations of a trajectory are from 
                      and the DataSet which contains the observations.
          \param e        A given envelope.
          \param r        A given spatial relation.
          \param travType The traverse type associated to the returned dataset.  

          \return The returned trajectoy data set.
          
          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<TrajectoryDataSet> getDataSet(const TrajectoryDataSetInfo& info, 
                                const te::gm::Envelope& e, te::gm::SpatialRelation r,               
                                te::common::TraverseType travType = te::common::FORWARDONLY);

         /*!
          \brief It returns a data set with observations of trajectories whose times 
                satisfy a given temporal relation. 

          When the DataSet contains more than one trajectory, the info parameter
          (TrajectoryDataSetInfo) must contain the id of the desire trajectory that
          must be returned as a TrajectoryDataSet.

          The possible temporal relations are:
          1. BEFORE; 2. AFTER; 3. DURING;  4. EQUALS;
          5. MEETS; 6. OVERLAPS 7 - STARTS; 8 -FINISHES;

          \param info Information about the DataSource which the observation of trajectpries 
                      are from and the DataSet which contains the observations.
          \param  dt  A given datetime.
          \param  r   A given temporal relation.
          \param travType The traverse type associated to the returned dataset. 
          
          \return A pointer to a new TrajectoryDataSet. 

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note When the temporal relation is DURING, dt must be a time period.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<TrajectoryDataSet> getDataSet(const TrajectoryDataSetInfo& info, 
                                 const te::dt::DateTime& dt, te::dt::TemporalRelation r = te::dt::DURING,
                                 te::common::TraverseType travType = te::common::FORWARDONLY);

         /*!
          \brief It returns patches of a trajectory whose envelope 
                 satisfy a given spatial relation and times satisfy a given temporal relation. 

          When the DataSet contains more than one trajectory, the info parameter
          (TrajectoryDataSetInfo) must contain the id of the desire trajectory that
          must be returned as a TrajectoryDataSet.

          The possible temporal relations are:
          1. BEFORE; 2. AFTER; 3. DURING;  4. EQUALS;
          5. MEETS; 6. OVERLAPS 7 - STARTS; 8 -FINISHES;

          \param info Information about the DataSource which the observation of trajectpries 
                      are from and the DataSet which contains the observations.
          \param  dt  A given datetime.
          \param  tr  A given temporal relation.
          \param  e   A given envelope.
          \param  sr  A given spatial relation.
          \param travType The traverse type associated to the returned dataset. 
          
          \return A pointer to a new TrajectoryDataSet. 

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note When the temporal relation is DURING, dt must be a time period.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<TrajectoryDataSet> getDataSet(const TrajectoryDataSetInfo& info,
                                                           const te::dt::DateTime& dt, 
                                                           te::dt::TemporalRelation tr,
                                                           const te::gm::Envelope& e, 
                                                           te::gm::SpatialRelation sr = te::gm::INTERSECTS,
                                                           te::common::TraverseType travType = te::common::FORWARDONLY);

        /*!
          \brief It returns patches of a trajectory whose geometries 
                  satisfy a given spatial relation and times satisfy a given temporal relation. 
 
          When the DataSet contains more than one trajectory, the info parameter
          (TrajectoryDataSetInfo) must contain the id of the desire trajectory that
          must be returned as a TrajectoryDataSet.

          The possible spatial relations are: INTERSECTS
          The possible temporal relations are:
          1. BEFORE; 2. AFTER; 3. DURING;  4. EQUALS;
          5. MEETS; 6. OVERLAPS 7 - STARTS; 8 -FINISHES;

          \param info Information about the DataSource which the observations of trajectories 
                      are from and the DataSet which contains these observations.
          \param geom A given geometry.
          \param sr   A given spatial relation.
          \param dt   A given datetime.
          \param tr   A given temporal relation.
          \param travType The traverse type associated to the returned dataset. 

          \return The returned trajectoy data set.
          
          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It will NOT take the ownership of the given pointers.
          \note When the temporal relation is DURING, dt must be a time period.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<TrajectoryDataSet> getDataSet(const TrajectoryDataSetInfo& info, 
                                 const te::gm::Geometry& geom, te::gm::SpatialRelation sr,
                                 const te::dt::DateTime& dt, te::dt::TemporalRelation tr,
                                 te::common::TraverseType travType = te::common::FORWARDONLY);
        //@}

        /*! \name Operations for loading TimeSeriesDataSet */
        //@{
        
        /*!
          \brief It returns the information about all time series that exist in a single DataSet.
          
          The information about the DataSource and DataSet that contains time series is defined
          as a TimeSeriesDataSetInfo (input parameter).

          The information about each time series (its correct id) is returned as a 
          TimeSeriesDataSetInfo of the parameter output. 
          These results can be used to create TimeSeriesDataSetLayers.

          \param input    The information about the DataSource and DataSet that contains time series
          \param output   The information about each time series (its correct id).
          
          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note It can throw an Exception when internal errors occur.
        */
        static void getInfo( const TimeSeriesDataSetInfo& input, 
                              std::vector<TimeSeriesDataSetInfo>& output);
        
        /*!
          \brief It returns a data set with observations of time series. 

          When the DataSet contains more than one time series, the info parameter
          (TimeSeriesDataSetInfo) must contain the id of the desire time series that
          must be returned as a TimeSeriesDataSet.

          \param info Information about the DataSource which the observations of a time series 
                      are from and the DataSet which contains these observations.
          \param travType The traverse type associated to the returned datasets.

          \return The returned TimeSeriesDataSet.
          
          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<TimeSeriesDataSet> getDataSet(const TimeSeriesDataSetInfo& info, 
                              te::common::TraverseType travType = te::common::FORWARDONLY);

         /*!
          \brief It returns a data set with observations of time series whose times 
                satisfy a given temporal relation. 

          When the DataSet contains more than one time series, the info parameter
          (TimeSeriesDataSetInfo) must contain the id of the desire time series that
          must be returned as a TimeSeriesDataSet.

          The possible temporal relations are:
          1. BEFORE; 2. AFTER; 3. DURING;  4. EQUALS;
          5. MEETS; 6. OVERLAPS 7 - STARTS; 8 -FINISHES;

          \param info Information about the DataSource which the observation of time series 
                      are from and the DataSet which contains the observations.
          \param  dt  A given datetime.
          \param  r   A given temporal relation.
          \param travType The traverse type associated to the returned dataset.
          
          \return A pointer to a new TimeSeriesDataSet. 

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note When the temporal relation is DURING, dt must be a time period.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<TimeSeriesDataSet> getDataSet(const TimeSeriesDataSetInfo& info, 
                                 const te::dt::DateTime& dt, te::dt::TemporalRelation r = te::dt::DURING,
                                 te::common::TraverseType travType = te::common::FORWARDONLY);

        /*!
          \brief  It returns patches of a timeseries whose geometries 
                  satisfy a given spatial relation. 

          When the DataSet contains more than one trajectory, the info parameter
          (TrajectoryDataSetInfo) must contain the id of the desire trajectory that
          must be returned as a TrajectoryDataSet.

          \param info Information about the DataSource which the observations of a trajectory are from 
                      and the DataSet which contains the observations.
          \param e        A given envelope.
          \param r        A given spatial relation.
          \param travType The traverse type associated to the returned dataset.  

          \return The returned trajectoy data set.
          
          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<TimeSeriesDataSet> getDataSet(const TimeSeriesDataSetInfo& info, const te::gm::Envelope& e,
                                 te::gm::SpatialRelation r = te::gm::INTERSECTS,
                                 te::common::TraverseType travType = te::common::FORWARDONLY);

        /*!
          \brief  It returns data set with observations of time series whose geometries 
                  satisfy a given spatial relation. 

          \param info Information about the DataSource which the observations of a time series are from 
                      and the DataSet which contains the observations.
          \param geom     A given geometry.
          \param r        A given spatial relation.
          \param travType The traverse type associated to the returned dataset.  

          \return The returned time series data set.
          
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<TimeSeriesDataSet> getDataSet(const TimeSeriesDataSetInfo& info, const te::gm::Geometry& geom, 
                                 te::gm::SpatialRelation r = te::gm::INTERSECTS,
                                 te::common::TraverseType travType = te::common::FORWARDONLY);

        /*!
          \brief  It returns data set with observations of time series whose geometries 
                  satisfy a given spatial relation and times satisfy a given temporal relation 

          \param info Information about the DataSource which the observations of a time series are from 
                      and the DataSet which contains the observations.
          \param  dt  A given datetime.
          \param  tr  A given temporal relation.
          \param  e   A given envelope.
          \param  sr  A given spatial relation.
          \param travType The traverse type associated to the returned dataset.

          \return The returned time series data set.
          
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<TimeSeriesDataSet> getDataSet(const TimeSeriesDataSetInfo& info,
                                                    const te::dt::DateTime& dt, 
                                                    te::dt::TemporalRelation tr,
                                                    const te::gm::Envelope& e, 
                                                    te::gm::SpatialRelation sr = te::gm::INTERSECTS,
                                                    te::common::TraverseType travType = te::common::FORWARDONLY);

        /*!
          \brief  It returns data set with observations of time series whose geometries 
                  satisfy a given spatial relation and times satisfy a given temporal relation. 

          \param info Information about the DataSource which the observations of a time series are from 
                      and the DataSet which contains the observations.
          \param e        A given envelope.
          \param r        A given spatial relation.
          \param travType The traverse type associated to the returned dataset.  

          \return The returned time series data set.
          
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<TimeSeriesDataSet> getDataSet(const TimeSeriesDataSetInfo& info, 
                                 const te::gm::Geometry& geom, te::gm::SpatialRelation sr,
                                 const te::dt::DateTime& dt, te::dt::TemporalRelation tr,
                                 te::common::TraverseType travType = te::common::FORWARDONLY);

        //@}
        
        /*! \name Operations for loading spatiotemporal data types
                  (TimeSeries, Trajetory, CoverageSeries) directly from data sources, without using
                  DataSets. */
        //@{
        
        /*!
          \brief It returns a point coverage series. 

          \param info Information about the DataSource which the observations of a coverage series 
                      are from and the data sets which contain these observations.
          \param interp The interpolator associated to each  

          \return The returned CoverageSeries.
          
          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<CoverageSeries> getCoverageSeries( const PointCoverageSeriesDataSetInfo& info,
                                                                AbstractPointCoverageInterp* interp);  

        /*!
          \brief It returns a raster coverage series. 

          \param info Information about the DataSource which the observations of a coverage series 
                      are from and the data sets which contain these observations.

          \return The returned CoverageSeries.
          
          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<CoverageSeries> getCoverageSeries(const RasterCoverageSeriesDataSetInfo& info);

        /*!
          \brief It returns a raster coverage. 

          \param info Information about the DataSource which the observations of a raster coverage  
                      are from and the data sets which contain these observations.

          \return The returned RasterCoverage.
          
          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<RasterCoverage> getCoverage(const RasterCoverageDataSetInfo& info); 

        /*!
          \brief It returns a point coverage. 

          \param info     Information about the DataSource which the observations of a point coverage  
                          are from and the data sets which contain these observations.
          \param interp   The interpolator associated to the point coverage

          \return The returned PointCoverage.
          
          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It will take the ownership of the given pointers.
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<PointCoverage> getCoverage(const PointCoverageDataSetInfo& info, 
                                                        AbstractPointCoverageInterp* interp);

        /*!
          \brief It returns a time series associated to a point of a coverage series.

          If info contains information about raster coverages and its associated data source 
          supports raster data type, this function is executed by the data source.
          Otherwise, it is executed in two steps: (1) creating a CoverageSeries type
          and (2) using its method getTimeSeries.  

          \param info Information about the DataSource which the observations of a coverage series 
                      are from and the data sets which contain these observations.
          \param p  A given point.

          \return The returned CoverageSeries.
          
          \note The caller will take the ownership of the returned pointer.
          \note It can throw an Exception when internal errors occur.

          \\TO DO: (1) vantagem de usar o DataSetCoverageDataSet � ele fica armazenado no driver
          \\em mem�ria (stmem) ao inv�s de toda hora gerar o CoverageSeries para cada ponto
          \\que queremos extrair a time series. 
        */
        //static std::auto_ptr<CoverageSeries> getTimeSeries(const CoverageSeriesDataSetInfo& info, 
        //            const te::gm::Point& p); 
        //@}
        
        /*! \name Operations for loading temporal and spatial extent */
        //@{
        
        /*!
          \brief It returns the temporal extent of the data set with observations.

          \return The temporal extent of the observations.

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It will NOT take the ownership of the given pointer "info".
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<te::dt::DateTimePeriod> getTemporalExtent(const ObservationDataSetInfo& info);

        /*!
          \brief It returns the temporal extent of the data set with observations of a trajectory.

          \return The temporal extent of the observations of a trajectory.

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It will NOT take the ownership of the given pointer "info".
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<te::dt::DateTimePeriod> getTemporalExtent(const TrajectoryDataSetInfo& info);

        /*!
          \brief It returns the temporal extent of the data set with observations of a time series.

          \return The temporal extent of the observations of a time series.

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It will NOT take the ownership of the given pointer "info".
          \note It can throw an Exception when internal errors occur.
        */
        static std::auto_ptr<te::dt::DateTimePeriod> getTemporalExtent(const TimeSeriesDataSetInfo& info);

        /*!
          \brief It returns the temporal extent of the data set with observations of a coverage series.

          \return The temporal extent of the observations of a coverage series.

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer.
          \note It will NOT take the ownership of the given pointer "info".
          \note It can throw an Exception when internal errors occur.
        */
        //static std::auto_ptr<te::dt::DateTimePeriod> getTemporalExtent(const CoverageSeriesDataSetInfo& info);

        /*!
          \brief It returns the spatial extent of the observations, when there is an observed value of geometry type.

          \return The spatial extent of the observations.

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer. 
          \note It can throw an Exception when internal errors occur.
        */
        static te::gm::Envelope getSpatialExtent(const ObservationDataSetInfo& info);

        /*!
          \brief It returns the spatial extent of the observations of a trajectory. 

          \return The spatial extent of the observations of a trajectory.

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer. 
          \note It can throw an Exception when internal errors occur.
        */
        static te::gm::Envelope getSpatialExtent(const TrajectoryDataSetInfo& info);

        /*!
          \brief It returns the spatial extent of the observations of a coverage series. 

          \return The spatial extent of the observations of a coverage series.

          \note Before using this method, certify that the Data Source exists in the DataSourceManager and 
                its "id" is correct in the info parameter  
          \note The caller will take the ownership of the returned pointer. 
          \note It can throw an Exception when internal errors occur.
        */
        //static te::gm::Envelope getSpatialExtent(const CoverageSeriesDataSetInfo& info);
        //@}
        
        /*!
          \brief It initialize the STDataLoader

          It creates a ST Memory data source and adds it into the DataSourceManager.

          \note The user must use this method before using the other methods.
          \note It can throw an Exception when internal errors occur.
        */
        static void initialize();

        /*!
          \brief It finalize the STDataLoader

          It closes (cleans its iternal DataSets) ST Memory data source in the DataSourceManager.

          \note The user must use this method after using the STDataLoader.
          \note It can throw an Exception when internal errors occur.
        */
        static void finalize();
                        
        /*! \brief Virtual destructor. */
        virtual ~STDataLoader();
      
      public:
        static bool sm_STDataLoaderInitialized;     //!< Indictes if the st data loader support was initialized

      protected:

        /*! \brief Protected constructor */
        STDataLoader();
    };

  } // end namespace st
}   // end namespace te

#endif  // __TERRALIB_ST_INTERNAL_STDATALOADER_H



