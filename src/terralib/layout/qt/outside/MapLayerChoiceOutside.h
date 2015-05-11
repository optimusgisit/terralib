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
  \file terralib/layout/qt/outside/MapLayerChoiceOutside.h

  \brief A dialog configure the input layer to address geocoding operation
*/

#ifndef __TERRALIB_LAYOUT_INTERNAL_MAP_LAYER_CHOICE_OUTSIDE_H
#define __TERRALIB_LAYOUT_INTERNAL_MAP_LAYER_CHOICE_OUTSIDE_H

// TerraLib
#include "../../../qt/widgets/utils/DoubleListWidget.h"
#include "../../../maptools/AbstractLayer.h"
#include "../../core/Config.h"
#include "terralib/dataaccess/datasource/DataSource.h"
#include "../../core/pattern/mvc/OutsideObserver.h"

// STL
#include <map>
#include <memory>

// Qt
#include <QDialog>

namespace Ui { class MapLayerChoice; }

// Forward declarations
class QModelIndex;

namespace te
{
  namespace layout
  {
    class OutsideController;
    class Observable;

    class TELAYOUTEXPORT MapLayerChoiceOutside : public QDialog, public OutsideObserver 
    {
      Q_OBJECT

      public:

        MapLayerChoiceOutside(OutsideController* controller, Observable* o);

        ~MapLayerChoiceOutside();

        /*!
          \brief Set the layer that can be used

          \param layers   List of AbstractLayerPtr
        */
        void setLayers(std::list<te::map::AbstractLayerPtr> layers);

        te::map::AbstractLayerPtr getLayer();

        te::da::DataSourcePtr getDataSource();

        virtual void updateObserver( ContextItem context );

        virtual void setPosition( const double& x, const double& y );

        virtual te::gm::Coord2D getPosition();
        
      protected slots:

        void onOkPushButtonClicked();

        void onCancelPushButtonClicked();

      private:

        std::auto_ptr<Ui::MapLayerChoice> m_ui;
        std::auto_ptr<te::qt::widgets::DoubleListWidget> m_widget;
        std::list<te::map::AbstractLayerPtr> m_layers;  //!< List of layers.
        te::da::DataSourcePtr m_dataSource;
        te::map::AbstractLayerPtr m_selectedLayer;  //!< Layer used for address geocoding
        std::vector<std::string> m_selectedProps; //!< Selected properties related to the selected Layer

    };
  }   // end namespace addressgeocoding
}     // end namespace te

#endif


