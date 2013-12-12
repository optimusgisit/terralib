// Examples
#include "DataAccessExamples.h"

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/datatype.h>
//#include <../terralib/dataaccess_h.h>
 
// STL
#include <iostream>
#include <exception>

void PostGISExample()
{ 
  try
  {
// let's give the minimal server connection information needed to connect to the database server
    std::map<std::string, std::string> connInfo;
    connInfo["PG_HOST"] = "atlas.dpi.inpe.br" ;   // or "localhost";
    connInfo["PG_PORT"] = "5433" ;
    connInfo["PG_USER"] = "postgres";
    connInfo["PG_PASSWORD"] = "postgres";
    connInfo["PG_DB_NAME"] = "terralib4";
    connInfo["PG_CONNECT_TIMEOUT"] = "4"; 
    
// create a data source using the data source factory
    std::auto_ptr<te::da::DataSource> ds = te::da::DataSourceFactory::make("POSTGIS");

// as we are going to use the data source, let�s set the connection info
    ds->setConnectionInfo(connInfo);

// let's open it with the connection info above!
    ds->open();

// shows how to print information about datasets stored in the data source
    //PrintCatalog(ds);

// shows how to retrieve and then print the datasets stored in the data source
//    PrintDataSets(ds);

// get a transactor to interact to the data source in the next examples
    std::auto_ptr<te::da::DataSourceTransactor> transactor = ds->getTransactor();

// shows how to use a spatial filter
    RetrieveUsingSpatialFilter(ds.get());

// shows several examples on how to retrieve the dataset extent
    DataSetGetExtent(transactor.get());

// it creates a DataSetType called 'our_country' using the schema 'public' in the given data source
    std::string dt_name = "public.our_country2";
    std::cout << std::endl << "Creating dataSet= " << dt_name  << std::endl;
    te::da::DataSetType* dtype = new te::da::DataSetType(dt_name);
    dtype->add(new te::dt::SimpleProperty("gid", te::dt::INT32_TYPE, true));
    dtype->add(new te::dt::StringProperty("country_name", te::dt::STRING));
    dtype->add(new te::dt::StringProperty("city_name", te::dt::VAR_STRING, 50, true));
    dtype->add(new te::gm::GeometryProperty("spatial_data", 4326, te::gm::GeometryType, true));

    te::da::DataSetType* datasetType = CreateDataSetType(dt_name,dtype,transactor.get());

// it adds a primary key to the given dataset type
    std::cout << std::endl << "Adding Primary Key to " << dt_name << std::endl;
    te::da::PrimaryKey* pk = AddPrimaryKey(datasetType->getName(), transactor.get());
    assert(pk);

// it adds an Unique Key to the given dataset type
    std::cout << std::endl << "Adding Unique Key to " << dt_name << std::endl;
    te::da::UniqueKey* uk = AddUniqueKey(datasetType->getName(), transactor.get());
    assert(uk);

// it adds a spatial index to the given dataset type
    std::cout << std::endl << "Adding spatial index to " << dt_name << std::endl;
    te::da::Index* idx = AddSpatialIndex(datasetType->getName(), transactor.get());
    assert(idx);

// it adds an integer property called 'population' to the given dataset type
    std::cout << std::endl << "Adding new Property population to " << dt_name << std::endl;
    te::dt::SimpleProperty* p = AddProperty(datasetType->getName(), transactor.get());

// Now, let's  remove things from the data source using transactor
    // first, drop the recently added property
    std::cout << std::endl << "Droping Property population of " << dt_name <<" using transactor or ds"<< std::endl;
    //(transactor.get())->dropProperty(datasetType->getName(), "population"); //not implemented yet
    DroppingDataSetTypeProperty(dt_name, "population",transactor.get()); 
    //or using ds
    //ds->dropProperty(dt_name, "population");

    // finally, drop the dataset we have created above via ds or via transactor
    std::cout << std::endl << "Droping dataSet " << dt_name << std::endl;
    //ds->dropDataSet(dt_name);
    //(transactor.get())->dropDataSet( datasetType->getName()); /Not implemented Yet
    DroppingDataSetType(datasetType->getName(),transactor.get()); 
    if (transactor->isInTransaction())
    {
      std::cout << std::endl << "Transactor in transaction! "<< std::endl;
    }
    //USE O DELETE transactor ANTES de FECHAR O BANCO. 
    //TEM QUE SER RELEASE E NAO GET senao ao sair do escopo, 
    //como � auto_ptr tenta destruir denovo e cai.
    delete transactor.release(); 
    ds->close(); //close nao libera o transactor. USE O delete ACIMA.  
    int i =1;
  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurried in PostGIS Example: " << e.what() << std::endl;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurried in PostGIS Example!" << std::endl;
  }
}

