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
  \file terralib/sqlite/SQLVisitor.h

  \brief A visitor for building an SQL statement using SQLite dialect.
*/

#ifndef __TERRALIB_SQLITE_INTERNAL_SQLVISITOR_H
#define __TERRALIB_SQLITE_INTERNAL_SQLVISITOR_H

// TerraLib
#include "../dataaccess/query/SQLVisitor.h"

// Boost
#include <boost/noncopyable.hpp>

namespace te
{
  namespace sqlite
  {
    /*!
      \class SQLVisitor
      
      \brief A visitor for building an SQL statement using SQLite dialect.
    */
    class SQLVisitor : public te::da::SQLVisitor
    {
      public:

        /*! \brief Default constructor. */
        SQLVisitor(const te::da::SQLDialect& dialect, std::string& sql);

        /*! \brief Destructor. */
        ~SQLVisitor() {}

        void visit(const te::da::LiteralByteArray& visited);
        void visit(const te::da::LiteralDateTime& visited);
        void visit(const te::da::LiteralEnvelope& visited);
        void visit(const te::da::LiteralGeom& visited);
    };

  } // end namespace sqlite
}   // end namespace te


#endif  // __TERRALIB_SQLITE_INTERNAL_SQLVISITOR_H

