/*  Copyright (C) 2001-2009 National Institute For Space Research (INPE) - Brazil.

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
  \file terralib/qt/widgets/rp/RegisterWizard.h

  \brief This file defines the RegisterWizard class.
*/

#ifndef __TERRALIB_QT_WIDGETS_RP_INTERNAL_REGISTERWIZARD_H
#define __TERRALIB_QT_WIDGETS_RP_INTERNAL_REGISTERWIZARD_H

// TerraLib
#include "../../../maptools/AbstractLayer.h"
#include "../Config.h"

// STL
#include <memory>

// Qt
#include <QtGui/QWizard>

namespace te
{
  namespace qt
  {
    namespace widgets
    {
      class LayerSearchWizardPage;
      class TiePointLocatorDialog;
      class RasterInfoWizardPage;
      /*!
        \class RegisterWizard

        \brief A Qt wizard that allows users to register a  image.
      */
      class TEQTWIDGETSEXPORT RegisterWizard : public QWizard
      {
        Q_OBJECT

        public:

          RegisterWizard(QWidget* parent);

          ~RegisterWizard(); 

        public:

          virtual bool validateCurrentPage();

          void setList(std::list<te::map::AbstractLayerPtr>& layerList);

          te::map::AbstractLayerPtr getOutputLayer();

        protected:

          void addPages();

          bool execute();

        protected slots:

          void onHelpButtonClicked();

        private:

          std::auto_ptr<te::qt::widgets::LayerSearchWizardPage> m_layerRefPage;
          std::auto_ptr<te::qt::widgets::LayerSearchWizardPage> m_layerAdjPage;
          std::auto_ptr<te::qt::widgets::RasterInfoWizardPage> m_rasterInfoPage;
          std::auto_ptr<te::qt::widgets::TiePointLocatorDialog> m_tiePointLocatorDialog;

          te::map::AbstractLayerPtr m_refLayer;
          te::map::AbstractLayerPtr m_adjLayer;
          te::map::AbstractLayerPtr m_outputLayer;
      };
    }   // end namespace widgets
  }     // end namespace qt
}       // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_RP_INTERNAL_REGISTERWIZARD_H
