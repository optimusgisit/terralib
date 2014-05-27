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


#ifndef TE_QT_WIDGETS_DATASETTABLEHORIZONTALHEADER_H
#define TE_QT_WIDGETS_DATASETTABLEHORIZONTALHEADER_H

#include <QtGui/QHeaderView>

namespace te
{
  namespace qt
  {
    namespace widgets
    {
      // Forward declaration
      class DataSetTableView;
      
      class DataSetTableHorizontalHeader : public QHeaderView
      {
          Q_OBJECT

        public:
          DataSetTableHorizontalHeader(DataSetTableView* view);

          void mousePressEvent(QMouseEvent* e);

        protected:
          DataSetTableView* m_view;
      };
      
    } // namespace widgets
  } // namespace qt
} // namespace te

#endif // TE_QT_WIDGETS_DATASETTABLEHORIZONTALHEADER_H