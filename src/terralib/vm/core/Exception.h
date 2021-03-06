/*
  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/vm/core/Exception.h

  \brief Exception classes for the TerraLib Virtual Machine Library.

  \author Frederico Augusto Bedê
  \author Gilberto Ribeiro de Queiroz
 */

#ifndef __TERRALIB_VM_CORE_EXCEPTION_H__
#define __TERRALIB_VM_CORE_EXCEPTION_H__

// TerraLib
#include "../../Exception.h"

namespace te
{
  namespace vm
  {
    namespace core
    {
      //! Base exception class for TerraLib Virtual Machine Library.
      struct Exception: virtual te::Exception { };
      
      //! An exception that tells that the virtual machine could not be created.
      struct CreationException: virtual Exception { };
      
      //! An exception that tells that the virtual machine could not build a script.
      struct BuildException: virtual Exception { };
      
      //! An exception that tells that the virtual machine could not run compiled script.
      struct RunException: virtual Exception { };

    }
  }  // end namespace core
}    // end namespace te

#endif  // __TERRALIB_VM_CORE_EXCEPTION_H__
