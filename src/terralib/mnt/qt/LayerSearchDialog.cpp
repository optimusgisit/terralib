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
\file terralib/mnt/qt/LayerSearchDialog.cpp

\brief A dialog for Layer Search
*/

//terralib
#include "LayerSearchDialog.h"
#include "ui_LayerSearchDialogForm.h"

#include "../../dataaccess/datasource/DataSourceInfoManager.h"
#include "../../dataaccess/utils/Utils.h"
#include "../../geometry/GeometryProperty.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>


te::mnt::LayerSearchDialog::LayerSearchDialog(QWidget* parent, Qt::WindowFlags f, bool mnt)
: QDialog(parent, f),
m_ui(new Ui::LayerSearchDialogForm)
{
  // add controls
  m_ui->setupUi(this);

  //signals

  connect(m_ui->m_startsRadioButton, SIGNAL(toggled(bool)), this, SLOT(onstartsEnabled(bool)));
  connect(m_ui->m_endsRadioButton, SIGNAL(toggled(bool)), this, SLOT(onendsEnabled(bool)));
  connect(m_ui->m_containsRadioButton, SIGNAL(toggled(bool)), this, SLOT(oncontainsEnabled(bool)));
  connect(m_ui->m_equalsRadioButton, SIGNAL(toggled(bool)), this, SLOT(onequalsEnabled(bool)));
  connect(m_ui->m_nameLineEdit, SIGNAL(editingFinished()), this, SLOT(onnameLineEditEditingFinished()));

  connect(m_ui->m_isolinesRadioButton, SIGNAL(toggled(bool)), this, SLOT(onisolinesEnabled(bool)));
  connect(m_ui->m_samplesRadioButton, SIGNAL(toggled(bool)), this, SLOT(onsamplesEnabled(bool)));
  connect(m_ui->m_gridRadioButton, SIGNAL(toggled(bool)), this, SLOT(ongridEnabled(bool)));
  connect(m_ui->m_tinRadioButton, SIGNAL(toggled(bool)), this, SLOT(ontinEnabled(bool)));
  connect(m_ui->m_othersRadioButton, SIGNAL(toggled(bool)), this, SLOT(onothersEnabled(bool)));
  connect(m_ui->m_allRadioButton, SIGNAL(toggled(bool)), this, SLOT(onallEnabled(bool)));

  connect(m_ui->m_layersTable, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(onOkPushButtonClicked()));

  connect(m_ui->m_helpPushButton, SIGNAL(clicked()), this, SLOT(onHelpPushButtonClicked()));
  connect(m_ui->m_okPushButton, SIGNAL(clicked()), this, SLOT(onOkPushButtonClicked()));
  connect(m_ui->m_cancelPushButton, SIGNAL(clicked()), this, SLOT(onCancelPushButtonClicked()));
  
  if (mnt)
    m_geom_name << "Samples" << "Isolines" << "TIN" << "Undefined" << "Grid";
  else
    m_geom_name << "Points" << "Lines" << "Polygons" << "Undefined" << "Raster";

}

te::mnt::LayerSearchDialog::~LayerSearchDialog()
{
}

void te::mnt::LayerSearchDialog::setLayers(std::list<te::map::AbstractLayerPtr> layers)
{
  m_layers = layers;
  m_ui->m_layersTable->clear();

  std::list<te::map::AbstractLayerPtr>::iterator it = m_layers.begin();

  int i = 1; //to starts from 1 in interface, but real index is 0

  while (it != m_layers.end())
  {
    if (it->get())
    {
      QTreeWidgetItem* item = new QTreeWidgetItem(m_ui->m_layersTable);
      item->setData(0, Qt::DisplayRole, i);
      //layer name
      item->setText(1, it->get()->getTitle().c_str());

      std::auto_ptr<te::da::DataSetType> dsType = it->get()->getSchema();
      //layer type
      if (dsType->hasRaster())
        item->setText(2, m_geom_name.at(4));
      else if (dsType->hasGeom())
      {
        std::auto_ptr<te::gm::GeometryProperty>geomProp(te::da::GetFirstGeomProperty(dsType.get()));
        item->setText(2, GetGeometryName(geomProp->getGeometryType()));
      }
      else
        item->setText(2, "");

      dsType.release();

      // Get the data souce info
      te::da::DataSourceInfoPtr info = te::da::DataSourceInfoManager::getInstance().get(it->get()->getDataSourceId());
      item->setText(3, info->getConnInfoAsString().c_str());
    }
    ++it;
    ++i;
  }
  m_ui->m_layersTable->resizeColumnToContents(0);
  m_ui->m_layersTable->resizeColumnToContents(1);
  m_ui->m_layersTable->resizeColumnToContents(2);
  m_ui->m_layersTable->resizeColumnToContents(3);
  m_ui->m_layersTable->sortItems(0, Qt::AscendingOrder);

}

