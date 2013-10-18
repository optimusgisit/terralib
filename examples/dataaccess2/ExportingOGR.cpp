// Examples
#include "DataAccessExamples.h"

// Terralib
#include <terralib/common.h>
#include <terralib/dataaccess.h>
#include <terralib/datatype.h>
#include <terralib/geometry.h>
#include <terralib/memory.h>
#include <terralib/raster.h>
#include <terralib/srs.h>

#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceManager.h>

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include <boost/foreach.hpp>

te::da::DataSet* create_ds_memory(const std::string& datasetName, te::da::DataSetType* datasettype);
void openFile(const std::string& filename, const std::string dstype);
void openDirectory(const std::string& filename, const std::string dstype);
void saveUsingOGR(const std::string& filename, const std::string drivername);
void testeGDAL();
void ExportingOGR()
{
    std::cout << std::endl << "1 - ";
    openFile(""TE_DATA_EXAMPLE_DIR"/data/rasters/cbers_b2_crop.tif","GDAL");

    std::cout << std::endl << "2 - ";
    openDirectory(""TE_DATA_EXAMPLE_DIR"/data/rasters","GDAL");
  
    std::cout << std::endl << "3 - ";
    openFile(""TE_DATA_EXAMPLE_DIR"/data/shp/munic_2001.shp","OGR");
  
    std::cout << std::endl << "4 - ";
    openFile(""TE_DATA_EXAMPLE_DIR"/data/shp","OGR");
  
    std::cout << std::endl << "5 - ";
    openFile(""TE_DATA_EXAMPLE_DIR"/data/ogr","OGR");
  
    saveUsingOGR(""TE_DATA_EXAMPLE_DIR"/data/ogr/testeOGR.shp", "ESRI Shapefile");
    saveUsingOGR(""TE_DATA_EXAMPLE_DIR"/data/ogr/testeOGR.kml", "KML");
    saveUsingOGR(""TE_DATA_EXAMPLE_DIR"/data/ogr/testeOGR.mif", "MapInfo File");
    saveUsingOGR(""TE_DATA_EXAMPLE_DIR"/data/ogr/testeOGR.json", "GeoJSON");
    saveUsingOGR(""TE_DATA_EXAMPLE_DIR"/data/ogr/testeOGR.gml", "GML");
  
  std::cout << std::endl << "6 - ";
  openFile(""TE_DATA_EXAMPLE_DIR"/data/ogr","OGR");
}

void openFile(const std::string& filename, const std::string dstype)
{
  std::map<std::string, std::string> connInfo;  
  connInfo["URI"] = filename;
  
  std::auto_ptr<te::da::DataSource> dsptr = te::da::DataSourceFactory::make(dstype);
  dsptr->setConnectionInfo(connInfo);
  dsptr->open();
  
  std::vector<std::string> dsNames = dsptr->getDataSetNames();
  
  std::cout  << std::endl << "Datasets in " << filename << ":" << std::endl;
  
  BOOST_FOREACH(std::string s, dsNames)
  {
    std::cout  << "\t"  << "\t" << s << std::endl;
  }
  dsNames.clear();
  dsptr->close();
}

void openDirectory(const std::string& filename, const std::string dstype)
{
  std::map<std::string, std::string> connInfo;
  connInfo["SOURCE"] = filename;
  
  std::auto_ptr<te::da::DataSource> dsptr = te::da::DataSourceFactory::make(dstype);
  dsptr->setConnectionInfo(connInfo);
  dsptr->open();
  
  std::vector<std::string> dsNames = dsptr->getDataSetNames();
  
  std::cout  << std::endl << "Datasets in " << filename << ":" << std::endl;
  
  BOOST_FOREACH(std::string s, dsNames)
  {
    std::cout  << "\t"  << "\t" << s << std::endl;
  }
  dsNames.clear();
  dsptr->close();
}

