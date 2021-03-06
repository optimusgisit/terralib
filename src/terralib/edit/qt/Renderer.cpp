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
  \file terralib/edit/qt/Renderer.cpp

  \brief This is a singleton for rendering geometries and features.
*/

// TerraLib
#include "../../common/STLUtils.h"
#include "../../dataaccess/dataset/ObjectId.h"
#include "../../geometry/Envelope.h"
#include "../../geometry/Geometry.h"
#include "../../geometry/LineString.h"
#include "../../maptools/MarkRendererManager.h"
#include "../../se/Mark.h"
#include "../../se/Utils.h"
#include "../../qt/widgets/canvas/Canvas.h"
#include "../../qt/widgets/Utils.h"
#include "../../srs/Config.h"
#include "../Feature.h"
#include "../Repository.h"
#include "../RepositoryManager.h"
#include "Renderer.h"

// STL
#include <map>

te::edit::Renderer::Renderer()
: m_canvas(0),
  m_srid(TE_UNKNOWN_SRS),
  m_currentGeomType(te::gm::UnknownGeometryType)
{
  setupDefaultStyle();
}

te::edit::Renderer::~Renderer()
{
  delete m_canvas;
}

void te::edit::Renderer::begin(QPaintDevice* device, const te::gm::Envelope& e, int srid)
{
  delete m_canvas;
  m_canvas = new te::qt::widgets::Canvas(device->width(), device->height());
  m_canvas->setDevice(device, false);
  m_canvas->setWindow(e.m_llx, e.m_lly, e.m_urx, e.m_ury);
  m_srid = srid;
}

void te::edit::Renderer::drawRepositories(const te::gm::Envelope& e, int srid)
{
  const std::map<std::string, Repository*>& repositories = RepositoryManager::getInstance().getRepositories();

  std::map<std::string, Repository*>::const_iterator it;
  for(it = repositories.begin(); it != repositories.end(); ++it)
    drawRepository(it->first, e, srid);
}

void te::edit::Renderer::drawRepository(const std::string& source, const te::gm::Envelope& e, int srid)
{
  Repository* repository = RepositoryManager::getInstance().getRepository(source);

  if(repository == 0)
    return;

  std::vector<Feature*> features = repository->getFeatures(e, srid);

  for (std::size_t i = 0; i < features.size(); ++i)
  {
    if (features[i]->getType() == TO_DELETE || features[i]->isColorChanged())
    {
      m_styleChanged = true;
      break;
    }
  }

  for (std::size_t i = 0; i < features.size(); ++i)
    draw(features[i], false);
}

