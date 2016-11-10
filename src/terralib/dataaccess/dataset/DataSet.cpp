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
  \file terralib/dataaccess/dataset/DataSet.cpp

  \brief A dataset is the unit of information manipulated by the data access module of TerraLib.
*/

// TerraLib
#include "../../core/utils/HexUtils.h"
#include "../../common/StringUtils.h"
#include "../../datatype/AbstractData.h"
#include "../../datatype/Array.h"
#include "../../datatype/ByteArray.h"
#include "../../datatype/DateTime.h"
#include "../../datatype/Property.h"
#include "../../datatype/SimpleData.h"
#include "../../raster/Raster.h"
#include "../utils/Utils.h"
#include "DataSet.h"
#include "DataSetType.h"

// STL
#include <memory>

// Boost
#include <boost/lexical_cast.hpp>

char te::da::DataSet::getChar(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getChar(i);
}

unsigned char te::da::DataSet::getUChar(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getUChar(i);
}

boost::int16_t te::da::DataSet::getInt16(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getInt16(i);
}

boost::int32_t te::da::DataSet::getInt32(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getInt32(i);
}

boost::int64_t te::da::DataSet::getInt64(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getInt64(i);
}

bool te::da::DataSet::getBool(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getBool(i);
}

float te::da::DataSet::getFloat(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getFloat(i);
}

double te::da::DataSet::getDouble(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getDouble(i);
}

std::string te::da::DataSet::getNumeric(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getNumeric(i);
}

std::string te::da::DataSet::getString(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getString(i);
}

std::auto_ptr<te::dt::ByteArray> te::da::DataSet::getByteArray(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getByteArray(i);
}

std::auto_ptr<te::gm::Geometry> te::da::DataSet::getGeometry(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getGeometry(i);
}

std::auto_ptr<te::rst::Raster> te::da::DataSet::getRaster(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getRaster(i);
}

std::auto_ptr<te::dt::DateTime> te::da::DataSet::getDateTime(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getDateTime(i);
}

std::auto_ptr<te::dt::Array> te::da::DataSet::getArray(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getArray(i);
}

std::auto_ptr<te::dt::AbstractData> te::da::DataSet::getValue(std::size_t i) const
{
  switch(getPropertyDataType(i))
  {
    case te::dt::CHAR_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(new te::dt::Char(getChar(i)));

    case te::dt::UCHAR_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(new te::dt::UChar(getUChar(i)));

    case te::dt::INT16_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(new te::dt::Int16(getInt16(i)));

    case te::dt::UINT16_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(new te::dt::UInt16(getInt16(i)));

    case te::dt::INT32_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(new te::dt::Int32(getInt32(i)));

    case te::dt::UINT32_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(new te::dt::UInt32(getInt32(i)));

    case te::dt::INT64_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(new te::dt::Int64(getInt64(i)));

    case te::dt::UINT64_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(new te::dt::UInt64(getInt64(i)));

    case te::dt::BOOLEAN_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(new te::dt::Boolean(getBool(i)));

    case te::dt::FLOAT_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(new te::dt::Float(getFloat(i)));

    case te::dt::DOUBLE_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(new te::dt::Double(getDouble(i)));

    case te::dt::NUMERIC_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(new te::dt::Numeric(getNumeric(i)));

    case te::dt::STRING_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(new te::dt::String(getString(i)));

    case te::dt::BYTE_ARRAY_TYPE:
       return std::auto_ptr<te::dt::AbstractData>(getByteArray(i));

    case te::dt::GEOMETRY_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(getGeometry(i));

    case te::dt::DATETIME_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(getDateTime(i));

    case te::dt::RASTER_TYPE:
      return std::auto_ptr<te::dt::AbstractData>(getRaster(i));

    default:
      return std::auto_ptr<te::dt::AbstractData>(0);
  }
}

std::auto_ptr<te::dt::AbstractData> te::da::DataSet::getValue(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getValue(i);
}

std::string te::da::DataSet::getAsString(std::size_t i, int precision) const
{
  std::string value;

  switch(getPropertyDataType(i))
  {
    case te::dt::CHAR_TYPE:
      value = getChar(i);
    break;

    case te::dt::UCHAR_TYPE:
      value = getUChar(i);
    break;

    case te::dt::INT16_TYPE:
      value = boost::lexical_cast<std::string>(getInt16(i));
    break;

    case te::dt::INT32_TYPE:
      value = boost::lexical_cast<std::string>(getInt32(i));
    break;

    case te::dt::INT64_TYPE:
      value = boost::lexical_cast<std::string>(getInt64(i));
    break;

    case te::dt::BOOLEAN_TYPE:
      value = boost::lexical_cast<std::string>(getBool(i));
    break;

    case te::dt::FLOAT_TYPE:
      value = boost::lexical_cast<std::string>(getFloat(i));
    break;

    case te::dt::DOUBLE_TYPE:
      value = te::common::Convert2String(getDouble(i), precision);
    break;

    case te::dt::NUMERIC_TYPE:
      value = getNumeric(i);
    break;

    case te::dt::STRING_TYPE:
      value = getString(i);
    break;

    case te::dt::BYTE_ARRAY_TYPE:
      {
        std::auto_ptr<te::dt::AbstractData> b(getByteArray(i));
        value = b->toString();
      }
    break;

    case te::dt::GEOMETRY_TYPE:
      {
        std::auto_ptr<te::dt::AbstractData> g(getGeometry(i));
        value = g->toString();
      }
    break;

    case te::dt::DATETIME_TYPE:
      {
        std::auto_ptr<te::dt::AbstractData> dTime(getDateTime(i));
        value = dTime->toString();
      }
    break;

    case te::dt::ARRAY_TYPE:
      {
        std::auto_ptr<te::dt::Array> a(getArray(i));
        value = a->toString();
      }
    break;

    case te::dt::RASTER_TYPE:
      {
        std::auto_ptr<te::dt::AbstractData> r(getRaster(i));
        value = r->toString();
      }
    break;

    default:
    break;
  }

  return value;
}

std::string te::da::DataSet::getAsString(const std::string& name, int precision) const
{
  std::size_t i = GetPropertyPos(this, name);

  return getAsString(i, precision);
}

bool te::da::DataSet::isNull(const std::string& name) const
{
  std::size_t i = GetPropertyPos(this, name);

  return isNull(i);
}

