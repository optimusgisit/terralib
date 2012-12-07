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
  \file terralib/qt/widgets/dataset/explorer/DataSetTreeView.h

  \brief A tree view for datasets of a data source.
*/

#ifndef __TERRALIB_QT_WIDGETS_DATASET_EXPLORER_INTERNAL_DATASETTREEVIEW_H
#define __TERRALIB_QT_WIDGETS_DATASET_EXPLORER_INTERNAL_DATASETTREEVIEW_H

// TerraLib
#include "../../../../dataaccess/datasource/DataSourceInfo.h"
#include "../../Config.h"

// STL
#include <list>

// Qt
#include <QtGui/QTreeView>

namespace te
{
  namespace qt
  {
    namespace widgets
    {
      class DataSetGroupItem;
      class DataSetItem;

      /*!
        \class DataSetTreeView

        \brief A tree view for datasets of a data source.

        \sa DataSetTreeViewModel, DataSetExplorer
      */
      class TEQTWIDGETSEXPORT DataSetTreeView : public QTreeView
      {
        Q_OBJECT

        public:

          DataSetTreeView(QWidget* parent = 0);

          ~DataSetTreeView();

          void set(const te::da::DataSourceInfoPtr& ds, bool useCheckableItems = false);

          std::list<DataSetItem*> getSelectedDataSets() const;

          bool hasSelectedDataSets() const;

        signals:

          void activated(DataSetItem* item);

          void activated(DataSetGroupItem* item);

          void clicked(DataSetItem* item);

          void clicked(DataSetGroupItem* item);

          void doubleClicked(DataSetItem* item);

          void doubleClicked(DataSetGroupItem* item);

          void entered(DataSetItem* item);

          void entered(DataSetGroupItem* item);

          void pressed(DataSetItem* item);

          void pressed(DataSetGroupItem* item);

          void toggled(DataSetItem* item);

          void toggled(DataSetGroupItem* item);

        protected slots:

          void onItemActivated(const QModelIndex & index);

          void onItemClicked(const QModelIndex & index);

          void onItemDoubleClicked(const QModelIndex & index);

          void onItemEntered(const QModelIndex & index);

          void onItemPressed(const QModelIndex & index);
      }; 
    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_DATASET_EXPLORER_INTERNAL_DATASETTREEVIEW_H