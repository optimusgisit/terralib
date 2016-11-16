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
  \file terralib/qt/widgets/se/StyleExplorer.cpp

  \brief A widget used to explore a style.
*/

// TerraLib
#include "../../../se/Style.h"
#include "../../../se/Symbolizer.h"
#include "../../../se/Rule.h"
#include "StyleExplorer.h"
#include "SymbologyPreview.h"

// STL
#include <cassert>
#include <vector>

te::qt::widgets::StyleExplorer::StyleExplorer(QWidget* parent)
  : QTreeWidget(parent),
  m_style(0)
{
  // Setup
  setAlternatingRowColors(true);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setHeaderHidden(true);
  setIconSize(QSize(16, 16));

  m_symbolizerNames["PolygonSymbolizer"] = tr("Polygon Symbol");
  m_symbolizerNames["LineSymbolizer"] = tr("Line Symbol");
  m_symbolizerNames["PointSymbolizer"] = tr("Point Symbol");
  m_symbolizerNames["RasterSymbolizer"] = tr("Raster Symbol");
  m_symbolizerNames["TextSymbolizer"] = tr("Text Symbol");

  // Signals & slots
  connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(onItemClicked(QTreeWidgetItem*, int)));
}

te::qt::widgets::StyleExplorer::~StyleExplorer()
{
}

void te::qt::widgets::StyleExplorer::setStyle(te::se::Style* style)
{
  m_style = style;

  updateStyleTree();
}

void te::qt::widgets::StyleExplorer::updateStyleTree()
{
  clear();

  if(!m_style)
    return;

  QTreeWidgetItem* root = new QTreeWidgetItem(this, STYLE);
  root->setText(0, tr("Style"));

  std::size_t nRules = m_style->getRules().size();

  for(std::size_t i = 0; i < nRules; ++i) // for each rule
  {
    const te::se::Rule* rule = m_style->getRule(i);
    const std::vector<te::se::Symbolizer*>& symbs = rule->getSymbolizers();

    QTreeWidgetItem* ruleItem = new QTreeWidgetItem(root, RULE);
    ruleItem->setText(0, tr("Rule"));
    ruleItem->setData(0, Qt::UserRole, (int)i);
    ruleItem->setIcon(0, QIcon(SymbologyPreview::build(symbs, iconSize())));

    for(std::size_t j = 0; j < symbs.size(); ++j) // for each symbolizer
    {
      QTreeWidgetItem* symbItem = new QTreeWidgetItem(ruleItem, SYMBOLIZER);

      QString symbTypeName = tr("Unknown Symbol");

      std::map<QString, QString>::iterator it = m_symbolizerNames.find(symbs[j]->getType().c_str());
      if(it != m_symbolizerNames.end())
        symbTypeName = it->second;

      QString count;
      count.setNum(j);

      symbTypeName.append(" " + count);

      symbItem->setText(0, symbTypeName);
      symbItem->setData(0, Qt::UserRole, static_cast<int>(j));
      symbItem->setIcon(0, QIcon(SymbologyPreview::build(symbs[j], iconSize())));

      if(j == 0)
      {
        setCurrentItem(symbItem);
        emit symbolizerClicked(symbs[j]);
      }
    }
  }

  expandAll();
}

te::se::Rule* te::qt::widgets::StyleExplorer::getCurrentRule()
{
  QTreeWidgetItem* selectedItem = getSelectedItem();

  if(selectedItem == 0)
    return 0;

  if(selectedItem->type() == RULE)
    return getRule(selectedItem);

  if(selectedItem->type() == SYMBOLIZER)
    return getRule(selectedItem->parent());

  return 0;
}

te::se::Symbolizer* te::qt::widgets::StyleExplorer::getCurrentSymbolizer()
{
  QTreeWidgetItem* selectedItem = getSelectedItem();

  if(selectedItem != 0 && selectedItem->type() == SYMBOLIZER)
    return getSymbolizer(selectedItem);

  return 0;
}

