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
  \file terralib/ogr/DataSet.h

  \brief Implementation of a DataSet for OGR data provider.
*/

#ifndef __TERRALIB_OGR_INTERNAL_DATASET_H
#define __TERRALIB_OGR_INTERNAL_DATASET_H

#include "Config.h"

// TerraLib
#include "../dataaccess/dataset/DataSet.h"

// STL
#include <vector>

// Forward declarations
class OGRLayer;
class OGRFeature;
class OGRDataSource;

namespace te
{
// Forward declaration
  namespace da
  {
    class DataSetType;
  }

  namespace ogr
  {
    /*!
      \class DataSet

      \brief Implementation of a DataSet for OGR data provider.

      \sa Transactor, te::da::DataSet
     */
    class TEOGREXPORT DataSet : public te::da::DataSet
    {
      public:

        /** @name Constructor/Destructor
         *  Initilizer methods.
         */
        //@{

        /*! \brief Constructor. */
        DataSet(OGRDataSource* dsrc, OGRLayer* layer);

        /*! \brief Destructor. */
        ~DataSet();

        //@}

        te::common::TraverseType getTraverseType() const { return te::common::FORWARDONLY; }

        te::common::AccessPolicy getAccessPolicy() const { return te::common::RAccess; }

        std::size_t getNumProperties() const;

        int getPropertyDataType(std::size_t pos) const;

        std::string getPropertyName(std::size_t pos) const;

        te::common::CharEncoding getPropertyCharEncoding(std::size_t i) const;

        std::string getDatasetNameOfProperty(std::size_t pos) const;

        bool isEmpty() const;

        bool isConnected() const { return true; }

        std::size_t size() const;

        std::auto_ptr<te::gm::Envelope> getExtent(std::size_t i);

        bool moveNext();

        bool movePrevious();

        bool moveBeforeFirst();

        bool moveFirst();

        bool moveLast();

        bool move(std::size_t i);

        bool isAtBegin() const;

        bool isBeforeBegin() const;

        bool isAtEnd() const;

        bool isAfterEnd() const;

        char getChar(std::size_t i) const;

        unsigned char getUChar(std::size_t i) const;

        boost::int16_t getInt16(std::size_t i) const;

        boost::int32_t getInt32(std::size_t i) const;

        boost::int64_t getInt64(std::size_t i) const;

        bool getBool(std::size_t i) const;

        float getFloat(std::size_t i) const;

        double getDouble(std::size_t i) const;

        std::string getNumeric(std::size_t i) const;

        std::string getString(std::size_t i) const;

        std::auto_ptr<te::dt::ByteArray> getByteArray(std::size_t i) const;

        std::auto_ptr<te::gm::Geometry> getGeometry(std::size_t i) const;

        std::auto_ptr<te::rst::Raster> getRaster(std::size_t i) const;

        std::auto_ptr<te::dt::DateTime> getDateTime(std::size_t i) const;

        std::auto_ptr<te::dt::Array> getArray(std::size_t i) const;

        bool isNull(std::size_t i) const;
      
      private:

        const unsigned char* getWKB() const;

      private:

        mutable te::da::DataSetType* m_dt;    //!< DataSetType.

        OGRDataSource* m_ogrDs;               //<! Pointer to OGR data source. 
        OGRLayer* m_layer;                    //<! A pointer to OGR Layer.
        OGRFeature* m_currentFeature;         //<! A pointer to current OGR Feature of layer.
        int m_i;                              //<! The current dataset index.

        mutable unsigned char* m_wkbArray;    //<! The array that stores the wkb generated by OGR Library.
        mutable int m_wkbArraySize;           //<! The size of wkb array.

        int m_srid;                           //!< The SRS id associated to this dataset.
    };

  } // end namespace ogr
}   // end namespace te

#endif  // __TERRALIB_OGR_INTERNAL_DATASET_H