void te::edit::Renderer::prepare(te::edit::Feature* feature)
{
  assert(m_canvas);
  assert(feature);
  
  te::gm::GeomType type = feature->getGeometry()->getGeomTypeId();
  te::edit::FeatureType op = feature->getType();

  if (m_currentGeomType == type && m_styleChanged == false)
    return; // No need reconfigure the canvas

  m_currentGeomType = type;

  switch(type)
  {
    case te::gm::PolygonType:
    case te::gm::PolygonZType:
    case te::gm::PolygonMType:
    case te::gm::PolygonZMType:
    case te::gm::MultiPolygonType:
    case te::gm::MultiPolygonZType:
    case te::gm::MultiPolygonMType:
    case te::gm::MultiPolygonZMType:
    {
      te::qt::widgets::Config2DrawPolygons(m_canvas, m_polygonFillColor, m_polygonContourColor, m_polygonContourWidth);
      m_canvas->setPolygonContourDashStyle(feature->getType() == te::edit::TO_BLOCKEDIT ? te::map::SolidLine : te::map::DashLine);

      if (feature->isColorChanged())
      {
        m_canvas->setPolygonFillColor(feature->getFillColor());
        m_canvas->setPolygonContourColor(feature->getContourColor());
      }

      if (op == te::edit::TO_DELETE)
      {
        QBrush f;

        f.setColor(Qt::black);
        f.setStyle(Qt::DiagCrossPattern);

        m_canvas->setPolygonFillColor(f);
      }
    }
    break;

    case te::gm::LineStringType:
    case te::gm::LineStringZType:
    case te::gm::LineStringMType:
    case te::gm::LineStringZMType:
    case te::gm::MultiLineStringType:
    case te::gm::MultiLineStringZType:
    case te::gm::MultiLineStringMType:
    case te::gm::MultiLineStringZMType:
    {
      te::qt::widgets::Config2DrawLines(m_canvas, m_lineColor, m_lineWidth);

      if (feature->isColorChanged())
        m_canvas->setLineColor(feature->getFillColor());

      if (op == te::edit::TO_DELETE)
        m_canvas->setLineDashStyle(te::map::DashLine);
    }
    break;

    case te::gm::PointType:
    case te::gm::PointZType:
    case te::gm::PointMType:
    case te::gm::PointZMType:
    case te::gm::MultiPointType:
    case te::gm::MultiPointZType:
    case te::gm::MultiPointMType:
    case te::gm::MultiPointZMType:
    {
      te::qt::widgets::Config2DrawPoints(m_canvas, m_pointMark, m_pointSize, m_pointFillColor, m_pointContourColor, m_pointContourWidth);

      if (feature->isColorChanged())
        m_canvas->setPointColor(feature->getFillColor());

      if (op == te::edit::TO_DELETE)
      {
        std::size_t size = 24;

        te::se::Stroke* stroke = te::se::CreateStroke("#FF0000", "2", "0.5");
        te::se::Fill* fill = te::se::CreateFill("#FFFFFF", "0.5");
        te::se::Mark* mark = te::se::CreateMark("square", stroke, fill);

        te::color::RGBAColor** rgba = te::map::MarkRendererManager::getInstance().render(mark, size);

        m_canvas->setPointColor(te::color::RGBAColor(0, 0, 0, TE_TRANSPARENT));
        m_canvas->setPointPattern(rgba, (int)size, (int)size);

        te::common::Free(rgba, size);
        delete mark;
      }
    }
    break;

    default:
      return;
  }
}

void te::edit::Renderer::prepare(te::gm::GeomType type)
{
  assert(m_canvas);

  if (m_currentGeomType == type && m_styleChanged == false)
    return; // No need reconfigure the canvas

  m_currentGeomType = type;

  switch(type)
  {
    case te::gm::PolygonType:
    case te::gm::PolygonZType:
    case te::gm::PolygonMType:
    case te::gm::PolygonZMType:
    case te::gm::MultiPolygonType:
    case te::gm::MultiPolygonZType:
    case te::gm::MultiPolygonMType:
    case te::gm::MultiPolygonZMType:

      te::qt::widgets::Config2DrawPolygons(m_canvas, m_polygonFillColor, m_polygonContourColor, m_polygonContourWidth);
      m_canvas->setPolygonContourDashStyle(te::map::DashLine);

    break;

    case te::gm::LineStringType:
    case te::gm::LineStringZType:
    case te::gm::LineStringMType:
    case te::gm::LineStringZMType:
    case te::gm::MultiLineStringType:
    case te::gm::MultiLineStringZType:
    case te::gm::MultiLineStringMType:
    case te::gm::MultiLineStringZMType:

      te::qt::widgets::Config2DrawLines(m_canvas, m_lineColor, m_lineWidth);

    break;

    case te::gm::PointType:
    case te::gm::PointZType:
    case te::gm::PointMType:
    case te::gm::PointZMType:
    case te::gm::MultiPointType:
    case te::gm::MultiPointZType:
    case te::gm::MultiPointMType:
    case te::gm::MultiPointZMType:

      te::qt::widgets::Config2DrawPoints(m_canvas, m_pointMark, m_pointSize, m_pointFillColor, m_pointContourColor, m_pointContourWidth);

    break;

    default:
      return;
  }
}

