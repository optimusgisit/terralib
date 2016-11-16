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
  \file terralib/qt/plugins/edit/ToolBar.h

  \brief The main toolbar of TerraLib Edit Qt plugin.
*/

#ifndef __TERRALIB_QT_PLUGINS_EDIT_INTERNAL_TOOLBAR_H
#define __TERRALIB_QT_PLUGINS_EDIT_INTERNAL_TOOLBAR_H

// TerraLib
#ifndef Q_MOC_RUN
#include "../../../maptools/AbstractLayer.h"
#endif
#include "Config.h"

// Qt
#include <QAction>
#include <QIcon>
#include <QList>
#include <QToolBar>
#include <QUndoView>

// STL
#include <string>

namespace te
{
  namespace edit
  {
    class GeometriesUpdateTool;
  }

  namespace qt
  {
    namespace af
    {
      class MapDisplay;

      namespace evt
      {
        struct Event;
      }
    }

    namespace plugins
    {
      namespace edit
      {
        /*!
          \class ToolBar

          \brief The main toolbar of TerraLib Edit Qt plugin.
        */
        class ToolBar : public QObject
        {
          Q_OBJECT

        public:

          ToolBar(QObject* parent = 0);

          ~ToolBar();

          QToolBar* get() const;

          void updateLayer(te::map::AbstractLayer* layer, const bool& stashed);

        public slots:

          void onEditActivated(bool checked);

        protected slots:

          void onSaveActivated();

          void onVertexToolActivated(bool checked);

          void onCreatePolygonToolActivated(bool checked);

          void onCreateLineToolActivated(bool checked);

          void onMoveGeometryToolActivated(bool checked);

          void onSnapOptionsActivated();

          void onAggregateAreaToolActivated(bool checked);

          void onSubtractAreaToolActivated(bool checked);

          void onDeleteGeometryToolActivated(bool checked);

          void onMergeGeometriesToolActivated(bool checked);

          void onCreateUndoViewActivated(bool checked);

          void onToolDeleted();

          void onSplitPolygonToolActivated(bool checked);

          void onFeatureAttributesActivated(bool checked);

          void onResetVisualizationToolActivated(bool checked);

          void onCreatePointToolActivated(bool checked);

          void onDeletePartToolActivated(bool checked);

          void onCreateLayerActivated();

        Q_SIGNALS:

          /*! This signal is emitted when the layer selection changed. */
          void layerSelectedObjectsChanged(const te::map::AbstractLayerPtr& layer);

          void triggered(te::qt::af::evt::Event* e);

          void stashed(te::map::AbstractLayer* layer);

          void geometriesEdited();

        protected:

          void enableCurrentTool(const bool& enable);

          void setCurrentTool(te::edit::GeometriesUpdateTool* tool, te::qt::af::MapDisplay* display);

          QToolBar* m_toolBar;
          QAction* m_editAction;
          QAction* m_saveAction;
          QAction* m_clearEditionAction;
          QAction* m_vertexToolAction;
          QAction* m_createPolygonToolAction;
          QAction* m_createLineToolAction;
          QAction* m_moveGeometryToolAction;
          QAction* m_snapOptionsAction;
          QAction* m_deleteGeometryToolAction;
          QAction* m_aggregateAreaToolAction;
          QAction* m_subtractAreaToolAction;
          QAction* m_featureAttributesAction;
          QAction* m_splitPolygonToolAction;
          QAction* m_mergeGeometriesToolAction;
          QAction* m_createPointToolAction;
          QAction* m_deletePartToolAction;
          QAction* m_createLayerAction;
          QAction* m_undoToolAction;
          QAction* m_redoToolAction;
          QList<QAction*> m_tools;
          QUndoView* m_undoView;

          te::edit::GeometriesUpdateTool* m_currentTool;

          bool m_usingStash;

          bool m_layerIsStashed;

          void enableActionsByGeomType(QList<QAction*> acts, const bool& enable);

        public:

          te::map::AbstractLayerPtr getSelectedLayer();

          bool m_isEnabled;

        protected:

          te::map::AbstractLayerPtr getLayer(const std::string& id);
          void initialize();
          void initializeActions();
          void createAction(QAction*& action, const QString& tooltip, const QString& icon, bool checkable, bool enabled, const QString& objName, const char* member);
          bool dataSrcIsPrepared(const te::map::AbstractLayerPtr& layer);
        };

      } // end namespace edit
    }   // end namespace plugins
  }     // end namespace qt
}       // end namespace te

#endif //__TERRALIB_QT_PLUGINS_EDIT_INTERNAL_TOOLBAR_H
