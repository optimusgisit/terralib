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
  \file terralib/qt/widgets/se/StyleControllerWidget.cpp

  \brief A widget used to controll the style se element.
*/

// TerraLib
#include "StyleControllerWidget.h"
#include "../../../core/filesystem/FileSystem.h"
#include "../../../dataaccess/utils/Utils.h"
#include "../../../geometry/GeometryProperty.h"
#include "../../../se/FeatureTypeStyle.h"
#include "../../../se/Font.h"
#include "../../../se/Rule.h"
#include "../../../se/TextSymbolizer.h"
#include "../../../se/Utils.h"
#include "../../../se/Style.h"
#include "../../../se/Symbolizer.h"
#include "../../../se/serialization/xml/Style.h"
#include "../../../xml/AbstractWriter.h"
#include "../../../xml/AbstractWriterFactory.h"
#include "../../../xml/Reader.h"
#include "../../../xml/ReaderFactory.h"
#include "StyleExplorer.h"
#include "Symbol.h"
#include "SymbolPreviewWidget.h"
#include "SymbolSelectorDialog.h"

// TerraLib - UIC
#include "ui_StyleControllerWidgetForm.h"

// Qt
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QString>

// STL
#include <cassert>
#include <fstream>

// Boost
#include <boost/filesystem/operations.hpp>

te::qt::widgets::StyleControllerWidget::StyleControllerWidget(QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f),
    m_ui(new Ui::StyleControllerWidgetForm)
{
  m_ui->setupUi(this);

  // Add the StyleExplorer
  m_explorer = new te::qt::widgets::StyleExplorer(m_ui->m_explorerFrame);
  QGridLayout* le = new QGridLayout(m_ui->m_explorerFrame);
  le->setContentsMargins(0, 0, 0, 0);
  le->addWidget(m_explorer);

  // Signals & slots
  connect(m_ui->m_iconSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(changeLegendIconSize(int)));
  connect(m_ui->m_addTextSymbToolButton, SIGNAL(clicked()), this, SLOT(onAddTextSymbolizerClicked()));
  connect(m_ui->m_addSymbToolButton, SIGNAL(clicked()), this, SLOT(onAddSymbolizerClicked()));
  connect(m_ui->m_removeSymbToolButton, SIGNAL(clicked()), this, SLOT(onRemoveSymbolizerClicked()));
  connect(m_ui->m_upSymbToolButton, SIGNAL(clicked()), this, SLOT(onUpSymbolizerClicked()));
  connect(m_ui->m_downSymbToolButton, SIGNAL(clicked()), this, SLOT(onDownSymbolizerClicked()));
  connect(m_ui->m_libManagerToolButton, SIGNAL(clicked()), this, SLOT(onLibraryManagerClicked()));
  connect(m_ui->m_exportStyleToolButton, SIGNAL(clicked()), this, SLOT(onExportClicked()));
  connect(m_ui->m_importStyleToolButton, SIGNAL(clicked()), this, SLOT(onImportClicked()));
  connect(m_ui->m_mapRefreshToolButton, SIGNAL(clicked()), this, SLOT(onMapRefreshClicked()));
  connect(m_ui->m_visualStyleRadioButton, SIGNAL(clicked(bool)), SLOT(onVisualStyleChecked(bool)));
  connect(m_ui->m_selectionStyleRadioButton, SIGNAL(clicked(bool)), SLOT(onSelectionStyleChecked(bool)));

  updateUi();
}

te::qt::widgets::StyleControllerWidget::~StyleControllerWidget()
{
}

void te::qt::widgets::StyleControllerWidget::setLayer(te::map::AbstractLayer* layer, std::string selColor)
{
  m_currentLayer = layer;

  if (!layer->getSelectionStyle())
  {
    std::auto_ptr<te::da::DataSetType> dsType = layer->getSchema();

    if (dsType->hasGeom())
    {
      te::gm::GeometryProperty* gp = te::da::GetFirstGeomProperty(dsType.get());

      te::se::Symbolizer* symbolizer = te::se::CreateSymbolizer(gp->getGeometryType(), selColor);

      te::se::Rule* rule = new te::se::Rule;

      if (symbolizer != 0)
        rule->push_back(symbolizer);

      te::se::FeatureTypeStyle* style = new te::se::FeatureTypeStyle;
      style->push_back(rule);

      layer->setSelectionStyle(style);

      m_ui->m_selectionStyleRadioButton->setEnabled(true);
    }
    else
    {
      m_ui->m_visualStyleRadioButton->setChecked(true);
      m_ui->m_selectionStyleRadioButton->setEnabled(false);
    }
  }
  else
  {
    m_ui->m_selectionStyleRadioButton->setEnabled(true);
  }

  if (m_ui->m_visualStyleRadioButton->isChecked())
  {
    m_explorer->setStyle(layer->getStyle());
    m_currentStyle = layer->getStyle();
  }
  else
  {
    m_explorer->setStyle(layer->getSelectionStyle());
    m_currentStyle = layer->getSelectionStyle();
  }

}

