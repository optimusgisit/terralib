/*  Copyright (C) 2009-2013 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/sqlite/FwDataSet.h

  \brief Implementation of a forward-only dataset for the TerraLib SQLite Data Access driver.
*/

#ifndef __TERRALIB_SQLITE_INTERNAL_FWDATASET_H
#define __TERRALIB_SQLITE_INTERNAL_FWDATASET_H

// TerraLib
#include "../dataaccess/dataset/DataSet.h"

// Forward declarations
extern "C"
{
  struct sqlite3_stmt;
  typedef struct sqlite3_stmt sqlite3_stmt;
}

namespace te
{
  namespace sqlite
  {
// Forward declaration
    class DataSourceTransactor;

    class FwDataSet : public te::da::DataSet
    {
      public:

        FwDataSet(sqlite3_stmt* stmt, DataSourceTransactor* t, const bool releaseStmt = true);

        ~FwDataSet();

        te::common::TraverseType getTraverseType() const;

        te::common::AccessPolicy getAccessPolicy() const;

        te::gm::Envelope* getExtent(std::size_t i);

        std::size_t getNumProperties() const;

        int getPropertyDataType(std::size_t i) const;

        std::string getPropertyName(std::size_t i) const;

        std::string getDatasetNameOfProperty(std::size_t i) const;

        bool isEmpty() const;

        std::size_t size() const;

        bool moveNext();

        bool movePrevious();
        
        bool moveFirst();

        bool moveBeforeFirst();

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

        te::dt::ByteArray* getByteArray(std::size_t i) const;

        te::gm::Geometry* getGeometry(std::size_t i) const;

        te::rst::Raster* getRaster(std::size_t i) const;

        te::dt::DateTime* getDateTime(std::size_t i) const; 

        te::dt::Array* getArray(std::size_t i) const;

        bool isNull(std::size_t i) const;

        int getBlobSize(int i) const;

        void getBlob(int i, void* buff) const;

      private:

        sqlite3_stmt* m_stmt;
        DataSourceTransactor* m_t;
        mutable te::da::DataSetType* m_dt;
        bool m_releaseStmt;
    };

  } // end namespace sqlite
}   // end namespace te


#endif  // __TERRALIB_SQLITE_INTERNAL_FWDATASET_H