void te::mnt::LayerSearchDialog::setActive(const QList<mntType> &types)
{
  m_ui->m_isolinesRadioButton->setDisabled(true);
  m_ui->m_samplesRadioButton->setDisabled(true);
  m_ui->m_gridRadioButton->setDisabled(true);
  m_ui->m_tinRadioButton->setDisabled(true);
  m_ui->m_othersRadioButton->setDisabled(true);
  m_ui->m_allRadioButton->setDisabled(true);

  for (int i = 0; i < types.size(); i++)
  {
    mntType t = types.at(i);
    switch (t)
    {
    case SAMPLE:
      m_ui->m_samplesRadioButton->setDisabled(false);
      m_ui->m_samplesRadioButton->setChecked(true);
      break;
    case ISOLINE:
      m_ui->m_isolinesRadioButton->setDisabled(false);
      m_ui->m_isolinesRadioButton->setChecked(true);
      break;
    case GRID:
      m_ui->m_gridRadioButton->setDisabled(false);
      m_ui->m_gridRadioButton->setChecked(true);
      break;
    case TIN:
      m_ui->m_tinRadioButton->setDisabled(false);
      m_ui->m_tinRadioButton->setChecked(true);
      break;
    case OTHER:
      m_ui->m_othersRadioButton->setDisabled(false);
      m_ui->m_othersRadioButton->setChecked(true);
    }
  }

  if (types.size() < 2)
    m_ui->m_represbuttonGroup->setVisible(false);
  else
  {
    m_ui->m_represbuttonGroup->setVisible(true);
    m_ui->m_allRadioButton->setDisabled(false);
  }

}

te::map::AbstractLayerPtr te::mnt::LayerSearchDialog::getLayer()
{
  return m_selectLayer;
}


const QString& te::mnt::LayerSearchDialog::GetGeometryName(te::gm::GeomType t)
{
  switch (t)
  {
  case te::gm::PointType:
  case te::gm::PointZType:
  case te::gm::PointZMType:
  case te::gm::PointMType:
  case te::gm::MultiPointType:
  case te::gm::MultiPointZType:
  case te::gm::MultiPointZMType:
  case te::gm::MultiPointMType:
    return m_geom_name.at(0);

  case te::gm::LineStringType:
  case te::gm::LineStringZType:
  case te::gm::LineStringZMType:
  case te::gm::LineStringMType:
  case te::gm::MultiLineStringType:
  case te::gm::MultiLineStringZType:
  case te::gm::MultiLineStringZMType:
  case te::gm::MultiLineStringMType:
    return m_geom_name.at(1);

  case te::gm::PolygonType:
  case te::gm::PolygonZType:
  case te::gm::PolygonZMType:
  case te::gm::PolygonMType:
  case te::gm::GeometryCollectionType:
  case te::gm::GeometryCollectionZType:
  case te::gm::GeometryCollectionZMType:
  case te::gm::GeometryCollectionMType:
  case te::gm::MultiPolygonType:
  case te::gm::MultiPolygonZType:
  case te::gm::MultiPolygonZMType:
  case te::gm::MultiPolygonMType:
    return m_geom_name.at(2);

  case te::gm::TINType:
  case te::gm::PolyhedralSurfaceType:
  case te::gm::TINZType:
  case te::gm::PolyhedralSurfaceZType:
  case te::gm::GeometryType:
    return m_geom_name.at(2);
  default:
    return m_geom_name.at(3);
  }
  return m_geom_name.at(3);
}