void saveUsingOGR(const std::string& filename, const std::string drivername)
{
  std::auto_ptr<te::da::DataSetType> dt(new te::da::DataSetType("testeOGR"));
  
  te::dt::SimpleProperty*   prop01 = new te::dt::SimpleProperty("id", te::dt::INT32_TYPE, true);
  prop01->setAutoNumber(true);
  
  te::dt::SimpleProperty*   prop02 = new te::dt::SimpleProperty("sensor", te::dt::INT32_TYPE, true);
  
  te::gm::GeometryProperty* prop03 = new te::gm::GeometryProperty("location", 0, te::gm::PointType, true);
  prop03->setSRID(4618);
  
  te::dt::DateTimeProperty* prop04 = new te::dt::DateTimeProperty( "date", te::dt::DATE, te::dt::UNKNOWN, true);
  
  te::dt::NumericProperty*  prop05 = new te::dt::NumericProperty("value", 7, 4, true);
  
  dt->add(prop01);
  dt->add(prop02);
  dt->add(prop03);
  dt->add(prop04);
  dt->add(prop05);
  
  std::auto_ptr<te::da::DataSet> dataset(create_ds_memory("teste10", dt.get()));
  
  std::map<std::string, std::string> connInfo;
  connInfo["URI"] = filename;
  connInfo["DRIVER"] = drivername;
  
  std::auto_ptr<te::da::DataSource> dsOGR = te::da::DataSourceFactory::make("OGR");
  dsOGR->setConnectionInfo(connInfo);
  dsOGR->open();
  
  dataset->moveBeforeFirst();
  
  te::da::Create(dsOGR.get(), dt.get(), dataset.get());

  dsOGR->close();
  dsOGR.release();
  connInfo.clear();
}


void testeGDAL()
{  
  ////definir uma imagem 50 x 50, 30x30 de resolucao, com canto superior esquerdo em -45,-23, 1 banda
  //te::gm::Coord2D* ulc = new te::gm::Coord2D(-45,-23);
  //te::rst::Grid* grid = new te::rst::Grid(50,50,30,30,ulc,4628);
  //te::rst::BandProperty* bprop = new te::rst::BandProperty(0,te::dt::UCHAR_TYPE);
  //te::rst::RasterProperty* rprop = new te::rst::RasterProperty("raster");
  //rprop->set(grid);
  //rprop->add(bprop);
  //
  //std::auto_ptr<te::da::DataSetType> datasettype(new te::da::DataSetType("testeIM.tif"));
  //datasettype->add(rprop);
  //
  //std::auto_ptr<te::da::DataSource> ds = te::da::DataSourceFactory::make("GDAL");
  //
  //std::map<std::string, std::string> connInfo;
  //connInfo["SOURCE"] = ""TE_DATA_EXAMPLE_DIR"/data/rasters2";
  //
  //std::auto_ptr<te::da::DataSource> dsGDAL = ds->create("GDAL", connInfo);
  //dsGDAL->open();
  //
  //std::map<std::string,std::string> options;
  //dsGDAL->createDataSet(datasettype.get(), options);
  //
  //std::auto_ptr<te::da::DataSet> dataset = dsGDAL->getDataSet("testeIM.tif");
  //std::auto_ptr<te::rst::Raster> rst = dataset->getRaster(0);
  //
  //for (unsigned int l=0; l<50; ++l)
  //  for (unsigned int c=0; c<50; ++c)
  //    rst->setValue(c, l, 3*l);
  //
  //dsGDAL->close();
}

