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
 \file terralib/itest/dataaccess/ITsDataSource.cpp

 \brief A integration test suit for the TerraLib Data Access Module.

 \author Carolina Galvão dos Santos
 */

// TerraLib
#include <terralib/core/utils/Platform.h>
#include <terralib/common.h>
#include <terralib/dataaccess.h>
#include <terralib/core/plugin.h>

// STL
#include <memory>
#include <map>
#include <string>

// GTest and GMock
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MockDataSource : public te::da::DataSource
{
  public:

    /* Mock constructor */
    MockDataSource() = default;

    /* Mock destructor */
    virtual ~MockDataSource() = default;

    /* Methods that are virtual, with sample declaration: */

    MOCK_CONST_METHOD0(getType, std::string ());
    MOCK_CONST_METHOD0(getConnectionInfo, const std::map<std::string, std::string>& ());
    MOCK_METHOD1(setConnectionInfo, void (const std::map<std::string, std::string>& connInfo));
    MOCK_METHOD0(open, void ());
    MOCK_METHOD0(close, void ());
    MOCK_CONST_METHOD0(isOpened, bool ());
    MOCK_CONST_METHOD0(isValid, bool ());
    MOCK_CONST_METHOD0(getCapabilities, const te::da::DataSourceCapabilities& ());
    MOCK_CONST_METHOD0(getDialect, const te::da::SQLDialect* ());
    MOCK_METHOD1(execute, void (const te::da::Query& command));
    MOCK_METHOD1(execute, void (const std::string& command));
    MOCK_METHOD1(escape, std::string (const std::string& value));
    MOCK_METHOD1(isDataSetNameValid, bool (const std::string& datasetName));
    MOCK_METHOD1(isPropertyNameValid, bool (const std::string& propertyName));
    MOCK_METHOD0(getDataSetNames, std::vector<std::string> ());
    MOCK_METHOD0(getNumberOfDataSets, std::size_t ());
    MOCK_METHOD1(getProperties, boost::ptr_vector<te::dt::Property> (const std::string& datasetName));
    MOCK_METHOD1(getPropertyNames, std::vector<std::string> (const std::string& datasetName));
    MOCK_METHOD1(getNumberOfProperties, std::size_t (const std::string& datasetName));
    MOCK_METHOD2(propertyExists, bool (const std::string& datasetName, const std::string& name));
    MOCK_METHOD2(addProperty, void (const std::string& datasetName, te::dt::Property* p));
    MOCK_METHOD2(dropProperty, void (const std::string& datasetName, const std::string& name));
    MOCK_METHOD3(renameProperty, void (const std::string& datasetName, const std::string& propertyName, const std::string& newPropertyName));
    MOCK_METHOD3(changePropertyDefinition, void (const std::string& datasetName, const std::string& propName, te::dt::Property* newProp));
    MOCK_METHOD3(changePropertiesDefinitions, void (const std::string& datasetName, const std::vector<std::string>& propsNames, const std::vector<te::dt::Property*> newProps));
    MOCK_METHOD2(primaryKeyExists, bool (const std::string& datasetName, const std::string& name));
    MOCK_METHOD2(addPrimaryKey, void (const std::string& datasetName, te::da::PrimaryKey* pk));
    MOCK_METHOD1(dropPrimaryKey, void (const std::string& datasetName));
    MOCK_METHOD1(getForeignKeyNames, std::vector<std::string> (const std::string& datasetName));
    MOCK_METHOD2(foreignKeyExists, bool (const std::string& datasetName, const std::string& name));
    MOCK_METHOD2(addForeignKey, void (const std::string& datasetName, te::da::ForeignKey* fk));
    MOCK_METHOD2(dropForeignKey, void (const std::string& datasetName, const std::string& fkName));
    MOCK_METHOD1(getUniqueKeyNames, std::vector<std::string> (const std::string& datasetName));
    MOCK_METHOD2(uniqueKeyExists, bool (const std::string& datasetName, const std::string& name));
    MOCK_METHOD2(addUniqueKey, void (const std::string& datasetName, te::da::UniqueKey* uk));
    MOCK_METHOD2(dropUniqueKey, void (const std::string& datasetName, const std::string& name));
    MOCK_METHOD1(getCheckConstraintNames, std::vector<std::string> (const std::string& datasetName));
    MOCK_METHOD2(checkConstraintExists, bool (const std::string& datasetName, const std::string& name));
    MOCK_METHOD2(addCheckConstraint, void (const std::string& datasetName, te::da::CheckConstraint* cc));
    MOCK_METHOD2(dropCheckConstraint, void (const std::string& datasetName, const std::string& name));
    MOCK_METHOD1(getIndexNames, std::vector<std::string> (const std::string& datasetName));
    MOCK_METHOD2(indexExists, bool (const std::string& datasetName, const std::string& name));
    MOCK_METHOD3(addIndex, void (const std::string& datasetName, te::da::Index* idx, const std::map<std::string, std::string>& options));
    MOCK_METHOD2(dropIndex, void (const std::string& datasetName, const std::string& idxName));
    MOCK_METHOD0(getSequenceNames, std::vector<std::string> ());
    MOCK_METHOD1(sequenceExists, bool (const std::string& name));
    MOCK_METHOD1(addSequence, void (te::da::Sequence* sequence));
    MOCK_METHOD1(dropSequence, void (const std::string& name));
    MOCK_METHOD1(getNumberOfItems, std::size_t (const std::string& datasetName));
    MOCK_METHOD0(hasDataSets, bool ());
    MOCK_METHOD1(dataSetExists, bool (const std::string& name));
    MOCK_METHOD2(createDataSet, void (te::da::DataSetType* dt, const std::map<std::string, std::string>& options));
    MOCK_METHOD3(cloneDataSet, void (const std::string& name, const std::string& cloneName, const std::map<std::string, std::string>& options));
    MOCK_METHOD1(dropDataSet, void (const std::string& name));
    MOCK_METHOD2(renameDataSet, void (const std::string& name, const std::string& newName));
    MOCK_METHOD4(add, void (const std::string& datasetName, te::da::DataSet* d, const std::map<std::string, std::string>& options, std::size_t limit));
    MOCK_METHOD0(getEncoding, te::core::EncodingType ());
    MOCK_METHOD2(remove, void (const std::string& datasetName, const te::da::ObjectIdSet* oids));
    MOCK_METHOD6(update, void (const std::string& datasetName, te::da::DataSet* dataset, const std::vector<std::size_t>& properties, const te::da::ObjectIdSet* oids, const std::map<std::string, std::string>& options, std::size_t limit));
    MOCK_METHOD4(update, void (const std::string& datasetName, te::da::DataSet* dataset, const std::vector< std::set<int> >& properties, const std::vector<size_t>& ids));
    MOCK_METHOD1(create, void(const std::map<std::string, std::string>& dsInfo));
    MOCK_METHOD1(drop, void(const std::map<std::string, std::string>& dsInfo));
    MOCK_METHOD1(exists, bool(const std::map<std::string, std::string>& dsInfo));
    MOCK_METHOD1(getDataSourceNames, std::vector<std::string>(const std::map<std::string, std::string>& dsInfo));
    MOCK_METHOD1(getEncodings, std::vector<te::core::EncodingType>(const std::map<std::string, std::string>& dsInfo));

    /* Methods that return an auto_ptr, must be override: */

    MOCK_METHOD0(getTransactorOfMock, te::da::DataSourceTransactor* ());
    virtual std::auto_ptr<te::da::DataSourceTransactor> getTransactor() override {
        return std::auto_ptr<te::da::DataSourceTransactor>(getTransactorOfMock());
    }
    MOCK_METHOD3(getDataSetOfMock, te::da::DataSet* (const std::string& name, te::common::TraverseType travType, const te::common::AccessPolicy accessPolicy));
    virtual std::auto_ptr<te::da::DataSet> getDataSet(const std::string& name, te::common::TraverseType travType, const te::common::AccessPolicy accessPolicy) override {
        return std::auto_ptr<te::da::DataSet>(getDataSetOfMock(name, travType, accessPolicy));
    }
    MOCK_METHOD6(getDataSetOfMock1, te::da::DataSet* (const std::string& name, const std::string& propertyName, const te::gm::Envelope* e, te::gm::SpatialRelation r, te::common::TraverseType travType, const te::common::AccessPolicy accessPolicy));
    virtual std::auto_ptr<te::da::DataSet> getDataSet(const std::string& name, const std::string& propertyName, const te::gm::Envelope* e, te::gm::SpatialRelation r, te::common::TraverseType travType, const te::common::AccessPolicy accessPolicy) override {
        return std::auto_ptr<te::da::DataSet>(getDataSetOfMock1(name, propertyName, e, r, travType, accessPolicy));
    }
    MOCK_METHOD6(getDataSetOfMock2, te::da::DataSet* (const std::string& name, const std::string& propertyName, const te::gm::Geometry* g, te::gm::SpatialRelation r, te::common::TraverseType travType, const te::common::AccessPolicy accessPolicy));
    virtual std::auto_ptr<te::da::DataSet> getDataSet(const std::string& name, const std::string& propertyName, const te::gm::Geometry* g, te::gm::SpatialRelation r, te::common::TraverseType travType, const te::common::AccessPolicy accessPolicy) override {
        return std::auto_ptr<te::da::DataSet>(getDataSetOfMock2(name, propertyName, g, r, travType, accessPolicy));
    }
    MOCK_METHOD3(queryOfMock, te::da::DataSet* (const te::da::Select& q, te::common::TraverseType travType, const te::common::AccessPolicy accessPolicy));
    virtual std::auto_ptr<te::da::DataSet> query(const te::da::Select& q, te::common::TraverseType travType, const te::common::AccessPolicy accessPolicy) override {
        return std::auto_ptr<te::da::DataSet>(queryOfMock(q, travType, accessPolicy));
    }
    MOCK_METHOD3(queryOfMock1, te::da::DataSet* (const std::string& query, te::common::TraverseType travType, const te::common::AccessPolicy accessPolicy));
    virtual std::auto_ptr<te::da::DataSet> query(const std::string& query, te::common::TraverseType travType, const te::common::AccessPolicy accessPolicy) override {
        return std::auto_ptr<te::da::DataSet>(queryOfMock1(query, travType, accessPolicy));
    }
    MOCK_METHOD1(getDataSetTypeOfMock, te::da::DataSetType* (const std::string& name));
    virtual std::auto_ptr<te::da::DataSetType> getDataSetType(const std::string& name) override {
        return std::auto_ptr<te::da::DataSetType>(getDataSetTypeOfMock(name));
    }
    MOCK_METHOD2(getPropertyOfMock, te::dt::Property* (const std::string& datasetName, const std::string& name));
    virtual std::auto_ptr<te::dt::Property> getProperty(const std::string& datasetName, const std::string& name) override {
        return std::auto_ptr<te::dt::Property>(getPropertyOfMock(datasetName, name));
    }
    MOCK_METHOD2(getPropertyOfMock1, te::dt::Property* (const std::string& datasetName, std::size_t propertyPos));
    virtual std::auto_ptr<te::dt::Property> getProperty(const std::string& datasetName, std::size_t propertyPos) override {
        return std::auto_ptr<te::dt::Property>(getPropertyOfMock1(datasetName, propertyPos));
    }
    MOCK_METHOD1(getPrimaryKeyOfMock, te::da::PrimaryKey* (const std::string& datasetName));
    virtual std::auto_ptr<te::da::PrimaryKey> getPrimaryKey(const std::string& datasetName) override {
        return std::auto_ptr<te::da::PrimaryKey> (getPrimaryKeyOfMock(datasetName));
    }
    MOCK_METHOD2(getForeignKeyOfMock, te::da::ForeignKey* (const std::string& datasetName, const std::string& name));
    virtual std::auto_ptr<te::da::ForeignKey> getForeignKey(const std::string& datasetName, const std::string& name) override {
        return std::auto_ptr<te::da::ForeignKey> (getForeignKeyOfMock(datasetName, name));
    }
    MOCK_METHOD2(getUniqueKeyOfMock, te::da::UniqueKey* (const std::string& datasetName, const std::string& name));
    virtual std::auto_ptr<te::da::UniqueKey> getUniqueKey(const std::string& datasetName, const std::string& name) override {
        return std::auto_ptr<te::da::UniqueKey> (getUniqueKeyOfMock(datasetName, name));
    }
    MOCK_METHOD2(getCheckConstraintOfMock, te::da::CheckConstraint* (const std::string& datasetName, const std::string& name));
    virtual std::auto_ptr<te::da::CheckConstraint> getCheckConstraint(const std::string& datasetName, const std::string& name) override {
        return std::auto_ptr<te::da::CheckConstraint> (getCheckConstraintOfMock(datasetName, name));
    }
    MOCK_METHOD2(getIndexOfMock, te::da::Index* (const std::string& datasetName, const std::string& name));
    virtual std::auto_ptr<te::da::Index> getIndex(const std::string& datasetName, const std::string& name) override {
        return std::auto_ptr<te::da::Index> (getIndexOfMock(datasetName, name));
    }
    MOCK_METHOD1(getSequenceOfMock, te::da::Sequence* (const std::string& name));
    virtual std::auto_ptr<te::da::Sequence> getSequence(const std::string& name) override {
        return std::auto_ptr<te::da::Sequence> (getSequenceOfMock(name));
    }
    MOCK_METHOD2(getExtentOfMock, te::gm::Envelope* (const std::string& datasetName, const std::string& propertyName));
    virtual std::auto_ptr<te::gm::Envelope> getExtent(const std::string& datasetName, const std::string& propertyName) override {
        return std::auto_ptr<te::gm::Envelope> (getExtentOfMock(datasetName, propertyName));
    }
    MOCK_METHOD2(getExtentOfMock1, te::gm::Envelope* (const std::string& datasetName, std::size_t propertyPos));
    virtual std::auto_ptr<te::gm::Envelope> getExtent(const std::string& datasetName, std::size_t propertyPos) override {
        return std::auto_ptr<te::gm::Envelope> (getExtentOfMock1(datasetName, propertyPos));
    }
};

