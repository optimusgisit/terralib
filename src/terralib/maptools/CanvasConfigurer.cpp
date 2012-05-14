/*  Copyright (C) 2001-2009 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/maptools/CanvasConfigurer.cpp
  
  \brief A Symbology Enconding visitor that configures a given canvas based on symbolizers elements.
*/

// TerraLib
#include "../common/StringUtils.h"
#include "../fe/Literal.h"
#include "../se/Fill.h"
#include "../se/Graphic.h"
#include "../se/GraphicStroke.h"
#include "../se/LineSymbolizer.h"
#include "../se/PointSymbolizer.h"
#include "../se/PolygonSymbolizer.h"
#include "../se/Stroke.h"
#include "../se/SvgParameter.h"
#include "../se/Symbolizer.h"
#include "AbstractMarkFactory.h"
#include "Canvas.h"
#include "CanvasConfigurer.h"
#include "Utils.h"

// STL
#include <cassert>
#include <cstdlib>
#include <vector>

std::map<std::string, te::map::LineCapStyle> te::map::CanvasConfigurer::sm_lineCapMap;
std::map<std::string, te::map::LineJoinStyle> te::map::CanvasConfigurer::sm_lineJoinMap;

te::map::CanvasConfigurer::CanvasConfigurer(te::map::Canvas* canvas)
  : m_canvas(canvas)
{
// LineCapMap
  sm_lineCapMap[TE_SE_BUTT_CAP  ] = te::map::FlatCap;
  sm_lineCapMap[TE_SE_ROUND_CAP ] = te::map::RoundCap;
  sm_lineCapMap[TE_SE_SQUARE_CAP] = te::map::SquareCap;

// LineJoinMap
  sm_lineJoinMap[TE_SE_MITRE_JOIN] = te::map::MiterJoin;
  sm_lineJoinMap[TE_SE_ROUND_JOIN] = te::map::RoundJoin;
  sm_lineJoinMap[TE_SE_BEVEL_JOIN] = te::map::BevelJoin;
}

te::map::CanvasConfigurer::~CanvasConfigurer()
{}

void te::map::CanvasConfigurer::config(const te::se::Symbolizer* symbolizer)
{
  symbolizer->accept(*this);
}

void te::map::CanvasConfigurer::visit(const te::se::Style& visited)
{
  // no need
}

void te::map::CanvasConfigurer::visit(const te::se::FeatureTypeStyle& visited)
{
// no need
}

void te::map::CanvasConfigurer::visit(const te::se::CoverageStyle& visited)
{
// no need
}

void te::map::CanvasConfigurer::visit(const te::se::Symbolizer& visited)
{
// no need
}

void te::map::CanvasConfigurer::visit(const te::se::PolygonSymbolizer& visited)
{
// Default configuration
  m_canvas->setPolygonContourColor(te::color::RGBAColor(0, 0, 0, TE_OPAQUE));
  m_canvas->setPolygonContourWidth(1);
  m_canvas->setPolygonContourDashStyle(te::map::SolidLine);
  m_canvas->setPolygonContourCapStyle(te::map::RoundCap);
  m_canvas->setPolygonContourJoinStyle(te::map::RoundJoin);
  m_canvas->setPolygonFillColor(te::color::RGBAColor(127, 127, 127, TE_OPAQUE));

// Configuring the polygon stroke...
  const te::se::Stroke* stroke = visited.getStroke();
  if(stroke)
    config(stroke, false);
  else
    m_canvas->setPolygonContourColor(te::color::RGBAColor(0, 0, 0, TE_TRANSPARENT)); // no stroke

// Configuring the polygon fill...
  const te::se::Fill* fill = visited.getFill();
  if(fill)
    config(fill);
  else
    m_canvas->setPolygonFillColor(te::color::RGBAColor(0, 0, 0, TE_TRANSPARENT)); // no fill
}

void te::map::CanvasConfigurer::visit(const te::se::LineSymbolizer& visited)
{
// Default configuration
  m_canvas->setLineColor(te::color::RGBAColor(0 , 0, 255, TE_OPAQUE));
  m_canvas->setLineWidth(1);
  m_canvas->setLineDashStyle(te::map::SolidLine);
  m_canvas->setLineCapStyle(te::map::RoundCap);
  m_canvas->setLineJoinStyle(te::map::RoundJoin);

// Configuring the line stroke...
  const te::se::Stroke* stroke = visited.getStroke();
  if(stroke)
    config(stroke);
  else
    m_canvas->setLineColor(te::color::RGBAColor(0, 0, 0, TE_TRANSPARENT)); // no stroke
}

void te::map::CanvasConfigurer::visit(const te::se::PointSymbolizer& visited)
{
  const te::se::Graphic* graphic = visited.getGraphic();
  if(graphic)
    config(graphic, te::map::CanvasConfigurer::Point);
}

void te::map::CanvasConfigurer::visit(const te::se::TextSymbolizer& visited)
{
// TODO
}

void te::map::CanvasConfigurer::visit(const te::se::RasterSymbolizer& visited)
{
// TODO
}

