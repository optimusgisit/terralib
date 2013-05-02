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
  \file DataSetTableDockWidget.h

  \brief A dock widget for table view objects.
*/

#ifndef __TERRALIB_QT_WIDGETS_TABLE_INTERNAL_DATASETTABLEDOCKWIDGET_H
#define __TERRALIB_QT_WIDGETS_TABLE_INTERNAL_DATASETTABLEDOCKWIDGET_H

#include "../Config.h"

// Qt
#include <QtGui/QDockWidget>

namespace te
{
  namespace map
  {
    // Forward declarations
    class AbstractLayer;
  }

  namespace qt
  {
    namespace widgets
    {
      // Forward declarations
      class DataSetTableView;

      /*!
        \class DataSetTableDockWidget

        \brief A dock widget for DataSetTableView objects.
      */
      class TEQTWIDGETSEXPORT DataSetTableDockWidget : public QDockWidget
      {
        public:

          /*!
            \brief Constructor.

            \param parent The widget parent.
          */
          DataSetTableDockWidget(QWidget* parent=0);

          /*!
            \brief Destructor.
          */
          ~DataSetTableDockWidget();

          /*!
            \brief Sets the layer to be showed on view. This DOES NOT takes the ownership of \a layer.

            \param layer The layer to be showed.
          */
          void setLayer(te::map::AbstractLayer* layer);

          /*!
            \brief Returns the layer being used.

            \return Layer being used.
          */
          te::map::AbstractLayer* getLayer() const;

        protected:

          DataSetTableView* m_view;           //!< The table view.

          te::map::AbstractLayer* m_layer;    //!< Layer being visualized.
      };
    }
  }
}

#endif //__TERRALIB_QT_WIDGETS_TABLE_INTERNAL_DATASETTABLEDOCKWIDGET_H