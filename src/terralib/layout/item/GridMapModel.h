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
  \file GridMapModel.h
   
   \brief Class that represents a "Model" part of GridMap MVC component. 
      Its coordinate system is the same of scene (millimeters). 
      This is also son of ItemModelObservable, so it can become observable.

  \ingroup layout
*/

#ifndef __TERRALIB_LAYOUT_INTERNAL_GRIDMAP_MODEL_H
#define __TERRALIB_LAYOUT_INTERNAL_GRIDMAP_MODEL_H

// TerraLib
#include "../core/property/Properties.h"
#include "../../maptools/Canvas.h"
#include "../../maptools/Enums.h"
#include "../core/property/Property.h"
#include "../../geometry/Envelope.h"
#include "../core/Utils.h"
#include "../../geometry/Coord2D.h"
#include "../../geometry/Point.h"
#include "../core/pattern/mvc/ItemModelObservable.h"
#include "../core/ContextItem.h"
#include "../core/Config.h"
#include "../core/pattern/derivativevisitor/AbstractVisitor.h"
#include "../core/Systematic.h"

// STL
#include <map>
#include <string>

namespace te
{
  namespace layout
  {

    /*!
      \brief Class that represents a "Model" part of GridMap MVC component. 
          Its coordinate system is the same of scene (millimeters). 
          He is also the son of ItemModelObservable, so it can become observable.
      	  
	    \ingroup layout

      \sa te::layout::ItemModelObservable
	  */
    class TELAYOUTEXPORT GridMapModel : public ItemModelObservable, public AbstractVisitor
    {
      public:

        /*!
          \brief Constructor
        */
        GridMapModel();

        /*!
          \brief Destructor
        */ 
        virtual ~GridMapModel();
        
        virtual te::layout::Property getProperty();

        virtual void updateProperty(Property property);

        virtual std::string getName();

        virtual void setName(std::string name);

        virtual bool isVisible();

        virtual void setVisible(bool visible);

        virtual void setMapScale(double scale);

        virtual void setWorldBox(te::gm::Envelope box);

        virtual void setBoxMapMM(te::gm::Envelope box);

        virtual void setBoundingBoxItemMM(te::gm::Envelope box);

        virtual void setSystematic(Systematic* sys);

        virtual std::map<te::gm::Point*, std::string> getGridInfo();

        virtual void setVisibleAllTexts(bool visible);

        virtual bool isVisibleAllTexts();

        virtual void setMapDisplacementX(double displacement);

        virtual void setMapDisplacementY(double displacement);

        virtual int getPointSize();

        virtual std::string getFontFamily();

        virtual Properties* getProperties() const;

        virtual void updateProperties(te::layout::Properties* properties);        

      protected:

        virtual void visitDependent(ContextItem context);

        virtual void calculateGaps(te::gm::Envelope box);

        virtual void gridTextFreeMemory();

        std::string m_mapName;

        te::layout::Property                    m_property;
        double                                  m_mapScale;
        std::string                             m_name;
        int                                     m_srid;
        Systematic*                             m_systematic;
        std::map<te::gm::Point*, std::string>   m_gridTexts;

        te::gm::Envelope     m_boxMapMM;
        te::gm::Envelope     m_boundingBoxItemMM;
        te::gm::Envelope     m_worldBox;
        double               m_mapDisplacementX;
        double               m_mapDisplacementY;

        /* Grid */
        bool   m_visible;
        double m_lneHrzGap;
        double m_lneVrtGap;
        double m_initialGridPointX; 
        double m_initialGridPointY;

        /* Just one is visible */
        EnumType* m_gridStyle;

        /* Line */
        EnumType*  m_lineStyle;
        te::color::RGBAColor    m_lineColor;
        int                     m_lineWidth;

        /* Text: Basic Configuration */
        int    m_pointTextSize;
        std::string m_fontText;
        te::color::RGBAColor  m_textColor;

        /* Text: Advanced configuration */
        bool   m_visibleAllTexts;
        bool m_superscriptText;
        double m_lneVrtDisplacement;
        double m_lneHrzDisplacement;
        bool   m_bottomText;
        bool   m_leftText;
        bool   m_rightText;
        bool   m_topText;
        bool   m_bottomRotateText;
        bool   m_leftRotateText;
        bool   m_rightRotateText;
        bool   m_topRotateText;  
    };
  }
}

#endif
