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
  \file terralib/qt/widgets/rp/ContrastDialogForm.cpp

  \brief This file defines a class for a Contrast dialog.
*/

// TerraLib
#include "../../af/ApplicationController.h"
#include "../../../raster/Grid.h"
#include "../../../common/progress/ProgressManager.h"
#include "../../../common/StringUtils.h"
#include "../../../dataaccess/dataset/DataSet.h"
#include "../../../dataaccess/utils/Utils.h"
#include "../../../geometry/GeometryProperty.h"
#include "../../../geometry/Utils.h"
#include "../../../maptools/Utils.h"
#include "../../../memory/DataSet.h"
#include "../../../memory/DataSetItem.h"
#include "../../../raster/Raster.h"
#include "../../../raster/RasterProperty.h"
#include "../../../raster/RasterSummaryManager.h"
#include "../../../raster/Utils.h"
#include "../../../rp/Module.h"
#include "../../../qt/widgets/rp/Utils.h"
#include "../../../se/ChannelSelection.h"
#include "../../../se/CoverageStyle.h"
#include "../../../se/RasterSymbolizer.h"
#include "../../../se/Rule.h"
#include "../../../se/SelectedChannel.h"
#include "../../../se/Utils.h"
#include "../../af/ApplicationController.h"
#include "../../af/events/ApplicationEvents.h"
#include "../../af/events/LayerEvents.h"
#include "../../widgets/tools/PolygonAcquire.h"
#include "../../widgets/tools/ExtentAcquire.h"
#include "../canvas/Canvas.h"
#include "../progress/ProgressViewerDialog.h"
#include "ContrastDialogForm.h"
#include "RasterHistogramWidget.h"
#include "RasterNavigatorWidget.h"
#include "OutputRasterWidget.h"
#include "ui_ContrastDialogForm.h"

// Qt
#include <QGridLayout>
#include <QMessageBox>

// Boost
#include <boost/lexical_cast.hpp>

//STL
#include <memory>

te::qt::widgets::ContrastDialogForm::ContrastDialogForm(QWidget* parent)
  : QDialog(parent),
  m_ui(new Ui::ContrastDialogForm),
    m_layer(0),
    m_raster(0),
    m_outputWidget(0)
{
  m_geom = 0;
  m_canvas = 0;
  m_parent = parent;
  m_lastText = "";
  m_previewRaster = 0;
  m_contrast = false;

// setup controls
  m_ui->setupUi(this);

  m_ui->m_histogramCheckBox->hide();

  fillContrastTypes();

  //build histogram
  QGridLayout* histogramLayout = new QGridLayout(m_ui->m_histWidget);
  m_histogramWidget.reset(new te::qt::widgets::RasterHistogramWidget(m_ui->m_histWidget));
  histogramLayout->addWidget(m_histogramWidget.get());
  histogramLayout->setContentsMargins(0,0,0,0);

  //build output parameters
  QGridLayout* outputLayout = new QGridLayout(m_ui->m_outputWidget);
  outputLayout->setContentsMargins(0, 0, 0, 0);
  m_outputWidget = new te::qt::widgets::OutputRasterWidget(m_ui->m_outputWidget);
  outputLayout->addWidget(m_outputWidget);

//connects
  connect(m_ui->m_contrastTypeComboBox, SIGNAL(activated(int)), this, SLOT(onContrastTypeComboBoxActivated(int)));
  connect(m_ui->m_bandTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(onCellClicked(int, int)));
  connect(m_ui->m_bandTableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onCellDoubleClicked(int, int)));
  connect(m_ui->m_bandTableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(onCellChanged(int, int)));
  connect(m_navigator.get(), SIGNAL(mapDisplayExtentChanged()), this, SLOT(onPreviewChanged()));
  connect(m_navigator.get(), SIGNAL(previewClicked()), this, SLOT(apply()));

  connect(m_ui->m_histogramCheckBox, SIGNAL(toggled(bool)), this, SLOT(onHistogramCheckBoxToggled(bool)));
  connect(m_ui->m_previewCheckBox, SIGNAL(toggled(bool)), this, SLOT(onPreviewCheckBoxToggled(bool)));

  connect(m_ui->m_allImageRadioButton, SIGNAL(toggled(bool)), this, SLOT(onAllImageRadioButtonToggled(bool)));
  connect(m_ui->m_visibleAreaRadioButton, SIGNAL(toggled(bool)), this, SLOT(onVisibleAreaRadioButtonToggled(bool)));
  connect(m_ui->m_roiRadioButton, SIGNAL(toggled(bool)), this, SLOT(onRoiRadioButtonToggled(bool)));
  connect(m_ui->m_newROIPushButton, SIGNAL(clicked()), this, SLOT(onNewROIPushButtonClicked()));
  connect(m_ui->m_okPushButton, SIGNAL(clicked()), this, SLOT(onOkPushButtonClicked()));

  m_ui->m_helpPushButton->setNameSpace("dpi.inpe.br.plugins");
  m_ui->m_helpPushButton->setPageReference("plugins/rp/rp_contrast.html");
}

te::qt::widgets::ContrastDialogForm::~ContrastDialogForm()
{
}

void te::qt::widgets::ContrastDialogForm::set(te::map::AbstractLayerPtr layer)
{
  m_layer = layer;

  std::auto_ptr<te::da::DataSetType> dsType = m_layer->getSchema();
  te::rst::RasterProperty* rasterProp = te::da::GetFirstRasterProperty(dsType.get());
  std::auto_ptr<te::da::DataSet> dsRaster = m_layer->getData();
  std::auto_ptr<te::rst::Raster> raster = dsRaster->getRaster(rasterProp->getName());
 
  for (int b = 0; b < raster->getNumberOfBands(); b++)
  {
    double min = raster->getBand(b)->getMinValue(true).real();
    double max = raster->getBand(b)->getMaxValue(true).real();
    m_minValue.push_back(min);
    m_maxValue.push_back(max);
  }

  if (raster->getBand(0)->getProperty()->getType() == te::dt::UCHAR_TYPE || raster->getBand(0)->getProperty()->getType() == te::dt::UINT16_TYPE ||
    raster->getBand(0)->getProperty()->getType() == te::dt::UINT32_TYPE || raster->getBand(0)->getProperty()->getType() == te::dt::UINT64_TYPE)
  {
    connect(m_histogramWidget.get(), SIGNAL(minValueSelected(int, int)), this, SLOT(onMinValueSelected(int, int)));
    connect(m_histogramWidget.get(), SIGNAL(maxValueSelected(int, int)), this, SLOT(onMaxValueSelected(int, int)));
  }
  else
  {
    connect(m_histogramWidget.get(), SIGNAL(minValueSelected(double, int)), this, SLOT(onMinValueSelected(double, int)));
    connect(m_histogramWidget.get(), SIGNAL(maxValueSelected(double, int)), this, SLOT(onMaxValueSelected(double, int)));
  }

  listBands();

  m_ui->m_roiRadioButton->setChecked(true);
}

void te::qt::widgets::ContrastDialogForm::setConfigurations()
{
  m_ui->m_newROIPushButton->setChecked(true);
  this->onNewROIPushButtonClicked();
}

void te::qt::widgets::ContrastDialogForm::setMapDisplay(te::qt::widgets::MapDisplay* mapDisplay)
{
  m_mapDisplay = mapDisplay;

  const te::gm::Envelope& env = m_mapDisplay->getExtent();

  QPixmap* draft = m_mapDisplay->getDraftPixmap();
  draft->fill(Qt::transparent);

  if (m_canvas)
  {
    delete m_canvas;
    m_canvas = 0;
  }

  // Prepares the canvas
  m_canvas = new Canvas(m_mapDisplay->width(), m_mapDisplay->height());
  m_canvas->setDevice(draft, false);
  m_canvas->setWindow(env.m_llx, env.m_lly, env.m_urx, env.m_ury);

  m_canvas->setPolygonContourWidth(2);
  m_canvas->setPolygonContourColor(te::color::RGBAColor(100, 177, 216, TE_OPAQUE));
  m_canvas->setPolygonFillColor(te::color::RGBAColor(100, 177, 216, 0));

  connect(m_mapDisplay, SIGNAL(extentChanged()), this, SLOT(onMapDisplayExtentChanged()));
}

