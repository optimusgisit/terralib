//TerraLib
#include "../Config.h"
#include <terralib/graph/builder/LDDGraphBuilder.h>
#include <terralib/graph/core/AbstractGraph.h>
#include <terralib/graph/Globals.h>
#include <terralib/se.h>
#include "GraphExamples.h"
#include "DisplayWindow.h"

// STL Includes
#include <iostream>

// BOOST Includes
#include <boost/shared_ptr.hpp>

// Qt
#include <QApplication>

te::se::Style* getLDDGraphStyle();

void CreateLDDGraph(bool draw)
{
  std::cout << std::endl << "Create LDD Graph..." << std::endl;

// graph name
  std::string graphName = "graphAsu";

// open raster
  std::string data_dir = TERRALIB_DATA_DIR;
  std::auto_ptr<te::rst::Raster> lddRaster = OpenRaster(data_dir + "/graph/asu30_dem.tif", 29193);

// graph type
  std::string graphType = te::graph::Globals::sm_graphFactoryDefaultObject;

// connection info
  std::string connInfo("memory:");

// graph information
  std::map<std::string, std::string> graphInfo;
  graphInfo["GRAPH_DATA_SOURCE_TYPE"] = "MEM";
  graphInfo["GRAPH_NAME"] = graphName;
  graphInfo["GRAPH_DESCRIPTION"] = "Generated by LDD Builder.";

  boost::shared_ptr<te::graph::AbstractGraph> graph;

// create graph
  try
  {
    te::graph::LDDGraphBuilder builder;

    if(!builder.build(lddRaster.get(), connInfo, graphType, graphInfo))
    {
      std::cout << std::endl << "An exception has occurred in Graph Example - CreateLDDGraph: " << builder.getErrorMessage() << std::endl;
    }

    graph = builder.getGraph();
  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurred in Graph Example - CreateLDDGraph: " << e.what() << std::endl;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred in Graph Example - CreateLDDGraph!" << std::endl;
  }

  if(draw)
  {
    // set visual
    te::se::Style* style = getLDDGraphStyle();

    //start qApp
    int argc = 0;
    QApplication app(argc, 0);

    DisplayWindow* w = new DisplayWindow();
    w->addGraph(graph.get(), *lddRaster->getExtent(), style);
    w->show();

    app.exec();
  }
}

te::se::Style* getLDDGraphStyle()
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
