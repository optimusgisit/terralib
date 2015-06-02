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
  \file GridGeodesicItem.cpp
   
  \brief 

  \ingroup layout
*/

// TerraLib
#include "GridGeodesicItem.h"
#include "../../core/pattern/mvc/ItemController.h"
#include "../../core/AbstractScene.h"
#include "../../core/pattern/mvc/Observable.h"
#include "../../../color/RGBAColor.h"
#include "../../../qt/widgets/Utils.h"
#include "../../../geometry/Envelope.h"
#include "../../../common/STLUtils.h"
#include "../../item/GridMapModel.h"
#include "MapItem.h"
#include "../../core/WorldTransformer.h"
#include "../../item/MapModel.h"
#include "../../item/GridGeodesicModel.h"
#include "../../../geometry/LinearRing.h"
#include "../../core/pattern/singleton/Context.h"
#include "../../core/Utils.h"
#include "../core/ItemUtils.h"

//Qt
#include <QStyleOptionGraphicsItem>
#include <QPointF>
#include <QLineF>

te::layout::GridGeodesicItem::GridGeodesicItem( ItemController* controller, Observable* o ) :
  GridMapItem(controller, o)
{  
  m_nameClass = std::string(this->metaObject()->className());
}

te::layout::GridGeodesicItem::~GridGeodesicItem()
{

}

void te::layout::GridGeodesicItem::drawGrid( QPainter* painter )
{
  if(!parentItem())
  {
    return;
  }

  GridGeodesicModel* model = dynamic_cast<GridGeodesicModel*>(m_model);
  if(!model)
  {
    return;
  }

  MapItem* item = dynamic_cast<MapItem*>(parentItem());
  if(!item)
  {
    return;    
  }

  MapModel* mapModel = dynamic_cast<MapModel*>(item->getModel());
  if(!mapModel)
  {
    return;    
  }

  if(!mapModel->isLoadedLayer())
  {
    return;
  }

  std::list<te::map::AbstractLayerPtr> layerListMap = mapModel->getLayers();

  std::list<te::map::AbstractLayerPtr>::iterator it;
  it = layerListMap.begin();

  te::map::AbstractLayerPtr layer = (*it);
  
  int srid = layer->getSRID();
  if(srid <= 0)
    return;

  clear();

  Utils* utils = Context::getInstance().getUtils();

  double scale = mapModel->getScale();
  te::gm::Envelope box = mapModel->getWorldInDegrees();
  te::gm::Envelope boxMM = mapModel->getMapBox();

  double wMargin = mapModel->getDisplacementX();
  double hMargin = mapModel->getDisplacementY();

  te::gm::Envelope newBoxMM(wMargin, hMargin, boxMM.getWidth() + wMargin, boxMM.getHeight() + hMargin);

  model->setSRID(srid);
  model->setMapScale(scale); // put visit
  model->calculateGaps(box);
  
  /* Box necessário para desenhar a curvatura */
  te::gm::Envelope planarBoxGeodesic = box;
  int zone = utils->calculatePlanarZone(box);

  utils->remapToPlanar(&planarBoxGeodesic, zone);
  model->setPlanarBox(planarBoxGeodesic);

  calculateVertical(box, newBoxMM, scale);
  calculateHorizontal(box, newBoxMM, scale);

  EnumGridStyleType* gridStyle = Enums::getInstance().getEnumGridStyleType();
  if(!gridStyle)
  {
    return;
  }

  if(model->getGridStyle() == gridStyle->getStyleContinuous())
  {
    drawContinuousLines(painter);
  }
  else if(model->getGridStyle() == gridStyle->getStyleCross())
  {
    drawCrossLines(painter);
  }
}

double te::layout::GridGeodesicItem::initVerticalLines( te::gm::Envelope geoBox )
{
  double			yInit = 0;
 
  GridGeodesicModel* model = dynamic_cast<GridGeodesicModel*>(m_model);
  if(!model)
  {
    return yInit;
  }
  
  yInit = model->getInitialGridPointY();
  if(yInit < geoBox.getLowerLeftY())
  {
    double dify = geoBox.getLowerLeftY() - yInit;
    int nParts = (int)(dify/model->getLneVrtGap());
    if(nParts == 0)
    {
      yInit = model->getInitialGridPointY();
    }
    else
    {
      yInit = yInit + (nParts * model->getLneVrtGap());
    }
  }
  return yInit;
}

double te::layout::GridGeodesicItem::initHorizontalLines( te::gm::Envelope geoBox )
{
  double			xInit = 0;

  GridGeodesicModel* model = dynamic_cast<GridGeodesicModel*>(m_model);
  if(!model)
  {
    return xInit;
  }

  xInit = model->getInitialGridPointX();
  if(xInit < geoBox.getLowerLeftX())
  {
    double difx = geoBox.getLowerLeftX() - xInit;
    int nParts = (int)(difx/model->getLneHrzGap());
    if(nParts == 0)
    {
      xInit = model->getInitialGridPointX();
    }
    else
    {
      xInit = xInit + (nParts * model->getLneHrzGap());
    }
  }
  return xInit;
}

