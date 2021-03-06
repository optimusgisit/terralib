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
  \file terralib/ogr/DataSet.cpp

  \brief Implementation of a DataSet for OGR data provider.
*/

// TerraLib
#include "../../../../core/translator/Translator.h"
#include "../../../../dataaccess/dataset/DataSetType.h"
#include "../../../../datatype/ByteArray.h"
#include "../../../../datatype/DateTimeProperty.h"
#include "../../../../datatype/StringProperty.h"
#include "../../../../datatype/TimeInstant.h"
#include "../../../../geometry/Envelope.h"
#include "../../../../geometry/Geometry.h"
#include "../../../../geometry/WKBReader.h"
#include "../../../../srs/Config.h"
#include "DataSource.h"
#include "DataSet.h"
#include "Exception.h"
#include "Utils.h"

// OGR
#include <ogrsf_frmts.h>
#include <ogr_core.h>
#include <gdal_priv.h>

// STL
#include <cassert>
#include <memory>

//te::gpkg::DataSet::DataSet(OGRDataSource* dsrc, OGRLayer* layer) :
te::gpkg::DataSet::DataSet(GDALDataset* dsrc, OGRLayer* layer) :
te::da::DataSet(),
m_dt(0),
m_ogrDs(dsrc),
m_layer(layer),
m_currentFeature(0),
m_i(-1),
m_wkbArray(0),
m_wkbArraySize(0),
m_srid(TE_UNKNOWN_SRS)
{
  assert(layer);

  layer->ResetReading();

  m_dt = Convert2TerraLib(layer->GetLayerDefn());
  
  assert(m_dt);

  if(m_dt->hasGeom())
  {
    OGRSpatialReference* osrs = m_layer->GetSpatialRef();
    if(osrs)
      m_srid = Convert2TerraLibProjection(osrs);
  }
}

te::gpkg::DataSet::~DataSet()
{
  OGRFeature::DestroyFeature(m_currentFeature);

  delete [] m_wkbArray;

  delete m_dt;

  m_ogrDs->ReleaseResultSet(m_layer);

  //OGRDataSource::DestroyDataSource(m_ogrDs);
  //GDALClose(m_ogrDs);
  GDALClose((GDALDatasetH)m_ogrDs);
}

std::size_t te::gpkg::DataSet::getNumProperties() const
{
  return m_dt->size();
}

int te::gpkg::DataSet::getPropertyDataType(std::size_t pos) const
{
  return m_dt->getProperty(pos)->getType();
}

std::string te::gpkg::DataSet::getPropertyName(std::size_t pos) const
{
  return m_dt->getProperty(pos)->getName();
}

std::string te::gpkg::DataSet::getDatasetNameOfProperty(std::size_t pos) const
{
  return "";
}

bool te::gpkg::DataSet::isEmpty() const
{
  return !(size() > 0);
}

std::auto_ptr<te::gm::Envelope> te::gpkg::DataSet::getExtent(std::size_t /*i*/)
{
  OGREnvelope psExtent;
  m_layer->GetExtent(&psExtent);
  te::gm::Envelope* env =  Convert2TerraLib(&psExtent);
  return std::auto_ptr<te::gm::Envelope>(env);
}

std::size_t te::gpkg::DataSet::size() const
{
  return m_layer->GetFeatureCount();
}

bool te::gpkg::DataSet::moveNext()
{
  OGRFeature::DestroyFeature(m_currentFeature);

  m_currentFeature = m_layer->GetNextFeature();

  m_i++;
  return m_currentFeature != 0;
}

bool te::gpkg::DataSet::movePrevious()
{
  return move(m_i - 1);
}

bool te::gpkg::DataSet::moveBeforeFirst()
{
  m_layer->ResetReading();
  m_i = -1;
  return true;
}

bool te::gpkg::DataSet::moveFirst()
{
  m_layer->ResetReading();
  m_i = -1;
  return moveNext();
}

bool te::gpkg::DataSet::moveLast()
{
  int lastPos = m_layer->GetFeatureCount() - 1;
  return move(lastPos);
}

bool te::gpkg::DataSet::move(std::size_t i)
{
  if(m_i == i)
    return true;

  int p = static_cast<int>(i);

  OGRErr error = m_layer->SetNextByIndex(p);

  m_i = p - 1;
  if(error == OGRERR_NONE)
    return moveNext();
  else
    return false;
}

bool te::gpkg::DataSet::isAtBegin() const
{
  return m_i == 0;
}

bool te::gpkg::DataSet::isBeforeBegin() const
{
  return m_i < 0;
}

bool te::gpkg::DataSet::isAtEnd() const
{
  return m_i >= (int)size();
}

bool te::gpkg::DataSet::isAfterEnd() const
{
  return m_i > static_cast<int>(size());
}

char te::gpkg::DataSet::getChar(std::size_t /*i*/) const
{
  throw te::common::Exception(TE_TR("OGR driver: getChar not supported."));
}

unsigned char te::gpkg::DataSet::getUChar(std::size_t /*i*/) const
{
  throw te::common::Exception(TE_TR("OGR driver: getUChar not supported."));
}

boost::int16_t te::gpkg::DataSet::getInt16(std::size_t /*i*/) const
{
  throw te::common::Exception(TE_TR("OGR driver: getInt16 not supported."));
}

