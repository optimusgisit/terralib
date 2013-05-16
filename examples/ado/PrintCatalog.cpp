/*  Copyright (C) 2011-2013 National Institute For Space Research (INPE) - Brazil.

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
  \file examples/ado/PrintCatalog.cpp

  \brief It displays the information about datasets stored in the data source catalog
*/

// Example
#include "ADOExamples.h"

// TerraLib
#include <terralib/common.h>
#include <terralib/dataaccess.h>

// STL
#include <iostream>


void PrintCatalog(te::da::DataSource* ds)
{
  if(ds == 0 || !ds->isOpened())
  {
    std::cout << "The informed data source is NULL or is closed!" << std::endl;
    return;
  }
 
  // Get a transactor to interact to the data source
  std::auto_ptr<te::da::DataSourceTransactor> t(ds->getTransactor());
  //te::da::DataSourceTransactor* t = ds->getTransactor();

  // From the transactor, take a catalog loader to find out the datasets stored in the data source
  std::auto_ptr<te::da::DataSourceCatalogLoader> cloader(t->getCatalogLoader());
  //te::da::DataSourceCatalogLoader* cloader = t->getCatalogLoader();

  // Retrieve the name of the datasets
  boost::ptr_vector<std::string> datasets;

  cloader->getDataSets(datasets);

  // Iterate over the dataset names to retrieve its information
  std::cout << "Printing information about datasets...\n\nNumber of datasets: " << datasets.size() << std::endl;

  for(std::size_t i = 0; i < datasets.size(); ++i)
  {
    // Get dataset information: the parameter true indicates that we want all the information about the dataset;
    // you can set it to false and see the difference (no primary key and other stuffs are retrieved)

    std::auto_ptr<te::da::DataSetType> dt(cloader->getDataSetType(datasets[i], true));

    std::cout << "\n=============================================================================" << std::endl;
    std::cout << "DataSet: " << dt->getName() << std::endl;
    std::cout << ">>>>>Number of attributes: " << dt->size() << std::endl;

    // Get the primary key information taking the column(s) that compose(s) the primary key(if any)
    std::cout << ">>>>>Primary Key Information" << std::endl;

    te::da::PrimaryKey* pk = dt->getPrimaryKey();
    if(pk != 0)
    {
      const std::vector<te::dt::Property*>& pkCols = pk->getProperties();

      std::vector<std::string> pNames(pkCols.size());
      for(std::size_t i = 0; i < pkCols.size(); ++i)
      {
        pNames[i]= pkCols[i]->getName();
        std::cout << "\t" << "Column " << i << ": " << pNames[i] << std::endl;
      }
    }
    else
      std::cout << "\t" << "The data set has no primary key!" << std::endl;

    // Get the unique key(s) information taking the column(s) that compose(s) each unique key(if any)
    std::cout << ">>>>>Unique Key Information" << std::endl;

    // Get the number of unique keys
    std::size_t numUniqueKeys = dt->getNumberOfUniqueKeys();

    if(numUniqueKeys == 0)
      std::cout << "\t" << "The data set has no unique keys!" << std::endl;

    for(std::size_t i = 0; i < numUniqueKeys; ++i)
    {
       te::da::UniqueKey* uk = dt->getUniqueKey(i);
       const std::vector<te::dt::Property*>& ukCols = uk->getProperties();

      std::vector<std::string> pNames(ukCols.size());
      for(std::size_t i = 0; i < ukCols.size(); ++i)
      {
        pNames[i]= ukCols[i]->getName();
        std::cout << "\t" << "Column " << i << ": " << pNames[i] << std::endl;
      }
    }

    // Get the index(es) information taking the column(s) that compose(s) each index(if any)
    std::cout << ">>>>>Index(es) Information" << std::endl;

    // Get the number of indexes
    std::size_t numIndexes = dt->getNumberOfIndexes();

    if(numIndexes == 0)
      std::cout << "\t" << "The data set has no index(es)!" << std::endl;
    else
      std::cout << "- Number of Indexes: " << numIndexes << std::endl;

    for(std::size_t i = 0; i < numIndexes; ++i)
    {
       te::da::Index* inx = dt->getIndex(i);

       std::cout << "- Index Name: " << inx->getName() << std::endl;
       
       const std::vector<te::dt::Property*>& inxCols = inx->getProperties();

      std::vector<std::string> pNames(inxCols.size()); 
      for(std::size_t i = 0; i < inxCols.size(); ++i)
      {
        pNames[i]= inxCols[i]->getName();
        std::cout << "   Column " << i << ": " << pNames[i] << std::endl;
      }
    }

    std::size_t numCC = dt->getNumberOfCheckConstraints();

    std::cout << ">>>>>Check Contraint(s) Information" << std::endl;

    std::cout << "  Number of Check-Constraints: " << numCC << std::endl;

    for(std::size_t i = 0; i < numCC; ++i)
    {
      te::da::CheckConstraint* cc =  dt->getCheckConstraint(i);
      std::cout << "  Check Constraint Name: " << cc->getName();
      std::cout << ", Expression = " << cc->getExpression() << std::endl;
    }
    
    std::cout << std::endl;
  }
}
