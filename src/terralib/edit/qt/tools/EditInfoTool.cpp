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
  \file terralib/edit/qt/tools/EditInfoTool.h

  \brief This class implements a concrete tool for edit attributes of geometry.
*/

//TerraLib
#include "../../../core/encoding/CharEncoding.h"
#include "../../../common/STLUtils.h"
#include "../../../geometry.h"
#include "../../../dataaccess/dataset/ObjectId.h"
#include "../../../dataaccess/dataset/ObjectIdSet.h"
#include "../../../dataaccess/query/In.h"
#include "../../../dataaccess/query/LiteralString.h"
#include "../../../dataaccess/utils/Utils.h"
#include "../../../datatype/AbstractData.h"
#include "../../../datatype/SimpleData.h"
#include "../../../qt/af/events/LayerEvents.h"
#include "../../../qt/af/events/MapEvents.h"
#include "../../../qt/widgets/canvas/MapDisplay.h"
#include "../../../qt/widgets/Utils.h"
#include "../../Feature.h"
#include "../../RepositoryManager.h"
#include "../../Utils.h"
#include "../Renderer.h"
#include "../Utils.h"
#include "EditInfoTool.h"

// Qt
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QGridLayout>
#include <QPushButton>

// STL
#include <cassert>
#include <memory>
#include <iostream>

te::edit::EditInfoTool::EditInfoTool(te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer, QObject* parent)
: GeometriesUpdateTool(display, layer.get(), parent),
  m_restrictivePropertyPos(0),
  m_dialog(new QDialog(display)),
  m_infoWidget(new QTreeWidget(display))
{
  updateCursor();

  QGridLayout* layout = new QGridLayout(m_dialog);

  m_dialog->setWindowTitle(tr("Edit Information"));
  m_dialog->setMinimumSize(300, 300);

  // Setup the widget that will be used to show the informations
  m_infoWidget->setWindowFlags(Qt::Tool);
  m_infoWidget->setAlternatingRowColors(true);
  m_infoWidget->setMinimumSize(250, 250);
  m_infoWidget->setColumnCount(2);

  QStringList labels;
  labels << tr("Property") << tr("Value");
  m_infoWidget->setHeaderLabels(labels);

  layout->addWidget(m_infoWidget,0,0,1,3);
  
  QLayoutItem* layoutItem = new QSpacerItem(40, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  layout->addItem(layoutItem, 1, 0);

  QPushButton* okPushButton = new QPushButton(tr("&Save"));
  QPushButton* cancelPushButton = new QPushButton(tr("&Cancel"));

  layout->addWidget(okPushButton, 1, 1);
  layout->addWidget(cancelPushButton, 1, 2);

  // Signals & slots
  connect(okPushButton, SIGNAL(pressed()), this,  SLOT(onOkPushButtonPressed()));
  connect(cancelPushButton, SIGNAL(pressed()), this, SLOT(onCancelPushButtonPressed()));
  connect(m_infoWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onAttributesTreeWidgetItemDoubleClicked(QTreeWidgetItem*, int)));

}

te::edit::EditInfoTool::~EditInfoTool()
{
  m_dialog->close();

  delete m_dialog;
}

bool te::edit::EditInfoTool::mousePressEvent(QMouseEvent* e)
{
  if (e->button() != Qt::LeftButton)
    return false;

  bool needRemap = false;

  // Clear info widget!
  m_infoWidget->clear();

  pickFeature(m_layer, GetPosition(e));

  if (m_feature == 0)
    return false;

  if (m_feature->getType() == te::edit::TO_ADD)
    return false;

  m_data = m_feature->getData();

  // Build the search envelope
  te::gm::Envelope reprojectedEnvelope = buildEnvelope(GetPosition(e));

  if ((m_layer->getSRID() != TE_UNKNOWN_SRS) &&
    (m_display->getSRID() != TE_UNKNOWN_SRS) &&
    (m_layer->getSRID() != m_display->getSRID()))
  {
    needRemap = true;
    reprojectedEnvelope.transform(m_display->getSRID(), m_layer->getSRID());
  }

  if (!reprojectedEnvelope.within(m_layer->getExtent()))
    return false;

  // Get the property names that compose the object id
  std::vector<std::string> oidPropertyNames;
  te::da::GetOIDPropertyNames(m_layer->getSchema().get(), oidPropertyNames);

  te::da::In* in = new te::da::In(oidPropertyNames[0]);

  in->add(new te::da::LiteralString(m_feature->getId()->getValueAsString()));

  m_dataset = m_layer->getData(in).release();

  if (m_dataset->size() == 0)
    return false;

  getInfo(reprojectedEnvelope);

  m_dialog->show();

  return true;
}

