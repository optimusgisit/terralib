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

// TerraLib
#include "../common/StringUtils.h"
#include "../core/translator/Translator.h"
#include "../core/uri/URI.h"
#include "../core/uri/Utils.h"
#include "../dataaccess/dataset/ObjectId.h"
#include "../dataaccess/dataset/ObjectIdSet.h"
#include "../dataaccess/query/DataSetName.h"
#include "../dataaccess/query/Query.h"
#include "../dataaccess/query/Select.h"
#include "../dataaccess/utils/Utils.h"
#include "../datatype/ByteArray.h"
#include "../datatype/Date.h"
#include "../datatype/TimeInstant.h"
#include "../geometry/Envelope.h"
#include "../geometry/GeometryProperty.h"
#include "../srs/SpatialReferenceSystemManager.h"
#include "../srs/Config.h"
#include "DataSource.h"
#include "DataSet.h"
#include "SQLVisitor.h"
#include "Transactor.h"
#include "Utils.h"

//BOOST
#include <boost/thread/locks.hpp>

// OGR
#include <ogrsf_frmts.h>

OGRFieldType GetOGRType(int te_type)
{
  switch (te_type)
  {
    case te::dt::CHAR_TYPE:
    case te::dt::UCHAR_TYPE:
    case te::dt::STRING_TYPE:
      return OFTString;
    break;

    case te::dt::INT16_TYPE:
    case te::dt::UINT16_TYPE:
    case te::dt::INT32_TYPE:
    case te::dt::UINT32_TYPE:
      return OFTInteger;
    break;

    case te::dt::INT64_TYPE:
    case te::dt::UINT64_TYPE:
      return OFTInteger64;
      break;

    case te::dt::FLOAT_TYPE:
    case te::dt::DOUBLE_TYPE:
    case te::dt::NUMERIC_TYPE:
      return OFTReal;
    break;

    case te::dt::DATETIME_TYPE:
      return OFTDateTime;
    break;
  };

  return OFTInteger;
}

te::ogr::Transactor::Transactor(DataSource* ds)
  : te::da::DataSourceTransactor(),
    m_ogrDs(ds),
    m_fid(0)
{
}

te::ogr::Transactor::~Transactor()
{
}

te::da::DataSource* te::ogr::Transactor::getDataSource() const
{
  return 0;
}

void te::ogr::Transactor::begin()
{
}

void te::ogr::Transactor::commit()
{
  if (!m_ogrDs->getOGRDataSource())
    return;

  m_ogrDs->getOGRDataSource()->FlushCache();
}

void te::ogr::Transactor::rollBack()
{
}

bool te::ogr::Transactor::isInTransaction() const
{
  return false;
}

std::auto_ptr<te::da::DataSet> te::ogr::Transactor::getDataSet(const std::string& name, 
                                          te::common::TraverseType /*travType*/, 
                                          bool /*connected*/,
                                          const te::common::AccessPolicy pol)
{
  boost::unique_lock< boost::mutex > lockGuard(getStaticMutex());

  if (!m_ogrDs->getOGRDataSource())
  {
    lockGuard.release();
    getStaticMutex().unlock();
    return std::auto_ptr<te::da::DataSet>();
  }

  CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(m_ogrDs->getEncoding()).c_str());

  unsigned int ogropenflags = (pol == te::common::RWAccess || pol == te::common::WAccess) ? GDAL_OF_UPDATE : GDAL_OF_READONLY;
  GDALDataset* ds = (GDALDataset*)GDALOpenEx(m_ogrDs->getOGRDataSource()->GetDescription(), ogropenflags, NULL, NULL, NULL);
  if (!ds)
  {
    CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(te::core::EncodingType::UTF8).c_str());
    return std::auto_ptr<te::da::DataSet>();
  }

  std::string sql = "SELECT FID, * FROM \"" + name + "\"";
  OGRLayer* layer = ds->ExecuteSQL(sql.c_str(), 0, 0);

  CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(te::core::EncodingType::UTF8).c_str());

  lockGuard.release();
  getStaticMutex().unlock();

  if(layer == 0)
    throw Exception(TE_TR("The informed data set could not be found in the data source."));

  return std::auto_ptr<te::da::DataSet>(new DataSet(ds, layer));
}

std::auto_ptr<te::da::DataSet> te::ogr::Transactor::getDataSet(const std::string& name,
                                          const std::string& /*propertyName*/,
                                          const te::gm::Envelope* e,
                                          te::gm::SpatialRelation /*r*/,
                                          te::common::TraverseType /*travType*/, 
                                          bool /*connected*/,
                                          const te::common::AccessPolicy pol)
{
  boost::unique_lock< boost::mutex > lockGuard(getStaticMutex());

  if (!m_ogrDs->getOGRDataSource())
  {
    lockGuard.release();
    getStaticMutex().unlock();
    return std::auto_ptr<te::da::DataSet>();
  }

  CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(m_ogrDs->getEncoding()).c_str());
  
  unsigned int ogropenflags = (pol == te::common::RWAccess || pol == te::common::WAccess) ? GDAL_OF_UPDATE : GDAL_OF_READONLY;
  GDALDataset* ds = (GDALDataset*)GDALOpenEx(m_ogrDs->getOGRDataSource()->GetDescription(), ogropenflags, NULL, NULL, NULL);
  if (!ds)
  {
    CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(te::core::EncodingType::UTF8).c_str());
    return std::auto_ptr<te::da::DataSet>();
  }

  std::string sql = "SELECT FID, * FROM \"" + name + "\"";
  OGRLayer* layer = ds->ExecuteSQL(sql.c_str(), 0, 0);
  
  CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(te::core::EncodingType::UTF8).c_str());

  lockGuard.release();
  getStaticMutex().unlock();

  if(layer == 0)
    throw Exception(TE_TR("The informed data set could not be found in the data source."));

  layer->SetSpatialFilterRect(e->m_llx, e->m_lly, e->m_urx, e->m_ury);
  
  return std::auto_ptr<te::da::DataSet>(new DataSet(ds, layer));
}

