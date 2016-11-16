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
  \file examples/dataaccess/main.cpp

  \brief A list of examples for the TerraLib TIN Module.
*/
#include "TINExamples.h"

// TerraLib
#include <terralib/core/filesystem/FileSystem.h>
#include <terralib/core/plugin.h>
#include <terralib/common/TerraLib.h>
#include <terralib/core/uri/URI.h>
#include <terralib/core/uri/Utils.h>

#include <terralib/dataaccess/datasource/DataSourceFactory.h>



#include <terralib/mnt/core/Tin.h>
#include <terralib/mnt/core/TINCreateIsolines.h>
#include <terralib/mnt/core/TINCalculateGrid.h>
#include <terralib/mnt/core/TINGeneration.h>
#include <terralib/mnt/core/Utils.h>

// STL
#include "iostream"

// Boost
#include <boost/filesystem.hpp>

//TESTES *********

#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/utils/Utils.h>

#include <terralib/datatype/Property.h>
#include <terralib/datatype/SimpleProperty.h>
#include <terralib/datatype/StringProperty.h>

#include <terralib/geometry/Envelope.h>
#include <terralib/geometry/Geometry.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/geometry/LineString.h>
#include <terralib/geometry/Point.h>
#include <terralib/geometry/TIN.h>
#include <terralib/geometry/GEOSGeometryFactory.h>


#include <terralib/geometry/GEOSWriter.h>
#include <terralib/geometry/GEOSReader.h>


#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>

#include <geos.h>
#include <geos/triangulate/DelaunayTriangulationBuilder.h>


te::da::DataSet* create_ds_memory(const std::string& datasetName, te::da::DataSetType* datasettype, std::auto_ptr<geos::geom::Geometry> results)
{
  te::mem::DataSet* ds = new te::mem::DataSet(datasettype);
  te::mem::DataSetItem* dataSetItem = new te::mem::DataSetItem(ds);

  std::size_t ntri = results->getNumGeometries();
  std::string gtype = results->getGeometryType();
  int id_tri = 0;

  geos::geom::Geometry *gout = results->clone();
  te::gm::GeometryCollection* tinout = dynamic_cast<te::gm::GeometryCollection*> (te::gm::GEOSReader::read(gout));
  tinout->setSRID(results.get()->getSRID());
  for (unsigned int tIdx = 0; tIdx < tinout->getNumGeometries(); ++tIdx)
  {
    te::mem::DataSetItem* dataSetItem = new te::mem::DataSetItem(ds);
    std::vector<std::string> pt_value;
    te::gm::Geometry* gout = tinout->getGeometryN(tIdx);
    std::size_t n = gout->getNPoints();
    te::gm::Polygon* gpol = dynamic_cast<te::gm::Polygon*>(gout);

    for (std::size_t i = 0; i < gpol->getNumRings(); ++i)
    {
      te::gm::Curve* c = gpol->getRingN(i);
      te::gm::LinearRing* lr = dynamic_cast<te::gm::LinearRing*>(c);
      for (std::size_t j = 0; j < lr->getNPoints(); ++j)
      {
        te::gm::Point *p = lr->getPointN(j);
        pt_value.push_back(p->toString());
      }
    }
    dataSetItem->setInt32("TIN_ID", id_tri++);
    dataSetItem->setString("Pt1", pt_value[0]);
    dataSetItem->setString("Pt2", pt_value[1]);
    dataSetItem->setString("Pt3", pt_value[2]);
    dataSetItem->setGeometry("triangle", (te::gm::Geometry*)gout->clone());

    ds->add(dataSetItem);
  }

  return ds;
}

void SaveTIN_shp(std::string dir, std::string out, std::auto_ptr<geos::geom::Geometry> results)
{
  std::auto_ptr<te::da::DataSetType> dt(new te::da::DataSetType(out));

  //Primary key
  te::dt::SimpleProperty* prop0 = new te::dt::SimpleProperty("TIN_ID", te::dt::INT32_TYPE);
  prop0->setAutoNumber(true);
  te::dt::SimpleProperty* prop1 = new te::dt::SimpleProperty("Pt1", te::dt::STRING_TYPE);
  te::dt::SimpleProperty* prop2 = new te::dt::SimpleProperty("Pt2", te::dt::STRING_TYPE);
  te::dt::SimpleProperty* prop3 = new te::dt::SimpleProperty("Pt3", te::dt::STRING_TYPE);
  te::gm::GeometryProperty* prop4 = new te::gm::GeometryProperty("triangle", 0, te::gm::TINZType, true);
  prop4->setSRID(results.get()->getSRID());
  dt->add(prop0);
  dt->add(prop1);
  dt->add(prop2);
  dt->add(prop3);
  dt->add(prop4);

  std::auto_ptr<te::da::DataSet> dataset(create_ds_memory(out, dt.get(), results));

  std::string filename(dir);
  filename.append(out);
  filename.append(".shp");

  std::unique_ptr< te::da::DataSource > datasource = te::da::DataSourceFactory::make("OGR", ("file://" + filename) );
  datasource->open();

  dataset->moveBeforeFirst();

  te::da::Create(datasource.get(), dt.get(), dataset.get());

  datasource->close();
  dt.release();

}

