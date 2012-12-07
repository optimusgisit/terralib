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
  \file terralib/qt/widgets/layer/selector/shp/ShpSelector.h

  \brief ....
*/

#ifndef __TERRALIB_QT_WIDGETS_LAYER_SELECTOR_SHP_INTERNAL_SHPSELECTOR_H
#define __TERRALIB_QT_WIDGETS_LAYER_SELECTOR_SHP_INTERNAL_SHPSELECTOR_H

// TerraLib
#include "../../../../../dataaccess/Definitions.h"
#include "../core/AbstractLayerSelector.h"

namespace te
{
  namespace qt
  {
    namespace widgets
    {
      /*!
        \class ShpSelector

        \brief ....
      */
      class TEQTWIDGETSEXPORT ShpSelector : public AbstractLayerSelector
      {
        public:

          ShpSelector(QWidget* parent = 0, Qt::WindowFlags f = 0);

          ~ShpSelector();

          void set(const te::da::DataSourceInfoPtr& ds);

          std::list<te::map::AbstractLayerPtr> getLayers();

        private:

          te::da::DataSourceInfoPtr m_datasource;
      }; 

    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_LAYER_SELECTOR_SHP_INTERNAL_SHPSELECTOR_H