std::auto_ptr<te::da::DataSet> te::ogr::Transactor::getDataSet(const std::string& name,
                                          const std::string& /*propertyName*/,
                                          const te::gm::Geometry* g,
                                          te::gm::SpatialRelation /*r*/,
                                          te::common::TraverseType /*travType*/, 
                                          bool /*connected*/,
                                          const te::common::AccessPolicy pol)
{
  boost::unique_lock< boost::mutex > lockGuard(getStaticMutex());

  if (!m_ogrDs->getOGRDataSource())
  {
    lockGuard.release();
    getStaticMutex().unlock();
    return std::auto_ptr<te::da::DataSet>();
  }

  CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(m_ogrDs->getEncoding()).c_str());

  unsigned int ogropenflags = (pol == te::common::RWAccess || pol == te::common::WAccess) ? GDAL_OF_UPDATE : GDAL_OF_READONLY;
  GDALDataset* ds = (GDALDataset*)GDALOpenEx(m_ogrDs->getOGRDataSource()->GetDescription(), ogropenflags, NULL, NULL, NULL);
  if (!ds)
  {
    CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(te::core::EncodingType::UTF8).c_str());
    return std::auto_ptr<te::da::DataSet>();
  }

  std::string sql = "SELECT FID, * FROM \"" + name + "\"";
  OGRLayer* layer = ds->ExecuteSQL(sql.c_str(), 0, 0);

  CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(te::core::EncodingType::UTF8).c_str());

  lockGuard.release();
  getStaticMutex().unlock();

  if(layer == 0)
    throw Exception(TE_TR("The informed data set could not be found in the data source."));
  
  OGRGeometry* ogrg = Convert2OGR(g);

  layer->SetSpatialFilter(ogrg);
  
  OGRGeometryFactory::destroyGeometry(ogrg);
  
  return std::auto_ptr<te::da::DataSet>(new DataSet(ds, layer));
}

std::auto_ptr<te::da::DataSet> te::ogr::Transactor::query(const te::da::Select& q,
                                      te::common::TraverseType /*travType*/, 
                                      bool /*connected*/,
                                      const te::common::AccessPolicy pol)
{
  boost::unique_lock< boost::mutex > lockGuard(getStaticMutex());

  if (!m_ogrDs->getOGRDataSource())
  {
    lockGuard.release();
    getStaticMutex().unlock();
    return std::auto_ptr<te::da::DataSet>();
  }

  CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(m_ogrDs->getEncoding()).c_str());

  unsigned int ogropenflags = (pol == te::common::RWAccess || pol == te::common::WAccess) ? GDAL_OF_UPDATE : GDAL_OF_READONLY;
  GDALDataset* ds = (GDALDataset*)GDALOpenEx(m_ogrDs->getOGRDataSource()->GetDescription(), ogropenflags, NULL, NULL, NULL);
  if (!ds)
  {
    CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(te::core::EncodingType::UTF8).c_str());
    return std::auto_ptr<te::da::DataSet>();
  }

  std::string sql;

  SQLVisitor visitor(*m_ogrDs->getDialect(), sql);

  q.accept(visitor);

  sql = RemoveSpatialSql(sql);

  OGRLayer* layer = ds->ExecuteSQL(sql.c_str(), 0, 0);
  
  CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(te::core::EncodingType::UTF8).c_str());

  lockGuard.release();
  getStaticMutex().unlock();

  if(layer == 0)
    throw Exception(TE_TR("Could not retrieve the DataSet from data source."));

  CPLSetConfigOption("SHAPE_ENCODING", "UTF-8");

  te::gm::Envelope* e = visitor.getMBR();

  if(e != 0)
    layer->SetSpatialFilterRect(e->m_llx, e->m_lly, e->m_urx, e->m_ury);

  return std::auto_ptr<te::da::DataSet>(new DataSet(ds, layer));
}