te::map::AbstractLayerPtr te::qt::widgets::ContrastDialogForm::getOutputLayer()
{
  return m_outputLayer;
}

te::map::AbstractLayerPtr te::qt::widgets::ContrastDialogForm::get()
{
  return m_layer;
}

te::rp::Contrast::InputParameters te::qt::widgets::ContrastDialogForm::getInputParams()
{
  //get contrast type
  int index = m_ui->m_contrastTypeComboBox->currentIndex();
  int contrastType = m_ui->m_contrastTypeComboBox->itemData(index).toInt();
  int nBands = m_ui->m_bandTableWidget->rowCount();

  te::rp::Contrast::InputParameters algoInputParams;

  if(contrastType == te::rp::Contrast::InputParameters::LinearContrastT)
  {
    algoInputParams.m_type = te::rp::Contrast::InputParameters::LinearContrastT;

    for(int i = 0; i < nBands; ++i)
    {
        QString valueMin = m_ui->m_bandTableWidget->item(i, 1)->text();
        algoInputParams.m_lCMinInput.push_back(valueMin.toDouble());

        QString valueMax = m_ui->m_bandTableWidget->item(i, 2)->text();
        algoInputParams.m_lCMaxInput.push_back(valueMax.toDouble());
    }
  }
  else if(contrastType == te::rp::Contrast::InputParameters::HistogramEqualizationContrastT)
  {
    algoInputParams.m_type = te::rp::Contrast::InputParameters::HistogramEqualizationContrastT;

    for(int i = 0; i < nBands; ++i)
    {
        QString valueMax = m_ui->m_bandTableWidget->item(i, 1)->text();
        algoInputParams.m_hECMaxInput.push_back(valueMax.toDouble());
    }
  }
  else if(contrastType == te::rp::Contrast::InputParameters::MeanAndStdContrastT)
  {
    algoInputParams.m_type = te::rp::Contrast::InputParameters::MeanAndStdContrastT;

    for(int i = 0; i < nBands; ++i)
    {
        QString valueMean = m_ui->m_bandTableWidget->item(i, 1)->text();
        algoInputParams.m_sMASCMeanInput.push_back(valueMean.toDouble());

        QString valueStdDev = m_ui->m_bandTableWidget->item(i, 2)->text();
        algoInputParams.m_sMASCStdInput.push_back(valueStdDev.toDouble());
    }
  }
  else if(contrastType == te::rp::Contrast::InputParameters::SquareContrastT)
  {
    algoInputParams.m_type = te::rp::Contrast::InputParameters::SquareContrastT;

    for(int i = 0; i < nBands; ++i)
    {
        QString valueMin = m_ui->m_bandTableWidget->item(i, 1)->text();
        algoInputParams.m_squareCMinInput.push_back(valueMin.toDouble());

        QString valueMax = m_ui->m_bandTableWidget->item(i, 2)->text();
        algoInputParams.m_squareCMaxInput.push_back(valueMax.toDouble());
      }
  }
  else if(contrastType == te::rp::Contrast::InputParameters::SquareRootContrastT)
  {
    algoInputParams.m_type = te::rp::Contrast::InputParameters::SquareRootContrastT;

    for(int i = 0; i < nBands; ++i)
    {
        QString valueMin = m_ui->m_bandTableWidget->item(i, 1)->text();
        algoInputParams.m_squareRootCMinInput.push_back(valueMin.toDouble());

        QString valueMax = m_ui->m_bandTableWidget->item(i, 2)->text();
        algoInputParams.m_squareRootCMaxInput.push_back(valueMax.toDouble());
    }
  }
  else if(contrastType == te::rp::Contrast::InputParameters::LogContrastT)
  {
    algoInputParams.m_type = te::rp::Contrast::InputParameters::LogContrastT;

    for(int i = 0; i < nBands; ++i)
    {
      QString valueMin = m_ui->m_bandTableWidget->item(i, 1)->text();
      algoInputParams.m_logCMinInput.push_back(valueMin.toDouble());

      QString valueMax = m_ui->m_bandTableWidget->item(i, 2)->text();
      algoInputParams.m_logCMaxInput.push_back(valueMax.toDouble());
    }
  }
  else if(contrastType == te::rp::Contrast::InputParameters::DecorrelationEnhancementT)
  {
    algoInputParams.m_type = te::rp::Contrast::InputParameters::DecorrelationEnhancementT;
  }

  for(int i = 0; i < nBands; ++i)
  {
     algoInputParams.m_inRasterBands.push_back(i);
  }

  return algoInputParams;
}

void te::qt::widgets::ContrastDialogForm::apply()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  //get preview raster
  te::rst::Raster* inputRst = m_navigator->getExtentRaster();

  //set contrast parameters
  te::rp::Contrast::InputParameters algoInputParams = getInputParams();

  algoInputParams.m_inRasterPtr = inputRst;

  te::rp::Contrast::OutputParameters algoOutputParams;

  std::map<std::string, std::string> rinfo;
  rinfo["MEM_RASTER_NROWS"] = boost::lexical_cast<std::string>(inputRst->getNumberOfRows());
  rinfo["MEM_RASTER_NCOLS"] = boost::lexical_cast<std::string>(inputRst->getNumberOfColumns());
  rinfo["MEM_RASTER_DATATYPE"] = boost::lexical_cast<std::string>(inputRst->getBandDataType(0));
  rinfo["MEM_RASTER_NBANDS"] = boost::lexical_cast<std::string>(inputRst->getNumberOfBands());

  algoOutputParams.m_createdOutRasterDSType = "MEM";
  algoOutputParams.m_createdOutRasterInfo = rinfo;

  //run contrast
  te::rp::Contrast algorithmInstance;

  try
  {
    if(algorithmInstance.initialize(algoInputParams))
    {
      if(algorithmInstance.execute(algoOutputParams))
      {
        m_navigator->drawRaster(algoOutputParams.m_outRasterPtr);

        //update histogram and replot
        m_histogramWidget->setOutputRaster(algoOutputParams.m_createdOutRasterPtr.release());

        drawHistogram();
      }
    }
  }
  catch(...)
  {
    QMessageBox::warning(this, tr("Warning"), tr("Constrast error."));
  }

  QApplication::restoreOverrideCursor();
}

void te::qt::widgets::ContrastDialogForm::onAllImageRadioButtonToggled(bool isChecked)
{
  if(isChecked)
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    std::auto_ptr<te::da::DataSet> ds = m_layer->getData();

    if (ds.get())
    {
      std::size_t rpos = te::da::GetFirstPropertyPos(ds.get(), te::dt::RASTER_TYPE);

      m_raster = ds->getRaster(rpos).release();

      m_histogramWidget->setInputRaster(m_raster);

      drawHistogram();

      if (m_ui->m_previewCheckBox->isChecked())
      {
        applyPreview();
      }
    }

    QApplication::restoreOverrideCursor();
  }
}

void te::qt::widgets::ContrastDialogForm::onBlueComboBoxCurrentIndexChanged(int index)
{
  int i = m_ui->m_bandTableWidget->currentRow();
  if (i == index)
    drawHistogram();

  if (m_ui->m_previewCheckBox->isChecked() && m_previewRaster)
    drawPreview(m_previewRaster);
}

