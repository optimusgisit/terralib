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
  \file terralib/st/Globals.cpp
   
  \brief An static class with global definitions for the TerraLib ST module.
*/

// TerraLib
#include "Globals.h"

const std::string te::st::Globals::sm_loaderFromDSIdentifier(TE_ST_DATALOADER_FROM_DS);  //!< The st data loader from data source.
const std::string te::st::Globals::sm_loaderFromMemDSIdentifier(TE_ST_DATALOADER_FROM_MEMDS); //!< The st data loader from in-mem data source.
const std::string te::st::Globals::sm_STMemoryDataSourceId(TE_STMEMORY_DATASOURCE_ID); 

