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
  \file LayerEvents.h

  \brief Contains a list of the layer events.
*/

#ifndef __TERRALIB_QT_AF_EVENTS_INTERNAL_LAYEREVENTS_H
#define __TERRALIB_QT_AF_EVENTS_INTERNAL_LAYEREVENTS_H

#include "../../../maptools2/Enums.h"
#include "../../../maptools2/AbstractLayer.h"
#include "Event.h"
#include "Enums.h"

namespace te
{
  namespace qt
  {
    namespace af
    {
      namespace evt
      {
        /*!
          \struct LayerAdded

          \brief This event signals that a new layer was created.

          \ingroup afevents
         */
        struct LayerAdded : public Event
        {
          /*!
            \brief Constructor.

            \param newLayer Pointer to the layer created.
          */
          LayerAdded(te::map::AbstractLayer* newlayer) : 
            Event(LAYER_ADDED),
            m_layer(newlayer)
          {
          }

          te::map::AbstractLayer* m_layer;  //!< Pointer to the new layer.
        };

        /*!
          \struct LayerVisibilityChanged

          \brief This event signals that a the layer visibility changed.

          \ingroup afevents
        */
        struct LayerVisibilityChanged : public Event 
        {
          /*! 
            \brief Constructor.

            \param layer The pointer to the layer.

            \param visibility The new visibility state: VISIBLE, NOT_VISIBLE, PARTIALLY_VISIBLE.
          */
          LayerVisibilityChanged(te::map::AbstractLayer* layer, te::map::Visibility visibility) :
            Event(LAYER_VISIBILITY_CHANGED),
            m_layer(layer),
            m_visibility(visibility)
          {
          };

          te::map::AbstractLayer* m_layer;     //!< Pointer to the layer.
          te::map::Visibility m_visibility;    //!< Layer visibility.
        };

        /*!
          \struct LayerSelected

          \brief This event signals that the selected layer on layer explorer has changed.
        
          \ingroup afevents
        */
        struct LayerSelected : public Event
        {
          /*!
            \brief Construtor.

            \param layer The new selected layer.
          */
          LayerSelected(te::map::AbstractLayer* layer) :
            Event(LAYER_SELECTED),
            m_layer(layer)
          {
          }
  
          te::map::AbstractLayer* m_layer;  //!< Pointer to the selected layer.
        };

        /*!
          \struct LayerSelectionChanged

          \brief This event signals that the selected objects in the layer has changed.
        */
        struct LayerSelectionChanged : public Event
        {
          /*!
            \brief Constructor

            \param layer The layer that its selected objects has changed.
          */
          LayerSelectionChanged(te::map::AbstractLayer* layer) :
            Event(LAYER_SELECTION_CHANGED),
            m_layer(layer)
          {
          }

          te::map::AbstractLayer* m_layer; //!< Layer that has changed its selection.
        };

        /*!
          \struct LayersChanged
          
          \brief This event signals that the layout of layers has changed.
        */
        struct LayersChanged : public Event
        {
          /*!
            \brief Constructor.

            \param layers The layers.
          */
          LayersChanged(const std::vector<te::map::AbstractLayerPtr>& layers) :
          Event(LAYERS_CHANGED),
            m_layers(layers)
          {
          }

          std::vector<te::map::AbstractLayerPtr> m_layers; //!< Vector of layers.
        };
      }
    }
  }
}

#endif //__TERRALIB_QT_AF_EVENTS_INTERNAL_LAYEREVENTS_H