te::gm::Envelope te::edit::EditInfoTool::buildEnvelope(const QPointF& pos)
{
  QPointF pixelOffset(4.0, 4.0);

  QRectF rect(pos - pixelOffset, pos + pixelOffset);

  // Converts rect boundary to world coordinates
  QPointF ll(rect.left(), rect.bottom());
  QPointF ur(rect.right(), rect.top());
  ll = m_display->transform(ll);
  ur = m_display->transform(ur);

  te::gm::Envelope env(ll.x(), ll.y(), ur.x(), ur.y());

  return env;
}

void te::edit::EditInfoTool::pickFeature(const te::map::AbstractLayerPtr& layer, const QPointF& pos)
{
  te::gm::Envelope env = buildEnvelope(pos);

  try
  {
    m_feature = PickFeature(layer, env, m_display->getSRID(), te::edit::TO_UPDATE);
  }
  catch (std::exception& e)
  {
    QMessageBox::critical(m_display, tr("Error"), QString(tr("The geometry cannot be selected from the layer. Details:") + " %1.").arg(e.what()));
  }
}

void te::edit::EditInfoTool::getInfo(const te::gm::Envelope& e)
{
  // Get the property pos that compose the object id
  te::da::GetOIDPropertyPos(m_layer->getSchema().get(), m_restrictivePropertyPos);

  // Get the geometry property position
  m_restrictivePropertyPos.push_back(te::da::GetFirstSpatialPropertyPos(m_dataset));

  // Generates a geometry from the given extent. It will be used to refine the results
  std::auto_ptr<te::gm::Geometry> geometryFromEnvelope(te::gm::GetGeomFromEnvelope(&e, m_layer->getSRID()));

  // The restriction point. It will be used to refine the results
  te::gm::Coord2D center = e.getCenter();
  te::gm::Point point(center.x, center.y, m_layer->getSRID());

  std::size_t gpos;
  gpos = te::da::GetPropertyPos(m_dataset, te::da::GetFirstGeomProperty(m_layer->getSchema().get())->getName());

  if (m_feature->getData().size() == 0)
  {
    // Fills the QTreeWidgetItem
    while (m_dataset->moveNext())
    {
      std::auto_ptr<te::gm::Geometry> g(dynamic_cast<te::gm::Geometry*>(m_feature->getGeometry()->clone()));
      g->setSRID(m_layer->getSRID());

      if (g->contains(&point) || g->crosses(geometryFromEnvelope.get()) || geometryFromEnvelope->contains(g.get()))
      {
        for (std::size_t i = 0; i < m_dataset->getNumProperties(); ++i)
        {
          QTreeWidgetItem* propertyItem = new QTreeWidgetItem;

          int pos = (int)te::da::GetPropertyPos(m_layer->getSchema().get(), m_dataset->getPropertyName(i));
          propertyItem->setText(0, m_dataset->getPropertyName(pos).c_str());

          if (m_dataset->getPropertyDataType(pos) == te::dt::GEOMETRY_TYPE)
            propertyItem->setIcon(0, QIcon::fromTheme("geometry"));

          if (!m_dataset->isNull(pos))
          {
            QString qvalue;

            if (m_dataset->getPropertyDataType(pos) == te::dt::STRING_TYPE)
            {
              std::string value = m_dataset->getString(pos);
              qvalue = QString::fromUtf8(value.c_str());
            }
            else
            {
              if (m_dataset->getPropertyDataType(pos) == te::dt::GEOMETRY_TYPE)
                qvalue = (m_dataset->getAsString(pos).substr(0, 15) + "...").c_str();
              else
                qvalue = m_dataset->getAsString(pos, 3).c_str();
            }

            propertyItem->setText(1, qvalue);
          }
          else // property null value!
            propertyItem->setText(1, tr(""));

          m_infoWidget->addTopLevelItem(propertyItem);

          // fill the m_data
          std::auto_ptr<te::dt::AbstractData> data(m_dataset->getValue(pos));
          m_data[pos] = data.release();
        }

        break;
      }
    }
  }
  else
  {
    while (m_dataset->moveNext())
    {
      std::auto_ptr<te::gm::Geometry> g(m_dataset->getGeometry(gpos));
      g->setSRID(m_layer->getSRID());

      if (g->contains(&point) || g->crosses(geometryFromEnvelope.get()) || geometryFromEnvelope->contains(g.get()))
      {
        std::map<std::size_t, te::dt::AbstractData* > ::iterator it;

        for (std::size_t i = 0; i < m_dataset->getNumProperties(); ++i)
        {
          QTreeWidgetItem* propertyItem = new QTreeWidgetItem;

          int pos = (int)te::da::GetPropertyPos(m_layer->getSchema().get(), m_dataset->getPropertyName(i));
          propertyItem->setText(0, m_dataset->getPropertyName(pos).c_str());

          it = m_data.find(pos);
          if (it != m_data.end())
            propertyItem->setText(1, QString(it->second->toString().c_str()));
          else
          {
            QString qvalue;

            if (m_dataset->getPropertyDataType(pos) == te::dt::GEOMETRY_TYPE)
            {
              propertyItem->setIcon(0, QIcon::fromTheme("geometry"));
              qvalue = (m_dataset->getAsString(pos).substr(0, 15) + "...").c_str();
            }
            else
              qvalue = m_dataset->getAsString(pos, 3).c_str();

            propertyItem->setText(1, qvalue);

          }

          m_infoWidget->addTopLevelItem(propertyItem);
        }
      }
    }
  }

  draw(); // to show feedback!
}

