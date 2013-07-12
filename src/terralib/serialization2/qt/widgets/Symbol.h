/*  Copyright (C) 2011-2011 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/serialization/qt/Symbol.h
   
  \brief Support for Symbol serialization.
*/

#ifndef __TERRALIB_SERIALIZATION_QT_WIDGETS_INTERNAL_SYMBOL_H
#define __TERRALIB_SERIALIZATION_QT_WIDGETS_INTERNAL_SYMBOL_H

// TerraLib
#include "../../Config.h"

namespace te
{
  namespace qt
  {
    namespace widgets
    {
      class Symbol;
    }
  }

  namespace xml
  {
    class Reader;
  }

  namespace serialize
  {
    TESERIALIZATIONEXPORT void ReadSymbolLibrary(const std::string& symbolLibraryFileName);

    TESERIALIZATIONEXPORT te::qt::widgets::Symbol* ReadSymbol(te::xml::Reader& reader);

  } // end namespace serialize
}   // end namespace te

#endif  // __TERRALIB_SERIALIZATION_QT_WIDGETS_INTERNAL_SYMBOL_H