void LoadIsolines(const std::string &filename,
  te::gm::MultiPoint &mpt,
  te::gm::MultiLineString &isolines,
  std::string &geostype, std::string atributo)
{
    int SRID = 4291;
  double tol = 0.00018000000;// 20.;//Pegar da interface (double)SGinfo->Scale()*0.4 / 1000.*multfactor;
  double maxdist = 20.*tol;
  double minedge = tol / 5.;

  std::unique_ptr<te::da::DataSource> datasource = te::da::DataSourceFactory::make("OGR", ("file://" + filename));
  datasource->open();

  te::da::DataSourceTransactor* transactor = (datasource->getTransactor()).release();

  std::vector<std::string> datasetNames = datasource->getDataSetNames();

  te::da::DataSet* inDset = (datasource->getDataSet(datasetNames[0])).release();

  const std::size_t np = inDset->getNumProperties();
  const std::size_t ng = inDset->size();

  //Lendo atributos do shape
  std::vector<std::string>pnames;
  std::vector<int> ptypes;
  for (std::size_t i = 0; i != np; ++i)
  {
    pnames.push_back(inDset->getPropertyName(i));
    ptypes.push_back(inDset->getPropertyDataType(i));
  }

  te::gm::MultiLineString isolines_simp(0, te::gm::MultiLineStringZType, isolines.getSRID());

  std::size_t pos = 0;
  while (inDset->moveNext())
  {
    std::auto_ptr<te::gm::Geometry> gin = inDset->getGeometry("OGR_GEOMETRY");
    //   std::cout << gin.get()->toString() <<std::endl;
    geostype = gin.get()->getGeometryType();//g->getGeometryType();

    if (geostype == "MultiPoint")
    {
      te::gm::MultiPoint *g = dynamic_cast<te::gm::MultiPoint*>(gin.get());
      std::size_t np = g->getNumGeometries();
       double value = inDset->getDouble(atributo); //teste pegar da interface
      for (std::size_t i = 0; i < np; ++i)
      {
        te::gm::Point *p = dynamic_cast<te::gm::Point*>(g->getGeometryN(i));
        te::gm::PointZ pz(p->getX(), p->getY(), value);
        mpt.add(dynamic_cast<te::gm::Geometry*>(pz.clone()));
      }
    }
    if (geostype == "LineString")
    {
      te::gm::LineString *l = dynamic_cast<te::gm::LineString*>(gin.get());
      double value = inDset->getDouble(atributo); //teste pegar da interface
      isolines.add(dynamic_cast<te::gm::Geometry*>(l->clone()));

      te::gm::LineString *ls = te::mnt::pointListSimplify(l, tol, maxdist, value);

      for (std::size_t j = 0; j < ls->getNPoints(); ++j)
      {
        mpt.add(ls->getPointN(j));
      }
    }
    if (geostype == "MultiLineString")
    {
      te::gm::MultiLineString *g = dynamic_cast<te::gm::MultiLineString*>(gin.get());
      std::size_t np = g->getNumGeometries();
      double value = inDset->getDouble(atributo); //teste pegar da interface
      for (std::size_t i = 0; i < np; ++i)
      {
        te::gm::LineString *l = dynamic_cast<te::gm::LineString*>(g->getGeometryN(i));
        te::gm::LineString *lz = new te::gm::LineString(l->size(), te::gm::LineStringZType, isolines.getSRID());
        for (std::size_t il = 0; il < l->size(); il++)
        {
          lz->setPointZ(il, l->getX(il), l->getY(il), value);
        }
        isolines.add(dynamic_cast<te::gm::Geometry*>(lz));
        l->setSRID(isolines.getSRID());
        te::gm::LineString *ls = te::mnt::pointListSimplify(l, tol, maxdist, value);

        for (std::size_t j = 0; j < ls->getNPoints(); ++j)
        {
          mpt.add(ls->getPointN(j));
        }

        isolines_simp.add(dynamic_cast<te::gm::Geometry*>(ls));
      }
    }

  }
  delete inDset;
  delete transactor;

}

