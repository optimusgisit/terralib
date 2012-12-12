/*  Copyright (C) 2011-2012 National Institute For Space Research (INPE) - Brazil.

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
  : QTreeWidget(parent)
{
  // Setup
  setAlternatingRowColors(true);

  // Signals & slots
  connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(onItemClicked(QTreeWidgetItem*, int)));
}

te::qt::widgets::StyleExplorer::~StyleExplorer()
{
}

void te::qt::widgets::StyleExplorer::setStyle(te::se::Style* style)
{
  assert(style);

  m_style = style;

  initialize();
}

void te::qt::widgets::StyleExplorer::initialize()
{
  clear();
  setColumnCount(1);
  setHeaderLabel(tr("Style Explorer"));

  QSize iconSize(32, 16);
  setIconSize(iconSize);

  QTreeWidgetItem* root = new QTreeWidgetItem(this, STYLE);
  root->setText(0, m_style->getType().c_str());

  std::size_t nRules = m_style->getNRules();
  for(std::size_t i = 0; i < nRules; ++i) // for each rule
  {
    const te::se::Rule* rule = m_style->getRule(i);
    std::vector<te::se::Symbolizer*> symbs = rule->getSymbolizers();

    QTreeWidgetItem* ruleItem = new QTreeWidgetItem(root, RULE);
    ruleItem->setText(0, tr("Rule"));
    ruleItem->setData(0, Qt::UserRole, (int)i);
    ruleItem->setIcon(0, QIcon(SymbologyPreview::build(symbs, iconSize)));

    for(std::size_t j = 0; j < symbs.size(); ++j) // for each symbolizer
    {
      QTreeWidgetItem* symbItem = new QTreeWidgetItem(ruleItem, SYMBOLIZER);
      symbItem->setText(0, tr(symbs[j]->getType().c_str()));
      symbItem->setData(0, Qt::UserRole, (int)j);
      symbItem->setIcon(0, QIcon(SymbologyPreview::build(symbs[j], iconSize)));
    }
  }

  expandAll();
}

const te::se::Rule* te::qt::widgets::StyleExplorer::getRule(QTreeWidgetItem* item) const
{
  assert(item && item->type() == RULE);
  
  // Gets the Rule index
  std::size_t index = item->data(0, Qt::UserRole).toUInt();
  assert(index >= 0 && index < m_style->getNRules());
  
  return m_style->getRule(index);
}

te::se::Symbolizer* te::qt::widgets::StyleExplorer::getSymbolizer(QTreeWidgetItem* item) const
{
  assert(item && item->type() == SYMBOLIZER);

  // Gets the rule associated with the symbolizer
  QTreeWidgetItem* parent = item->parent();
  assert(parent);
  const te::se::Rule* rule = getRule(parent);
  assert(rule);

  // Gets the rule symbolizers
  std::vector<te::se::Symbolizer*> symbs = rule->getSymbolizers();

  // Gets the Symbolizer index
  std::size_t index = item->data(0, Qt::UserRole).toUInt();
  assert(index >= 0 && index < symbs.size());

  return symbs[index];
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