void te::qt::widgets::ContrastDialogForm::fillContrastTypes()
{
  m_ui->m_contrastTypeComboBox->clear();

  m_ui->m_contrastTypeComboBox->addItem(tr("Linear"), te::rp::Contrast::InputParameters::LinearContrastT);
  m_ui->m_contrastTypeComboBox->addItem(tr("Histogram Equalization"), te::rp::Contrast::InputParameters::HistogramEqualizationContrastT);
  m_ui->m_contrastTypeComboBox->addItem(tr("Square Constrast"), te::rp::Contrast::InputParameters::SquareContrastT);
  m_ui->m_contrastTypeComboBox->addItem(tr("Square Root Constrast"), te::rp::Contrast::InputParameters::SquareRootContrastT);
  m_ui->m_contrastTypeComboBox->addItem(tr("Log Constrast"), te::rp::Contrast::InputParameters::LogContrastT);
  m_ui->m_contrastTypeComboBox->addItem(tr("Mean and Standard Deviation"), te::rp::Contrast::InputParameters::MeanAndStdContrastT);
  m_ui->m_contrastTypeComboBox->addItem(tr("Decorrelation Enhancement"), te::rp::Contrast::InputParameters::DecorrelationEnhancementT);
}

te::se::ChannelSelection* te::qt::widgets::ContrastDialogForm::getChannelSelection()
{
  te::se::Style* style = m_layer->getStyle();
  assert(style);

  te::se::CoverageStyle* cs = dynamic_cast<te::se::CoverageStyle*>(style);
  assert(cs);

  // get the raster symbolizer
  std::size_t nRules = cs->getRules().size();
  assert(nRules >= 1);

  // for while, consider one rule
  const te::se::Rule* rule = cs->getRule(0);

  const std::vector<te::se::Symbolizer*>& symbolizers = rule->getSymbolizers();
  assert(!symbolizers.empty());

  // for while, consider one raster symbolizer
  te::se::RasterSymbolizer* symbolizer = dynamic_cast<te::se::RasterSymbolizer*>(symbolizers[0]);
  assert(symbolizer);

  return symbolizer->getChannelSelection();
}

void te::qt::widgets::ContrastDialogForm::listBands()
{
  assert(m_layer.get());

  //get input raster
  std::auto_ptr<te::da::DataSet> ds = m_layer->getData();

  if(ds.get())
  {
    std::size_t rpos = te::da::GetFirstPropertyPos(ds.get(), te::dt::RASTER_TYPE);

    std::auto_ptr<te::rst::Raster> inputRst = ds->getRaster(rpos);

    if(inputRst.get())
    {
      m_ui->m_bandTableWidget->setRowCount(inputRst->getNumberOfBands());

      for (unsigned int b = 0; b < inputRst->getNumberOfBands(); ++b)
      {
        std::string band = te::common::Convert2String(b);
        QTableWidgetItem *item = new QTableWidgetItem(QString(band.c_str()));

        m_ui->m_bandTableWidget->setItem(b, 0, item);
      }

      m_channelSelect = getChannelSelection();

      if(m_channelSelect->getColorCompositionType() == te::se::RGB_COMPOSITION)
      {
        m_ui->m_channelTableWidget->setRowCount(3);

        //red channel
        QTableWidgetItem* redChannel = new QTableWidgetItem("Red Channel");
        redChannel->setIcon(QIcon::fromTheme("channel-red").pixmap(16, 16));
        m_ui->m_channelTableWidget->setItem(0, 0, redChannel);

        QTableWidgetItem* item = m_ui->m_channelTableWidget->item(0, 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        m_redComboBox = new QComboBox();
        m_redComboBox->setParent(m_ui->m_channelTableWidget);
        m_ui->m_channelTableWidget->setCellWidget(0, 1, m_redComboBox);

        //green channel
        QTableWidgetItem* greenChannel = new QTableWidgetItem("Green Channel");
        greenChannel->setIcon(QIcon::fromTheme("channel-green").pixmap(16, 16));
        m_ui->m_channelTableWidget->setItem(1, 0, greenChannel);

        item = m_ui->m_channelTableWidget->item(1, 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        m_greenComboBox = new QComboBox();
        m_greenComboBox->setParent(m_ui->m_channelTableWidget);
        m_ui->m_channelTableWidget->setCellWidget(1, 1, m_greenComboBox);

        //blue channel
        QTableWidgetItem* blueChannel = new QTableWidgetItem("Blue Channel");
        blueChannel->setIcon(QIcon::fromTheme("channel-blue").pixmap(16, 16));
        m_ui->m_channelTableWidget->setItem(2, 0, blueChannel);

        item = m_ui->m_channelTableWidget->item(2, 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        m_blueComboBox = new QComboBox();
        m_blueComboBox->setParent(m_ui->m_channelTableWidget);
        m_ui->m_channelTableWidget->setCellWidget(2, 1, m_blueComboBox);

        for(unsigned int b = 0; b < inputRst->getNumberOfBands(); ++b)
        {
          m_redComboBox->addItem(QString::number(b));
          m_greenComboBox->addItem(QString::number(b));
          m_blueComboBox->addItem(QString::number(b));
        }

        std::string name = m_channelSelect->getRedChannel()->getSourceChannelName();
        m_redComboBox->setCurrentText(name.c_str());
        name = m_channelSelect->getGreenChannel()->getSourceChannelName();
        m_greenComboBox->setCurrentText(name.c_str());
        name = m_channelSelect->getBlueChannel()->getSourceChannelName();
        m_blueComboBox->setCurrentText(name.c_str());

        connect(m_redComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onRedComboBoxCurrentIndexChanged(int)));
        connect(m_greenComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onGreenComboBoxCurrentIndexChanged(int)));
        connect(m_blueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onBlueComboBoxCurrentIndexChanged(int)));
      }
      else if(m_channelSelect->getColorCompositionType() == te::se::GRAY_COMPOSITION)
      {
        m_ui->m_channelTableWidget->setRowCount(1);

        //mono channel
        QTableWidgetItem* monoChannel = new QTableWidgetItem("Mono Channel");
        monoChannel->setIcon(QIcon::fromTheme("channel-gray").pixmap(16, 16));
        m_ui->m_channelTableWidget->setItem(0, 0, monoChannel);

        QTableWidgetItem* item = m_ui->m_channelTableWidget->item(0, 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        m_monoComboBox = new QComboBox();
        m_monoComboBox->setParent(m_ui->m_channelTableWidget);
        m_ui->m_channelTableWidget->setCellWidget(0, 1, m_monoComboBox);

        for(unsigned int b = 0; b < inputRst->getNumberOfBands(); ++b)
        {
          m_monoComboBox->addItem(QString::number(b));
        }

        std::string name = m_channelSelect->getGrayChannel()->getSourceChannelName();
        m_monoComboBox->setCurrentText(name.c_str());

        connect(m_monoComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onMonoComboBoxCurrentIndexChanged(int)));
      }
      else if(m_channelSelect->getColorCompositionType() == te::se::RED_COMPOSITION)
      {
        m_ui->m_channelTableWidget->setRowCount(1);

        //red channel
        QTableWidgetItem* redChannel = new QTableWidgetItem("Red Channel");
        redChannel->setIcon(QIcon::fromTheme("channel-red").pixmap(16, 16));
        m_ui->m_channelTableWidget->setItem(0, 0, redChannel);

        QTableWidgetItem* item = m_ui->m_channelTableWidget->item(0, 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        m_redComboBox = new QComboBox();
        m_redComboBox->setParent(m_ui->m_channelTableWidget);
        m_ui->m_channelTableWidget->setCellWidget(0, 1, m_redComboBox);

        for (unsigned int b = 0; b < inputRst->getNumberOfBands(); ++b)
        {
          m_redComboBox->addItem(QString::number(b));
        }

        std::string name = m_channelSelect->getRedChannel()->getSourceChannelName();
        m_redComboBox->setCurrentText(name.c_str());

        connect(m_redComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onRedComboBoxCurrentIndexChanged(int)));
      }
      else if(m_channelSelect->getColorCompositionType() == te::se::GREEN_COMPOSITION)
      {
        m_ui->m_channelTableWidget->setRowCount(1);

        //green channel
        QTableWidgetItem* greenChannel = new QTableWidgetItem("Green Channel");
        greenChannel->setIcon(QIcon::fromTheme("channel-green").pixmap(16, 16));
        m_ui->m_channelTableWidget->setItem(0, 0, greenChannel);

        QTableWidgetItem* item = m_ui->m_channelTableWidget->item(0, 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        m_greenComboBox = new QComboBox();
        m_greenComboBox->setParent(m_ui->m_channelTableWidget);
        m_ui->m_channelTableWidget->setCellWidget(0, 1, m_greenComboBox);

        for (unsigned int b = 0; b < inputRst->getNumberOfBands(); ++b)
        {
          m_greenComboBox->addItem(QString::number(b));
        }

        std::string name = m_channelSelect->getGreenChannel()->getSourceChannelName();
        m_greenComboBox->setCurrentText(name.c_str());

        connect(m_greenComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onGreenComboBoxCurrentIndexChanged(int)));
      }
      else if(m_channelSelect->getColorCompositionType() == te::se::BLUE_COMPOSITION)
      {
        m_ui->m_channelTableWidget->setRowCount(1);

        //blue channel
        QTableWidgetItem* blueChannel = new QTableWidgetItem("Blue Channel");
        blueChannel->setIcon(QIcon::fromTheme("channel-blue").pixmap(16, 16));
        m_ui->m_channelTableWidget->setItem(0, 0, blueChannel);

        QTableWidgetItem* item = m_ui->m_channelTableWidget->item(0, 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        m_blueComboBox = new QComboBox();
        m_blueComboBox->setParent(m_ui->m_channelTableWidget);
        m_ui->m_channelTableWidget->setCellWidget(0, 1, m_blueComboBox);

        for (unsigned int b = 0; b < inputRst->getNumberOfBands(); ++b)
        {
          m_blueComboBox->addItem(QString::number(b));
        }

        std::string name = m_channelSelect->getBlueChannel()->getSourceChannelName();
        m_blueComboBox->setCurrentText(name.c_str());

        connect(m_blueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onBlueComboBoxCurrentIndexChanged(int)));
      }
    }
  }

  QComboBox* cb = dynamic_cast<QComboBox*>(m_ui->m_channelTableWidget->cellWidget(0, 1));
  int index = cb->currentIndex();

  m_ui->m_bandTableWidget->selectRow(index);

  onContrastTypeComboBoxActivated(m_ui->m_contrastTypeComboBox->currentIndex());

  m_ui->m_bandTableWidget->resizeColumnsToContents();
  m_ui->m_channelTableWidget->resizeColumnsToContents();
#if (QT_VERSION >= 0x050000)
  m_ui->m_bandTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  m_ui->m_channelTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
  m_ui->m_bandTableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  m_ui->m_channelTableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif
}