boost::int32_t te::gpkg::DataSet::getInt32(std::size_t i) const
{
  return m_currentFeature->GetFieldAsInteger(i);
}

boost::int64_t te::gpkg::DataSet::getInt64(std::size_t i) const
{
  return m_currentFeature->GetFieldAsInteger64(i);
}

bool te::gpkg::DataSet::getBool(std::size_t /*i*/) const
{
  throw te::common::Exception(TE_TR("OGR driver: getBool not supported."));
}

float te::gpkg::DataSet::getFloat(std::size_t /*i*/) const
{
  throw te::common::Exception(TE_TR("OGR driver: getFloat not supported."));
}

double te::gpkg::DataSet::getDouble(std::size_t i) const
{
  return m_currentFeature->GetFieldAsDouble(i);
}

std::string te::gpkg::DataSet::getNumeric(std::size_t i) const
{
  return m_currentFeature->GetFieldAsString(i);
}

std::string te::gpkg::DataSet::getString(std::size_t i) const
{
  return m_currentFeature->GetFieldAsString(i);
}

std::auto_ptr<te::dt::ByteArray> te::gpkg::DataSet::getByteArray(std::size_t i) const
{
  int size = 0;
  GByte* bytes = m_currentFeature->GetFieldAsBinary(i, &size);

  te::dt::ByteArray* byteArray = new te::dt::ByteArray(size);
  byteArray->copy((char*)bytes, size);
  
  return std::auto_ptr<te::dt::ByteArray>(byteArray);
}

std::auto_ptr<te::gm::Geometry> te::gpkg::DataSet::getGeometry(std::size_t /*i*/) const
{
  char* wkb = (char*)getWKB();

  te::gm::Geometry* geom = te::gm::WKBReader::read(wkb);
  geom->setSRID(m_srid);

  return std::auto_ptr<te::gm::Geometry>(geom);
}

std::auto_ptr<te::rst::Raster> te::gpkg::DataSet::getRaster(std::size_t /*i*/) const
{
  throw te::common::Exception(TE_TR("OGR driver: getRaster not supported."));
}

std::auto_ptr<te::dt::DateTime> te::gpkg::DataSet::getDateTime(std::size_t i) const
{
  if(m_dt == 0)
    return std::auto_ptr<te::dt::DateTime>(0);

  int pnYear,
      pnMonth,
      pnDay,
      pnHour,
      pnMinute,
      pnSecond,
      pnTZFlag;

  if(m_currentFeature->GetFieldAsDateTime(i, &pnYear, &pnMonth, &pnDay, &pnHour, &pnMinute, &pnSecond, &pnTZFlag) == FALSE)
    return std::auto_ptr<te::dt::DateTime>(new te::dt::Date);

  te::dt::Property* p = m_dt->getProperty(i);
  te::dt::DateTimeType subType = static_cast<te::dt::DateTimeProperty*>(p)->getSubType();
  te::dt::DateTime* dateTime = 0;

  if(subType == te::dt::DATE)
  {
    dateTime = new te::dt::Date((unsigned short)pnYear, (unsigned short)pnMonth, (unsigned short)pnDay);
  }
  else if(subType == te::dt::TIME_DURATION)
  {
    dateTime = new te::dt::TimeDuration(pnHour, pnMinute, pnSecond);
  }
  else if(subType == te::dt::TIME_INSTANT)
  {
    te::dt::Date d(pnYear, pnMonth, pnDay);
    te::dt::TimeDuration td(pnHour, pnMinute, pnSecond);
    dateTime = new te::dt::TimeInstant(d, td);
  }

  return std::auto_ptr<te::dt::DateTime>(dateTime);
}

std::auto_ptr<te::dt::Array> te::gpkg::DataSet::getArray(std::size_t /*i*/) const
{
  return std::auto_ptr<te::dt::Array>(0); // Not supported by OGR library
}

bool te::gpkg::DataSet::isNull(std::size_t i) const
{
  if(m_currentFeature->IsFieldSet(i) == 0)
    return true;

  return false;
}

const unsigned char* te::gpkg::DataSet::getWKB() const
{
  // The OGR library supports only one geometry field
  OGRGeometry* geom = m_currentFeature->GetGeometryRef()->clone();

  if(geom == 0)
    return 0;

  if(geom->getGeometryType() == wkbPolygon)
    geom = OGRGeometryFactory::forceToMultiPolygon(geom);
  else if(geom->getGeometryType() == wkbLineString)
    geom = OGRGeometryFactory::forceToMultiLineString(geom);
  else if(geom->getGeometryType() == wkbPoint)
    geom = OGRGeometryFactory::forceToMultiPoint(geom);

  int wkbSize = geom->WkbSize();
  
  if(wkbSize > m_wkbArraySize)
  {
    m_wkbArraySize = wkbSize;
    delete [] m_wkbArray;
    m_wkbArray = new unsigned char[m_wkbArraySize];
  }

  geom->exportToWkb(wkbNDR, m_wkbArray);

// 2.5D geometries have a special code!
  if(geom->getGeometryType() & 0x80000000)
  {
    unsigned int newcode = 0x0FFFFFFF & geom->getGeometryType();
    newcode += 1000;
    memcpy(m_wkbArray + 1, &newcode, sizeof(unsigned int));
  }

  OGRGeometryFactory::destroyGeometry(geom);

  return (const unsigned char*)m_wkbArray;
}
