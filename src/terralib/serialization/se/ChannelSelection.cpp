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
  \file terralib/serialization/se/ChannelSelection.cpp
 
  \brief Support for ChannelSelection serialization.
*/

// TerraLib
#include "../../se/ChannelSelection.h"
#include "../../xml/Reader.h"
#include "../../xml/Writer.h"
#include "ChannelSelection.h"
#include "SelectedChannel.h"
#include "Utils.h"

// STL
#include <cassert>
#include <memory>

te::se::ChannelSelection* te::serialize::ReadChannelSelection(te::xml::Reader& reader)
{
  return 0;
}

void te::serialize::Save(const te::se::ChannelSelection* cs, te::xml::Writer& writer)
{
  if(cs == 0)
    return;

  writer.writeStartElement("ChannelSelection");

  WriterSelectedChannelHelper("GrayChannel", cs->getGrayChannel(), writer);
  WriterSelectedChannelHelper("RedChannel", cs->getRedChannel(), writer);
  WriterSelectedChannelHelper("GreenChannel", cs->getGreenChannel(), writer);
  WriterSelectedChannelHelper("BlueChannel", cs->getBlueChannel(), writer);

  writer.writeEndElement("ChannelSelection");
}