std::auto_ptr<te::da::DataSet> te::ogr::Transactor::query(const std::string& query,
                                      te::common::TraverseType /*travType*/, 
                                      bool /*connected*/,
                                      const te::common::AccessPolicy pol)
{
  boost::unique_lock< boost::mutex > lockGuard(getStaticMutex());

  if (!m_ogrDs->getOGRDataSource())
  {
    lockGuard.release();
    getStaticMutex().unlock();
    return std::auto_ptr<te::da::DataSet>();
  }

  CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(m_ogrDs->getEncoding()).c_str());

  unsigned int ogropenflags = (pol == te::common::RWAccess || pol == te::common::WAccess) ? GDAL_OF_UPDATE : GDAL_OF_READONLY;
  GDALDataset* ds = (GDALDataset*)GDALOpenEx(m_ogrDs->getOGRDataSource()->GetDescription(), ogropenflags, NULL, NULL, NULL);
  if (!ds)
  {
    CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(te::core::EncodingType::UTF8).c_str());
    return std::auto_ptr<te::da::DataSet>();
  }
  
  // Adding FID attribute case "SELECT *"
  std::string queryCopy = query;
  std::size_t pos = queryCopy.find("*");
  if(pos != std::string::npos)
  {
    std::string fid = "FID, *";
    queryCopy.replace(pos, 1, fid);
  }

  OGRLayer* layer = ds->ExecuteSQL(queryCopy.c_str(), 0, 0);
  
  CPLSetConfigOption("SHAPE_ENCODING", te::core::CharEncoding::getEncodingName(te::core::EncodingType::UTF8).c_str());

  lockGuard.release();
  getStaticMutex().unlock();

  if(layer == 0)
  throw Exception(TE_TR("Could not retrieve the DataSet from data source."));

  return std::auto_ptr<te::da::DataSet>(new DataSet(ds, layer));
}

void te::ogr::Transactor::execute(const te::da::Query& command)
{
  std::string sql;
  SQLVisitor v(*m_ogrDs->getDialect(), sql);

  command.accept(v);

  execute(sql);
}

void te::ogr::Transactor::execute(const std::string& command)
{
  if (!m_ogrDs->getOGRDataSource())
    return;

  OGRLayer* layer = m_ogrDs->getOGRDataSource()->ExecuteSQL(command.c_str(), 0, "");

  if(layer != 0)
    m_ogrDs->getOGRDataSource()->ReleaseResultSet(layer);
}

std::auto_ptr<te::da::PreparedQuery> te::ogr::Transactor::getPrepared(const std::string& /*qName*/)
{
  return std::auto_ptr<te::da::PreparedQuery>(0);
}

std::auto_ptr<te::da::BatchExecutor> te::ogr::Transactor::getBatchExecutor()
{
  return std::auto_ptr<te::da::BatchExecutor>(0);
}

void te::ogr::Transactor::cancel()
{
}

boost::int64_t te::ogr::Transactor::getLastGeneratedId()
{
  return m_fid;
}

std::string te::ogr::Transactor::escape(const std::string& value)
{
  return value;
}

std::vector<std::string> te::ogr::Transactor::getDataSetNames()
{
  std::vector<std::string> names;
  
  if (!m_ogrDs->getOGRDataSource())
    return names;

  for(int i=0; i<m_ogrDs->getOGRDataSource()->GetLayerCount(); i++)
    names.push_back(m_ogrDs->getOGRDataSource()->GetLayer(i)->GetName());

  return names;
}

std::size_t te::ogr::Transactor::getNumberOfDataSets()
{
  if (!m_ogrDs->getOGRDataSource())
    return 0;
  
  return m_ogrDs->getOGRDataSource()->GetLayerCount();
}
    
std::auto_ptr<te::da::DataSetType> te::ogr::Transactor::getDataSetType(const std::string& name)
{
  if (!m_ogrDs->getOGRDataSource())
    return std::auto_ptr<te::da::DataSetType>();
  
  std::string sql("SELECT FID, * FROM \"");
  sql += name + "\"";

  OGRLayer* l = m_ogrDs->getOGRDataSource()->ExecuteSQL(sql.c_str(), 0, 0);

  if(l==0)
    return std::auto_ptr<te::da::DataSetType>();

  std::auto_ptr<te::da::DataSetType> type(Convert2TerraLib(l->GetLayerDefn()));

  type->setName(name);

  const char* colIdName = l->GetFIDColumn();

  if(colIdName == 0 || colIdName[0] == '\0')
    colIdName = "FID";

  int pos = l->GetLayerDefn()->GetFieldIndex(colIdName);
  if(pos >= 0)
  {
    te::da::PrimaryKey* pk = new te::da::PrimaryKey(colIdName, type.get());
    pk->add(type->getProperty(pos));
  }

  int srs = te::ogr::Convert2TerraLibProjection(l->GetSpatialRef());

  te::gm::GeometryProperty* gp = te::da::GetFirstGeomProperty(type.get());
  
  if(gp != 0)
    gp->setSRID(srs);

  m_ogrDs->getOGRDataSource()->ReleaseResultSet(l);

  return type;
}

std::auto_ptr<te::da::DataSetTypeCapabilities> te::ogr::Transactor::getCapabilities(const std::string &name)
{
  std::auto_ptr<te::da::DataSetTypeCapabilities> cap(new te::da::DataSetTypeCapabilities);

  OGRLayer* l = m_ogrDs->getOGRDataSource()->GetLayerByName(name.c_str());

  if(l != 0)
  {
    cap->setSupportAddColumn((l->TestCapability(OLCCreateField) == 0) ? false : true);
    cap->setSupportRemoveColumn((l->TestCapability(OLCDeleteField) == 0) ? false : true);
    cap->setSupportDataEdition((l->TestCapability(OLCRandomWrite) == 0) ? false : true);
  }

  return cap;
}

