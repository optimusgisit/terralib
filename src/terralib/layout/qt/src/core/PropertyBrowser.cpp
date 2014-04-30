/*  Copyright (C) 2001-2014 National Institute For Space Research (INPE) - Brazil.

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
  \file PropertyBrowser.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "PropertyBrowser.h"

// Qt
#include <QRegExpValidator>
#include <QRegExp>
#include <QWidget>
#include "../../../../../../third-party/qt/propertybrowser/qtvariantproperty.h"
#include "../../../../../../third-party/qt/propertybrowser/qteditorfactory.h"

te::layout::PropertyBrowser::PropertyBrowser(QObject* parent) :
  QObject(parent),
  m_propertyEditor(0),
  m_variantPropertyEditorManager(0),
  m_strDlgManager(0)
{
  createManager();
}

te::layout::PropertyBrowser::~PropertyBrowser()
{
  if(m_propertyEditor)
  {
    delete m_propertyEditor;
    m_propertyEditor = 0;
  }

  if(m_variantPropertyEditorManager)
  {
    delete m_variantPropertyEditorManager;
    m_variantPropertyEditorManager = 0;
  }

  if(m_strDlgManager)
  {
    delete m_strDlgManager;
    m_strDlgManager = 0;
  }
}

void te::layout::PropertyBrowser::createManager()
{
  //Qt - The Property Browser
  m_propertyEditor = new QtTreePropertyBrowser;

  m_variantPropertyEditorManager = new QtVariantPropertyManager;
  connect(m_variantPropertyEditorManager, SIGNAL(valueChanged(QtProperty*, const QVariant &)),
    this, SLOT(propertyEditorValueChanged(QtProperty *, const QVariant &)));

  m_strDlgManager = new QtStringPropertyManager();

  QtVariantEditorFactory* variantPropertyEditorFactory = new QtVariantEditorFactory;

  QtDlgEditorFactory* dlgFactory = new QtDlgEditorFactory();

  connect(dlgFactory, SIGNAL(internalDlg(QWidget *, QtProperty *)), this, SLOT(onSetDlg(QWidget *, QtProperty *)));

  m_propertyEditor->setFactoryForManager(m_strDlgManager, dlgFactory);
  m_propertyEditor->setFactoryForManager(m_variantPropertyEditorManager, variantPropertyEditorFactory);
  m_propertyEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_propertyEditor->setResizeMode(QtTreePropertyBrowser::ResizeToContents);
}

void te::layout::PropertyBrowser::propertyEditorValueChanged( QtProperty *property, const QVariant &value )
{
  QList<QtBrowserItem *> list = m_propertyEditor->items(property);
  changePropertyValue(property, list);
}

void te::layout::PropertyBrowser::updateExpandState()
{
  QList<QtBrowserItem *> list = m_propertyEditor->topLevelItems();
  QListIterator<QtBrowserItem *> it(list);
  while (it.hasNext()) {
    QtBrowserItem *item = it.next();
    QtProperty *prop = item->property();
    m_idToExpanded[m_propertyToId[prop]] = m_propertyEditor->isExpanded(item);
  }
}

void te::layout::PropertyBrowser::clearAll()
{
  updateExpandState();

  QMap<QtProperty *, QString>::ConstIterator itProp = m_propertyToId.constBegin();
  while (itProp != m_propertyToId.constEnd()) {
    delete itProp.key();
    itProp++;
  }
  m_propertyToId.clear();
  m_idToProperty.clear();
}

void te::layout::PropertyBrowser::addPropertyItem(QtProperty *property, const QString &id)
{
  m_propertyToId[property] = id;
  m_idToProperty[id] = property; 
  QtBrowserItem *item = m_propertyEditor->addProperty(property);
  if (m_idToExpanded.contains(id))
    m_propertyEditor->setExpanded(item, m_idToExpanded[id]);
}

void te::layout::PropertyBrowser::onChangeFilter( const QString& filter )
{
  QRegExp rx;
  QString search_text = filter;
  bool doesContain = false;
  search_text.replace(" ","|"); // to make it possible to look up all words given for the search
  rx.setPattern(search_text);
  rx.setCaseSensitivity(Qt::CaseInsensitive);
  
  QList<QtProperty*> list = m_propertyEditor->properties();
  foreach( QtProperty* prop, list) 
  {
    doesContain = false;
    if(prop) 
    {
      doesContain = prop->propertyName().contains(rx);
      changeVisibility(m_propertyEditor->items(prop), doesContain);
    }
  }
}

void te::layout::PropertyBrowser::changeVisibility( QList<QtBrowserItem*> items, bool visible )
{
  foreach(QtBrowserItem* item, items) 
  {
    if(item)
    {
      m_propertyEditor->setItemVisible(item, visible);
    }
  }
}

QtTreePropertyBrowser* te::layout::PropertyBrowser::getPropertyEditor()
{
  return m_propertyEditor;
}

QtVariantPropertyManager* te::layout::PropertyBrowser::getVariantPropertyManager()
{
  return m_variantPropertyEditorManager;
}

bool te::layout::PropertyBrowser::addProperty( Property property )
{
  QtVariantProperty* vproperty = 0;

  /* Colocar condicional, para saber qual o tipo, e qual m�todo to... chamar */
  if(property.getType() == DataTypeString)
  {
    vproperty = m_variantPropertyEditorManager->addProperty(QVariant::String, tr(property.getName().c_str()));
    vproperty->setValue(property.getValue().toString().c_str());
  }

  if(property.getType() == DataTypeDouble)
  {
    vproperty = m_variantPropertyEditorManager->addProperty(QVariant::Double, tr(property.getName().c_str()));
    vproperty->setValue(property.getValue().toDouble());
  }

  if(property.getType() == DataTypeInt)
  {
    vproperty = m_variantPropertyEditorManager->addProperty(QVariant::Int, tr(property.getName().c_str()));
    vproperty->setValue(property.getValue().toInt());
  }

  if(vproperty)
  {
    addPropertyItem(vproperty, QLatin1String(property.getName().c_str()));
    return true;
  }
  return false;
}

bool te::layout::PropertyBrowser::removeProperty( Property property )
{
  return true;
}

te::layout::Property te::layout::PropertyBrowser::getProperty( std::string name )
{
  Property prop;
  return prop;
}

te::layout::Property te::layout::PropertyBrowser::getPropertyFromPosition( int pos )
{
  Property prop;
  return prop;
}