te::qt::widgets::StyleExplorer* te::qt::widgets::StyleControllerWidget::getStyleExplorer() const
{
  return m_explorer;
}

void te::qt::widgets::StyleControllerWidget::updateUi()
{
  m_ui->m_upSymbToolButton->setIcon(QIcon::fromTheme("go-up").pixmap(16,16));
  m_ui->m_downSymbToolButton->setIcon(QIcon::fromTheme("go-down").pixmap(16,16));
  m_ui->m_addTextSymbToolButton->setIcon(QIcon::fromTheme("list-abc-add").pixmap(16, 16));
  m_ui->m_addSymbToolButton->setIcon(QIcon::fromTheme("list-add").pixmap(16, 16));
  m_ui->m_removeSymbToolButton->setIcon(QIcon::fromTheme("list-remove").pixmap(16,16));
  m_ui->m_libManagerToolButton->setIcon(QIcon::fromTheme("library").pixmap(16,16));
  m_ui->m_importStyleToolButton->setIcon(QIcon::fromTheme("document-open").pixmap(16, 16));
  m_ui->m_exportStyleToolButton->setIcon(QIcon::fromTheme("document-save-as").pixmap(16, 16));
  m_ui->m_mapRefreshToolButton->setIcon(QIcon::fromTheme("map-draw").pixmap(16,16));
}

void te::qt::widgets::StyleControllerWidget::onAddTextSymbolizerClicked()
{
  te::se::Rule* rule = m_explorer->getCurrentRule();

  if (rule == 0)
  {
    QMessageBox::information(this, tr("Style"), tr("Select a rule first."));
    return;
  }

  //creates new text symbolizer representation
  te::se::Fill* fill = te::se::CreateFill("#000000", "1.0");
  te::se::Font* font = te::se::CreateFont("Arial", "8", te::se::Font::StyleNormal, te::se::Font::WeightNormal);
  te::se::Symbolizer* symb = te::se::CreateTextSymbolizer("", fill, font);

  rule->push_back(symb);

  m_explorer->updateStyleTree();
}

void te::qt::widgets::StyleControllerWidget::onAddSymbolizerClicked()
{
  te::se::Rule* rule = m_explorer->getCurrentRule();

  if(rule == 0)
  {
    QMessageBox::information(this, tr("Style"), tr("Select a rule first."));
    return;
  }

  te::se::Symbolizer* symb = m_explorer->getCurrentSymbolizer();
 
  if(symb)
    rule->push_back(symb->clone());
  else
  {
    assert(!rule->getSymbolizers().empty());

    const te::se::Symbolizer* symb = rule->getSymbolizer(0);
    assert(symb);

    rule->push_back(symb->clone());
  }

  m_explorer->updateStyleTree();
}

void te::qt::widgets::StyleControllerWidget::onRemoveSymbolizerClicked()
{
   te::se::Symbolizer* symb = m_explorer->getCurrentSymbolizer();

  if(symb == 0)
  {
    QMessageBox::information(this, tr("Style"), tr("Select a symbol first."));
    return;
  }

  te::se::Rule* rule = m_explorer->getCurrentRule();
  assert(rule);

  if(rule->getSymbolizers().size() == 1)
  {
    QMessageBox::information(this, tr("Style"), tr("The rule must have at least one symbol."));
    return;
  }

  for(std::size_t i = 0; i < rule->getSymbolizers().size(); ++i)
  {
    if(rule->getSymbolizer(i) == symb)
    {
      rule->removeSymbolizer(i);
      break;
    }
  }

  m_explorer->updateStyleTree();
}

void te::qt::widgets::StyleControllerWidget::onUpSymbolizerClicked()
{
  te::se::Symbolizer* symb = m_explorer->getCurrentSymbolizer();

  if(symb == 0)
  {
    QMessageBox::information(this, tr("Style"), tr("Select a symbol first."));
    return;
  }

  m_explorer->goUpSymbolizer();
}

void te::qt::widgets::StyleControllerWidget::onDownSymbolizerClicked()
{
  te::se::Symbolizer* symb = m_explorer->getCurrentSymbolizer();

  if(symb == 0)
  {
    QMessageBox::information(this, tr("Style"), tr("Select a symbol first."));
    return;
  }

  m_explorer->goDownSymbolizer();
}

void te::qt::widgets::StyleControllerWidget::onLibraryManagerClicked()
{
  te::se::Rule* rule = m_explorer->getCurrentRule();

  if(!rule)
  {
    QMessageBox::information(this, tr("Style"), tr("Select a rule first."));
    return;
  }

  Symbol* symbol = te::qt::widgets::SymbolSelectorDialog::getSymbol(this, tr("Symbol Selector"));
  if(symbol == 0)
    return;

  std::vector<te::se::Symbolizer*> symbolizers;
  for(std::size_t i = 0; i < symbol->getSymbolizersCount(); ++i)
    symbolizers.push_back(symbol->getSymbolizer(i)->clone());

  rule->setSymbolizers(symbolizers);

  m_explorer->updateStyleTree();
}