te::da::DataSet* create_ds_memory(const std::string& datasetName, te::da::DataSetType* datasettype)
{
  te::mem::DataSet* ds = new te::mem::DataSet(datasettype);
  
  te::gm::Point auxPoint1( 23.5, 78.6, 4326);
  te::mem::DataSetItem* dsItem01 = new te::mem::DataSetItem(ds);
  dsItem01->setInt32(0, 1); //id
  dsItem01->setInt32(1, 233); //sensor_id
  dsItem01->setGeometry(2, auxPoint1); //location
  
  boost::gregorian::date d1(boost::gregorian::greg_year(2010),boost::gregorian::greg_month(1),15);
  te::dt::DateTime* datetime1 = new te::dt::Date(d1);
  
  dsItem01->setDateTime(3,*datetime1); //measure_date
  dsItem01->setNumeric(4, "65.89"); //measure_value
  
  te::gm::Point auxPoint2( 25.7, 80.5, 4326 );
  te::mem::DataSetItem* dsItem02 = new te::mem::DataSetItem(ds);
  dsItem02->setInt32(0, 2); //id
  dsItem02->setInt32(1, 245); //sensor_id
  dsItem02->setGeometry(2, auxPoint2); //location
  dsItem02->setDateTime(3, *datetime1); //measure_date
  dsItem02->setNumeric(4, "80.90"); //measure_value
  
  te::gm::Point auxPoint3( 24.6, 83.5, 4326 );
  te::mem::DataSetItem* dsItem03 = new te::mem::DataSetItem(ds);
  dsItem03->setInt32(0, 3); //id
  dsItem03->setInt32(1, 255); //sensor_id
  dsItem03->setGeometry(2, auxPoint3); //location
  dsItem03->setDateTime(3, *datetime1); //measure_date
  dsItem03->setNumeric(4, "87.90"); //measure_value
  
  boost::gregorian::date d2(boost::gregorian::greg_year(2010),boost::gregorian::greg_month(2),15);
  te::dt::DateTime* datetime2 = new te::dt::Date(d2);
  
  te::gm::Point auxPoint4( 23.5, 78.6, 4326 );
  te::mem::DataSetItem* dsItem04 = new te::mem::DataSetItem(ds);
  dsItem04->setInt32(0, 4); //id
  dsItem04->setInt32(1, 233); //sensor_id
  dsItem04->setGeometry(2, auxPoint4); //location
  dsItem04->setDateTime(3, *datetime2); //measure_date
  dsItem04->setNumeric(4, "90.89"); //measure_value
  
  te::gm::Point auxPoint5( 25.7, 80.5, 4326 );
  te::mem::DataSetItem* dsItem05 = new te::mem::DataSetItem(ds);
  dsItem05->setInt32(0, 5); //id
  dsItem05->setInt32(1, 245); //sensor_id
  dsItem05->setGeometry(2, auxPoint5); //location
  dsItem05->setDateTime(3, *datetime2); //measure_date
  dsItem05->setNumeric(4, "73.90"); //measure_value
  
  te::gm::Point auxPoint6( 24.6, 83.5, 4326 );
  te::mem::DataSetItem* dsItem06 = new te::mem::DataSetItem(ds);
  dsItem06->setInt32(0, 6); //id
  dsItem06->setInt32(1, 255); //sensor_id
  dsItem06->setGeometry(2, auxPoint6); //location
  dsItem06->setDateTime(3, *datetime2); //measure_date
  dsItem06->setNumeric(4, "45.90"); //measure_value
  
  boost::gregorian::date d3(boost::gregorian::greg_year(2010),boost::gregorian::greg_month(3),15);
  te::dt::DateTime* datetime3 = new te::dt::Date(d3);
  
  te::gm::Point auxPoint7( 23.5, 78.6, 4326 );
  te::mem::DataSetItem* dsItem07 = new te::mem::DataSetItem(ds);
  dsItem07->setInt32(0, 7); //id
  dsItem07->setInt32(1, 233); //sensor_id
  dsItem07->setGeometry(2, auxPoint7); //location
  dsItem07->setDateTime(3, *datetime3); //measure_date
  dsItem07->setNumeric(4, "76.89"); //measure_value
  
  te::gm::Point auxPoint8( 25.7, 80.5, 4326 );
  te::mem::DataSetItem* dsItem08 = new te::mem::DataSetItem(ds);
  dsItem08->setInt32(0, 8); //id
  dsItem08->setInt32(1, 245); //sensor_id
  dsItem08->setGeometry(2, auxPoint8); //location
  dsItem08->setDateTime(3,*datetime3); //measure_date
  dsItem08->setNumeric(4, "78.15"); //measure_value
  
  te::gm::Point auxPoint9( 24.6, 83.5, 4326 );
  te::mem::DataSetItem* dsItem09 = new te::mem::DataSetItem(ds);
  dsItem09->setInt32(0, 9); //id
  dsItem09->setInt32(1, 255); //sensor_id
  dsItem09->setGeometry(2, auxPoint9); //location
  dsItem09->setDateTime(3, *datetime3); //measure_date
  dsItem09->setNumeric(4, "50.32"); //measure_value
  
  //acrescentar esse metodo no data set em memoria!!!!
  ds->add(dsItem01);
  ds->add(dsItem02);
  ds->add(dsItem03);
  ds->add(dsItem04);
  ds->add(dsItem05);
  ds->add(dsItem06);
  ds->add(dsItem07);
  ds->add(dsItem08);
  ds->add(dsItem09);
  
  return ds;
}