void te::qt::widgets::ContrastDialogForm::applyPreview()
{
  if (!m_raster)
    return;

  //progress
  te::qt::widgets::ProgressViewerDialog v(this);
  int id = te::common::ProgressManager::getInstance().addViewer(&v);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  //set contrast parameters
  te::rp::Contrast::InputParameters algoInputParams = getInputParams();

  algoInputParams.m_inRasterPtr = m_raster;

  te::rp::Contrast::OutputParameters algoOutputParams;

  std::map<std::string, std::string> rinfo;
  rinfo["MEM_RASTER_NROWS"] = boost::lexical_cast<std::string>(m_raster->getNumberOfRows());
  rinfo["MEM_RASTER_NCOLS"] = boost::lexical_cast<std::string>(m_raster->getNumberOfColumns());
  rinfo["MEM_RASTER_DATATYPE"] = boost::lexical_cast<std::string>(m_raster->getBandDataType(0));
  rinfo["MEM_RASTER_NBANDS"] = boost::lexical_cast<std::string>(m_raster->getNumberOfBands());

  algoOutputParams.m_createdOutRasterDSType = "MEM";
  algoOutputParams.m_createdOutRasterInfo = rinfo;

  //run contrast
  te::rp::Contrast algorithmInstance;

  try
  {
    if (algorithmInstance.initialize(algoInputParams))
    {
      if (algorithmInstance.execute(algoOutputParams))
      {
        te::dt::AbstractData* abs = algoOutputParams.m_outRasterPtr->clone();

        te::rst::Raster* rst = static_cast<te::rst::Raster*>(abs);
        
        m_previewRaster = rst;

        double min = m_previewRaster->getBand(0)->getMinValue(true).real();
        double max = m_previewRaster->getBand(0)->getMaxValue(true).real();

        m_histogramWidget->setOutputRaster(algoOutputParams.m_createdOutRasterPtr.release());

        if (m_ui->m_previewCheckBox->isChecked())
          drawPreview(m_previewRaster);
      }
    }
  }
  catch (...)
  {
    QMessageBox::warning(this, tr("Warning"), tr("Constrast error."));

    te::common::ProgressManager::getInstance().removeViewer(id);

    QApplication::restoreOverrideCursor();
  }

  te::common::ProgressManager::getInstance().removeViewer(id);

  QApplication::restoreOverrideCursor();
}

void te::qt::widgets::ContrastDialogForm::getRasterFromROI()
{
  switch (m_geom->getGeomTypeId())
  {
    case te::gm::PolygonType:
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      te::gm::Envelope* env = new te::gm::Envelope(*m_geom->getMBR());

      // verify if is necessary convert the raster to the given srid
      bool needRemap = false;
      if ((m_mapDisplay->getSRID() != TE_UNKNOWN_SRS) && (m_layer->getSRID() != TE_UNKNOWN_SRS) && (m_mapDisplay->getSRID() != m_layer->getSRID()))
        needRemap = true;

      // create a SRS converter
      std::auto_ptr<te::srs::Converter> converter(new te::srs::Converter());

      if (needRemap)
      {
        converter->setSourceSRID(m_mapDisplay->getSRID());
        converter->setTargetSRID(m_layer->getSRID());

        converter->convert(env->getLowerLeftX(), env->getLowerLeftY(), env->m_llx, env->m_lly);
        converter->convert(env->getUpperRightX(), env->getUpperRightY(), env->m_urx, env->m_ury);
      }

      std::auto_ptr<te::da::DataSetType> dsType = m_layer->getSchema();
      te::rst::RasterProperty* rasterProp = te::da::GetFirstRasterProperty(dsType.get());
      std::auto_ptr<te::da::DataSet> dsRaster = m_layer->getData();
      std::auto_ptr<te::rst::Raster> raster = dsRaster->getRaster(rasterProp->getName());

      std::map<std::string, std::string> info;
      info["FORCE_MEM_DRIVER"] = "TRUE";

      //execute clipping
      m_raster = raster->trim(env, info);

      m_histogramWidget->setInputRaster(m_raster);

      drawHistogram();

      if (m_ui->m_previewCheckBox->isChecked() && m_contrast)
      {
        applyPreview();
      }

      QApplication::restoreOverrideCursor();

      break;
    }
  }
}