void TesteGEOS()
{


  int SRID = 4291;
  double tol = 0.00018000000;// 20.;//Pegar da interface (double)SGinfo->Scale()*0.4 / 1000.*multfactor;
  double maxdist = 20.*tol;
  double minedge = tol / 5.;

  const geos::geom::GeometryFactory *geomFact = te::gm::GEOSGeometryFactory::getGeomFactory();
  te::gm::MultiPoint mpt(0, te::gm::MultiPointZType, SRID);
  te::gm::MultiLineString isolines_simp(0, te::gm::MultiLineStringZType, SRID);
  std::string geostype;

  //Lendo shape das Isolinhas
  std::string filename(TERRALIB_DATA_DIR "/mnt/Isolinhas.shp");

  LoadIsolines(filename, mpt, isolines_simp, geostype, "avg_z");
  mpt.setSRID(SRID);
  isolines_simp.setSRID(SRID);

  geos::triangulate::DelaunayTriangulationBuilder *tri = new geos::triangulate::DelaunayTriangulationBuilder();

   std::auto_ptr<geos::geom::Geometry> results;
   geos::geom::MultiPoint* gin = te::gm::GEOSWriter::write(&mpt);
   tri->setTolerance(minedge);
   tri->setSites(*gin);

   std::string dirout(TERRALIB_DATA_DIR "/mnt/");
   std::string fileout("Isolinhas_GEOS_TIN");


    results = tri->getTriangles(*geomFact);
    results.get()->setSRID(SRID);
    SaveTIN_shp(dirout, fileout, results);

    results.release();

}
///******

void GenerateTIN()
{
  //Lendo shape das Isolinhas
  std::string filename_iso(TERRALIB_DATA_DIR "/mnt/Isolinhas.shp");
  std::string filename_pts(TERRALIB_DATA_DIR "/mnt/Pontos_cotados.shp");

  int SRID = 32723;
  double tol = 20;// 0.00018000000;// 20.;//Pegar da interface (double)SGinfo->Scale()*0.4 / 1000.*multfactor;
  double maxdist = 20.*tol;
  double minedge = tol / 5.;
  std::string atributoz_iso("Avg_Z");
  std::string atributoz_sample("Avg_Z");

  te::da::DataSourcePtr srcDsiso(te::da::DataSourceFactory::make("OGR", ("file://" + filename_iso)).release());
  srcDsiso->open();

  std::string inDsetNameiso = "Isolinhas";
  if (!srcDsiso->dataSetExists(inDsetNameiso))
  {
    std::cout << "Input dataset not found: " << inDsetNameiso << std::endl;
    throw;
  }

  std::auto_ptr<te::da::DataSet> inDsetiso = srcDsiso->getDataSet(inDsetNameiso);
  std::auto_ptr<te::da::DataSetType> inDsetTypeiso(srcDsiso->getDataSetType(inDsetNameiso));

  te::da::DataSourcePtr srcDspts(te::da::DataSourceFactory::make("OGR", ("file://" + filename_pts)).release());
  srcDspts->open();

  std::string inDsetNamepts = "Pontos_cotados";
  if (!srcDspts->dataSetExists(inDsetNamepts))
  {
    std::cout << "Input dataset not found: " << inDsetNamepts << std::endl;
    throw;
  }

  std::auto_ptr<te::da::DataSet> inDsetpts = srcDspts->getDataSet(inDsetNamepts);
  std::auto_ptr<te::da::DataSetType> inDsetTypepts(srcDspts->getDataSetType(inDsetNamepts));

  std::string file_result = TERRALIB_DATA_DIR "/mnt/TIN.shp";

  te::da::DataSourcePtr trgDs(te::da::DataSourceFactory::make("OGR",("file://" + file_result)).release());
  trgDs->open();

  std::string outDS = "TIN";

  if (trgDs->dataSetExists(outDS))
  {
    std::cout << "A dataset with the same requested output dataset name already exists: " << outDS << std::endl;
    throw;
  }

  te::mnt::TINGeneration *Tin = new te::mnt::TINGeneration();

  Tin->setInput(srcDsiso, inDsetNameiso, inDsetTypeiso, te::mnt::Isolines);
  Tin->setInput(srcDspts, inDsetNamepts, inDsetTypepts, te::mnt::Samples);
  Tin->setOutput(trgDs, outDS);
  Tin->setParams(tol, maxdist, minedge, atributoz_iso, atributoz_sample);
  Tin->setSRID(SRID);

  bool result = Tin->run();

  trgDs->close();
  inDsetTypeiso.release();
  srcDsiso->close();
  inDsetTypepts.release();
  srcDspts->close();

}

