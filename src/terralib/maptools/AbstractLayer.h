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
  \file terralib/maptools/AbstractLayer.h

  \brief This is the base class for Layers.
*/

#ifndef __TERRALIB_MAPTOOLS_INTERNAL_ABSTRACTLAYER_H
#define __TERRALIB_MAPTOOLS_INTERNAL_ABSTRACTLAYER_H

// TerraLib
#include "../common/Enums.h"
#include "../common/TreeItem.h"
#include "../dataaccess/dataset/DataSet.h"
#include "../dataaccess/dataset/DataSetType.h"
#include "../geometry/Enums.h"
#include "../geometry/Envelope.h"
#include "Config.h"
#include "Enums.h"

// STL
#include <memory>
#include <string>

namespace te
{
// Forward declaration
  namespace dt { class Property; }

  namespace da
  {
    class Expression;
    class ObjectIdSet;
  }

  namespace gm { class Geometry; }

  namespace se { class Style; }

  namespace map
  {
// Forward declaration
    class Canvas;
    class Chart;
    class Grouping;

    typedef te::da::DataSetType LayerSchema;

    /*!
      \class AbstractLayer

      \brief This is the base class for layers.

      \ingroup map

      \sa TreeItem, DataSetLayer, QueryLayer, FolderLayer, RasterLayer
    */
    class TEMAPEXPORT AbstractLayer : public te::common::TreeItem
    {
      public:

        /*!
          \brief It initializes a new layer.

          \param parent The parent layer (NULL if it has no parent).
        */
        AbstractLayer(AbstractLayer* parent = 0);

        /*!
          \brief It initializes a new layer.

          \param id     The layer id.
          \param parent The parent layer (NULL if it has no parent).
        */
        AbstractLayer(const std::string& id, AbstractLayer* parent = 0);

        /*!
          \brief It initializes a new layer.

          \param id     The layer id.
          \param title  The title is a brief description about the layer.
          \param parent The parent layer (NULL if it has no parent).
        */
        AbstractLayer(const std::string& id, const std::string& title, AbstractLayer* parent = 0);

        /*! \brief Virtual destructor. */
        virtual ~AbstractLayer();

        /*!
          \brief It returns the layer id.

          \return The layer id.
        */
        virtual const std::string& getId() const;

        /*!
          \brief It sets the layer id.

          \param id The layer id.
        */
        virtual void setId(const std::string& id);

        /*!
          \brief It returns the layer title.

          \return The layer title.
        */
        virtual const std::string& getTitle() const;

        /*!
          \brief It sets the layer title.

          \param title The layer title.
        */
        virtual void setTitle(const std::string& title);

        /*!
          \brief It returns a list with the descendants of this layer.

          \return A list containing the descendants of this layer.
        */
        std::vector<te::map::AbstractLayer*> getDescendants();

        /*!
          \brief It returns a list with the ancestors of this layer.

          \return A list containing the ancestors of this layer.
        */
        std::vector<te::map::AbstractLayer*> getAncestors();

        /*!
          \brief It returns the layer visibility.

          This information can be used, for instance, to know if the layer must be drawn.

          \return The layer visibility.
        */
        virtual Visibility getVisibility() const;

        /*!
          \brief It sets the layer visibility.

          This information can be used, for instance, to know if the layer must be drawn.

          \param v The layer visibility.
        */
        virtual void setVisibility(Visibility v);

        /*! 
          \brief It gets the flag that indicates if the layer visibility has changed.

          \return If true, the layer visibility has changed.
        */
        bool hasVisibilityChanged();

        /*! 
          \brief It sets that the status of the layer visibility is to be changed or not..

          \param visChanged If true, the status of the layer visibility is to be changed; otherwise, it returns false.
        */
        void setVisibilityAsChanged(bool visChanged);

        /*!
          \brief It updates the visibility of the ancestors of this layer, if any.
        */
        void updateVisibilityOfAncestors();

        /*!
          \brief It updates the visibility of this layer.
        */
        virtual void updateVisibility();

        /*!
          \brief It returns the Layer extent (or minimum bounding box).

          \return The Layer extent (or minimum bounding box) with coordinates in the same SRS as the layer.
        */
        virtual const te::gm::Envelope& getExtent() const;

