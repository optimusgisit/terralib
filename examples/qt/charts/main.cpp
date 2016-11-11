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
  \file main.cpp

  \brief Examples about chart widgets
 */

// TerraLib
#include "../../Config.h"
#include <terralib/core/utils/Platform.h>
#include <terralib/core/plugin.h>
#include <terralib/common.h>
#include <terralib/dataaccess.h>


//Utils
#include <terralib/qt/widgets/charts/Utils.h>

// Qt Chart
#include <terralib/qt/widgets/charts/Scatter.h>
#include <terralib/qt/widgets/charts/ScatterChart.h>
#include <terralib/qt/widgets/charts/Histogram.h>
#include <terralib/qt/widgets/charts/HistogramChart.h>
#include <terralib/qt/widgets/charts/ChartDisplay.h>
#include <terralib/qt/widgets/charts/ChartDisplayWidget.h>
#include <terralib/qt/widgets/charts/ChartStyle.h>

// Qt
#include <QApplication>

// STL
#include <exception>
#include <iostream>

void LoadOGRModule()
{
  try
  {
    te::core::PluginInfo info;
  
    std::string plugin_path = te::core::FindInTerraLibPath("share/terralib/plugins");

#ifdef TERRALIB_MOD_OGR_ENABLED
    info = te::core::JSONPluginInfoSerializer(plugin_path + "/te.da.ogr.teplg.json");
    te::core::PluginManager::instance().insert(info);
    te::core::PluginManager::instance().load(info.name);

#endif
  }
  catch(...)
  {
    std::cout << std::endl << "Failed to load the data source OGR driver: unknown exception!" << std::endl;
  }
}

void generateHistogram(te::da::DataSet* dataset, te::da::DataSourceTransactor* transactor)
{
  //Getting the Column that will be used to populate the chart
  std::string renda = "densidade";

  int rendaIdx= te::da::GetPropertyPos(dataset, renda);

  std::auto_ptr<te::da::DataSetType> dt =  transactor->getDataSetType("sp_cities");

  //Creating the histogram and it's chart with the given dataset
  te::qt::widgets::Histogram* histogram = te::qt::widgets::createHistogram(dataset, dt.get(), rendaIdx, 50, -1);
  te::qt::widgets::HistogramChart* chart = new te::qt::widgets::HistogramChart(histogram);

  //Creating and adjusting the chart Display's style.
  te::qt::widgets::ChartStyle* chartStyle = new te::qt::widgets::ChartStyle();
  chartStyle->setTitle(QString::fromUtf8("Histogram"));
  chartStyle->setAxisX(QString::fromUtf8(renda.c_str()));
  chartStyle->setAxisY(QString::fromUtf8("Frequency"));

  //Creating and adjusting the chart Display
  te::qt::widgets::ChartDisplay* chartDisplay = new te::qt::widgets::ChartDisplay(0, QString::fromUtf8("Histogram"), chartStyle);
  chartDisplay->adjustDisplay();
  chart->attach(chartDisplay);

  //Adjusting the chart widget, once it's closed all the other pointers will be deleted. Check the charts documentation for further notes on pointer ownership
  te::qt::widgets::ChartDisplayWidget* displayWidget = new te::qt::widgets::ChartDisplayWidget(chart, te::qt::widgets::HISTOGRAM_CHART, chartDisplay);
  displayWidget->show();
  displayWidget->setWindowTitle("Histogram");
  displayWidget->setAttribute(Qt::WA_DeleteOnClose, true);
}

void generateScatter(te::da::DataSet* dataset, te::da::DataSourceTransactor* transactor)
{
    //Getting the Column that will be used to populate the chart
  std::string renda = "densidade";
  std::string anosest = "codmicro";

  int rendaIdx= te::da::GetPropertyPos(dataset, renda);
  int anosestIdx= te::da::GetPropertyPos(dataset, anosest);

  std::auto_ptr<te::da::DataSetType> dt =  transactor->getDataSetType("sp_cities");

  //Creating the scatter and it's chart with the given dataset
  te::qt::widgets::Scatter* scatter = te::qt::widgets::createScatter(dataset, dt.get(), rendaIdx, anosestIdx, -1);
  te::qt::widgets::ScatterChart* chart = new te::qt::widgets::ScatterChart(scatter);

  //Creating and adjusting the chart Display's style.
  te::qt::widgets::ChartStyle* chartStyle = new te::qt::widgets::ChartStyle();
  chartStyle->setTitle(QString::fromUtf8("Scatter"));
  chartStyle->setAxisX(QString::fromUtf8(renda.c_str()));
  chartStyle->setAxisY(QString::fromUtf8(anosest.c_str()));

  //Creating and adjusting the chart Display
  te::qt::widgets::ChartDisplay* chartDisplay = new te::qt::widgets::ChartDisplay(0, QString::fromUtf8("Scatter"), chartStyle);
  chartDisplay->adjustDisplay();
  chart->attach(chartDisplay);

  //Adjusting the chart widget, once it's closed all the other pointers will be deleted. Check the charts documentation for further notes on pointer ownership
  te::qt::widgets::ChartDisplayWidget* displayWidget = new te::qt::widgets::ChartDisplayWidget(chart, te::qt::widgets::SCATTER_CHART, chartDisplay);
  displayWidget->show();
  displayWidget->setWindowTitle("Scatter");
  displayWidget->setAttribute(Qt::WA_DeleteOnClose, true);
}

int main(int /*argc*/, char** /*argv*/)
{
// initialize Terralib support
  TerraLib::getInstance().initialize();
  te::core::plugin::InitializePluginSystem();
  try
  {
    LoadOGRModule();
    
    // Get a dataset
    std::string connInfo("file://");
  
    std::string data_dir = TERRALIB_DATA_DIR;
    connInfo += data_dir + "/shape";  

    std::unique_ptr<te::da::DataSource> ds = te::da::DataSourceFactory::make("OGR", connInfo);
    ds->open();
    
    std::auto_ptr<te::da::DataSourceTransactor> transactor = ds->getTransactor();

    std::auto_ptr<te::da::DataSet> dataset = transactor->getDataSet("sp_cities");
    if(dataset.get()==0)
    {
       return 0;
    }

    int argc = 1;
    QApplication app(argc, 0);
    QString title("Testing Chart Widgets");

    generateHistogram(dataset.get(), transactor.get());
    generateScatter(dataset.get(), transactor.get());

    int ret;
    ret = app.exec();
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

// finalize Terralib support
  te::core::PluginManager::instance().clear();
  te::core::plugin::FinalizePluginSystem();
  TerraLib::getInstance().finalize();

  return EXIT_SUCCESS;
}
