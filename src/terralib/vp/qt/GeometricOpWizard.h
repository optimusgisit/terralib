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
  \file terralib/vp/qt/GeometricOpWizard.h

  \brief A Qt dialog that allows users to run the basic geometric operations
  defined by VP module.
*/

#ifndef __TERRALIB_VP_INTERNAL_GEOMETRICOPWIZARD_H
#define __TERRALIB_VP_INTERNAL_GEOMETRICOPWIZARD_H

// TerraLib
#ifndef Q_MOC_RUN
#include "../../dataaccess/datasource/DataSourceInfo.h"
#include "../../maptools/AbstractLayer.h"
#include "../../qt/widgets/layer/search/LayerSearchWizardPage.h"
#endif
#include "../Config.h"
#include "../Enums.h"

// STL
#include <memory>

// Qt
#include <QWizard>

namespace te
{
  namespace vp
  {
    class GeometricOpWizardPage;
    class GeometricOpOutputWizardPage;

    /*!
      \class GeometricOpWizard

      \brief This class is GUI used to define the wizard for the VP geometric
      operation.
    */
    class TEVPEXPORT GeometricOpWizard : public QWizard
    {
      Q_OBJECT

      public:

        GeometricOpWizard(QWidget* parent);

        ~GeometricOpWizard();

      public:

        virtual bool validateCurrentPage();

        void setList(std::list<te::map::AbstractLayerPtr>& layerList);

        void setLayer(te::map::AbstractLayerPtr layer);

        te::map::AbstractLayerPtr getInLayer();

        std::vector<te::map::AbstractLayerPtr> getOutLayer();

      protected:

        void addPages();

        bool execute();

      private:

        std::auto_ptr<te::vp::GeometricOpWizardPage> m_geomOpPage;
        std::auto_ptr<te::vp::GeometricOpOutputWizardPage> m_geomOpOutputPage;
        std::auto_ptr<te::qt::widgets::LayerSearchWizardPage> m_layerSearchPage;

        te::map::AbstractLayerPtr m_inLayer;
        te::da::DataSourceInfoPtr m_outputDatasource;
        std::vector<te::map::AbstractLayerPtr> m_outLayer;
        std::vector<te::vp::GeometricOperation> m_ops;
        std::string m_attribute;

        int m_layerSearchId;
    };
  }     // end namespace vp
}       // end namespace te

#endif  // __TERRALIB_VP_INTERNAL_GEOMETRICOPWIZARD_H
