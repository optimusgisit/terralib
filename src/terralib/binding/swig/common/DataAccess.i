/*
 * File: DataAccess.i
 *
 * Data Access
 */

#define TEDATAACCESSEXPORT

namespace te {
  namespace common {
    %template(DSMgerSingleton) Singleton < te::da::DataSourceManager >;
    %template(DSInfoMgerSingleton) Singleton < te::da::DataSourceInfoManager >;
  }
}

%ignore te::da::GetDataSource;

typedef te::common::Singleton< te::da::DataSourceManager > DSMgerSingleton;
typedef te::common::Singleton< te::da::DataSourceInfoManager > DSInfoMgerSingleton;

#ifdef SWIGPYTHON

%auto_ptr(te::dt::DateTime)
%auto_ptr(te::dt::AbstractData)
%auto_ptr(te::dt::ByteArray)
%auto_ptr(te::dt::Array)
%auto_ptr(te::da::DataSet)
%auto_ptr(te::da::DataSource)
%auto_ptr(te::da::DataSourceTransactor)
%auto_ptr(te::da::Sequence)
%auto_ptr(te::da::CheckConstraint)
%auto_ptr(te::da::ForeignKey)
%auto_ptr(te::da::PrimaryKey)
%auto_ptr(te::da::UniqueKey)
%auto_ptr(te::da::Index)
%auto_ptr(te::dt::Property)
%auto_ptr(te::da::DataSetTypeCapabilities)
%auto_ptr(te::da::DataSetType)
%auto_ptr(te::gm::Envelope)
%auto_ptr(te::gm::Geometry)
%auto_ptr(te::rst::Raster)
%auto_ptr(te::da::PreparedQuery)
%auto_ptr(te::da::BatchExecutor)
%auto_ptr(te::da::Fields)
%auto_ptr(te::da::Expression)
%auto_ptr(te::da::Select)

%unique_ptr(te::dt::DateTime)
%unique_ptr(te::dt::AbstractData)
%unique_ptr(te::dt::ByteArray)
%unique_ptr(te::dt::Array)
%unique_ptr(te::da::DataSet)
%unique_ptr(te::da::DataSource)
%unique_ptr(te::da::DataSourceTransactor)
%unique_ptr(te::da::Sequence)
%unique_ptr(te::da::CheckConstraint)
%unique_ptr(te::da::ForeignKey)
%unique_ptr(te::da::PrimaryKey)
%unique_ptr(te::da::UniqueKey)
%unique_ptr(te::da::Index)
%unique_ptr(te::dt::Property)
%unique_ptr(te::da::DataSetTypeCapabilities)
%unique_ptr(te::da::DataSetType)
%unique_ptr(te::gm::Envelope)
%unique_ptr(te::gm::Geometry)
%unique_ptr(te::rst::Raster)
%unique_ptr(te::da::PreparedQuery)
%unique_ptr(te::da::BatchExecutor)
%unique_ptr(te::da::Fields)
%unique_ptr(te::da::Expression)
%unique_ptr(te::da::Select)

#else

AUTO_PTR_TYPEMAPS(te::dt::DateTime)
AUTO_PTR_TYPEMAPS(te::dt::AbstractData)
AUTO_PTR_TYPEMAPS(te::dt::ByteArray)
AUTO_PTR_TYPEMAPS(te::dt::Array)
AUTO_PTR_TYPEMAPS(te::da::DataSet)
AUTO_PTR_TYPEMAPS(te::da::DataSource)
AUTO_PTR_TYPEMAPS(te::da::DataSourceTransactor)
AUTO_PTR_TYPEMAPS(te::da::PreparedQuery)
AUTO_PTR_TYPEMAPS(te::da::BatchExecutor)
AUTO_PTR_TYPEMAPS(te::da::Sequence)
AUTO_PTR_TYPEMAPS(te::da::CheckConstraint)
AUTO_PTR_TYPEMAPS(te::da::ForeignKey)
AUTO_PTR_TYPEMAPS(te::da::PrimaryKey)
AUTO_PTR_TYPEMAPS(te::da::DataSetTypeConverter)
AUTO_PTR_TYPEMAPS(te::da::UniqueKey)
AUTO_PTR_TYPEMAPS(te::da::Index)
AUTO_PTR_TYPEMAPS(te::da::Fields)
AUTO_PTR_TYPEMAPS(te::da::Expression)
AUTO_PTR_TYPEMAPS(te::da::Select)
AUTO_PTR_TYPEMAPS(te::dt::Property)
AUTO_PTR_TYPEMAPS(te::da::DataSetTypeCapabilities)
AUTO_PTR_TYPEMAPS(te::da::DataSetType)
AUTO_PTR_TYPEMAPS(te::gm::Envelope)
AUTO_PTR_TYPEMAPS(te::gm::Geometry)
AUTO_PTR_TYPEMAPS(te::rst::Raster)