void CreateDataSource(te::da::DataSource* datasource)
{
  /* Load OGR plugin */
  te::core::PluginInfo info;
  std::string plugins_path = te::core::FindInTerraLibPath("share/terralib/plugins");
  info = te::core::JSONPluginInfoSerializer(plugins_path + "/te.da.ogr.teplg.json");
  te::core::PluginManager::instance().insert(info);
  te::core::PluginManager::instance().load(info.name);

  /* Create DataSource */
  std::unique_ptr <te::da::DataSource> ds = te::da::DataSourceFactory::make("OGR");
  ds.reset(datasource);

  std::map<std::string, std::string> connInfo;
  std::string data_dir = TERRALIB_DATA_DIR;
  connInfo["URI"] = data_dir + "/DadosEric/Municipios.shp";

  ds->setConnectionInfo(connInfo); /* Exp.1 */

  ds->open(); /* Exp.2 */

  bool isOpened = ds->isOpened(); /* Exp.3 */

  if(isOpened)
  {
    ds->close(); /* Exp.4 */

    isOpened = ds->isOpened(); /* Exp.5 */
  }
}

using ::testing::AtLeast;
using ::testing::_;
using ::testing::Return;
using ::testing::InSequence;

TEST(DataSourceTest, CanCreateDataSource) {
  /* Testing a method that realize a simple sequence
   * of calls to open and close a DataSource */

  /* Mock class declaration */
  std::unique_ptr<MockDataSource> mockDS (new MockDataSource());

  {
    InSequence dummy;
    /* In Sequence means that this expectations
     * should happen in that order */

    /* Exp.1 */
    EXPECT_CALL(*mockDS.get(), setConnectionInfo(_))
        .Times(AtLeast(1));

    /* Exp.2 */
    EXPECT_CALL(*mockDS.get(), open())
        .Times(AtLeast(1))
        .WillOnce(Return());

    /* Exp.3 */
    EXPECT_CALL(*mockDS.get(), isOpened())
        .Times(AtLeast(1))
        .WillOnce(Return(true));

    /* Exp.4 */
    EXPECT_CALL(*mockDS.get(), close())
        .Times(AtLeast(1))
        .WillOnce(Return());

    /* Exp.5 */
    EXPECT_CALL(*mockDS.get(), isOpened())
        .Times(AtLeast(1))
        .WillOnce(Return(false));
  }

  /* Calls of mock class */

  TerraLib::getInstance().initialize();

  te::core::plugin::InitializePluginSystem();

  CreateDataSource(mockDS.release());

  te::core::plugin::FinalizePluginSystem();

  TerraLib::getInstance().finalize();
}
