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
  \file MultiThreadMapDisplay.h

  \brief A multi thread Qt4 widget to control the display of a set of layers.
*/

#ifndef __TERRALIB_QT_WIDGETS_INTERNAL_MULTITHREAD_MAPDISPLAY_H
#define __TERRALIB_QT_WIDGETS_INTERNAL_MULTITHREAD_MAPDISPLAY_H

// TerraLib
#include "MapDisplay.h"

// Qt
#include <QtGui/QImage>

// STL
#include <vector>

namespace te
{
  namespace qt
  {
    namespace widgets
    {
// Forward declarations
      class DrawLayerThread;

      /*!
        \class MultiThreadMapDisplay

        \brief A multi thread Qt4 widget to control the display of a set of layers.
      */
      class TEQTWIDGETSEXPORT MultiThreadMapDisplay : public MapDisplay
      {
        Q_OBJECT

        public:

          /** @name Initializer Methods
           *  Methods related to instantiation and destruction.
           */
          //@{

          /*!
            \brief It constructs an empty multi thread map display with the given dimensions which is a child of parent, with widget flags set to f.

            \param size The map display size.
            \param showFeedback A flag that indicates if the map display will show drawing feedback.
            \param parent The widget's parent.
            \param f Widget window flags.
          */
          MultiThreadMapDisplay(const QSize& size, const bool& showFeedback = true, QWidget* parent = 0, Qt::WindowFlags f = 0);

          /*! \brief Destructor. */
          ~MultiThreadMapDisplay();

          //@}

          /** @name MapDisplay Methods
           *  Methods to configure the MapDisplay.
           */
          //@{

          virtual void setLayerList(const std::list<te::map::AbstractLayer*>& order);

          virtual void setExtent(const te::gm::Envelope& e);

          //@}

          /*!
            \brief Transforms the given point, in screen coordinates, to a point in world coordinates.

            \param p A point in screen coordinates.

            \return The point in world coordinates.

            \note This method will return a null point if the transform can not be done.
          */
          virtual QPointF transform(const QPointF& p);

        protected:

          /*!
            \brief It displays the all layers.

            \note It may need to retrieve data in order top display the layers.
          */
          virtual void draw();

        private:

          void updateTransform(); /*  Note: For while... I need the class CoordTransform! */

        protected slots:

          void showFeedback(const QImage& image);

          void onDrawLayerFinished(const int& index, const QImage& image);

        private:

          /** @name Copy Constructor and Assignment Operator
           *  Copy constructor and assignment operator not allowed.
           */
          //@{

          /*!
            \brief Copy constructor not allowed.

            \param rhs The right-hand-side copy that would be used to copy from.
          */
          MultiThreadMapDisplay(const MultiThreadMapDisplay& rhs);

          /*!
            \brief Assignment operator not allowed.

            \param rhs The right-hand-side copy that would be used to copy from.

            \return A reference to this object.
          */
          MultiThreadMapDisplay& operator=(const MultiThreadMapDisplay& rhs);

          //@}

        protected:

          std::vector<DrawLayerThread*> m_threads; // The set of threads used to draw the layer list.
          std::map<int, QImage> m_images;          // The set of images built by each thread. It will be used to compose the final result, keeping the layer list order.
          bool m_showFeedback;                     // A flag that indicates if the map display will show drawing feedback.
          bool m_isDrawing;                        // A flag that indicates if the map display is drawing.

          QMatrix m_matrix; /*  Used to converts coordinates screen -> world. Note: For while... I need the class CoordTransform! */
      };

    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_INTERNAL_MULTITHREAD_MAPDISPLAY_H