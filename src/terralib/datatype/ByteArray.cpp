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
  \file terralib/datatype/ByteArray.cpp

  \brief A class for representing binary data.
*/

// TerraLib
#include "../core/utils/HexUtils.h"
#include "ByteArray.h"

// STL
#include <cassert>
#include <cstring>

te::dt::ByteArray::ByteArray()
  : m_data(0),
    m_capacity(0),
    m_bytesOccupied(0)
{
}

te::dt::ByteArray::ByteArray(std::size_t capacity)
  : m_data(0),
    m_capacity(capacity),
    m_bytesOccupied(0)
{
  m_data = new char[capacity];
}

te::dt::ByteArray::ByteArray(char* data, std::size_t size)
: m_data(data),
  m_capacity(size),
  m_bytesOccupied(size)
{
}

te::dt::ByteArray::ByteArray(char* data, std::size_t capacity, std::size_t usedBytes)
  : m_data(data),
    m_capacity(capacity),
    m_bytesOccupied(usedBytes)
{
}

te::dt::ByteArray::ByteArray(const ByteArray& rhs)
  : m_capacity(rhs.m_capacity),
    m_bytesOccupied(rhs.m_bytesOccupied)
{
  if(m_capacity)
  {
    m_data = new char[m_capacity];
    memcpy(m_data, rhs.m_data, rhs.m_bytesOccupied);
  }
  else
  {
    m_data = 0;  
  }
}

te::dt::ByteArray& te::dt::ByteArray::operator=(const ByteArray& rhs)
{
  if(this != &rhs)
  {
    delete [] m_data;
    m_capacity = rhs.m_capacity;
    m_bytesOccupied = rhs.m_bytesOccupied;

    if(m_capacity)
    {
      m_data = new char[m_capacity];
      memcpy(m_data, rhs.m_data, rhs.m_bytesOccupied);
    }
    else
    {
      m_data = 0;  
    }
  }

  return *this;
}

te::dt::ByteArray::~ByteArray()
{
  delete [] m_data;
}

char* te::dt::ByteArray::getData() const
{
  return m_data;
}

void te::dt::ByteArray::take(char* data, std::size_t size)
{
  clear();

  m_data = data;
  m_capacity = size;
  m_bytesOccupied = size;
}

void te::dt::ByteArray::take(char* data, std::size_t capacity, std::size_t usedBytes)
{
  clear();

  m_data = data;
  m_capacity = capacity;
  m_bytesOccupied = usedBytes;
}

void te::dt::ByteArray::copy(char* data, std::size_t size)
{
  if(size > m_capacity)
  {
     clear();
     m_data = new char[size];
     m_capacity = size;
  }

  m_bytesOccupied = size;

  memcpy(m_data, data, size);
}

void te::dt::ByteArray::copy(char* data, std::size_t size, std::size_t offset)
{
  if((offset + size) > m_capacity)
  {
    std::size_t newCapacity = offset + size;

    char* newData = new char[newCapacity];

    memcpy(newData, m_data, offset);

    memcpy(newData + offset, data, size);

    take(newData, newCapacity);
  }
  else
  {
    memcpy(m_data + offset, data, size);

    m_bytesOccupied = offset + size;
  }
}

std::size_t te::dt::ByteArray::capacity() const
{
  return m_capacity;
}

std::size_t te::dt::ByteArray::bytesUsed() const
{
  return m_bytesOccupied;
}

void te::dt::ByteArray::setBytesUsed(std::size_t size)
{
  m_bytesOccupied = size;
}

void te::dt::ByteArray::clear()
{
  delete [] m_data;
  m_data = 0;
  m_capacity = 0;
  m_bytesOccupied = 0;
}

te::dt::AbstractData* te::dt::ByteArray::clone() const
{
  return new ByteArray(*this);
}

std::string te::dt::ByteArray::toString() const
{
  if(m_bytesOccupied == 0)
    return std::string("");

  char* hexStr = te::core::Binary2Hex(m_data, m_bytesOccupied);

  std::string result(hexStr);

  delete [] hexStr;

  return result;
}