void te::mnt::LayerSearchDialog::ApplyFilter()
{
  if (m_namefilter.isEmpty() && m_geomfilter.isEmpty())
  {
    QTreeWidgetItemIterator it(m_ui->m_layersTable);
    while (*it) {
      (*it)->setHidden(false);
      ++it;
    }
    return;
  }

  QTreeWidgetItemIterator it(m_ui->m_layersTable);
  while (*it) {
    (*it)->setHidden(true);
    ++it;
  }

  QList<QTreeWidgetItem *> select_itens1 = m_namefilter.isEmpty() ? m_ui->m_layersTable->findItems("*", Qt::MatchWildcard, 1) : \
                                                                    m_ui->m_layersTable->findItems(m_namefilter, m_matchflags, 1);
  QList<QTreeWidgetItem *> select_itens2 = m_geomfilter.isEmpty() ? m_ui->m_layersTable->findItems("*", Qt::MatchWildcard, 2) : \
                                                                    m_ui->m_layersTable->findItems(m_geomfilter, Qt::MatchRegExp, 2);
  for (int i = 0; i < select_itens1.size(); i++)
  {
    QTreeWidgetItem *item = select_itens1.at(i);
    if (select_itens2.contains(item))
      item->setHidden(false);
  }
}

void te::mnt::LayerSearchDialog::onstartsEnabled(bool)
{
  m_matchflags = Qt::MatchStartsWith;
  ApplyFilter();
}

void te::mnt::LayerSearchDialog::onendsEnabled(bool)
{
  m_matchflags = Qt::MatchEndsWith;
  ApplyFilter();

}

void te::mnt::LayerSearchDialog::oncontainsEnabled(bool)
{
  m_matchflags = Qt::MatchContains;
  ApplyFilter();
}

void te::mnt::LayerSearchDialog::onequalsEnabled(bool)
{
  m_matchflags = Qt::MatchExactly;
  ApplyFilter();
}

void te::mnt::LayerSearchDialog::onnameLineEditEditingFinished()
{
  m_namefilter = m_ui->m_nameLineEdit->text();
  ApplyFilter();
}

void te::mnt::LayerSearchDialog::onisolinesEnabled(bool)
{
  m_geomfilter = m_geom_name.at(1);
  ApplyFilter();
}

void te::mnt::LayerSearchDialog::onsamplesEnabled(bool)
{
  m_geomfilter = m_geom_name.at(0);
  ApplyFilter();
}

void te::mnt::LayerSearchDialog::ongridEnabled(bool)
{
  m_geomfilter = m_geom_name.at(4);
  ApplyFilter();
}

void te::mnt::LayerSearchDialog::ontinEnabled(bool)
{
  m_geomfilter = m_geom_name.at(2);
  ApplyFilter();
}

void te::mnt::LayerSearchDialog::onothersEnabled(bool)
{
  m_geomfilter = m_geom_name.at(3);
  ApplyFilter();
}

void te::mnt::LayerSearchDialog::onallEnabled(bool flag)
{
  if (!flag)
    return;

  m_geomfilter.clear();
  bool vazio = true;
  if (m_ui->m_isolinesRadioButton->isEnabled())
  {
    m_geomfilter = m_geom_name.at(1);
    vazio = false;
  }

  if (m_ui->m_samplesRadioButton->isEnabled())
  {
    if (!vazio)
      m_geomfilter += "|";
    m_geomfilter += m_geom_name.at(0);
    vazio = false;

  }
  if (m_ui->m_gridRadioButton->isEnabled())
  {
    if (!vazio)
      m_geomfilter += "|";
    m_geomfilter += m_geom_name.at(4);
    vazio = false;
  }
  if (m_ui->m_tinRadioButton->isEnabled())
  {
    if (!vazio)
      m_geomfilter += "|";
    m_geomfilter += m_geom_name.at(2);
    vazio = false;
  }
  if (m_ui->m_othersRadioButton->isEnabled())
  {
    if (!vazio)
      m_geomfilter += "|";
    m_geomfilter += m_geom_name.at(3);
    vazio = false;
  }

  ApplyFilter();
}


void te::mnt::LayerSearchDialog::onHelpPushButtonClicked()
{
}

void te::mnt::LayerSearchDialog::onOkPushButtonClicked()
{
  QTreeWidgetItem* item = m_ui->m_layersTable->currentItem();
  m_selectlayer_index = (size_t)item->data(0, Qt::DisplayRole).toInt() - 1; //because adds 1 to display from 1 and not from 0
  std::list<te::map::AbstractLayerPtr>::iterator it = m_layers.begin();
  for (size_t i = 0; i < m_selectlayer_index; i++)
    it++;
  if (it != m_layers.end())
    m_selectLayer = *it;
  else
    m_selectLayer = 0;

  accept();

}

void te::mnt::LayerSearchDialog::onCancelPushButtonClicked()
{
  reject();
}

