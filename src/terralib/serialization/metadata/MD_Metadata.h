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
  \file terralib/serialization/metadata/MD_Metadata.h
   
  \brief Auxiliary classes and functions to serialize metadata from a XML document.
*/

#ifndef __TERRALIB_SERIALIZATION_METADADATA_INTERNAL_MD_METADATA_H
#define __TERRALIB_SERIALIZATION_METADADATA_INTERNAL_MD_METADATA_H

// TerraLib
#include "../Config.h"

// STL
#include <map>
#include <string>

// Boost
#include <boost/function.hpp>

namespace te
{
  namespace md 
  { 
    class MD_Metadata; 
    class CI_ResponsibleParty; 
  }

  namespace xml
  {
    class Reader;
    class Writer;
  }

  namespace serialize
  {
//    TESERIALIZATIONEXPORT te::md::MD_Metadata* ReadMetadataa(te::xml::Reader& reader);
    
    TESERIALIZATIONEXPORT void Save(const te::md::MD_Metadata* md, te::xml::Writer& writer);
    
    TESERIALIZATIONEXPORT void Save(const te::md::CI_ResponsibleParty& ctc, te::xml::Writer& writer);
    
    TESERIALIZATIONEXPORT void Save(const te::md::MD_Identification& id, te::xml::Writer& writer);
    
    TESERIALIZATIONEXPORT void Save(const te::md::CI_Citation* cite, te::xml::Writer& writer);
    
  } // end namespace serialize
}   // end namespace te

#endif  // __TERRALIB_SERIALIZATION_METADADATA_INTERNAL_MD_METADATA_H