void te::layout::GridGeodesicItem::calculateVertical( te::gm::Envelope geoBox, te::gm::Envelope boxMM, double scale )
{
  GridGeodesicModel* model = dynamic_cast<GridGeodesicModel*>(m_model);
  if(!model)
  {
    return;
  }

  // Draw a horizontal line and the y coordinate change(vertical)

  Utils* utils = Context::getInstance().getUtils();
  ItemUtils* itemUtils = Context::getInstance().getItemUtils();

  te::gm::Envelope planarBox = model->getPlanarBox();

  WorldTransformer transf = utils->getTransformGeo(model->getPlanarBox(), boxMM);
  transf.setMirroring(false);

  int zone = utils->calculatePlanarZone(geoBox);

  double y1 = initVerticalLines(geoBox);

  for( ; y1 < geoBox.getUpperRightY() ; y1 += model->getLneVrtGap())
  {
    if(y1 < geoBox.getLowerLeftY())
      continue;

    te::gm::Envelope env(geoBox.getLowerLeftX(), y1, geoBox.getUpperRightX(), y1);

    te::gm::LinearRing* line = 0;
    line = utils->addCoordsInX(env, y1, model->getLneVrtGap());
    
    // Line curvature: of latlong to planar;
    // Draw line: planar to mm
    utils->remapToPlanar(line, zone);
    utils->convertToMillimeter(transf, line);

    te::gm::Envelope* ev = const_cast<te::gm::Envelope*>(line->getMBR());

    QLineF lne(ev->getLowerLeftX(), ev->getLowerLeftY(), ev->getUpperRightX(), ev->getUpperRightY());
    m_horizontalLines.push_back(lne);

    std::string text = utils->convertDecimalToDegree(y1, model->isDegreesText(), model->isMinutesText(), model->isSecondsText());

    // text left
    QPointF ptLeft(ev->getLowerLeftX() - model->getLneHrzDisplacement(), ev->getLowerLeftY());
    m_leftTexts[text] =  ptLeft;

    // text right
    QPointF ptRight(ev->getUpperRightX() + model->getLneHrzDisplacement(), ev->getUpperRightY());
    m_rightTexts[text] = ptRight;

    if(line)
    {
      delete line;
      line = 0;
    }
  }
}

void te::layout::GridGeodesicItem::calculateHorizontal( te::gm::Envelope geoBox, te::gm::Envelope boxMM, double scale )
{
  GridGeodesicModel* model = dynamic_cast<GridGeodesicModel*>(m_model);
  if(!model)
  {
    return;
  }

  // Draw a vertical line and the x coordinate change(horizontal)

  Utils* utils = Context::getInstance().getUtils();
  ItemUtils* itemUtils = Context::getInstance().getItemUtils();

  te::gm::Envelope planarBox = model->getPlanarBox();

  WorldTransformer transf = utils->getTransformGeo(planarBox, boxMM);
  transf.setMirroring(false);

  int zone = utils->calculatePlanarZone(geoBox);

  double x1 = initHorizontalLines(geoBox);

  for( ; x1 < geoBox.getUpperRightX() ; x1 += model->getLneHrzGap())
  {
    if(x1 < geoBox.getLowerLeftX())
      continue;

    te::gm::Envelope env(x1, geoBox.getLowerLeftY(), x1, geoBox.getUpperRightY());
    te::gm::LinearRing* line = 0;
    line = utils->addCoordsInY(env, x1, model->getLneHrzGap());

    // Line curvature: of latlong to planar;
    // Draw line: planar to mm
    utils->remapToPlanar(line, zone);
    utils->convertToMillimeter(transf, line);

    te::gm::Envelope* ev = const_cast<te::gm::Envelope*>(line->getMBR());

    QLineF lne(ev->getLowerLeftX(), ev->getLowerLeftY(), ev->getUpperRightX(), ev->getUpperRightY());
    m_verticalLines.push_back(lne);

    std::string text = utils->convertDecimalToDegree(x1, model->isDegreesText(), model->isMinutesText(), model->isSecondsText());

    // text bottom
    QPointF ptBottom(ev->getLowerLeftX(), ev->getLowerLeftX() - model->getLneVrtDisplacement());
    m_bottomTexts[text] = ptBottom;

    // text top
    QPointF ptTop(ev->getUpperRightX(), ev->getUpperRightY() + model->getLneVrtDisplacement());
    m_topTexts[text] = ptTop;

    if(line)
    {
      delete line;
      line = 0;
    }
  }
}