boost::ptr_vector<te::dt::Property> te::ogr::Transactor::getProperties(const std::string& datasetName)
{
  boost::ptr_vector<te::dt::Property> ps;

  if (!m_ogrDs->getOGRDataSource())
    return ps;

  std::string sql("SELECT FID, * FROM \"");
  sql += datasetName + "\"";

  OGRLayer* l = m_ogrDs->getOGRDataSource()->ExecuteSQL(sql.c_str(), 0, 0);

  if(l!=0)
  {
    int srs = te::ogr::Convert2TerraLibProjection(l->GetSpatialRef());
    std::auto_ptr<te::da::DataSetType> dt(Convert2TerraLib(l->GetLayerDefn(),srs));
    std::vector<te::dt::Property*> props = dt->getProperties();
    std::vector<te::dt::Property*>::iterator it;

    for(it=props.begin(); it!=props.end(); ++it)
      ps.push_back((*it)->clone());
  }

  m_ogrDs->getOGRDataSource()->ReleaseResultSet(l);

  return ps;
}

std::auto_ptr<te::dt::Property> te::ogr::Transactor::getProperty(const std::string& datasetName, const std::string& name)
{
  if (!m_ogrDs->getOGRDataSource())
    return std::auto_ptr<te::dt::Property>();

  int idx = -1;
  std::string sql("SELECT FID, * FROM \"");
  sql += datasetName + "\"";

  OGRLayer* l = m_ogrDs->getOGRDataSource()->ExecuteSQL(sql.c_str(), 0, 0);

  if(l != 0)
    idx = l->GetLayerDefn()->GetFieldIndex(name.c_str());

  m_ogrDs->getOGRDataSource()->ReleaseResultSet(l);

  return getProperty(datasetName, idx);
}

std::auto_ptr<te::dt::Property> te::ogr::Transactor::getProperty(const std::string& datasetName, std::size_t propertyPos)
{
  GDALDataset* ogrds = m_ogrDs->getOGRDataSource();
  if (!ogrds)
    return std::auto_ptr<te::dt::Property>();

  std::auto_ptr<te::dt::Property> res;
  std::string sql ("SELECT FID, * FROM \"");
  sql += datasetName + "\"";

  OGRLayer* l = ogrds->ExecuteSQL(sql.c_str(), 0, 0);

  if(l != 0)
  {
    OGRFeatureDefn* def = l->GetLayerDefn();
    OGRFieldDefn* fdef = def->GetFieldDefn((int)propertyPos);

    if(fdef != 0)
      res.reset(Convert2TerraLib(fdef));
  }

  m_ogrDs->getOGRDataSource()->ReleaseResultSet(l);

  return res;
}

std::vector<std::string> te::ogr::Transactor::getPropertyNames(const std::string& datasetName)
{
  GDALDataset* ogrds = m_ogrDs->getOGRDataSource();
  if (!ogrds)
    return std::vector<std::string>();
  
  std::vector<std::string> res;
  std::string sql ("SELECT FID, * FROM \"");
  sql += datasetName + "\"";

  OGRLayer* l = ogrds->ExecuteSQL(sql.c_str(), 0, 0);

  if(l != 0)
  {
    OGRFeatureDefn* def = l->GetLayerDefn();

    for(int i=0; i<def->GetFieldCount(); i++)
      res.push_back(def->GetFieldDefn(i)->GetNameRef());
  }

  ogrds->ReleaseResultSet(l);

  return res;
}

std::size_t te::ogr::Transactor::getNumberOfProperties(const std::string& datasetName)
{
  GDALDataset* ogrds = m_ogrDs->getOGRDataSource();
  if (!ogrds)
    return 0;

  std::string sql("SELECT FID, * FROM \"");
  sql += datasetName + "\"";

  OGRLayer* l = ogrds->ExecuteSQL(sql.c_str(), 0, 0);

  int res = 0;

  if(l != 0)
  {
    res = l->GetLayerDefn()->GetFieldCount();
    ogrds->ReleaseResultSet(l);
  }

  return res;
}

bool te::ogr::Transactor::propertyExists(const std::string& datasetName, const std::string& name)
{
  GDALDataset* ogrds = m_ogrDs->getOGRDataSource();
  if (!ogrds)
    return false;

  std::string sql("SELECT FID, * FROM \"");
  sql += datasetName + "\"";
  bool res = false;

  OGRLayer* l = ogrds->ExecuteSQL(sql.c_str(), 0, 0);

  if(l != 0)
  {
    res = (l->GetLayerDefn()->GetFieldIndex(name.c_str()) != -1);
    ogrds->ReleaseResultSet(l);
  }

  return res;
}

void te::ogr::Transactor::addProperty(const std::string& datasetName, te::dt::Property* p)
{
  if (!m_ogrDs->getOGRDataSource())
    return;

  OGRLayer* l = m_ogrDs->getOGRDataSource()->GetLayerByName(datasetName.c_str());

  if(l != 0)
  {
    if(p->getType() != te::dt::GEOMETRY_TYPE)
    {
      OGRFieldDefn* nField = Convert2OGR(p);
      OGRErr error = l->CreateField(nField);

      delete nField;

      if(error != OGRERR_NONE)
        throw Exception(TE_TR("Error when attempting add the property: " + p->getName() + "."));

      error = l->SyncToDisk();

      if(error != OGRERR_NONE)
        throw Exception(TE_TR("Error saving changes on the file."));
    }
  }
}