void te::edit::Renderer::draw(te::gm::Geometry* geom, bool showVertexes)
{
  assert(m_canvas);
  assert(geom);

  if((geom->getSRID() != TE_UNKNOWN_SRS) && (m_srid != TE_UNKNOWN_SRS) && (geom->getSRID() != m_srid))
    geom->transform(m_srid);

  prepare(geom->getGeomTypeId());

  m_canvas->draw(geom);

  if(showVertexes)
    drawVertexes(geom);
}

void te::edit::Renderer::draw(te::edit::Feature* feature, bool showVertexes)
{
  assert(m_canvas);
  assert(feature);

  if ((feature->getGeometry()->getSRID() != TE_UNKNOWN_SRS) && (m_srid != TE_UNKNOWN_SRS) && (feature->getGeometry()->getSRID() != m_srid))
    feature->getGeometry()->transform(m_srid);

  prepare(feature);

  m_canvas->draw(feature->getGeometry());

  if (showVertexes)
    drawVertexes(feature->getGeometry());
}

void te::edit::Renderer::drawVertexes(te::gm::Geometry* geom)
{
  assert(geom);

  std::vector<te::gm::LineString*> lines;
  GetLines(geom, lines);

  prepare(te::gm::PointType);

  drawVertexes(lines);
}

void te::edit::Renderer::drawVertexes(const std::vector<te::gm::LineString*>& lines)
{
  for(std::size_t i = 0; i < lines.size(); ++i)
    drawVertexes(lines[i]);
}

void te::edit::Renderer::drawVertexes(te::gm::LineString* line)
{
  assert(m_canvas);
  assert(line);

  if((line->getSRID() != TE_UNKNOWN_SRS) && (m_srid != TE_UNKNOWN_SRS) && (line->getSRID() != m_srid))
    line->transform(m_srid);

  for(std::size_t j = 0; j < line->getNPoints(); ++j)
  {
    std::auto_ptr<te::gm::Point> point(line->getPointN(j));
    m_canvas->draw(point.get());
  }
}

void te::edit::Renderer::end()
{
  delete m_canvas;
  m_canvas = 0;

  m_srid = TE_UNKNOWN_SRS;

  m_currentGeomType = te::gm::UnknownGeometryType;

  setupDefaultStyle();
}

void te::edit::Renderer::setPolygonStyle(const QColor& fillColor, const QColor& contourColor, const std::size_t& contourWidth)
{
  m_polygonFillColor = fillColor;
  m_polygonContourColor = contourColor;
  m_polygonContourWidth = contourWidth;

  m_styleChanged = true;

}

void te::edit::Renderer::setPointStyle(const QString& mark, const QColor& fillColor, const QColor& contourColor,
                                       const std::size_t& contourWidth, const std::size_t& size)
{
  m_pointMark = mark;
  m_pointFillColor = fillColor;
  m_pointContourColor = contourColor;
  m_pointContourWidth = contourWidth;
  m_pointSize = size;

  m_styleChanged = true;
}

void te::edit::Renderer::setLineStyle(const QColor& lineColor, const std::size_t& lineWidth)
{
  m_lineColor = lineColor;
  m_lineWidth = lineWidth;

  m_styleChanged = true;
}

void te::edit::Renderer::setupDefaultStyle()
{
  m_polygonFillColor = QColor(TE_EDIT_FEATURE_FILL_COLOR);
  m_polygonContourColor = QColor(TE_EDIT_FEATURE_CONTOUR_COLOR);
  m_polygonContourWidth = 2;

  m_lineColor = QColor(0, 0, 0, 80);
  m_lineWidth = 5;

  m_pointMark = "circle";
  m_pointFillColor = Qt::red;
  m_pointContourColor = Qt::black;
  m_pointContourWidth = 1;
  m_pointSize = 8;

  m_styleChanged = false;
}
