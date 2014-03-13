//TerraLib
#include <terralib/graph/core/AbstractGraph.h>
#include <terralib/graph/functions/SpatialWeightsExchanger.h>
#include <terralib/graph/Globals.h>
#include <terralib/se.h>
#include "GraphExamples.h"
#include "DisplayWindow.h"

// STL Includes
#include <iostream>

// BOOST Includes
#include <boost/shared_ptr.hpp>

// Qt
#include <QtGui/QApplication>

te::se::Style* getSWFGraphStyle();

void ExchangeSpatialWeightsFiles(bool draw)
{
  //create Spatial Weights Files
  boost::shared_ptr<te::graph::AbstractGraph> graphGAL;
  boost::shared_ptr<te::graph::AbstractGraph> graphGWT;

  try
  {
    std::auto_ptr<te::da::DataSource> dataSource = OpenOGRDataSource(TE_DATA_EXAMPLE_DIR "/data/graph/BR_Estados.shp");
    std::string dataSetName = "BR_Estados";
    std::string columnId = "FID";

    te::graph::SpatialWeightsExchanger swe;

//Import
    std::string graphType = te::graph::Globals::sm_graphFactoryDefaultObject;

    std::map<std::string, std::string> dsInfo;

    std::map<std::string, std::string> graphInfo;
    graphInfo["GRAPH_DATA_SOURCE_TYPE"] = "MEM";


    //GAL
    std::string galFileName = TE_DATA_EXAMPLE_DIR "/data/graph/BR_Estados_distance.gal";

    graphInfo["GRAPH_NAME"] = "spatialWeightsGAL";
    graphInfo["GRAPH_DESCRIPTION"] = "Generated by Spatial Weights Exchanger - GAL.";

    te::graph::AbstractGraph* gGAL = swe.importFromGAL(galFileName, dsInfo, graphType, graphInfo, dataSource.get());
    graphGAL.reset(gGAL);

    //GWT
    std::string gwtFileName = TE_DATA_EXAMPLE_DIR "/data/graph/BR_Estados_distance.gwt";

    graphInfo["GRAPH_NAME"] = "spatialWeightsGWT";
    graphInfo["GRAPH_DESCRIPTION"] = "Generated by Spatial Weights Exchanger - GWT.";

    te::graph::AbstractGraph* gGWT = swe.importFromGWT(gwtFileName, dsInfo, graphType, graphInfo, dataSource.get());
    graphGWT.reset(gGWT);

//Draw
    if(draw)
    {
      // set visual
      te::se::Style* style = getSWFGraphStyle();

      std::auto_ptr<te::gm::Envelope> ext = getDataSetExtent(dataSource.get(), dataSetName);

      //start qApp
      int argc = 0;
      QApplication app(argc, 0);

      //display for GAL graph
      DisplayWindow* wGAL = new DisplayWindow();
      wGAL->setPNGPrefix("swf_gal_");
      wGAL->addGraph(graphGAL.get(), *ext.get(), style);
      wGAL->show();

      //display for GWT graph
      DisplayWindow* wGWT = new DisplayWindow();
      wGWT->setPNGPrefix("swf_gwt_");
      wGWT->addGraph(graphGWT.get(), *ext.get(), style);
      wGWT->show();

      app.exec();
    }

//Export

    //GAL
    galFileName = TE_DATA_EXAMPLE_DIR "/data/graph/BR_Estados_distance_2.gal";
    swe.exportToGAL(graphGAL.get(), galFileName, dataSetName, columnId);

    //GWT
    gwtFileName = TE_DATA_EXAMPLE_DIR "/data/graph/BR_Estados_distance_2.gwt";
    swe.exportToGWT(graphGWT.get(), gwtFileName, 0, dataSetName, columnId);

  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occuried in Graph Example - ExchangeSpatialWeightsFiles: " << e.what() << std::endl;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occuried in Graph Example - ExchangeSpatialWeightsFiles!" << std::endl;
  }
}

te::se::Style* getSWFGraphStyle()
{
  te::se::Stroke* stroke = te::se::CreateStroke("#FFFF00", "1.0");
  te::se::LineSymbolizer* lineSymbolizer = te::se::CreateLineSymbolizer(stroke);

  te::se::Fill* markFill = te::se::CreateFill("#009900", "1.0");
  te::se::Stroke* markStroke = te::se::CreateStroke("#000000", "1");
  te::se::Mark* mark = te::se::CreateMark("circle", markStroke, markFill);
  te::se::Graphic* graphic = te::se::CreateGraphic(mark, "4", "", "");
  te::se::PointSymbolizer* pointSymbolizer = te::se::CreatePointSymbolizer(graphic);
  
  te::se::Rule* rule = new te::se::Rule;
  rule->push_back(lineSymbolizer);
  rule->push_back(pointSymbolizer);

  te::se::FeatureTypeStyle* style = new te::se::FeatureTypeStyle;
  style->push_back(rule);

  return style;
}
