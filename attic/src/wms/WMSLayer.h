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
  \file terralib/wms/WMSLayer.h

  \brief A layer with reference to a WMS Layer.
*/

#ifndef __TERRALIB_WMS_INTERNAL_WMSLAYER_H
#define __TERRALIB_WMS_INTERNAL_WMSLAYER_H

// TerraLib
#include "../maptools/AbstractLayer.h"
#include "Config.h"

// STL
#include <memory>

namespace te
{
  namespace wms
  {
    /*!
      \class WMSLayer

      \brief A layer with reference to a WMS Layer.
    */
    class TEWMSEXPORT WMSLayer : public te::map::AbstractLayer
    {
      public:

        /*!
          \brief It initializes a new layer.

          \param parent The parent layer (NULL if it has no parent).
        */
        WMSLayer(te::map::AbstractLayer* parent = 0);

        /*!
          \brief It initializes a new layer.

          \param id     The layer id.
          \param parent The parent layer (NULL if it has no parent).
        */
        WMSLayer(const std::string& id, te::map::AbstractLayer* parent = 0);

        /*!
          \brief It initializes a new Layer.

          \param id     The layer id.
          \param title  The title is a brief description about the layer.
          \param parent The parent layer (NULL if it has no parent).
        */
        WMSLayer(const std::string& id, const std::string& title, te::map::AbstractLayer* parent = 0);

        /*! \brief Destructor. */
        ~WMSLayer();

        std::auto_ptr<te::map::LayerSchema> getSchema() const;

        std::auto_ptr<te::da::DataSet> getData(te::common::TraverseType travType = te::common::FORWARDONLY,
                                               const te::common::AccessPolicy accessPolicy = te::common::RAccess) const;

        std::auto_ptr<te::da::DataSet> getData(const std::string& propertyName,
                                               const te::gm::Envelope* e,
                                               te::gm::SpatialRelation r = te::gm::INTERSECTS,
                                               te::common::TraverseType travType = te::common::FORWARDONLY,
                                               const te::common::AccessPolicy accessPolicy = te::common::RAccess) const;

        std::auto_ptr<te::da::DataSet> getData(const std::string& propertyName,
                                               const te::gm::Geometry* g,
                                               te::gm::SpatialRelation r,
                                               te::common::TraverseType travType = te::common::FORWARDONLY,
                                               const te::common::AccessPolicy accessPolicy = te::common::RAccess) const;

        std::auto_ptr<te::da::DataSet> getData(te::da::Expression* restriction,
                                               te::common::TraverseType travType = te::common::FORWARDONLY,
                                               const te::common::AccessPolicy accessPolicy = te::common::RAccess) const;

        std::auto_ptr<te::da::DataSet> getData(const te::da::ObjectIdSet* oids,
                                               te::common::TraverseType travType = te::common::FORWARDONLY,
                                               const te::common::AccessPolicy accessPolicy = te::common::RAccess) const;

        bool isValid() const;

        void draw(te::map::Canvas* canvas, const te::gm::Envelope& bbox, int srid, const double& scale, bool* cancel);

        const std::string& getType() const;

        const std::string& getDataSetName() const;

        void setDataSetName(const std::string& name);

        const std::string& getDataSourceId() const;

        void setDataSourceId(const std::string& id);

        const std::string& getRendererType() const;

        void setRendererType(const std::string& t);

        void setWidth(const std::size_t& width);

        void setHeight(const std::size_t& height);

        void setFormat(const std::string& format);

      private:

        std::string buildRequest() const;

      private:

        std::string m_datasetName;                            //!< The dataset name where we will retrieve the layer data.
        std::string m_datasourceId;                           //!< The DataSource associated to this layer.
        std::string m_rendererType;                           //!< A pointer to the internal renderer used to paint this layer.

        std::size_t m_width;                                  //!< Width that will be requested.
        std::size_t m_height;                                 //!< Height that will be requested.
        std::string m_format;                                 //!< Format that will be requested.

        mutable std::auto_ptr<te::map::LayerSchema> m_schema; //!< The WMS layer schema.

        static const std::string sm_type;                      //!< A static data member used in the implementation of getType method.
    };

    typedef boost::intrusive_ptr<WMSLayer> WMSLayerPtr;

  } // end namespace wms
}   // end namespace te

#endif  // __TERRALIB_WMS_INTERNAL_WMSLAYER_H