bool te::qt::widgets::ContrastDialogForm::execute()
{
  if (m_outputWidget->fileExists())
  {
    QMessageBox::warning(this, tr("Contrast"), tr("File already exists."));
    return false;
  }

  //get layer
  te::map::AbstractLayerPtr l = m_layer;
  std::auto_ptr<te::da::DataSet> ds = l->getData();

  std::size_t rpos = te::da::GetFirstPropertyPos(ds.get(), te::dt::RASTER_TYPE);

  std::auto_ptr<te::rst::Raster> inputRst = ds->getRaster(rpos);

  //run contrast
  te::rp::Contrast algorithmInstance;

  te::rp::Contrast::InputParameters algoInputParams = getInputParams();
  algoInputParams.m_inRasterPtr = inputRst.get();
  algoInputParams.m_enableProgress = true;

  te::rp::Contrast::OutputParameters algoOutputParams;
  algoOutputParams.m_createdOutRasterDSType = m_outputWidget->getType();
  algoOutputParams.m_createdOutRasterInfo = m_outputWidget->getInfo();
  algoOutputParams.m_outRasterBands = algoInputParams.m_inRasterBands;

  //progress
  te::qt::widgets::ProgressViewerDialog v(this);
  int id = te::common::ProgressManager::getInstance().addViewer(&v);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  try
  {
    if (algorithmInstance.initialize(algoInputParams))
    {
      if (algorithmInstance.execute(algoOutputParams))
      {
        algoOutputParams.reset();

        //set output layer
        m_outputLayer = te::qt::widgets::createLayer(m_outputWidget->getType(),
          m_outputWidget->getInfo());

        QMessageBox::information(this, tr("Contrast"), tr("Contrast enhencement ended sucessfully"));
      }
      else
      {
        QMessageBox::critical(this, tr("Contrast"), tr("Contrast enhencement execution error.") +
          (" " + te::rp::Module::getLastLogStr()).c_str());

        QApplication::restoreOverrideCursor();

        return false;
      }
    }
    else
    {
      QMessageBox::critical(this, tr("Contrast"), tr("Contrast enhencement initialization error.") +
        (" " + te::rp::Module::getLastLogStr()).c_str());

      QApplication::restoreOverrideCursor();

      return false;
    }
  }
  catch (const std::exception& e)
  {
    QMessageBox::warning(this, tr("Contrast"), e.what());

    te::common::ProgressManager::getInstance().removeViewer(id);

    QApplication::restoreOverrideCursor();

    return false;
  }
  catch (...)
  {
    QMessageBox::warning(this, tr("Contrast"), tr("An exception has occurred!"));

    te::common::ProgressManager::getInstance().removeViewer(id);

    QApplication::restoreOverrideCursor();

    return false;
  }

  te::common::ProgressManager::getInstance().removeViewer(id);

  QApplication::restoreOverrideCursor();

  emit addLayer(m_outputLayer);

  return true;
}

void te::qt::widgets::ContrastDialogForm::closeEvent(QCloseEvent* e)
{
  if (m_previewRaster)
  {
    delete m_previewRaster;
    m_previewRaster = 0;
  }

  if (m_raster)
  {
    delete m_raster;
    m_raster = 0;
  }

  m_canvas->clear();
  m_mapDisplay->repaint();

  if (m_canvas)
  {
    delete m_canvas;
    m_canvas = 0;
  }

  emit closeTool();
}

void te::qt::widgets::ContrastDialogForm::resetWindow(te::map::AbstractLayerPtr layer)
{
  if (!m_canvas)
    return;

  m_contrast = false;

  m_canvas->clear();
  m_mapDisplay->repaint();

  if (m_geom)
  {
    delete m_geom;
    m_geom = 0;
  }

  m_histogramWidget->clear();

  m_ui->m_bandTableWidget->clear();
  m_ui->m_channelTableWidget->clear();

  m_ui->m_roiRadioButton->toggled(true);

  set(layer);

  std::list<te::map::AbstractLayerPtr> layerList;
  layerList.push_back(layer);

  te::gm::Envelope displayExtent = te::map::GetExtent(layerList, m_mapDisplay->getSRID(), true);

  m_mapDisplay->setExtent(displayExtent, false);

  m_mapDisplay->setSRID(layer->getSRID(), false);

  m_mapDisplay->setLayerList(layerList);

  m_mapDisplay->refresh(true);
}

void te::qt::widgets::ContrastDialogForm::drawPreview(te::rst::Raster* raster)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  const te::gm::Envelope& envRaster = *raster->getExtent();
  const te::gm::Envelope& env = m_mapDisplay->getExtent();

  te::se::ChannelSelection* channel = getChannelSelection();

  te::se::Style* style = te::se::CreateCoverageStyle(raster->getNumberOfBands());

  te::se::CoverageStyle* cs = dynamic_cast<te::se::CoverageStyle*>(style);
  assert(cs);

  // get the raster symbolizer
  std::size_t nRules = cs->getRules().size();
  assert(nRules >= 1);

  // for while, consider one rule
  const te::se::Rule* rule = cs->getRule(0);

  const std::vector<te::se::Symbolizer*>& symbolizers = rule->getSymbolizers();
  assert(!symbolizers.empty());

  // for while, consider one raster symbolizer
  te::se::RasterSymbolizer* symbolizer = dynamic_cast<te::se::RasterSymbolizer*>(symbolizers[0]);
  assert(symbolizer);
  
  if (m_channelSelect->getColorCompositionType() == te::se::RGB_COMPOSITION)
  {
    QComboBox* cb = dynamic_cast<QComboBox*>(m_ui->m_channelTableWidget->cellWidget((int)0, 1));
    int redBand = cb->currentIndex();
    cb = dynamic_cast<QComboBox*>(m_ui->m_channelTableWidget->cellWidget((int)1, 1));
    int greenBand = cb->currentIndex();
    cb = dynamic_cast<QComboBox*>(m_ui->m_channelTableWidget->cellWidget((int)2, 1));
    int blueBand = cb->currentIndex();

    te::se::SelectedChannel* redChannel = new te::se::SelectedChannel();
    redChannel->setSourceChannelName(te::common::Convert2String(redBand));
    te::se::SelectedChannel* greenChannel = new te::se::SelectedChannel();
    greenChannel->setSourceChannelName(te::common::Convert2String(greenBand));
    te::se::SelectedChannel* blueChannel = new te::se::SelectedChannel();
    blueChannel->setSourceChannelName(te::common::Convert2String(blueBand));

    te::se::ChannelSelection* channelSel = symbolizer->getChannelSelection()->clone();

    channelSel->setRedChannel(redChannel);
    channelSel->setGreenChannel(greenChannel);
    channelSel->setBlueChannel(blueChannel);

    symbolizer->setChannelSelection(channelSel);
  }
  else if (m_channelSelect->getColorCompositionType() == te::se::RED_COMPOSITION)
  {
    int redBand = m_ui->m_channelTableWidget->item(0, 0)->text().toInt();
  }
  else if (m_channelSelect->getColorCompositionType() == te::se::GREEN_COMPOSITION)
  {
    int greenBand = m_ui->m_channelTableWidget->item(0, 0)->text().toInt();
  }
  else if (m_channelSelect->getColorCompositionType() == te::se::BLUE_COMPOSITION)
  {
    int blueBand = m_ui->m_channelTableWidget->item(0, 0)->text().toInt();
  }
  else if (m_channelSelect->getColorCompositionType() == te::se::GRAY_COMPOSITION)
  {
    int grayBand = m_ui->m_channelTableWidget->item(0, 0)->text().toInt();
  }

  // Draw raster
  te::map::DrawRaster(raster, m_canvas, envRaster, raster->getSRID(), env, m_mapDisplay->getSRID(), cs, m_mapDisplay->getScale());

  if(m_ui->m_roiRadioButton->isChecked())
  {
    if (!m_geom->isValid())
      return;

    m_canvas->draw(m_geom);
  }

  m_mapDisplay->repaint();

  delete style;

  QApplication::restoreOverrideCursor();
}

