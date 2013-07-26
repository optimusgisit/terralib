/*  Copyright (C) 2008-2013 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/postgis/Connection.h

  \brief A class that implements a connection to a database.  
*/

#ifndef __TERRALIB_DATAACCESS_INTERNAL_CONNECTION_H
#define __TERRALIB_DATAACCESS_INTERNAL_CONNECTION_H

// TerraLib
#include "../Config.h"

// STL
#include <string>

// Boost
#include <boost/noncopyable.hpp>

namespace te
{
  namespace da
  {
    /*!
      \class Connection

      \brief A class that implements a connection to a data source.

      This class models a physical connection to a data source.
      It is designed to work with the connection pool.

      \sa AbstractConnectionPool
    */
    class TEDATAACCESSEXPORT Connection : public boost::noncopyable
    {
      public:

        /*! \brief Constructor. */
        Connection();

        /*! \brief Destructor. */
        virtual ~Connection();

        /*!
          \brief It executes the given SQL command and throws away the result.

          \param command Any SQL command.

          \exception Exception It throws an exception if the query execution fails.
        */
        virtual void execute(const std::string& command) = 0;
    };

  } // end namespace da
}   // end namespace te

#endif  // __TERRALIB_DATAACCESS_INTERNAL_CONNECTION_H