        /*!
          \brief It sets the Layer extent (or minimum bounding box).

          \param mbr The Layer extent (or minimum bounding box).

          \pre The mbr coordinates must be in the same SRS as the layer.
        */
        virtual void setExtent(const te::gm::Envelope& mbr);

        /*!
          \brief It returns the Spatial Reference System ID associated to the Layer.

          \return The Spatial Reference System ID associated to the Layer.
        */
        virtual int getSRID() const;

        /*!
          \brief It sets the Spatial Reference System ID associated to the Layer.

          \param srid The Spatial Reference System ID to be associated to the Layer.
        */
        virtual void setSRID(int srid);

        /*!
          \brief It adds the given oids to the selected group of this Layer.

          \param oids The oids that will be added.

          \note The layer will take the ownership of the given pointer.
        */
        virtual void select(te::da::ObjectIdSet* oids);

        /*!
          \brief It returns the selected group of this Layer.

          \retrun The selected group of this Layer.

          \note The caller will NOT take the ownership of the given pointer.
        */
        virtual const te::da::ObjectIdSet* getSelected() const;

        /*!
          \brief It removes the given oids from the selected group of this Layer.

          \param oids The oids that will be removed.

          \note The layer will NOT take the ownership of the given pointer.
        */
        virtual void deselect(const te::da::ObjectIdSet* oids);

        /*!
          \brief It clears the selected group of this Layer.
        */
        virtual void clearSelected();

        /*!
          \brief It returns the Style associated to the layer.

          \return The Style associated to the layer.

          \note The caller will NOT take the ownership of the given pointer.
        */
        virtual te::se::Style* getStyle() const;

        /*!
          \brief It sets the Style associated to the layer.

          \param style The Style associated to the layer. 

          \note The layer will take the ownership of the given pointer.
        */
        virtual void setStyle(te::se::Style* style);

        /*!
        \brief It returns the selection Style associated to the layer.

        \return The Style associated to the layer.

        \note The caller will NOT take the ownership of the given pointer.
        */
        virtual te::se::Style* getSelectionStyle() const;

        /*!
        \brief It sets the selection Style associated to the layer.

        \param style The Style associated to the layer.

        \note The layer will take the ownership of the given pointer.
        */
        virtual void setSelectionStyle(te::se::Style* style);

        /*!
          \brief It returns the Grouping associated to the Layer.

          \return The Grouping associated to the Layer.

          \note The caller will NOT take the ownership of the given pointer.
        */
        virtual te::map::Grouping* getGrouping() const;

        /*!
          \brief It sets the Grouping associated to the Layer.

          \param grouping The Grouping to be associated to the Layer.

          \note The layer will take the ownership of the given pointer.
        */
        virtual void setGrouping(te::map::Grouping* grouping);

        /*!
          \brief It returns the Chart associated to the Layer.

          \return The Chart associated to the Layer.

          \note The caller will NOT take the ownership of the given pointer.
        */
        virtual te::map::Chart* getChart() const;

        /*!
          \brief It sets the Chart associated to the Layer.

          \param chart The Chart to be associated to the Layer.

          \note The layer will take the ownership of the given pointer.
        */
        virtual void setChart(te::map::Chart* chart);

        /*!
          \brief

          \return
        */
        virtual const std::string& getGeomPropertyName() const;

        /*!
          \brief

          \param name
        */
        virtual void setGeomPropertytName(const std::string& name);

        /*!
          \brief It returns the layer schema.

          \return The Layer schema.

          \note The caller will take the ownership of the returned layer schema.
        */
        virtual std::auto_ptr<LayerSchema> getSchema() const = 0;

        /*!
          \brief It gets the dataset identified by the layer name.

          \param travType The traverse type associated to the returned dataset. 
          \param accessPolicy Access policy.

          \return The caller of this method will take the ownership of the returned dataset.

          \exception Exception It can throws an exception if:
                     <ul>
                     <li>something goes wrong during the data retrieval</li>
                     <li>if the data source driver doesn't support the traversal type</li>
                     <li>if the data source driver doesn't support the access policy</li>
                     </ul>

          \note Not thread-safe!
        */
        virtual std::auto_ptr<te::da::DataSet> getData(te::common::TraverseType travType = te::common::FORWARDONLY,
                                                       const te::common::AccessPolicy accessPolicy = te::common::RAccess) const = 0;