void te::qt::widgets::ContrastDialogForm::onContrastTypeComboBoxActivated(int index)
{
  int row = m_ui->m_bandTableWidget->currentRow();

  int contrastType = m_ui->m_contrastTypeComboBox->itemData(index).toInt();

  if(contrastType == te::rp::Contrast::InputParameters::LinearContrastT ||
     contrastType == te::rp::Contrast::InputParameters::SquareContrastT ||
     contrastType == te::rp::Contrast::InputParameters::SquareRootContrastT ||
     contrastType == te::rp::Contrast::InputParameters::LogContrastT)
  {
    bool reset = true;

    if(m_ui->m_bandTableWidget->horizontalHeaderItem(2) && m_ui->m_bandTableWidget->horizontalHeaderItem(2)->text() == tr("Minimum") &&
       m_ui->m_bandTableWidget->horizontalHeaderItem(3) && m_ui->m_bandTableWidget->horizontalHeaderItem(3)->text() == tr("Maximum"))
       reset = false;

    if(reset)
    {
      QStringList list;
      list.append(tr("Band"));
      list.append(tr("Minimum"));
      list.append(tr("Maximum"));

      m_ui->m_bandTableWidget->setColumnCount(3);
      m_ui->m_bandTableWidget->setHorizontalHeaderLabels(list);

      int nBands = m_ui->m_bandTableWidget->rowCount();

      for(int i = 0; i < nBands; ++i)
      {
        QTableWidgetItem* itemMin = new QTableWidgetItem(te::common::Convert2String(m_minValue[i], 2).c_str());
        itemMin->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
        m_ui->m_bandTableWidget->setItem(i, 1, itemMin);

        QTableWidgetItem* itemMax = new QTableWidgetItem(te::common::Convert2String(m_maxValue[i], 2).c_str());
        itemMax->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
        m_ui->m_bandTableWidget->setItem(i, 2, itemMax);
      }
    }

    m_histogramWidget->setMinimumValueEnabled(true);
    m_histogramWidget->setMaximumValueEnabled(true);

    m_histogramWidget->updateMinimumValueLabel(tr("Minimum"));
    m_histogramWidget->updateMaximumValueLabel(tr("Maximum"));

    m_histogramWidget->updateMinimumValueLine(m_minValue[row], false);
    m_histogramWidget->updateMaximumValueLine(m_maxValue[row], false);

    m_ui->m_tipLabel->setText(tr("Use left button to set minimum value and right button to define maximum value over the histogram."));
  }
  else if(contrastType == te::rp::Contrast::InputParameters::HistogramEqualizationContrastT)
  {
    QStringList list;
    list.append(tr("Band"));
    list.append(tr("Maximum"));

    m_ui->m_bandTableWidget->setColumnCount(2);
    m_ui->m_bandTableWidget->setHorizontalHeaderLabels(list);

    int nBands = m_ui->m_bandTableWidget->rowCount();

    for(int i = 0; i < nBands; ++i)
    {
      QTableWidgetItem* itemMax = new QTableWidgetItem(te::common::Convert2String(m_maxValue[i], 2).c_str());
      itemMax->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
      m_ui->m_bandTableWidget->setItem(i, 1, itemMax);
    }

    m_histogramWidget->setMinimumValueEnabled(true);
    m_histogramWidget->setMaximumValueEnabled(false);

    m_histogramWidget->updateMinimumValueLabel(tr("Maximum"));

    m_histogramWidget->updateMinimumValueLine(m_maxValue[row], false);

    m_ui->m_tipLabel->setText(tr("Use left button to set maximum value over the histogram."));
  }
  else if(contrastType == te::rp::Contrast::InputParameters::MeanAndStdContrastT)
  {
    QStringList list;
    list.append(tr("Band"));
    list.append(tr("Mean"));
    list.append(tr("Std Dev"));

    m_ui->m_bandTableWidget->setColumnCount(3);
    m_ui->m_bandTableWidget->setHorizontalHeaderLabels(list);

    int nBands = m_ui->m_bandTableWidget->rowCount();

    for(int i = 0; i < nBands; ++i)
    {
      QTableWidgetItem* itemMean = new QTableWidgetItem("127");
      itemMean->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
      m_ui->m_bandTableWidget->setItem(i, 1, itemMean);

      QTableWidgetItem* itemStdDev = new QTableWidgetItem("50");
      itemStdDev->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
      m_ui->m_bandTableWidget->setItem(i, 2, itemStdDev);
    }

    m_histogramWidget->setMinimumValueEnabled(true);
    m_histogramWidget->setMaximumValueEnabled(true);

    m_histogramWidget->updateMinimumValueLabel(tr("Mean"));
    m_histogramWidget->updateMaximumValueLabel(tr("Std Dev"));

    m_histogramWidget->updateMinimumValueLine(127, false);
    m_histogramWidget->updateMaximumValueLine(50, false);

    m_ui->m_tipLabel->setText(tr("Use left button to set Mean value and right button to define Std Dev value over the histogram."));
  }
  else if(contrastType == te::rp::Contrast::InputParameters::DecorrelationEnhancementT)
  {
    QStringList list;
    list.append(tr("Band"));

    m_ui->m_bandTableWidget->setColumnCount(1);
    m_ui->m_bandTableWidget->setHorizontalHeaderLabels(list);

    m_histogramWidget->setMinimumValueEnabled(false);
    m_histogramWidget->setMaximumValueEnabled(false);

    m_ui->m_tipLabel->setText(tr(""));
  }

  if (m_raster)
    drawHistogram();

  m_ui->m_bandTableWidget->resizeColumnsToContents();
#if (QT_VERSION >= 0x050000)
  m_ui->m_bandTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
  m_ui->m_bandTableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif
}

void te::qt::widgets::ContrastDialogForm::onCellClicked(int row, int column)
{
    drawHistogram();
}

void te::qt::widgets::ContrastDialogForm::onCellDoubleClicked(int row, int column)
{
  if (column == 0)
    return;

  m_lastText = m_ui->m_bandTableWidget->item(row, column)->text().toUtf8().data();
}

void te::qt::widgets::ContrastDialogForm::onCellChanged(int row, int column)
{
  if (!isdigit(*m_ui->m_bandTableWidget->item(row, column)->text().toUtf8().data()))
  {
    m_ui->m_bandTableWidget->item(row, column)->setText(m_lastText.c_str());
    return;
  }

  if (m_ui->m_roiRadioButton && (!m_geom || !m_geom->isValid()))
    return;

  int band = m_ui->m_bandTableWidget->item(row, 0)->text().toInt();

  if (column == 1)
  {
      double value = m_ui->m_bandTableWidget->item(row, column)->text().toDouble();
      onMinValueSelected(value, band);
  }
  else if (column == 2)
  {
    double value = m_ui->m_bandTableWidget->item(row, column)->text().toDouble();
    onMaxValueSelected(value, band);
  }

  applyPreview();
  drawHistogram();
}

void te::qt::widgets::ContrastDialogForm::onGreenComboBoxCurrentIndexChanged(int index)
{
  int i = m_ui->m_bandTableWidget->currentRow();
  if (i == index)
    drawHistogram();

  if (m_ui->m_previewCheckBox->isChecked() && m_previewRaster)
    drawPreview(m_previewRaster);
}

void te::qt::widgets::ContrastDialogForm::onMinValueSelected(int value, int band)
{
  if (!m_raster)
    return;

  m_contrast = true;

  double value1;
  int row = m_ui->m_bandTableWidget->currentRow();

  int index = m_ui->m_contrastTypeComboBox->currentIndex();

  int contrastType = m_ui->m_contrastTypeComboBox->itemData(index).toInt();

  if(contrastType == te::rp::Contrast::InputParameters::LinearContrastT ||
     contrastType == te::rp::Contrast::InputParameters::SquareContrastT ||
     contrastType == te::rp::Contrast::InputParameters::SquareRootContrastT ||
     contrastType == te::rp::Contrast::InputParameters::LogContrastT)
  {
    m_ui->m_bandTableWidget->item(row, 1)->setText(QString::number(value));
    m_ui->m_bandTableWidget->setCurrentCell(row, 1);

      m_histogramWidget->updateMinimumValueLine(value, true);
  }
  else if(contrastType == te::rp::Contrast::InputParameters::HistogramEqualizationContrastT)
  {
    m_ui->m_bandTableWidget->item(row, 1)->setText(QString::number(value));
    m_ui->m_bandTableWidget->setCurrentCell(row, 1);

    m_histogramWidget->updateMinimumValueLine(value, true);
  }
  else if(contrastType == te::rp::Contrast::InputParameters::MeanAndStdContrastT)
  {
    m_ui->m_bandTableWidget->item(row, 1)->setText(QString::number(value));
    m_ui->m_bandTableWidget->setCurrentCell(row, 1);

    m_histogramWidget->updateMinimumValueLine(value, true);
  }
  else
  {
    return;
  }
}

