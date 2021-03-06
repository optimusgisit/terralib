// Examples
#include "DataAccessExamples.h"

// TerraLib
#include <terralib/dataaccess/dataset/CheckConstraint.h>
#include <terralib/dataaccess/dataset/CheckConstraint.h>
#include <terralib/dataaccess/dataset/PrimaryKey.h>
#include <terralib/dataaccess/dataset/Index.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/postgis/DataSource.h>
#include <terralib/postgis/DataSourceFactory.h>
#include <terralib/postgis/PreparedQuery.h>

//
// STL
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

std::unique_ptr<te::da::DataSource> GetPostGISConnection();

void ObjectId()
{
  std::unique_ptr<te::da::DataSource> ds = GetPostGISConnection();
  if (!ds.get())
    return;
  
  ds->open();
  
  // get a transactor to interact to the data source in the next examples
  std::auto_ptr<te::da::DataSourceTransactor> transactor = ds->getTransactor();
  assert(transactor.get());
  // now we will retrieve all cities that intersects a given box
  te::gm::Envelope box(-43.9329795837402, -20.6328010559082, -43.4036407470703, -20.1612071990967);
  
  std::auto_ptr<te::da::DataSet> dataset = transactor->getDataSet("public.br_munic_2001", "geom", &box, te::gm::INTERSECTS);
 
  assert(dataset.get());
  std::cout << "DataSet size: " << dataset->size() << std::endl;

// let�s generate the oids using the primary key  
  std::vector<std::string> pk_name;
  pk_name.push_back("gid");
  te::da::ObjectIdSet* oids = te::da::GenerateOIDSet(dataset.get(), pk_name);
  
  assert(oids);
  
// let's retrieve the identified elements via transactor
  std::auto_ptr<te::da::DataSet> identified = transactor->getDataSet("public.br_munic_2001",oids);
  assert(identified.get());
  assert(dataset->size() == identified->size());
  identified->moveBeforeFirst();
  dataset->moveBeforeFirst(); //otherwise if will be at the end of the dataset and nothing will be printed
 
  std::cout << "== DataSet Retrieved From Box == " << std::endl;
  PrintDataSet("public.br_munic_2001", dataset.get());
  
  std::cout << "== DataSet Retrieved From ObjectIdSet knowing pk == " << std::endl;
  PrintDataSet("munic_2001_identified",identified.get());

// Another way to get oids using DataSetType...tries to use the pk, uk or all properties (less geom) if pk and uk do not exist.
  dataset->moveBeforeFirst();
  std::auto_ptr<te::da::DataSetType> dt1 =  transactor->getDataSetType("public.br_munic_2001");
  te::da::ObjectIdSet* oids1 = te::da::GenerateOIDSet( dataset.get(), dt1.get());
  std::auto_ptr<te::da::DataSet> identified1 = transactor->getDataSet("public.br_munic_2001",oids1);
  
  std::cout << "== DataSet Retrieved From ObjectIdSet using dataSetType to discover pk == " << std::endl;
  PrintDataSet("munic_2001_identified",identified1.get());

  // Cleaning All
  delete oids1;
}

void ObjectId_query()
{
  std::unique_ptr<te::da::DataSource> ds = GetPostGISConnection();
  if (!ds.get())
    return;

  // get a transactor to interact to the data source in the next examples
  std::auto_ptr<te::da::DataSourceTransactor> transactor = ds->getTransactor();
  assert(transactor.get());


// now we will retrieve all cities that intersects a given box (dataset with 16 elements) and use it in other methods
  te::gm::Envelope box(-43.9329795837402, -20.6328010559082, -43.4036407470703, -20.1612071990967);

  std::auto_ptr<te::da::DataSet> dataset = transactor->getDataSet("br_munic_2001_wout_pk", "geom", &box, te::gm::INTERSECTS);
  //std::auto_ptr<te::da::DataSet> dataset = transactor->getDataSet("br_munic_2001_wout_pk");

  assert(dataset.get());
  std::cout << "DataSet size: " << dataset->size() << std::endl;

// Building Select from oids and property names, and using transactor->query(buildselect.get());
  /*bool ini2 = */dataset->moveBeforeFirst();
  std::auto_ptr<te::da::DataSetType> dt1 =  transactor->getDataSetType("br_munic_2001_wout_pk");

  std::vector<std::string> pnames;
  te::da::GetOIDPropertyNames(dt1.get(),pnames); //It will return pk or uk or all non geom properties

  te::da::ObjectIdSet* oids = te::da::GenerateOIDSet( dataset.get(), pnames);

// Building te::da::Select expression from oids and property names
  /*bool ini1 =*/ dataset->moveBeforeFirst();
  std::vector<std::string> pnames1;
  std::vector<int> ptypes1;
  te::da::GetPropertyInfo(dataset.get(), pnames1,ptypes1);
  
  std::auto_ptr<te::da::Select> buildselect  = te::da::BuildSelect("br_munic_2001_wout_pk", pnames,oids);
  std::auto_ptr<te::da::Select> buildselect1  = te::da::BuildSelect("br_munic_2001_wout_pk", pnames1,oids);

// let's retrieve the elements using a query with the te::da::Select expression based on oids
  std::auto_ptr<te::da::DataSet> identifiedSelect = transactor->query(buildselect.get());
  std::cout << "== DataSet Retrieved From buildselect == " << std::endl;
  std::cout << "DataSet size: " << identifiedSelect->size() << std::endl;

  PrintDataSet("br_munic_2001_wout_pk_Select", identifiedSelect.get());

// let's retrieve the elements using a query with the te::da::Select expression based on oids
  std::auto_ptr<te::da::DataSet> identifiedSelect1 = transactor->query(buildselect1.get());
  std::cout << "== DataSet Retrieved From buildselect including geom == " << std::endl;
  std::cout << "DataSet size: " << identifiedSelect1->size() << std::endl;

  PrintDataSet("br_munic_2001_wout_pk_Select1", identifiedSelect1.get());

// let's get the Select from the generated OIDS  //?? get expression 
  te::da::Expression* select = oids->getExpression();
  assert(select);

// let's print the SQL
  std::string sql;
  te::da::SQLVisitor sqlConverter(*ds->getDialect(), sql);
  select->accept(sqlConverter);

  std::cout << "The SQL is: " << sql << std::endl;

// let's retrieve the identified elements
  std::string sqlplus = "select * from public.br_munic_2001_wout_pk  where ";
  sqlplus+= sql; 
  std::auto_ptr<te::da::DataSet> identified = transactor->query(sqlplus);

  assert(identified.get());
  std::cout << "== DataSet size= " << dataset->size() << " == " << std::endl;
  std::cout << "== DataSet Retrieved From ObjectIdSet = 14 identified  " << identified->size() <<" == " << std::endl;

  //assert(dataset->size() == identified->size()); //16 == 14??

  std::cout << "== DataSet with out pk using Expression == " << std::endl;

  PrintDataSet("br_munic_2001_wout_pk_identified", identified.get());

  // Cleaning All!
  delete select;
  delete oids;
}
