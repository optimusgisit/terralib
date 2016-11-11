
#include "../Config.h"
#include <terralib/attributefill/VectorToVectorOp.h>
#include <terralib/attributefill/VectorToVectorMemory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceManager.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/geometry/Utils.h>
#include <terralib/maptools/DataSetLayer.h>
#include <terralib/se/Utils.h>

// STL
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

// Boost
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

te::map::DataSetLayerPtr createLayer(te::da::DataSourcePtr source, te::da::DataSetTypePtr& dataset)
{
  static boost::uuids::basic_random_generator<boost::mt19937> gen;

  if (dataset.get() == 0)
  {
    std::cout << "Can not convert a NULL dataset to a layer!" << std::endl;
    return 0;
  }

  boost::uuids::uuid u = gen();
  std::string id = boost::uuids::to_string(u);

  std::string title = dataset->getTitle().empty() ? dataset->getName() : dataset->getTitle();

  te::map::DataSetLayerPtr layer(new te::map::DataSetLayer(id, title));
  layer->setDataSetName(dataset->getName());
  layer->setDataSourceId(source->getId());
  layer->setVisibility(te::map::NOT_VISIBLE);
  layer->setRendererType("ABSTRACT_LAYER_RENDERER");

  if (dataset->size() == 0)
  {
    te::da::DataSourcePtr ds(te::da::DataSourceManager::getInstance().find(source->getId()));
    te::da::LoadProperties(dataset.get(), source->getId());
  }

  te::gm::GeometryProperty* gp = te::da::GetFirstGeomProperty(dataset.get());
  std::auto_ptr<te::gm::Envelope> mbr(te::da::GetExtent(dataset->getName(), gp->getName(), source->getId()));
  layer->setSRID(gp->getSRID());
  if (mbr.get() != 0)
    layer->setExtent(*mbr);
  layer->setStyle(te::se::CreateFeatureTypeStyle(gp->getGeometryType()));

  return layer;
}

bool VectorToVector()
{
  std::string filename(TERRALIB_DATA_DIR "/shape/munic_2001.shp");

  std::string srcInfo("file://" + filename);

  static boost::uuids::basic_random_generator<boost::mt19937> gen;
  boost::uuids::uuid u = gen();
  std::string id = boost::uuids::to_string(u);
  te::da::DataSourcePtr toSource(te::da::DataSourceFactory::make("OGR", srcInfo).release());
  toSource->open();
  toSource->setId(id);

  te::da::DataSourceManager::getInstance().insert(toSource);
  
  filename = TERRALIB_DATA_DIR "/shape/poligono_unico.shp";

  srcInfo = "file://" + filename;

  u = gen();
  id = boost::uuids::to_string(u);
  te::da::DataSourcePtr fromSource(te::da::DataSourceFactory::make("OGR", srcInfo).release());
  fromSource->open();
  fromSource->setId(id);

  te::da::DataSourceManager::getInstance().insert(fromSource);

  std::string toDsName = "munic_2001";
  std::string fromDsName = "poligono_unico";

  te::da::DataSetTypePtr toDt = toSource->getDataSetType(toDsName);
  te::da::DataSetTypePtr fromDt = toSource->getDataSetType(fromDsName);

  if (!toSource->dataSetExists(toDsName))
  {
    std::cout << "\"To\" dataset not found: " << toDsName << std::endl;
    return false;
  }

  if (!fromSource->dataSetExists(fromDsName))
  {
    std::cout << "\"From\" dataset not found: " << fromDsName << std::endl;
    return false;
  }

  std::unique_ptr<te::attributefill::VectorToVectorOp> v2v(new te::attributefill::VectorToVectorMemory());

  te::map::AbstractLayerPtr toLayer = createLayer(toSource, toDt);
  te::map::AbstractLayerPtr fromLayer = createLayer(fromSource, fromDt);

  std::map<std::string, std::vector<te::attributefill::OperationType> > options;

  options["class_name"] = std::vector<te::attributefill::OperationType>(te::attributefill::PERCENT_TOTAL_AREA);

  std::vector<std::string> toLayerProps;
  std::vector<te::dt::Property*> props = toDt->getProperties();
  for (std::size_t i = 0; i < props.size(); ++i)
  {
    toLayerProps.push_back(props[i]->getName());
  }

  te::da::DataSourcePtr outSource;
  std::string outDsName;


  std::string outDataSetName = "v2v_result";

  filename = TERRALIB_DATA_DIR "/shape/v2v_result.shp";

  srcInfo = "file://" + filename;

  outSource.reset(te::da::DataSourceFactory::make("OGR", srcInfo).release());
  outSource->open();

  if (outSource->dataSetExists("v2v_result"))
  {
    std::cout << "There is already a dataset with the requested name in the output data source. Remove it or select a new name and try again." << fromDsName << std::endl;
    return false;
  }

  v2v->setInput(fromLayer, toLayer);
  v2v->setParams(options, toLayerProps);
  v2v->setOutput(outSource, outDsName);

  return true;
}