void te::ogr::Transactor::dropProperty(const std::string& datasetName, const std::string& name)
{
  OGRLayer* l = m_ogrDs->getOGRDataSource()->GetLayerByName(datasetName.c_str());

  if(l != 0)
  {
    if(!l->TestCapability(OLCDeleteField))
      throw Exception(TE_TR("This dataset do not support remove properties operation."));

    int fPos = l->GetLayerDefn()->GetFieldIndex(name.c_str());

    if(fPos < 0)
      throw Exception(TE_TR("Field not found."));

    OGRErr error = l->DeleteField(fPos);

    if(error != OGRERR_NONE)
      throw Exception(TE_TR("Error when attempting remove the property."));

    error = l->SyncToDisk();

    if(error != OGRERR_NONE)
      throw Exception(TE_TR("Error saving changes on the file."));
  }
}

void te::ogr::Transactor::renameProperty(const std::string& datasetName,
                            const std::string& propertyName,
                            const std::string& newPropertyName)
{
  if (!m_ogrDs->getOGRDataSource())
    return;

  OGRLayer* l = m_ogrDs->getOGRDataSource()->GetLayerByName(datasetName.c_str());

  if(l != 0)
  {
    int idx = l->GetLayerDefn()->GetFieldIndex(propertyName.c_str());

    if(idx == -1)
      throw Exception(TE_TR("Field to be renamed does not exists."));

    OGRFieldDefn* df = l->GetLayerDefn()->GetFieldDefn(idx);

    OGRFieldDefn* dfn = new OGRFieldDefn(df);

    dfn->SetName(newPropertyName.c_str());

    OGRErr err = l->AlterFieldDefn(idx, dfn, ALTER_NAME_FLAG);

    if(err != OGRERR_NONE)
      throw Exception(TE_TR("Fail to rename field."));
  }
}

void te::ogr::Transactor::changePropertyDefinition(const std::string& datasetName, const std::string& propName, te::dt::Property* newProp)
{
  if (!m_ogrDs->getOGRDataSource())
    return;

  std::auto_ptr<te::dt::Property> p;

  p.reset(newProp);

  OGRLayer* l = m_ogrDs->getOGRDataSource()->GetLayerByName(datasetName.c_str());

  if(l != 0)
  {
    if(!l->TestCapability(OLCAlterFieldDefn))
      throw Exception(TE_TR("This data source do not support the operation of alter columns type."));

    int idx = l->GetLayerDefn()->GetFieldIndex(propName.c_str());

    if(idx == -1)
      throw Exception(TE_TR("Field to be renamed does not exists."));

    OGRFieldDefn* dfn = new OGRFieldDefn(l->GetLayerDefn()->GetFieldDefn(idx));

    dfn->SetType(GetOGRType(newProp->getType()));

    OGRErr err = l->AlterFieldDefn(idx, dfn, ALTER_TYPE_FLAG);

    if(err != OGRERR_NONE)
      throw Exception(TE_TR("Fail to to change field type."));

    std::string name = m_ogrDs->getOGRDataSource()->GetDescription();

    err = l->SyncToDisk();
  }
}

std::auto_ptr<te::da::PrimaryKey> te::ogr::Transactor::getPrimaryKey(const std::string& datasetName)
{
  if (!m_ogrDs->getOGRDataSource())
    return std::auto_ptr<te::da::PrimaryKey>();
  
  std::auto_ptr<te::da::PrimaryKey> res;
  std::string sql("SELECT FID, * FROM \"");
  sql += datasetName + "\"";

  OGRLayer* layer = m_ogrDs->getOGRDataSource()->ExecuteSQL(sql.c_str(), 0, 0);

  if(layer != 0)
  {
    const char* colIdName = layer->GetFIDColumn();

    if(colIdName == 0 || colIdName[0] == '\0')
      colIdName = "FID";

    int pos = layer->GetLayerDefn()->GetFieldIndex(colIdName);
  
    if(pos >= 0)
    {
      res.reset(new te::da::PrimaryKey);
      res->add(getProperty(datasetName, pos).get());
    }
  }

  m_ogrDs->getOGRDataSource()->ReleaseResultSet(layer);

  return res;
}

bool te::ogr::Transactor::primaryKeyExists(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  return false;
}

void te::ogr::Transactor::addPrimaryKey(const std::string& /*datasetName*/, te::da::PrimaryKey* /*pk*/)
{
}

void te::ogr::Transactor::dropPrimaryKey(const std::string& /*datasetName*/)
{
}

std::auto_ptr<te::da::ForeignKey> te::ogr::Transactor::getForeignKey(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  return std::auto_ptr<te::da::ForeignKey>();
}

std::vector<std::string> te::ogr::Transactor::getForeignKeyNames(const std::string& /*datasetName*/)
{
  return std::vector<std::string>();
}

bool te::ogr::Transactor::foreignKeyExists(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  return false;
}

void te::ogr::Transactor::addForeignKey(const std::string& /*datasetName*/, te::da::ForeignKey* /*fk*/)
{
}

void te::ogr::Transactor::dropForeignKey(const std::string& /*datasetName*/, const std::string& /*fkName*/)
{
}

std::auto_ptr<te::da::UniqueKey> te::ogr::Transactor::getUniqueKey(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  return std::auto_ptr<te::da::UniqueKey>();
}

std::vector<std::string> te::ogr::Transactor::getUniqueKeyNames(const std::string& /*datasetName*/)
{
  return std::vector<std::string>();
}

bool te::ogr::Transactor::uniqueKeyExists(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  return false;
}

void te::ogr::Transactor::addUniqueKey(const std::string& /*datasetName*/, te::da::UniqueKey* /*uk*/)
{
}