// Hack from http://stackoverflow.com/a/13374558 to making only one column of a QTreeWidgetItem editable
void te::edit::EditInfoTool::onAttributesTreeWidgetItemDoubleClicked(QTreeWidgetItem* item, int column)
{
  bool isrestrictive = false;

  for (std::size_t i = 0; i < m_restrictivePropertyPos.size(); ++i)
  {
    if ((int)m_restrictivePropertyPos[i] == m_infoWidget->currentIndex().row())
    {
      isrestrictive = true;
      break;
    }
  }

  Qt::ItemFlags tmp = item->flags();
  if (column == 1 && !isrestrictive)
    item->setFlags(tmp | Qt::ItemIsEditable);
  else if (tmp & Qt::ItemIsEditable)
    item->setFlags(tmp ^ Qt::ItemIsEditable);

}

std::auto_ptr<te::dt::AbstractData> te::edit::EditInfoTool::getValue(int type, QString value) const
{
  switch (type)
  {
  case te::dt::INT16_TYPE:
    return std::auto_ptr<te::dt::AbstractData>(new te::dt::Int16(atoi(value.toUtf8().data())));

  case te::dt::INT32_TYPE:
    return std::auto_ptr<te::dt::AbstractData>(new te::dt::Int32(atoi(value.toUtf8().data())));

  case te::dt::INT64_TYPE:
    return std::auto_ptr<te::dt::AbstractData>(new te::dt::Int64(atoi(value.toUtf8().data())));

  case te::dt::FLOAT_TYPE:
    return std::auto_ptr<te::dt::AbstractData>(new te::dt::Float(atof(value.toUtf8().data())));

  case te::dt::DOUBLE_TYPE:
    return std::auto_ptr<te::dt::AbstractData>(new te::dt::Double(atof(value.toUtf8().data())));

  case te::dt::NUMERIC_TYPE:
    return std::auto_ptr<te::dt::AbstractData>(new te::dt::Numeric(value.toUtf8().data()));

  case te::dt::STRING_TYPE:
    return std::auto_ptr<te::dt::AbstractData>(new te::dt::String(value.toUtf8().data()));

  default:
    return std::auto_ptr<te::dt::AbstractData>(0);
  }
}

void te::edit::EditInfoTool::onOkPushButtonPressed()
{
  if (m_feature == 0)
    return;

  m_dataset->moveBeforeFirst();

  if (m_dataset->moveNext())
  {
    int level_item = 0;
    for (std::map<std::size_t, te::dt::AbstractData*>::const_iterator it = m_data.begin(); it != m_data.end(); ++it)
    {
      QTreeWidgetItem* propertyItem = m_infoWidget->topLevelItem(level_item);

      std::auto_ptr<te::dt::AbstractData> data(getValue(m_dataset->getPropertyDataType(it->first), propertyItem->text(1)));

      m_data[it->first] = data.release();

      m_dataset->getValue(it->first);

      level_item++;
    }
  }

  storeFeature();
}

void te::edit::EditInfoTool::onCancelPushButtonPressed()
{
  m_dialog->close();
}

void te::edit::EditInfoTool::draw()
{
  const te::gm::Envelope& env = m_display->getExtent();
  if (!env.isValid())
    return;

  // Clear!
  QPixmap* draft = m_display->getDraftPixmap();
  draft->fill(Qt::transparent);

  // Initialize the renderer
  Renderer& renderer = Renderer::getInstance();
  renderer.begin(draft, env, m_display->getSRID());

  // Draw the layer edited geometries
  renderer.drawRepository(m_layer->getId(), env, m_display->getSRID());

  if (m_feature == 0)
  {
    renderer.end();
    m_display->repaint();
    return;
  }

  // Draw the current geometry
  renderer.setPolygonStyle(QColor(0, 255, 0, 80), Qt::red, 3);
  renderer.draw(m_feature->getGeometry(), false);

  renderer.end();

  m_display->repaint();

}

void te::edit::EditInfoTool::updateCursor()
{
  m_display->setCursor(Qt::WhatsThisCursor);
}

void te::edit::EditInfoTool::storeFeature()
{
  m_feature->setData(m_data);
  m_feature->setType(te::edit::TO_UPDATE);

  RepositoryManager::getInstance().addFeature(m_layer->getId(), m_feature->clone());
}