UNIQUE_PTR_TYPEMAPS(te::dt::DateTime)
UNIQUE_PTR_TYPEMAPS(te::dt::AbstractData)
UNIQUE_PTR_TYPEMAPS(te::dt::ByteArray)
UNIQUE_PTR_TYPEMAPS(te::dt::Array)
UNIQUE_PTR_TYPEMAPS(te::da::DataSet)
UNIQUE_PTR_TYPEMAPS(te::da::DataSource)
UNIQUE_PTR_TYPEMAPS(te::da::DataSourceTransactor)
UNIQUE_PTR_TYPEMAPS(te::da::PreparedQuery)
UNIQUE_PTR_TYPEMAPS(te::da::BatchExecutor)
UNIQUE_PTR_TYPEMAPS(te::da::Sequence)
UNIQUE_PTR_TYPEMAPS(te::da::CheckConstraint)
UNIQUE_PTR_TYPEMAPS(te::da::ForeignKey)
UNIQUE_PTR_TYPEMAPS(te::da::PrimaryKey)
UNIQUE_PTR_TYPEMAPS(te::da::DataSetTypeConverter)
UNIQUE_PTR_TYPEMAPS(te::da::UniqueKey)
UNIQUE_PTR_TYPEMAPS(te::da::Index)
UNIQUE_PTR_TYPEMAPS(te::da::Fields)
UNIQUE_PTR_TYPEMAPS(te::da::Expression)
UNIQUE_PTR_TYPEMAPS(te::da::Select)
UNIQUE_PTR_TYPEMAPS(te::dt::Property)
UNIQUE_PTR_TYPEMAPS(te::da::DataSetTypeCapabilities)
UNIQUE_PTR_TYPEMAPS(te::da::DataSetType)
UNIQUE_PTR_TYPEMAPS(te::gm::Envelope)
UNIQUE_PTR_TYPEMAPS(te::gm::Geometry)
UNIQUE_PTR_TYPEMAPS(te::rst::Raster)

#endif // SWIGPYTHON

%nodefaultctor te::da::DataSourceFactory;

%{
#include "terralib/dataaccess/Enums.h"
#include "terralib/dataaccess/dataset/DataSet.h"
#include "terralib/dataaccess/dataset/DataSetTypeCapabilities.h"
#include "terralib/dataaccess/dataset/DataSetType.h"
#include "terralib/dataaccess/dataset/ObjectId.h"
#include "terralib/dataaccess/dataset/ObjectIdSet.h"
#include "terralib/dataaccess/dataset/PrimaryKey.h"
#include "terralib/dataaccess/dataset/DataSetTypeConverter.h"
#include "terralib/dataaccess/datasource/DataSource.h"
#include "terralib/dataaccess/datasource/BatchExecutor.h"
#include "terralib/dataaccess/datasource/PreparedQuery.h"
#include "terralib/dataaccess/datasource/DataSourceTransactor.h"
#include "terralib/dataaccess/datasource/DataSourceInfo.h"
#include "terralib/dataaccess/datasource/DataSourceManager.h"
#include "terralib/dataaccess/datasource/DataSourceFactory.h"
#include "terralib/dataaccess/query/Field.h"
#include "terralib/dataaccess/query/Fields.h"
#include "terralib/dataaccess/utils/Utils.h"
#include "terralib/dataaccess/datasource/DataSourceInfoManager.h"
#include "terralib/dataaccess/serialization/xml/Serializer.h"
#include "terralib/memory/DataSet.h"
#include "terralib/memory/DataSetItem.h"

using te::dt::Property;

static void OpenDataSource(const te::da::DataSourceInfo& info)
{
  te::da::DataSourceManager::getInstance().open(info.getId(), info.getType(), info.getConnInfo());
}
%}

//Python doesn't work with namespaces
#ifndef SWIGPYTHON

%nspace te::da::DataSetType;
%nspace te::da::DataSet;
%nspace te::da::ObjectId;
%nspace te::da::ObjectIdSet;
%nspace te::da::DataSource;
%nspace te::da::DataSourceInfo;
%nspace te::da::DataSourceManager;
%nspace te::da::DataSourceFactory;
%nspace te::da::DataSourceInfoManager;
%nspace te::da::PrimaryKey;
%nspace te::da::DataSetTypeConverter;

#endif // SWIGPYTHON

#ifdef SWIGLUA
// TODO: ITS IS MADE BECAUSE ENUM 'te::da::ConstraintType' THERE IS A KEY REDEFINED (REVIEW)
%rename (UNKNOWN_CONSTRAINT) te::da::UNKNOWN;							   
#endif

%include "terralib/dataaccess/Enums.h"
%include "terralib/dataaccess/dataset/DataSetType.h"
%include "terralib/dataaccess/dataset/DataSet.h"
%include "terralib/dataaccess/dataset/ObjectId.h"
%include "terralib/dataaccess/dataset/ObjectIdSet.h"
%include "terralib/dataaccess/dataset/PrimaryKey.h"
#include "terralib/dataaccess/dataset/DataSetTypeConverter.h"
%include "terralib/dataaccess/datasource/DataSource.h"
%include "terralib/dataaccess/datasource/DataSourceInfo.h"
%include "terralib/dataaccess/datasource/DataSourceManager.h"
%include "terralib/dataaccess/datasource/DataSourceFactory.h"
%include "terralib/dataaccess/datasource/BatchExecutor.h"
%include "terralib/dataaccess/datasource/PreparedQuery.h"
%include "terralib/dataaccess/datasource/DataSourceTransactor.h"
%include "terralib/dataaccess/query/Field.h"
%include "terralib/dataaccess/query/Fields.h"
%include "terralib/dataaccess/utils/Utils.h"
%include "terralib/dataaccess/datasource/DataSourceInfoManager.h"
%include "terralib/dataaccess/serialization/xml/Serializer.h"

%newobject te::da::DataSourceFactory::make(const std::string& dsType);

//Wrap function
void OpenDataSource(const te::da::DataSourceInfo& info);