void te::qt::widgets::StyleExplorer::goUpSymbolizer()
{
  QTreeWidgetItem* currentItem = getSelectedItem();

  if(currentItem == 0 || currentItem->type() != SYMBOLIZER)
    return;

  // Gets the symbolizer index
  std::size_t index = currentItem->data(0, Qt::UserRole).toUInt();

  // Checks if the current symbolizer is the first
  if(index == 0)
    return;

  te::se::Rule* rule = getCurrentRule();
  assert(rule);

  swapSymbolizers(rule, index, index - 1);

  updateStyleTree();
}

void te::qt::widgets::StyleExplorer::goDownSymbolizer()
{
  QTreeWidgetItem* currentItem = getSelectedItem();

  if(currentItem == 0 || currentItem->type() != SYMBOLIZER)
    return;

  te::se::Rule* rule = getCurrentRule();
  assert(rule);

  // Gets the symbolizer index
  std::size_t index = currentItem->data(0, Qt::UserRole).toUInt();

  // Checks if the current symbolizer is the last
  if(index == rule->getSymbolizers().size() - 1)
    return;

  swapSymbolizers(rule, index, index + 1);

  updateStyleTree();
}

void te::qt::widgets::StyleExplorer::setLegendIconSize(int size)
{
  setIconSize(QSize(size, size));

  updateStyleTree();
}

void te::qt::widgets::StyleExplorer::importStyle(te::se::Style* style, bool isVisual)
{
  emit styleImported(style, isVisual);
}

void te::qt::widgets::StyleExplorer::onSymbolizerChanged(te::se::Symbolizer* symb)
{
  QTreeWidgetItem* symbolizerItem = getSelectedItem();

  // Updating item
  symbolizerItem->setIcon(0, QIcon(SymbologyPreview::build(symb, iconSize())));

  te::se::Rule* rule = getCurrentRule();
  QTreeWidgetItem* ruleItem = symbolizerItem->parent();

  if(rule && ruleItem)
  {
    const std::vector<te::se::Symbolizer*>& symbs = rule->getSymbolizers();
    ruleItem->setIcon(0, QIcon(SymbologyPreview::build(symbs, iconSize())));
  }
}

te::se::Rule* te::qt::widgets::StyleExplorer::getRule(QTreeWidgetItem* item)
{
  assert(item && item->type() == RULE);
  
  // Gets the Rule index
  std::size_t index = item->data(0, Qt::UserRole).toUInt();
  assert((index >= 0) && (index < m_style->getRules().size()));
  
  return m_style->getRule(index);
}

te::se::Symbolizer* te::qt::widgets::StyleExplorer::getSymbolizer(QTreeWidgetItem* item)
{
  assert(item && item->type() == SYMBOLIZER);

  // Gets the rule associated with the symbolizer
  QTreeWidgetItem* parent = item->parent();
  assert(parent);

  const te::se::Rule* rule = getRule(parent);
  assert(rule);

  // Gets the rule symbolizers
  const std::vector<te::se::Symbolizer*>& symbs = rule->getSymbolizers();

  // Gets the Symbolizer index
  std::size_t index = item->data(0, Qt::UserRole).toUInt();
  assert(index >= 0 && index < symbs.size());

  return symbs[index];
}

QTreeWidgetItem* te::qt::widgets::StyleExplorer::getSelectedItem() const
{
  QList<QTreeWidgetItem*> selected = selectedItems();

  if(selected.empty())
    return 0;

  return selected.at(0);
}

void te::qt::widgets::StyleExplorer::swapSymbolizers(te::se::Rule* r, int indexFirst, int indexSecond)
{
  assert(r);

  // Copy the symbolizers
  te::se::Symbolizer* first = r->getSymbolizer(indexFirst)->clone();
  te::se::Symbolizer* second = r->getSymbolizer(indexSecond)->clone();

  // Swap symbolizers
  r->setSymbolizer(indexFirst, second);
  r->setSymbolizer(indexSecond, first);
}

void te::qt::widgets::StyleExplorer::onItemClicked(QTreeWidgetItem* item, int /*column*/)
{
  switch(item->type())
  {
    case STYLE:
    break;

    case RULE:
      emit ruleClicked(getRule(item));
    break;

    case SYMBOLIZER:
      emit symbolizerClicked(getSymbolizer(item));
    break;
  }
}