void te::ogr::Transactor::dropUniqueKey(const std::string& /*datasetName*/, const std::string& /*name*/)
{
}

std::auto_ptr<te::da::CheckConstraint> te::ogr::Transactor::getCheckConstraint(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  return std::auto_ptr<te::da::CheckConstraint>();
}

std::vector<std::string> te::ogr::Transactor::getCheckConstraintNames(const std::string& /*datasetName*/)
{
  return std::vector<std::string>();
}

bool te::ogr::Transactor::checkConstraintExists(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  return false;
}

void te::ogr::Transactor::addCheckConstraint(const std::string& /*datasetName*/, te::da::CheckConstraint* /*cc*/)
{
}

void te::ogr::Transactor::dropCheckConstraint(const std::string& /*datasetName*/, const std::string& /*name*/)
{
}

std::auto_ptr<te::da::Index> te::ogr::Transactor::getIndex(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  return std::auto_ptr<te::da::Index>();
}

std::vector<std::string> te::ogr::Transactor::getIndexNames(const std::string& /*datasetName*/)
{
  return std::vector<std::string>();
}

bool te::ogr::Transactor::indexExists(const std::string& /*datasetName*/, const std::string& /*name*/)
{
  return false;
}

void te::ogr::Transactor::addIndex(const std::string& /*datasetName*/, te::da::Index* /*idx*/,
                      const std::map<std::string, std::string>& /*options*/)
{
}

void te::ogr::Transactor::dropIndex(const std::string& /*datasetName*/, const std::string& /*idxName*/)
{
}

std::auto_ptr<te::da::Sequence> te::ogr::Transactor::getSequence(const std::string& /*name*/)
{
  return std::auto_ptr<te::da::Sequence>();
}

std::vector<std::string> te::ogr::Transactor::getSequenceNames()
{
  return std::vector<std::string>();
}

bool te::ogr::Transactor::sequenceExists(const std::string& /*name*/)
{
  return false;
}

void te::ogr::Transactor::addSequence(te::da::Sequence* /*sequence*/)
{
}

void te::ogr::Transactor::dropSequence(const std::string& /*name*/)
{
}

std::auto_ptr<te::gm::Envelope> te::ogr::Transactor::getExtent(const std::string& datasetName,
                                                  const std::string& propertyName)
{
  if (!m_ogrDs->getOGRDataSource())
    return std::auto_ptr<te::gm::Envelope>();
  
  std::auto_ptr<te::gm::Envelope> res;
  std::string sql("SELECT ");
  sql += propertyName + " FROM \"";
  sql += datasetName + "\"";

  OGRLayer* l = m_ogrDs->getOGRDataSource()->ExecuteSQL(sql.c_str(), 0, 0);

  if(l != 0)
  {
    std::auto_ptr<OGREnvelope> env(new OGREnvelope);

    if(l->GetExtent(env.get()) != OGRERR_NONE)
    {
      m_ogrDs->getOGRDataSource()->ReleaseResultSet(l);
      throw Exception(TE_TR("Error when attempting get extent."));
    }

    res.reset(Convert2TerraLib(env.get()));

    m_ogrDs->getOGRDataSource()->ReleaseResultSet(l);
  }

  return res;
}

std::auto_ptr<te::gm::Envelope> te::ogr::Transactor::getExtent(const std::string& datasetName,
                                                  std::size_t /*propertyPos*/)
{
  return getExtent(datasetName, "OGR_GEOMETRY");
}

std::size_t te::ogr::Transactor::getNumberOfItems(const std::string& datasetName)
{
  if (!m_ogrDs->getOGRDataSource())
    return 0;
  
  OGRLayer* l = m_ogrDs->getOGRDataSource()->GetLayerByName(datasetName.c_str());

  if(l != 0)
    return l->GetFeatureCount();

  return 0;
}

bool te::ogr::Transactor::hasDataSets()
{
  if (!m_ogrDs->getOGRDataSource())
    return false;

  return (m_ogrDs->getOGRDataSource()->GetLayerCount() > 0);
}

bool te::ogr::Transactor::dataSetExists(const std::string& name)
{
  if (!m_ogrDs->getOGRDataSource())
    return false;
  
  return (m_ogrDs->getOGRDataSource()->GetLayerByName(name.c_str()) != 0);
}

