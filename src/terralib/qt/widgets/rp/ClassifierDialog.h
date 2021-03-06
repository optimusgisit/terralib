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
  \file terralib/qt/widgets/rp/ClassifierDialog.h

  \brief A dialog used to execute image classification.
*/

#ifndef __TERRALIB_QT_WIDGETS_RP_INTERNAL_CLASSIFIERDIALOG_H
#define __TERRALIB_QT_WIDGETS_RP_INTERNAL_CLASSIFIERDIALOG_H

// TerraLib
#include "../Config.h"
#ifndef Q_MOC_RUN
#include "../../../raster/Raster.h"
#endif

// Qt
#include <QDialog>

// STL
#include <map>
#include <string>

// Boost
#ifndef Q_MOC_RUN
#include <boost/shared_ptr.hpp>
#endif

// Forward user interface declaration
namespace Ui
{
  class ClassifierForm;
}

namespace te
{
  namespace qt
  {
    namespace widgets
    {
      /*!
        \class ClassifierDialog

        \brief A dialog used to execute image classification.
      */
      class TEQTWIDGETSEXPORT ClassifierDialog : public QDialog
      {
        Q_OBJECT

        public:

          /*!
            \brief Constructs a basic dialog which is a child of parent, with widget flags set to f.

            \param inputRasterPtr      Input raster pointer.
            \param inputPolygons       Vector of input polygons.
            \param outpuRasterDSType   Output raster data source type (as described in te::rst::RasterFactory).
            \param outpuRasterInfo     The necessary information to create the raster (as described in te::rst::RasterFactory).
            \param parent              Parent widget pointer.
            \param f                   Widget flags.
          */
          ClassifierDialog(const te::rst::Raster* inputRasterPtr,
                           std::vector<te::gm::Polygon*> inputPolygons,
                           const std::string& outpuRasterDSType,
                           const std::map<std::string, std::string>& outpuRasterInfo,
                           QWidget* parent = 0,
                           Qt::WindowFlags f = 0);

          /*! \brief Destructor. */
          ~ClassifierDialog();

          /*!
            \brief Returns the output result raster.

            \param outputRasterPtr A pointer to the output result raster

            \return true if OK, false on errors.
          */
          bool getOutputRaster(boost::shared_ptr< te::rst::Raster >& outputRasterPtr);

        protected slots:

          void on_okPushButton_clicked();

        private:

          Ui::ClassifierForm* m_uiPtr;                               //!< User interface.
          const te::rst::Raster* m_inputRasterPtr;                   //!< Input raster pointer.
          std::vector<te::gm::Polygon*> m_inputPolygons;             //!< Vector of input polygons.
          std::string m_outpuRasterDSType;                           //!< Output raster data source type (as described in te::rst::RasterFactory).
          std::map<std::string, std::string> m_outpuRasterInfo;      //!< The necessary information to create the raster (as described in te::rst::RasterFactory).
          boost::shared_ptr<te::rst::Raster> m_outputRasterPtr;      //!< Output raster pointer.
      };

    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_RP_INTERNAL_CLASSIFIERDIALOG_H