        /*!
          \brief It gets the dataset identified by the layer name using a spatial filter over the specified property.

          \param propertyName  The name of the spatial property used to apply the spatial filter.
          \param e             A rectangle to be used as a spatial filter when retrieving datasets.
          \param r             The spatial relation to be used during the filter.
          \param travType      The traverse type associated to the returned dataset. 
          \param accessPolicy  Access policy.

          \return The caller of this method will take the ownership of the returned DataSet.

          \exception Exception It can throws an exception if:
                     <ul>
                     <li>something goes wrong during data retrieval</li>
                     <li>if the data source driver doesn't support the traversal type</li>
                     <li>if the data source driver doesn't support the access policy</li>
                     </ul>

          \note Transactor will not take the ownership of the given envelope.

          \note The envelope coordinates should be in the same coordinate system as the dataset.

          \note Not thread-safe!
        */
        virtual std::auto_ptr<te::da::DataSet> getData(const std::string& propertyName,
                                                       const te::gm::Envelope* e,
                                                       te::gm::SpatialRelation r = te::gm::INTERSECTS,
                                                       te::common::TraverseType travType = te::common::FORWARDONLY,
                                                       const te::common::AccessPolicy accessPolicy = te::common::RAccess) const = 0;

        /*!
          \brief It gets the dataset identified by the layer name using a spatial filter over the given geometric property.

          \param propertyName  The name of the spatial property used to apply the spatial filter.
          \param g             A geometry to be used as a spatial filter when retrieving datasets.
          \param r             The spatial relation to be used during the filter.
          \param travType      The traverse type associated to the returned dataset. 
          \param accessPolicy  Access policy.

          \return The caller of this method will take the ownership of the returned DataSet.

          \exception Exception It can throws an exception if:
                     <ul>
                     <li>something goes wrong during data retrieval</li>
                     <li>if the data source driver doesn't support the traversal type</li>
                     <li>if the data source driver doesn't support the access policy</li>
                     </ul>

          \note Transactor will not take the ownership of the given geometry.

          \note The geometry coordinates should be in the same coordinate system as the dataset.
          
          \note Not thread-safe!
        */
        virtual std::auto_ptr<te::da::DataSet> getData(const std::string& propertyName,
                                                       const te::gm::Geometry* g,
                                                       te::gm::SpatialRelation r,
                                                       te::common::TraverseType travType = te::common::FORWARDONLY,
                                                       const te::common::AccessPolicy accessPolicy = te::common::RAccess) const = 0;

        /*!
          \brief It gets the dataset identified by the layer name using the given restriction.

          \param restriction  The restriction expression that will be used.
          \param travType     The traverse type associated to the returned dataset. 
          \param accessPolicy Access policy.

          \return The caller of this method will take the ownership of the returned DataSet.

          \exception Exception It can throws an exception if:
                      <ul>
                      <li>something goes wrong during data retrieval</li>
                      <li>if the data source driver doesn't support the traversal type</li>
                      <li>if the data source driver doesn't support the access policy</li>
                      </ul>

          \note Not thread-safe!
        */
        virtual std::auto_ptr<te::da::DataSet> getData(te::da::Expression* restriction,
                                                       te::common::TraverseType travType = te::common::FORWARDONLY,
                                                       const te::common::AccessPolicy accessPolicy = te::common::RAccess) const = 0;

        /*!
          \brief It gets the dataset from the given set of objects identification.

          \param oids         The set of object ids.
          \param travType     The traverse type associated to the returned dataset.
          \param accessPolicy Access policy.

          \return The caller of this method will take the ownership of the returned dataset.

          \exception Exception It can throws an exception if:
                     <ul>
                     <li>something goes wrong during the data retrieval</li>
                     <li>if the data source driver doesn't support the traversal type</li>
                     <li>if the data source driver doesn't support the access policy</li>
                     </ul>

          \note Not thread-safe!
        */
        virtual std::auto_ptr<te::da::DataSet> getData(const te::da::ObjectIdSet* oids,
                                                       te::common::TraverseType travType = te::common::FORWARDONLY,
                                                       const te::common::AccessPolicy accessPolicy = te::common::RAccess) const = 0;

