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
  \file terralib/edit/qt/tools/MoveGeometryTool.h

  \brief This class implements a concrete tool to move geometries.
*/

#ifndef __TERRALIB_EDIT_QT_INTERNAL_MOVEGEOMETRYTOOL_H
#define __TERRALIB_EDIT_QT_INTERNAL_MOVEGEOMETRYTOOL_H

// TerraLib
#include "../../../geometry/Envelope.h"
#ifndef Q_MOC_RUN
#include "../../../maptools/AbstractLayer.h"
#endif
#include "../core/command/AddCommand.h"
#include "../core/UndoStackManager.h"
#include "../Config.h"
#include "GeometriesUpdateTool.h"

// Qt
#include <QPointF>

//STL
#include <map>

namespace te
{
  namespace qt
  {
    namespace widgets
    {
      class Canvas;
      class MapDisplay;
    }
  }

  namespace edit
  {
// Forward declaration
    class Feature;

    /*!
      \class MoveGeometryTool

      \brief This class implements a concrete tool to move geometries.
    */
    class TEEDITQTEXPORT MoveGeometryTool : public GeometriesUpdateTool
    {
      Q_OBJECT

      public:

        /** @name Initializer Methods
          *  Methods related to instantiation and destruction.
          */
        //@{

        /*!
          \brief It constructs a move geometry tool associated with the given map display.

          \param display The map display associated with the tool.
          \param parent The tool's parent.

          \note The tool will NOT take the ownership of the given pointers.
        */
        MoveGeometryTool(te::qt::widgets::MapDisplay* display, const te::map::AbstractLayerPtr& layer, const te::edit::MouseEventEdition mouseEventToSave, QObject *parent = 0);
        /*! \brief Destructor. */
        ~MoveGeometryTool();

        //@}

        /** @name AbstractTool Methods
          *  Methods related with tool behavior.
          */
        //@{

        bool mousePressEvent(QMouseEvent* e);

        bool mouseMoveEvent(QMouseEvent* e);

        bool mouseReleaseEvent(QMouseEvent* e);

        //@}

        void resetVisualizationTool();

      private:

        void reset();

        void pickFeature(const QPointF& pos);

        te::gm::Envelope buildEnvelope(const QPointF& pos);

        void draw();

        void updateCursor();

        void storeFeature();

        void storeUndoCommand();

      private slots:

        void onExtentChanged();

      protected:

        bool m_selected;
        bool m_moveStarted;                            //!< Flag that indicates if move operation was started.
        QPointF m_origin;                              //!< Origin point on mouse pressed.
        QPointF m_delta;                               //!< Difference between pressed point and destination point on mouse move.
        std::vector<te::edit::Feature*> m_vecFeature;
        MouseEventEdition m_mouseEventToSave;
        UndoStackManager& m_stack;
    };

  }   // end namespace edit
}     // end namespace te

#endif  // __TERRALIB_EDIT_QT_INTERNAL_MOVEGEOMETRYTOOL_H