void te::qt::widgets::ContrastDialogForm::onMinValueSelected(double value, int band)
{
  if (!m_raster)
    return;

  m_contrast = true;

  double value1;
  int row = m_ui->m_bandTableWidget->currentRow();

  int index = m_ui->m_contrastTypeComboBox->currentIndex();

  int contrastType = m_ui->m_contrastTypeComboBox->itemData(index).toInt();

  if (contrastType == te::rp::Contrast::InputParameters::LinearContrastT ||
    contrastType == te::rp::Contrast::InputParameters::SquareContrastT ||
    contrastType == te::rp::Contrast::InputParameters::SquareRootContrastT ||
    contrastType == te::rp::Contrast::InputParameters::LogContrastT)
  {
    m_ui->m_bandTableWidget->item(row, 1)->setText(QString::number(value));
    m_ui->m_bandTableWidget->setCurrentCell(row, 1);

    m_histogramWidget->updateMinimumValueLine(value, true);
  }
  else if (contrastType == te::rp::Contrast::InputParameters::HistogramEqualizationContrastT)
  {
    m_ui->m_bandTableWidget->item(row, 1)->setText(QString::number(value));
    m_ui->m_bandTableWidget->setCurrentCell(row, 1);

    m_histogramWidget->updateMinimumValueLine(value, true);
  }
  else if (contrastType == te::rp::Contrast::InputParameters::MeanAndStdContrastT)
  {
    m_ui->m_bandTableWidget->item(row, 1)->setText(QString::number(value));
    m_ui->m_bandTableWidget->setCurrentCell(row, 1);

    m_histogramWidget->updateMinimumValueLine(value, true);
  }
  else
  {
    return;
  }
}

void te::qt::widgets::ContrastDialogForm::onMaxValueSelected(int value, int band)
{
  if (!m_raster)
    return;

  m_contrast = true;

  int row = m_ui->m_bandTableWidget->currentRow();

  int index = m_ui->m_contrastTypeComboBox->currentIndex();

  int contrastType = m_ui->m_contrastTypeComboBox->itemData(index).toInt();

  if(contrastType == te::rp::Contrast::InputParameters::LinearContrastT ||
     contrastType == te::rp::Contrast::InputParameters::SquareContrastT ||
     contrastType == te::rp::Contrast::InputParameters::SquareRootContrastT ||
     contrastType == te::rp::Contrast::InputParameters::LogContrastT)
  {
    m_ui->m_bandTableWidget->item(row, 2)->setText(QString::number(value));
    m_ui->m_bandTableWidget->setCurrentCell(row, 2);

    m_histogramWidget->updateMaximumValueLine(value, true);
  }
  else if(contrastType == te::rp::Contrast::InputParameters::MeanAndStdContrastT)
  {
    if (!m_ui->m_bandTableWidget->item(row, 1)->text().isEmpty())
    {
      double mean = m_ui->m_bandTableWidget->item(row, 1)->text().toDouble();

      double stdDev = std::abs(mean - (double)value); 

      m_ui->m_bandTableWidget->item(row, 2)->setText(QString::number(stdDev));
      m_ui->m_bandTableWidget->setCurrentCell(row, 2);

      m_histogramWidget->updateMaximumValueLine(value, true);
    }
  }
  else
  {
    return;
  }
}

void te::qt::widgets::ContrastDialogForm::onMaxValueSelected(double value, int band)
{
  if (!m_raster)
    return;

  m_contrast = true;

  int row = m_ui->m_bandTableWidget->currentRow();

  int index = m_ui->m_contrastTypeComboBox->currentIndex();

  int contrastType = m_ui->m_contrastTypeComboBox->itemData(index).toInt();

  if (contrastType == te::rp::Contrast::InputParameters::LinearContrastT ||
    contrastType == te::rp::Contrast::InputParameters::SquareContrastT ||
    contrastType == te::rp::Contrast::InputParameters::SquareRootContrastT ||
    contrastType == te::rp::Contrast::InputParameters::LogContrastT)
  {
    m_ui->m_bandTableWidget->item(row, 2)->setText(QString::number(value));
    m_ui->m_bandTableWidget->setCurrentCell(row, 2);

    m_histogramWidget->updateMaximumValueLine(value, true);
  }
  else if (contrastType == te::rp::Contrast::InputParameters::MeanAndStdContrastT)
  {
    if (!m_ui->m_bandTableWidget->item(row, 1)->text().isEmpty())
    {
      double mean = m_ui->m_bandTableWidget->item(row, 1)->text().toDouble();

      double stdDev = std::abs(mean - value);

      m_ui->m_bandTableWidget->item(row, 2)->setText(QString::number(stdDev));
      m_ui->m_bandTableWidget->setCurrentCell(row, 2);

      m_histogramWidget->updateMaximumValueLine(value, true);
    }
  }
  else
  {
    return;
  }
}

void te::qt::widgets::ContrastDialogForm::onMonoComboBoxCurrentIndexChanged(int index)
{
  m_ui->m_bandTableWidget->setCurrentCell(0, 1);
  drawHistogram();
}

void te::qt::widgets::ContrastDialogForm::onNewROIPushButtonClicked()
{
  if (m_ui->m_newROIPushButton->isChecked())
  {
    te::qt::widgets::ExtentAcquire* ea = new te::qt::widgets::ExtentAcquire(m_mapDisplay, Qt::ArrowCursor);
    m_mapDisplay->setCurrentTool(ea);

    connect(ea, SIGNAL(extentAcquired(te::gm::Envelope)), this, SLOT(onEnvelopeAcquired(te::gm::Envelope)));
  }
  else
  {
    m_mapDisplay->setCurrentTool(NULL);
  }
}

void te::qt::widgets::ContrastDialogForm::onHistogramCheckBoxToggled(bool isChecked)
{
  m_ui->m_histogramGroupBox->setVisible(isChecked);

  if (!isChecked)
  {
    m_ui->m_histAreaGroupBox->setVisible(false);
  }
    
  this->adjustSize();
}

void te::qt::widgets::ContrastDialogForm::onOkPushButtonClicked()
{
  //check output parameters
  if (m_outputWidget->getOutputRaster().empty())
  {
    QMessageBox::information(this, "Contrast", "Output image is not defined.");
    return;
  }

  if (!execute())
  {
    QMessageBox::information(this, "Contrast", "Error to apply contrast.");
    return;
  }
}

void te::qt::widgets::ContrastDialogForm::onPreviewCheckBoxToggled(bool isChecked)
{
  if (isChecked)
  {
    applyPreview();
  }
  else
  {
    if (m_raster)
      drawPreview(m_raster);
  }
}

void te::qt::widgets::ContrastDialogForm::onRedComboBoxCurrentIndexChanged(int index)
{
  int i = m_ui->m_bandTableWidget->currentRow();
  if (i == index)
    drawHistogram();

  if (m_ui->m_previewCheckBox->isChecked() && m_previewRaster)
    drawPreview(m_previewRaster);
}

void te::qt::widgets::ContrastDialogForm::onRoiRadioButtonToggled(bool isChecked)
{
  if (isChecked)
  {
    m_ui->m_newROIPushButton->setEnabled(true);
  }
  else
  {
    m_ui->m_newROIPushButton->setEnabled(false);
  }
}

void te::qt::widgets::ContrastDialogForm::onVisibleAreaRadioButtonToggled(bool isChecked)
{
  if(isChecked)
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    te::gm::Envelope reprojectedBBOX(m_mapDisplay->getExtent());
    reprojectedBBOX.transform(m_mapDisplay->getSRID(), m_layer->getSRID());

    if (!reprojectedBBOX.intersects(m_layer->getExtent()))
    {
      QMessageBox::warning(this, tr("Warning"), tr("Visible area doesn't intersect with the layer."));
      return;
    }

    te::gm::Envelope ibbox = reprojectedBBOX.intersection(m_layer->getExtent());

    m_geom = 0;

    if (ibbox.isValid())
      m_geom = te::gm::GetGeomFromEnvelope(&ibbox, m_layer->getSRID());

    m_canvas->draw(m_geom);

    m_mapDisplay->repaint();

    getRasterFromROI();

    QApplication::restoreOverrideCursor();
  }
}

