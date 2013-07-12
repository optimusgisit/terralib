/*  Copyright (C) 2010-2013 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/qt/widgets/charts/Histogram.h

  \brief A class to represent a histogram. 
*/

#ifndef __TERRALIB_QT_WIDGETS_INTERNAL_HISTOGRAM_H
#define __TERRALIB_QT_WIDGETS_INTERNAL_HISTOGRAM_H

//TerraLib
#include "../Config.h"
#include "HistogramUtils.h"

//STL
#include <map>
#include <set>
#include <string>
#include <vector>

namespace te
{

  namespace da  { class ObjectId; class ObjectIdSet;}
  namespace dt  { class AbstractData; }

  namespace qt
  {
    namespace widgets
    {
       /*!
        \class Histogram

        \brief A class to represent a Histogram.
      */
      class TEQTWIDGETSEXPORT Histogram 
      {
        public:

          /*!
            \brief Constructor
          */
          Histogram();

          /*! 
            \brief Destructor.
          */
          ~Histogram();

            /*!            
            \brief It returns the histogram's type.  

            \return The histogram's type.  

          */

          int& getType();
      
          /*!            
            \brief It sets the histogram's type. 

            \param new_type The new type 
          */

          void setType(int new_type);

          /*!            
            \brief It returns the map containing the histogram values. 

            \return A  map containing the histogram values. 
          */

          std::map<double, unsigned int> getValues();

          /*!            
            \brief It returns the map containing the histogram String values. 

            \return A  map containing the histogram values. 
          */

          std::map<std::string, unsigned int> getStringValues();

          /*!            
            \brief It returns the histogram's minimum value. 

            \return The histogram's minimum value. 
          */

          double& getMinValue();
      
          /*!            
            \brief It sets the histogram's minimum value. 

            \param new_minValue The new minimum value.
          */

          void setMinValue(double new_minValue);

          /*!            
            \brief It returns the histogram's interval. 

            \return The histogram's interval. 
          */

          double& getInterval();
      
          /*!            
            \brief It sets the histogram's interval. 

            \param new_values The new histogram's interval.
          */

          void setInterval(double new_Interval);

           /*!            
            \brief It returns the histogram's string set of intervals. 

            \return The histogram's interval. 
          */

          std::set <std::string>& getStringInterval();
      
          /*!            
            \brief It sets the histogram's string set of intervals. 

            \param new_values The new histogram's interval.
          */

          void setStringInterval( std::set <std::string> new_Interval);

           /*!            
            \brief It adds a new value to the map containing the histogram values. 

            \param new_value    The value that will be added.
            \param valuesOIds   A vector containing all the ObjectIds contained by the interval

            \note It will take the ownwership of the given pointers
          */

          void insert (std::pair<te::dt::AbstractData*, unsigned int> new_value, std::vector<te::da::ObjectId*> valuesOIds);

           /*!            
            \brief It adds a new value to the map containing the histogram values. 

            \param new_value The value that will be added.
          */

          void insert(std::pair<te::dt::AbstractData*, unsigned int> new_value);

          te::da::ObjectIdSet* find(te::dt::AbstractData* interval);

          const te::dt::AbstractData* find(const te::da::ObjectId* oid);

          void adjustOids(te::dt::AbstractData* interval, std::vector<te::da::ObjectId*> valuesOIds);

        private:

          int                                    m_histogramType;    //!< Histogram's type
          HistogramValues                        m_values;           //!< Histogram's values;
          double                                 m_minValue;         //!< Histogram's minimum numeric value
          double                                 m_interval;         //!< Histogram's numeric interval
          std::set <std::string>                 m_StringIntervals;  //!< Histogram unique strings set, represents string intervals
          IntervalToObjectIdSet                  m_valuesOids;       //!< The intervals and ObjecIds ordered in a boost multi index container
      };
    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_INTERNAL_HISTOGRAM_H