void GenerateIso()
{
  std::string filename = TERRALIB_DATA_DIR "/mnt/TIN.shp";

  int SRID = 32723;
  double tol = 2;// 20.;//Pegar da interface (double)SGinfo->Scale()*0.4 / 1000.*multfactor;

  std::vector<double> val;
  std::vector<double> gval;
  for (double n = 1010; n <= 1180; n += 10)
    val.push_back(n);

  te::da::DataSourcePtr srcDs(te::da::DataSourceFactory::make("OGR", ("file://" + filename)).release());
  srcDs->open();

  std::string inDsetName = "TIN";
  if (!srcDs->dataSetExists(inDsetName))
  {
    std::cout << "Input dataset not found: " << inDsetName << std::endl;
    throw;
  }

  std::auto_ptr<te::da::DataSet> inDset = srcDs->getDataSet(inDsetName);
  std::auto_ptr<te::da::DataSetType> inDsetType(srcDs->getDataSetType(inDsetName));

  std::string file_result = TERRALIB_DATA_DIR "/mnt/TIN_iso.shp";

    te::da::DataSourcePtr trgDs(te::da::DataSourceFactory::make("OGR", ("file://" + file_result)).release());
  trgDs->open();

  std::string outDS = "TIN_iso";

  if (trgDs->dataSetExists(outDS))
  {
    std::cout << "A dataset with the same requested output dataset name already exists: " << outDS << std::endl;
    throw;
  }

  te::mnt::TINCreateIsolines *Tin = new te::mnt::TINCreateIsolines();

  Tin->setInput(srcDs, inDsetName, inDsetType);
  Tin->setOutput(trgDs, outDS);
  Tin->setSRID(SRID);
  Tin->setParams(val, gval, tol);

  bool result = Tin->run();

  trgDs->close();
  inDsetType.release();
  srcDs->close();

}

void CalculateGrid()
{
  std::string filename = TERRALIB_DATA_DIR "/mnt/TIN.shp";

  int SRID = 32723;
  double tol = 2;// 20.;//Pegar da interface (double)SGinfo->Scale()*0.4 / 1000.*multfactor;
  double resx = 30, resy = 30;

  te::da::DataSourcePtr srcDs(te::da::DataSourceFactory::make("OGR", ("file://" + filename)).release());
  srcDs->open();

  std::string inDsetName = "TIN";
  if (!srcDs->dataSetExists(inDsetName))
  {
    std::cout << "Input dataset not found: " << inDsetName << std::endl;
    throw;
  }

  std::auto_ptr<te::da::DataSet> inDset = srcDs->getDataSet(inDsetName);
  std::auto_ptr<te::da::DataSetType> inDsetType(srcDs->getDataSetType(inDsetName));

  boost::filesystem::path uri(TERRALIB_DATA_DIR "/mnt/TIN_GRD_quint1.tif");
  std::string dsName = "TIN_GRD_quint";

  if (te::core::FileSystem::exists(uri.string()))
  {
    std::cout << "Output file already exists. Remove it or select a new name and try again.";
    throw;
  }


  te::da::DataSourcePtr dsOGR(te::da::DataSourceFactory::make("OGR", ("file://" + uri.string())).release());

  dsOGR->open();
  if (dsOGR->dataSetExists(dsName))
  {
    std::cout << "There is already a dataset with the requested name in the output data source. Remove it or select a new name and try again.";
    throw;
  }

  te::mnt::TINCalculateGrid *Tin = new te::mnt::TINCalculateGrid();
  std::map<std::string, std::string> options = te::core::Expand(dsOGR->getConnectionInfo().query());

  Tin->setInput(srcDs, inDsetName, inDsetType);
  Tin->setOutput(options);
  Tin->setSRID(SRID);
  Tin->setParams(resx, resy, te::mnt::Quintico);

  bool result = Tin->run();

  inDsetType.release();
  srcDs->close();
  dsOGR->close();
  delete Tin;

}

int main(int /*argc*/, char** /*argv*/)
{
  try
  {
    // Initialize the Terralib support
    TerraLib::getInstance().initialize();
    te::core::plugin::InitializePluginSystem();
    LoadModules();

    TesteGEOS();

    GenerateTIN();

    GenerateIso();

    CalculateGrid();

    te::core::PluginManager::instance().clear();
    te::core::plugin::FinalizePluginSystem();

    TerraLib::getInstance().finalize();
  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurred: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred!" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
