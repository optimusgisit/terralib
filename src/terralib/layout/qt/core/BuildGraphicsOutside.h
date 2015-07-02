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
  \file BuildGraphicsOutside.h
   
   \brief Class responsible for creating or building widgets. All objects are children of Qwidget and OutsideObserver.
   Only place where the model and the controller is instantiated and the MVC outside is created.

  \ingroup layout
*/

#ifndef __TERRALIB_LAYOUT_INTERNAL_BUILD_GRAPHICS_OUTSIDE_H
#define __TERRALIB_LAYOUT_INTERNAL_BUILD_GRAPHICS_OUTSIDE_H

//TerraLib
#include "../../core/AbstractBuildGraphicsOutside.h"
#include "../../core/Config.h"
#include "../../core/enum/EnumType.h"

class QWidget;

namespace te
{
  namespace layout
  {
	/*!
  \brief Class responsible for creating or building widgets. All objects are children of Qwidget and OutsideObserver.
    Only place where the model and the controller is instantiated and the MVC outside is created.
	  
	  \ingroup layout
	*/
    class TELAYOUTEXPORT BuildGraphicsOutside : public AbstractBuildGraphicsOutside
    {
      public:

		/*!
          \brief Constructor
        */
        BuildGraphicsOutside();

		/*!
          \brief Destructor
        */
        virtual ~BuildGraphicsOutside();
        

        /*!
          \brief Method to create a widget from the type.
		  
		      \param mode type of the object will created
		  
		      \return item value
        */
        QWidget* createOuside(te::layout::EnumType* type);

      protected:
	  
		    /*!
          \brief Create a widget of GridSettings type 

		      \return new object 
        */
        QWidget* createGridSettings();

        /*!
          \brief Create a widget of Toolbar type 

		      \return new object 
        */
        QWidget* createToolbar();

        /*!
          \brief Create a widget of Properties Browser type 

		      \return new object 
        */
        QWidget* createProperties();

        /*!
          \brief Create a widget of Object Inspector type 

		      \return new object 
        */
        QWidget* createObjectInspector();

        /*!
          \brief Create a widget of Page Setup type 

		      \return new object 
        */
        QWidget* createPageSetup();

        /*!
          \brief Create a widget of Systematic Scale type 

		      \return new object 
        */
        QWidget* createSystematicScale();

        /*!
          \brief Create a widget of EditTemplate type 

		      \return new object 
        */
        QWidget* createEditTemplate();

        /*!
          \brief Create a widget of Text Grid Settings type 

		      \return new object 
        */
        QWidget* createTextGridSettingss();

        /*!
          \brief Create a widget of Map Layer Choice type 

		      \return new object 
        */
        QWidget* createMapLayerChoice();

        /*!
          \brief Create a widget of SVG View type 

		      \return new object 
        */
        QWidget* createSVGDialog();

        /*!
          \brief Create a widget of Color Dialog type 

		      \return new object 
        */
        QWidget* createColorDialog();

        /*!
          \brief Create a widget of Font Dialog type 

		      \return new object 
        */
        QWidget* createFontDialog();

        /*!
          \brief Create a widget of Legend Choice type 

		      \return new object 
        */
        QWidget* createLegendChoice();
    };
  }
}

#endif