void te::map::CanvasConfigurer::config(const te::se::Stroke* stroke, const bool& fromLineSymbolizer)
{
  const te::se::Graphic* graphicFill = stroke->getGraphicFill();
  if(graphicFill)
    fromLineSymbolizer ? config(graphicFill, te::map::CanvasConfigurer::Line) : config(graphicFill, te::map::CanvasConfigurer::Contour);

  int alpha = TE_OPAQUE;
  const te::se::SvgParameter* opacity = stroke->getOpacity();
  if(opacity)
    alpha = (int)(te::map::GetDouble(opacity) * TE_OPAQUE);

  const te::se::SvgParameter* color = stroke->getColor();
  if(color)
  {
    te::color::RGBAColor rgba = te::map::GetColor(color);
    rgba = te::color::RGBAColor(rgba.getRed(), rgba.getGreen(), rgba.getBlue(), alpha);
    fromLineSymbolizer ? m_canvas->setLineColor(rgba) : m_canvas->setPolygonContourColor(rgba);
  }

  const te::se::SvgParameter* width = stroke->getWidth();
  if(width)
    fromLineSymbolizer ? m_canvas->setLineWidth(te::map::GetInt(width)) : m_canvas->setPolygonContourWidth(te::map::GetInt(width));

  const te::se::SvgParameter* linecap = stroke->getLineCap();
  if(linecap)
  {
    std::map<std::string, te::map::LineCapStyle>::iterator it = sm_lineCapMap.find(te::map::GetString(linecap));
    if(it != sm_lineCapMap.end())
      fromLineSymbolizer ? m_canvas->setLineCapStyle(it->second) : m_canvas->setPolygonContourCapStyle(it->second);
  }

  const te::se::SvgParameter* linejoin = stroke->getLineJoin();
  if(linejoin)
  {
    std::map<std::string, te::map::LineJoinStyle>::iterator it = sm_lineJoinMap.find(te::map::GetString(linejoin));
    if(it != sm_lineJoinMap.end())
      fromLineSymbolizer ? m_canvas->setLineJoinStyle(it->second) : m_canvas->setPolygonContourJoinStyle(it->second);
  }

  const te::se::SvgParameter* dasharray = stroke->getDashArray();
  if(dasharray)
  {
    std::string value = te::map::GetString(dasharray);
    std::vector<double> pattern;
    te::map::GetDashStyle(value, pattern);
    fromLineSymbolizer ? m_canvas->setLineDashStyle(pattern) : m_canvas->setPolygonContourDashStyle(pattern);
  }

  /* TODO: 1) stroke-dashoffset;
           2) Should be verified the GraphicStroke. */
}

void te::map::CanvasConfigurer::config(const te::se::Fill* fill)
{
  const te::se::Graphic* graphicFill = fill->getGraphicFill();
  if(graphicFill)
  {
    config(graphicFill, te::map::CanvasConfigurer::Fill);
    return;
  }

  int alpha = TE_OPAQUE;
  const te::se::SvgParameter* opacity = fill->getOpacity();
  if(opacity)
    alpha = (int)(te::map::GetDouble(opacity) * TE_OPAQUE);

  const te::se::SvgParameter* color = fill->getColor();
  if(color)
  {
    te::color::RGBAColor rgba = te::map::GetColor(color);
    rgba = te::color::RGBAColor(rgba.getRed(), rgba.getGreen(), rgba.getBlue(), alpha);
    m_canvas->setPolygonFillColor(rgba);
  }
}

void te::map::CanvasConfigurer::config(const te::se::Graphic* graphic, te::map::CanvasConfigurer::ConfigStyle configStyle)
{
  // Gets the graphic size
  const te::se::ParameterValue* size = graphic->getSize();
  int sizeValue = 16; /* TODO: Defines a default size! */
  if(size)
    sizeValue = te::map::GetInt(size);

  // Gets the graphic rotation 
  const te::se::ParameterValue* rotation = graphic->getRotation();
  double angle = 0.0;
  if(rotation)
    angle = te::map::GetDouble(rotation);

  int alpha = TE_OPAQUE;
  const te::se::ParameterValue* opacity = graphic->getOpacity();
  if(opacity)
    alpha = (int)(te::map::GetDouble(opacity) * TE_OPAQUE);

  const std::vector<te::se::Mark*> marks = graphic->getMarks();
   /* TODO: Here we have a set of marks. Need review! 
            Maybe keep a reference to the dataset and make the draw here... */
  if(!marks.empty())
  {
    for(std::size_t i = 0; i < marks.size(); ++i)
    {
      te::color::RGBAColor** rgba = te::map::AbstractMarkFactory::make(marks[i], sizeValue);
      switch(configStyle)
      {
        case te::map::CanvasConfigurer::Fill:
          m_canvas->setPolygonFillPattern(rgba, sizeValue, sizeValue);
          m_canvas->setPolygonPatternRotation(angle);
          m_canvas->setPolygonPatternOpacity(alpha);
        break;

        case te::map::CanvasConfigurer::Contour:
          m_canvas->setPolygonContourPattern(rgba, sizeValue, sizeValue);
          m_canvas->setPolygonContourPatternRotation(angle);
          m_canvas->setPolygonContourPatternOpacity(alpha);
        break;

        case te::map::CanvasConfigurer::Line:
          m_canvas->setLinePattern(rgba, sizeValue, sizeValue);
          m_canvas->setLinePatternRotation(angle);
          m_canvas->setLinePatternOpacity(alpha);
        break;

        case te::map::CanvasConfigurer::Point:
          m_canvas->setPointPattern(rgba, sizeValue, sizeValue);
          m_canvas->setPointPatternRotation(angle);
          m_canvas->setPointPatternOpacity(alpha);
        break;
      }
    }
  }
}
