


%{
  #include "terralib/dataaccess.h"

  static te::da::DataSource* MakeDataSource(const std::string& dsType, const std::string& connInfo)
  {
    boost::uuids::basic_random_generator<boost::mt19937> gen;
    boost::uuids::uuid u = gen();
    std::string dsId = boost::uuids::to_string(u);

    return te::da::DataSourceManager::getInstance().get(dsId, dsType, connInfo).get();
  }

  static te::da::DataSource* GetDataSource(const std::string& dSrcId)
  {
    return te::da::DataSourceManager::getInstance().find(dSrcId).get();
  }

%}

%include "terralib/dataaccess/datasource/DataSource.h";


%extend te::da::DataSource {
	te::da::DataSet* newDataSet(const std::string& name)
	{
		return self->getDataSet(name).release();
	}
}

%extend te::da::DataSetType {
  te::da::DataSetType* cloneDataSetType(te::da::DataSetType* dtOrigin)
  {
    return static_cast<te::da::DataSetType*>(dtOrigin->clone());
  }
}

// Wrap function
te::da::DataSource* MakeDataSource(const std::string& dsType, const std::string& connInfo);
te::da::DataSource* GetDataSource(const std::string& dSrcId);