void te::qt::widgets::StyleControllerWidget::onExportClicked()
{  
  QString styleFile = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::currentPath(), "Style file (*.sld)");

  if (styleFile.isEmpty())
    return;

  QFileInfo info(styleFile);

  if (info.suffix().isEmpty())
    styleFile.append(".sld");

  writeStyle(m_currentStyle, styleFile.toUtf8().data());
}

void te::qt::widgets::StyleControllerWidget::onImportClicked()
{
  QString styleFile = QFileDialog::getOpenFileName(this, "Select a style file", QDir::currentPath(), "Style file (*.sld)");

  if (!styleFile.isEmpty())
    readStyle(styleFile.toUtf8().data());
  else
    return;
}

void te::qt::widgets::StyleControllerWidget::onMapRefreshClicked()
{
  emit mapRefresh();
}

void te::qt::widgets::StyleControllerWidget::changeLegendIconSize(int size)
{
  m_explorer->setLegendIconSize(size);
}

void te::qt::widgets::StyleControllerWidget::onVisualStyleChecked(bool state)
{
  if (state && m_currentLayer->getStyle())
  {
    m_explorer->setStyle(m_currentLayer->getStyle());
    m_currentStyle = m_currentLayer->getStyle();
  }
}

void te::qt::widgets::StyleControllerWidget::onSelectionStyleChecked(bool state)
{
  if (state && m_currentLayer->getSelectionStyle())
  {
    m_explorer->setStyle(m_currentLayer->getSelectionStyle());
    m_currentStyle = m_currentLayer->getSelectionStyle();
  }
}

void te::qt::widgets::StyleControllerWidget::writeStyle(const te::se::Style* style, std::string path)
{
  boost::filesystem::wpath file = te::core::FileSystem::absolutePath(path);

  {
    std::auto_ptr<te::xml::AbstractWriter> writer(te::xml::AbstractWriterFactory::make());

    writer->setURI(file.string());
    writer->writeStartDocument("UTF-8", "no");

    writer->writeStartElement("StyledLayerDescriptor");

    writer->writeAttribute("xmlns", "http://www.opengis.net/sld");
    writer->writeAttribute("xmlns:ogc", "http://www.opengis.net/ogc");
    writer->writeAttribute("xmlns:se", "http://www.opengis.net/se");
    writer->writeAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    writer->writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    writer->writeAttribute("xsi:schemaLocation", "http://www.opengis.net/sld StyledLayerDescriptor.xsd");

    writer->writeAttribute("version", style->getVersion());

    writer->writeStartElement("NamedLayer");
    writer->writeStartElement("UserStyle");

    te::se::serialize::Style::getInstance().write(style, *writer.get());

    writer->writeEndElement("UserStyle");
    writer->writeEndElement("NamedLayer");

    writer->writeEndElement("StyledLayerDescriptor");
    writer->writeToFile();
  }
}

void te::qt::widgets::StyleControllerWidget::readStyle(std::string path)
{
  std::auto_ptr<te::se::Style> style;

  boost::filesystem::wpath file = te::core::FileSystem::absolutePath(path);
  {
    std::auto_ptr<te::xml::Reader> reader(te::xml::ReaderFactory::make());
    reader->setValidationScheme(false);

    reader->read(file.string());
    reader->next();

    if (checkSymbolizer(file.string()))
    {
      if ((reader->getNodeType() == te::xml::START_ELEMENT) &&
        (reader->getElementLocalName() == "StyledLayerDescriptor"))
        reader->next();

      if ((reader->getNodeType() == te::xml::START_ELEMENT) &&
        (reader->getElementLocalName() == "NamedLayer"))
        reader->next();

      if ((reader->getNodeType() == te::xml::START_ELEMENT) &&
        (reader->getElementLocalName() == "UserStyle"))
        reader->next();

      if ((reader->getNodeType() == te::xml::START_ELEMENT) &&
        (reader->getElementLocalName() == "FeatureTypeStyle"))
      {
        if (reader->getNodeType() == te::xml::START_ELEMENT)
          style.reset(te::se::serialize::Style::getInstance().read(*reader.get()));
      }
      m_explorer->importStyle(style.release(), m_ui->m_visualStyleRadioButton->isChecked());
    }
    else
      QMessageBox::warning(this, tr("Style Explorer"), tr("The selected theme is not compatible with the current data."));
      return;
  }
}

bool te::qt::widgets::StyleControllerWidget::checkSymbolizer(std::string file)
{
  std::string sldFile;
  std::ifstream t(file);
  std::stringstream buffer;
  buffer << t.rdbuf();

  sldFile = buffer.str();

  std::size_t found = sldFile.find(m_explorer->getCurrentSymbolizer()->getType());

  if (found != std::string::npos)
    return true;
  else
    return false;
}
