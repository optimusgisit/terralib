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
  \file terralib/dataaccess/Module.h
   
  \brief This singleton defines the TerraLib Data Access module entry.
*/

#ifndef __TERRALIB_DATAACCESS_INTERNAL_MODULE_H
#define __TERRALIB_DATAACCESS_INTERNAL_MODULE_H

// TerraLib
#include "../common/Singleton.h"

namespace te
{
  namespace da
  {
    /*!
      \class Module

      \brief This singleton defines the TerraLib Data Access module entry.
    */
    class Module : public te::common::Singleton<Module>
    {
      friend class te::common::Singleton<Module>;

      protected:

        /*! \brief The singleton constructor is not callable outside the class. */
        Module();

        /* \brief Destructor. */
        ~Module();

      private:

        /*!
          \brief This is the startup function for the TerraLib Data Access module.

          \note This method can initialize the pool manager if automatic initialization is on.
        */
        static void initialize();

        /*!
          \brief This is the cleanup function for the TerraLib Data Access module.

          \note This method can finalize the pool manager if automatic initialization is on.
        */
        static void finalize();

      private:

        static const Module& sm_module; //!< Just to make a static initialization.
    };

  } // end namespace da
}   // end namespace te

#endif  // __TERRALIB_DATAACCESS_INTERNAL_MODULE_H

