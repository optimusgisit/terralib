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
  \file terralib/myqt/widgets/mapdisplay/DataSetDisplay.cpp

  \brief A map display for a dataset.
*/

// TerraLib
#include "../../../dataaccess/dataset/DataSet.h"
#include "../../../dataaccess/dataset/DataSetType.h"
#include "../../../dataaccess/datasource/DataSource.h"
#include "../../../dataaccess/datasource/DataSourceManager.h"
#include "../../../dataaccess/datasource/DataSourceTransactor.h"
#include "../../../dataaccess/utils/Utils.h"
#include "../../../geometry/Envelope.h"
#include "../../../geometry/Geometry.h"
#include "../../../geometry/GeometryProperty.h"
#include "../canvas/Canvas.h"
#include "DataSetDisplay.h"

// STL
#include <cassert>

// Qt
#include <QtGui/QMessageBox>
#include <QtGui/QResizeEvent>

te::qt::widgets::DataSetDisplay::DataSetDisplay(QWidget * parent, Qt::WindowFlags f)
  : QFrame(parent, f)
{
}

te::qt::widgets::DataSetDisplay::~DataSetDisplay()
{
}

void te::qt::widgets::DataSetDisplay::draw(const te::da::DataSetTypePtr& dataset, const te::da::DataSourceInfoPtr& ds)
{
  if(ds.get() == 0)
    return;

  try
  {
    te::da::DataSourcePtr auxDs(te::da::DataSourceManager::getInstance().get(ds->getId(), ds->getType(), ds->getConnInfo()));

    draw(dataset, auxDs);
  }
  catch(const std::exception& e)
  {
    QMessageBox::warning(this,
                         tr("TerraLib Qt Components"),
                         tr(e.what()));
  }
  catch(...)
  {
    QMessageBox::warning(this,
                         tr("TerraLib Qt Components"),
                         tr("Unknown error when displaying dataset!"));
  }
}

void te::qt::widgets::DataSetDisplay::draw(const te::da::DataSetTypePtr& dataset, const te::da::DataSourcePtr& ds)
{
  if((dataset.get() == 0) || (ds.get() == 0))
    return;

  if(m_canvas.get() == 0)
    m_canvas.reset(new Canvas(width(), height()));

  std::auto_ptr<te::da::DataSourceTransactor> transactor(ds->getTransactor());

  if(dataset->size() == 0)
    te::da::LoadProperties(dataset.get(), transactor.get());

  if(!dataset->hasGeom())
  {
    m_canvas->setBackgroundColor(te::color::RGBAColor(0, 0, 0, 255)); //  fill black
    repaint();
    return;
  }

  te::gm::GeometryProperty* gp = dataset->getDefaultGeomProperty();

  te::da::LoadExtent(gp, transactor.get());

  if(gp->getExtent() == 0)
    return;

  std::auto_ptr<te::gm::Envelope> mbr(new te::gm::Envelope(*(gp->getExtent())));

  m_canvas->calcAspectRatio(mbr.get());

  m_canvas->setWindow(mbr->getLowerLeftX(), mbr->getLowerLeftY(), mbr->getUpperRightX(), mbr->getUpperRightY());

  std::auto_ptr<te::da::DataSet> feature(transactor->getDataSet(dataset->getName()));

  while(feature->moveNext())
  {
    std::auto_ptr<te::gm::Geometry> g(feature->getGeometry());
    m_canvas->draw(g.get());
  }

  repaint();
}

void te::qt::widgets::DataSetDisplay::clear()
{
  if(m_canvas.get() == 0)
    m_canvas.reset(new Canvas(width(), height()));

  m_canvas->setBackgroundColor(te::color::RGBAColor(255, 255, 255, 0));
  m_canvas->clear();

  repaint();
}

void te::qt::widgets::DataSetDisplay::paintEvent(QPaintEvent* /*e*/)
{
  if(m_canvas.get() == 0)
    return;

  QPainter painter(this);
  
  painter.drawPixmap(0, 0, *(m_canvas->getPixmap()));

  painter.end();
 
}

void te::qt::widgets::DataSetDisplay::resizeEvent(QResizeEvent* e)
{
  assert(e);

  if(m_canvas.get() == 0)
    return;

  m_canvas->resize(e->size().width(), e->size().height());

  QFrame::resizeEvent(e);
}