void te::ogr::Transactor::createDataSet(te::da::DataSetType* dt, const std::map<std::string, std::string>& /*options*/)
{
  if (!m_ogrDs->getOGRDataSource())
  {
    m_ogrDs->createOGRDataSource();
  }

  if (!m_ogrDs->getOGRDataSource())
    return;
  
  if(!m_ogrDs->getOGRDataSource()->TestCapability(ODsCCreateLayer))
    throw Exception(TE_TR("This driver does not support dataset creation."));

  OGRwkbGeometryType geomType = wkbUnknown;
  OGRSpatialReference* srs = 0;
  if(dt->hasGeom())
  {
    geomType = Convert2OGR(te::da::GetFirstGeomProperty(dt)->getGeometryType());
    int srid = te::da::GetFirstGeomProperty(dt)->getSRID();
    if (srid != TE_UNKNOWN_SRS)
    {
      srs = Convert2OGRProjection( srid );
    }
  }
  
  char** papszOptions = 0;

  std::map<std::string, std::string> kvp = te::core::Expand(m_ogrDs->getConnectionInfo().query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  while(it != kvp.end())
  {
    papszOptions = CSLSetNameValue(papszOptions, it->first.c_str(), it->second.c_str());
    ++it;
  }

  OGRLayer* newLayer = m_ogrDs->getOGRDataSource()->CreateLayer(dt->getName().c_str(), srs, geomType, papszOptions);
  
  if(papszOptions)
    CSLDestroy(papszOptions);

  if(newLayer == 0)
    throw Exception(TE_TR("Error when attempting create the dataset type."));
  
  dt->setName(newLayer->GetName());

// add the properties
  for (size_t i = 0; i < dt->size(); ++i)
  {
    te::dt::Property* p = dt->getProperty(i);

    if (te::common::Convert2UCase(p->getName()) == "FID")
      continue;

    addProperty(dt->getName(), p);
  }
}

void te::ogr::Transactor::cloneDataSet(const std::string& name,
                          const std::string& cloneName,
                          const std::map<std::string, std::string>& /*options*/)
{
  if (!m_ogrDs->getOGRDataSource())
    return;

  if(!m_ogrDs->getOGRDataSource()->TestCapability(ODsCCreateLayer))
    throw Exception(TE_TR("This driver does not support creates a dataset."));

  OGRLayer* l = m_ogrDs->getOGRDataSource()->GetLayerByName(name.c_str());

  if(l == 0)
    throw Exception(TE_TR("Could not retrieve the DataSet from data source."));

  OGRLayer* cl = m_ogrDs->getOGRDataSource()->CopyLayer(l, cloneName.c_str());

  if(cl == 0)
    throw Exception(TE_TR("Error when attempting clone the dataset."));
}

void te::ogr::Transactor::dropDataSet(const std::string& name)
{
  if (!m_ogrDs->getOGRDataSource())
    return;

  if(!m_ogrDs->getOGRDataSource()->TestCapability(ODsCDeleteLayer))
    throw Exception(TE_TR("This driver does not support remove a dataset."));

  int i=0;

  for(; i<m_ogrDs->getOGRDataSource()->GetLayerCount(); i++)
    if(name.compare(m_ogrDs->getOGRDataSource()->GetLayer(i)->GetName()) == 0)
      break;

  if(i == m_ogrDs->getOGRDataSource()->GetLayerCount())
    throw Exception(TE_TR("Could not retrieve the DataSet from data source."));

  if(m_ogrDs->getOGRDataSource()->DeleteLayer(i) != OGRERR_NONE)
    throw Exception(TE_TR("Error when attempting to remove the dataset."));
}

void te::ogr::Transactor::renameDataSet(const std::string& /*name*/, const std::string& /*newName*/)
{
}

void te::ogr::Transactor::add(const std::string& datasetName,
                  te::da::DataSet* d,
                  const std::map<std::string, std::string>& options,
                  std::size_t limit)
{
  if(limit == 0)
    limit = std::string::npos;
  
  if (!m_ogrDs->getOGRDataSource())
    return;

  OGRLayer* layer = m_ogrDs->getOGRDataSource()->GetLayerByName(datasetName.c_str());

  if(layer == 0)
    throw Exception(TE_TR("Could not retrieve the DataSet from data source."));

  try
  {
    begin();

    std::size_t nproperties = d->getNumProperties();

    std::size_t nProcessedRows = 0;

    while(d->moveNext() && (nProcessedRows != limit))
    {
      OGRFeature* feat = OGRFeature::CreateFeature(layer->GetLayerDefn());

      std::size_t currfield = 0;

      for(std::size_t i = 0; i != nproperties; ++i)
      {
        if(d->isNull(i))
        {
          if(d->getPropertyDataType(i) != te::dt::GEOMETRY_TYPE)
            ++currfield;

          continue;
        }

        if(te::common::Convert2UCase(d->getPropertyName(i)) == "FID")
          continue;

        switch(d->getPropertyDataType(i))
        {
          case te::dt::INT16_TYPE:
            feat->SetField((int)currfield, d->getInt16(i));
            ++currfield;
          break;

          case te::dt::INT32_TYPE:
            feat->SetField((int)currfield, d->getInt32(i));
            ++currfield;
          break;

          case te::dt::INT64_TYPE:
            feat->SetField((int)currfield, (GIntBig)d->getInt64(i));
            ++currfield;
            break;

          case te::dt::STRING_TYPE:
            feat->SetField((int)currfield, d->getAsString(i).c_str());
            ++currfield;
          break;

          case te::dt::DOUBLE_TYPE:
            feat->SetField((int)currfield, d->getDouble(i));
            ++currfield;
           break;

          case te::dt::NUMERIC_TYPE:
            feat->SetField((int)currfield, atof(d->getNumeric(i).c_str()));
            ++currfield;
          break;

          case te::dt::BYTE_ARRAY_TYPE:
            {
              std::auto_ptr<te::dt::ByteArray> ba(d->getByteArray(i));
              feat->SetField((int)currfield, ba->bytesUsed(), reinterpret_cast<unsigned char*>(ba->getData()));
              ++currfield;
            }
          break;

          case te::dt::DATETIME_TYPE:
            {
              std::auto_ptr<te::dt::DateTime> dtm(d->getDateTime(i));

              te::dt::Date* dtime = dynamic_cast<te::dt::Date*>(dtm.get());

              if(dtime)
              {
                feat->SetField((int)currfield,
                               static_cast<int>(dtime->getYear()),
                               static_cast<int>(dtime->getMonth()),
                               static_cast<int>(dtime->getDay()));
                ++currfield;
                break;
              }

              te::dt::TimeDuration* tduration = dynamic_cast<te::dt::TimeDuration*>(dtm.get());

              if(tduration)
              {
                feat->SetField((int)currfield, 0, 0, 0,
                               static_cast<int>(tduration->getHours()),
                               static_cast<int>(tduration->getMinutes()),
                               static_cast<int>(tduration->getSeconds()));
                ++currfield;
                break;
              }

              te::dt::TimeInstant* tinst = dynamic_cast<te::dt::TimeInstant*>(dtm.get());

              if(tinst)
              {
                feat->SetField((int)currfield,
                               static_cast<int>(tinst->getDate().getYear()),
                               static_cast<int>(tinst->getDate().getMonth()),
                               static_cast<int>(tinst->getDate().getDay()),
                               static_cast<int>(tinst->getTime().getHours()),
                               static_cast<int>(tinst->getTime().getMinutes()),
                               static_cast<float>(tinst->getTime().getSeconds()));
                ++currfield;
                break;
              }

              throw Exception (TE_TR("Unsupported date and time type by OGR."));
            }
          break;

          case te::dt::GEOMETRY_TYPE:
            {
              std::auto_ptr<te::gm::Geometry> geom(d->getGeometry(i));
              OGRGeometry* OGRgeom = Convert2OGR(geom.get());
              feat->SetGeometryDirectly(OGRgeom);
            }
          break;

          default:
            throw Exception(TE_TR("Unsupported data type by OGR."));
        }
      }

      if(layer->CreateFeature(feat) != OGRERR_NONE)
      {
        OGRFeature::DestroyFeature(feat);
        throw Exception(TE_TR("Fail to insert dataset item."));
      }

      m_fid = feat->GetFID();

      OGRFeature::DestroyFeature(feat);
      nProcessedRows++;
    }

    commit();
  }
  catch(Exception& e)
  {
    rollBack();
    throw e;
  }
}

void te::ogr::Transactor::remove(const std::string& datasetName, const te::da::ObjectIdSet* oids)
{
  if(!m_ogrDs->getOGRDataSource())
    return;

  OGRLayer* l = m_ogrDs->getOGRDataSource()->GetLayerByName(datasetName.c_str());

  if(l == 0)
    throw Exception(TE_TR("Could not retrieve the DataSet from data source."));

  if(!l->TestCapability(OLCDeleteFeature))
    throw Exception(TE_TR("Driver does not support removal of features."));

  std::set<te::da::ObjectId*, te::common::LessCmp<te::da::ObjectId*> >::const_iterator it = oids->begin();

  begin();

  while(it != oids->end())
  {
    std::string id = (*it)->getValueAsString();
    
    if(l->DeleteFeature(atoi(id.c_str())) != OGRERR_NONE)
    {
      rollBack();
      throw Exception(TE_TR("Error when attempting to remove the feature."));
    }

    ++it;
  }

  l->SyncToDisk();

  commit();

  m_ogrDs->getOGRDataSource()->ExecuteSQL(("REPACK " + datasetName).c_str(), NULL, NULL);
}


void te::ogr::Transactor::update(const std::string& datasetName,
                    te::da::DataSet* /*dataset*/,
                    const std::vector<std::size_t>& properties,
                    const te::da::ObjectIdSet* /*oids*/,
                    const std::map<std::string, std::string>& /*options*/,
                    std::size_t /*limit*/)
{
}

void te::ogr::Transactor::update(const std::string &datasetName, te::da::DataSet *dataset, const std::vector< std::set<int> >& properties,
                                 const std::vector<size_t>& ids)
{
  if(m_ogrDs->getOGRDataSource() == 0)
    throw Exception(TE_TR("Data source failure"));

  OGRLayer* l = m_ogrDs->getOGRDataSource()->GetLayerByName(datasetName.c_str());

  if(l == 0)
    throw Exception(TE_TR("Could not retrieve dataset"));

  dataset->moveFirst();
  int i = 0;

  do
  {
    size_t id_pos = ids[0];
    int id = dataset->getInt32(id_pos);

    OGRFeature* feat = l->GetFeature(id)->Clone();

    std::set<int> ls = properties[i];
    std::set<int>::iterator it;

    for(it = ls.begin(); it != ls.end(); ++it)
    {
      int fpos = *it;
      int fpos_o = fpos - 1;

      switch(dataset->getPropertyDataType(fpos))
      {
        case te::dt::INT32_TYPE:
          feat->SetField(fpos_o, dataset->getInt32(fpos));
        break;

        case te::dt::DOUBLE_TYPE:
        case te::dt::NUMERIC_TYPE:
          feat->SetField(fpos_o, dataset->getDouble(fpos));
        break;

        case te::dt::STRING_TYPE:
          feat->SetField(fpos_o, dataset->getString(fpos).c_str());
        break;

        case te::dt::GEOMETRY_TYPE:
        {
          std::auto_ptr<te::gm::Geometry> gm = dataset->getGeometry(fpos);
          feat->SetGeometry(te::ogr::Convert2OGR(gm.get()));
        }
      }
    }

    l->SetFeature(feat);

    i++;
  } while (dataset->moveNext());

  l->SyncToDisk();
}

void te::ogr::Transactor::optimize(const std::map<std::string, std::string>& /*opInfo*/)
{
}