        /*!
          \brief It returns the layer type.

          \return The layer type.
        */
        virtual const std::string& getType() const = 0;

        /*!
          \brief It returns true if the layer can be used for instance to draw, otherwise, it returns false.

          This method can be used to check if the data referenced by the layer is available (accessible), or not.

          \return True, if the layer is valid, otherwise, it returns false.
        */
        virtual bool isValid() const = 0;

        /*!
          \brief It draws the layer geographic objects in the given canvas using the informed SRS.

          The informed bounding box can be used to constraint the layer objects to be drawn.

          The bbox coordinates must be in the same Spatial Reference System given by srid.

          \param canvas The canvas were the layer objects will be drawn.
          \param bbox   The interest area to render the map.
          \param srid   The SRS to be used to draw the layer objects.
          \param scale  The current scale of display.
        */
        virtual void draw(Canvas* canvas, const te::gm::Envelope& bbox, int srid, const double& scale, bool* cancel) = 0;

        /*!
          \brief It returns the composition mode.

          Composition modes are used to specify how the pixels in one image, the source, are merged with the pixel in another image, the destination.

          \return The composition mode enum.
        */
        te::map::CompositionMode getCompositionMode() const;

        /*!
          \brief It sets the composition mode.

          Composition modes are used to specify how the pixels in one image, the source, are merged with the pixel in another image, the destination.

          \param mode The composition mode enum.
        */
        void setCompositionMode(te::map::CompositionMode mode);

        /*!
        \brief It returns the encoding type.

        Char enconding is used to inform to data source how to load string information from data.

        \return The encoding type.
        */
        te::core::EncodingType getEncoding() const;

        /*!
        \brief It set the encoding type.

        Char enconding is used to inform to data source how to load string information from data.

        \param The encoding type.
        */
        void setEncoding(te::core::EncodingType et);

        /*!
          \brief Its indicate that the layer schema is out of date.
        */
        virtual void setOutOfDate();

        /*!
        \brief

        \return
        */
        const std::string& getDataSetName() const;

        /*!
        \brief

        \param name
        */
        void setDataSetName(const std::string& name);

        /*!
          \brief

          \return
        */
        virtual const std::string& getDataSourceId() const;

        /*!
          \brief

          \param id
        */
        virtual void setDataSourceId(const std::string& id);

      protected:

        std::string m_id;                            //!< Layer id.
        std::string m_title;                         //!< A brief description of this Layer that can be used by applications to show a text identifying this layer.
        te::gm::Envelope m_mbr;                      //!< The layer bounding box.
        int m_srid;                                  //!< The identifier of the layer spatial reference system.
        Visibility m_visibility;                     //!< It indicates the layer visibility.
        bool m_visibilityChanged;                    //!< It indicates if the layer visibility has changed.
        te::da::ObjectIdSet* m_selected;             //!< The selected group of the layer.
        te::se::Style* m_style;                      //!< The style to be applied to the geographic objects in the layer.
        te::se::Style* m_selectionStyle;             //!< The selection style to be applied to the geographic objects in the layer.
        te::map::Grouping* m_grouping;               //!< The grouping information.
        te::map::Chart* m_chart;                     //!< The chart information.
        std::string m_geomPropertyName;              //!< The name of the referenced geometry property.
        te::map::CompositionMode m_compositionMode;  //!< The composition mode used to merged the canvas.
        std::string m_datasetName;                   //!< The dataset name where we will retrieve the layer objects.
        std::string m_datasourceId;                  //!< DataSource id.
        te::core::EncodingType m_encoding;           //!< The char encoding of the layer;
    };

    typedef boost::intrusive_ptr<AbstractLayer> AbstractLayerPtr;

  } // end namespace map
}   // end namespace te

#endif  // __TERRALIB_MAPTOOLS_INTERNAL_ABSTRACTLAYER_H