void te::qt::widgets::ContrastDialogForm::drawHistogram()
{
  int bandIdx = -1;

  int i = m_ui->m_bandTableWidget->currentRow();

  if(i == -1)
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  bool hasChannel = false;

  if (m_channelSelect->getColorCompositionType() == te::se::RGB_COMPOSITION)
  {
    int band;
    for (int c = 0; c < 3; c++)
    {
      QComboBox* cb = dynamic_cast<QComboBox*>(m_ui->m_channelTableWidget->cellWidget((int)c, 1));
      band = cb->currentIndex();

      if (i == band)
      {
        if (c == 0)
          m_histogramWidget->setChartInputColor(255, 0, 0);
        else if (c == 1)
          m_histogramWidget->setChartInputColor(0, 255, 0);
        else
          m_histogramWidget->setChartInputColor(0, 0, 255);

        hasChannel = true;
      }
    }
  }
  else if (m_channelSelect->getColorCompositionType() == te::se::RED_COMPOSITION)
  {
    QComboBox* cb = dynamic_cast<QComboBox*>(m_ui->m_channelTableWidget->cellWidget((int)0, 1));
    int band = cb->currentIndex();

    if (i == band)
    {
      m_histogramWidget->setChartInputColor(255, 0, 0);
      hasChannel = true;
    }
  }
  else if (m_channelSelect->getColorCompositionType() == te::se::GREEN_COMPOSITION)
  {
    QComboBox* cb = dynamic_cast<QComboBox*>(m_ui->m_channelTableWidget->cellWidget((int)0, 1));
    int band = cb->currentIndex();

    if (i == band)
    {
      m_histogramWidget->setChartInputColor(0, 255, 0);
      hasChannel = true;
    }
  }
  else if (m_channelSelect->getColorCompositionType() == te::se::BLUE_COMPOSITION)
  {
    QComboBox* cb = dynamic_cast<QComboBox*>(m_ui->m_channelTableWidget->cellWidget((int)0, 1));
    int band = cb->currentIndex();

    if (i == band)
    {
      m_histogramWidget->setChartInputColor(0, 0, 255);
      hasChannel = true;
    }
  }
  else if (m_channelSelect->getColorCompositionType() == te::se::GRAY_COMPOSITION)
  {
    QComboBox* cb = dynamic_cast<QComboBox*>(m_ui->m_channelTableWidget->cellWidget((int)0, 1));
    int band = cb->currentIndex();

    if (i == band)
    {
      m_histogramWidget->setChartInputColor(0, 0, 0);
      hasChannel = true;
    }
  }

  if (!hasChannel)
    m_histogramWidget->setChartInputColor(127, 127, 127);

  bandIdx = m_ui->m_bandTableWidget->item(i, 0)->text().toInt();

  int index = m_ui->m_contrastTypeComboBox->currentIndex();

  int contrastType = m_ui->m_contrastTypeComboBox->itemData(index).toInt();

  if(contrastType == te::rp::Contrast::InputParameters::LinearContrastT ||
     contrastType == te::rp::Contrast::InputParameters::SquareContrastT ||
     contrastType == te::rp::Contrast::InputParameters::SquareRootContrastT ||
     contrastType == te::rp::Contrast::InputParameters::LogContrastT)
  {
    QTableWidgetItem* item = m_ui->m_bandTableWidget->item(bandIdx, 1);

    double min = m_ui->m_bandTableWidget->item(bandIdx, 1)->text().toDouble();
    double max = m_ui->m_bandTableWidget->item(bandIdx, 2)->text().toDouble();

    m_histogramWidget->updateMinimumValueLine(min, false);
    m_histogramWidget->updateMaximumValueLine(max, false);
  }
  else if(contrastType == te::rp::Contrast::InputParameters::HistogramEqualizationContrastT)
  {
    double min = m_ui->m_bandTableWidget->item(bandIdx, 1)->text().toDouble();

    m_histogramWidget->updateMinimumValueLine(min, false);
  }
  else if(contrastType == te::rp::Contrast::InputParameters::MeanAndStdContrastT)
  {
    double min = m_ui->m_bandTableWidget->item(bandIdx, 1)->text().toDouble();
    double max = m_ui->m_bandTableWidget->item(bandIdx, 2)->text().toDouble();

    m_histogramWidget->updateMinimumValueLine(min, false);
    m_histogramWidget->updateMaximumValueLine(max, false);
  }

  m_histogramWidget->drawHistogram(bandIdx);

  QApplication::restoreOverrideCursor();
}

void te::qt::widgets::ContrastDialogForm::onApplicationTriggered(te::qt::af::evt::Event* e)
{
  switch (e->m_id)
  {
    case te::qt::af::evt::LAYER_SELECTED:
    {
      te::qt::af::evt::LayerSelected* evt = static_cast<te::qt::af::evt::LayerSelected*>(e);

      te::map::AbstractLayerPtr layer = evt->m_layer;
      assert(layer);

      if (layer == 0)
      {
        return;
      }

      resetWindow(layer);
    }
      break;

    default:
      break;
  }
}

void te::qt::widgets::ContrastDialogForm::onMapDisplayExtentChanged()
{
  if (!m_geom || !m_geom->isValid())
    return;

  delete m_canvas;
  m_canvas = 0;

  const te::gm::Envelope& env = m_mapDisplay->getExtent();

  QPixmap* draft = m_mapDisplay->getDraftPixmap();
  draft->fill(Qt::transparent);

  // Prepares the canvas
  m_canvas = new Canvas(m_mapDisplay->width(), m_mapDisplay->height());
  m_canvas->setDevice(draft, false);
  m_canvas->setWindow(env.m_llx, env.m_lly, env.m_urx, env.m_ury);

  m_canvas->setPolygonContourWidth(2);
  m_canvas->setPolygonContourColor(te::color::RGBAColor(100, 177, 216, TE_OPAQUE));
  m_canvas->setPolygonFillColor(te::color::RGBAColor(100, 177, 216, 0));

  m_canvas->draw(m_geom);

  m_mapDisplay->repaint();

  if (m_ui->m_previewCheckBox->isChecked())
  {
    drawPreview(m_previewRaster);
  }
}

void te::qt::widgets::ContrastDialogForm::onEnvelopeAcquired(te::gm::Envelope env)
{
  if (!env.isValid())
    return;

  if (env.getLowerLeftX() == 0 && env.getLowerLeftY() == 0 && env.getUpperRightX() == 0 && env.getUpperRightY() == 0)
    return;

  // verify if is necessary convert the raster to the given srid
  bool needRemap = false;
  if ((m_mapDisplay->getSRID() != TE_UNKNOWN_SRS) && (m_layer->getSRID() != TE_UNKNOWN_SRS) && (m_mapDisplay->getSRID() != m_layer->getSRID()))
    needRemap = true;

  te::gm::Envelope envOut = env;

  // create a SRS converter
  std::auto_ptr<te::srs::Converter> converter(new te::srs::Converter());

  if (needRemap)
  {
    converter->setSourceSRID(m_mapDisplay->getSRID());
    converter->setTargetSRID(m_layer->getSRID());

    converter->convert(envOut.getLowerLeftX(), envOut.getLowerLeftY(), envOut.m_llx, envOut.m_lly);
    converter->convert(envOut.getUpperRightX(), envOut.getUpperRightY(), envOut.m_urx, envOut.m_ury);
  }

  if (!envOut.intersects(m_layer->getExtent()))
  {
    QMessageBox::warning(this, tr("Warning"), tr("ROI is invalid."));
    return;
  }

  m_geom = 0;

  if (env.isValid())
    m_geom = te::gm::GetGeomFromEnvelope(&env, m_layer->getSRID());

  m_canvas->draw(m_geom);

  m_mapDisplay->repaint();

  getRasterFromROI();
}
