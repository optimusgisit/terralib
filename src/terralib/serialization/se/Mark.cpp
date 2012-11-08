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
  \file terralib/serialization/se/Mark.cpp
 
  \brief Support for Mark serialization.
*/

// TerraLib
#include "../../se/Mark.h"
#include "../../xml/Reader.h"
#include "../../xml/Writer.h"
#include "../xlink/SimpleLink.h"
#include "Fill.h"
#include "InlineContent.h"
#include "Mark.h"
#include "Stroke.h"

// STL
#include <cassert>
#include <memory>

te::se::Mark* te::serialize::ReadMark(te::xml::Reader& reader)
{
  return 0;
}

void te::serialize::Save(const te::se::Mark* mark, te::xml::Writer& writer)
{
  if(mark == 0)
    return;

  writer.writeStartElement("Mark");

  const std::string* wellKnownName = mark->getWellKnownName();
  if(wellKnownName)
    writer.writeElement("WellKnownName", *wellKnownName);
  else
  {
    const te::xl::SimpleLink* link = mark->getOnlineResource();
    if(link)
    {
      writer.writeStartElement("OnlineResource");
      Save(link, writer);
      writer.writeEndElement("OnlineResource");
    }
    else
    {
      const te::se::InlineContent* ic = mark->getInlineContent();
      assert(ic);
      Save(ic, writer);

      const std::string* format = mark->getFormat();
      assert(format);
      writer.writeElement("Format", *format);

      writer.writeElement("MarkIndex", mark->getMarkIndex());
    }
  }

  Save(mark->getStroke(), writer);
  Save(mark->getFill(), writer);

  writer.writeEndElement("Mark");
}
