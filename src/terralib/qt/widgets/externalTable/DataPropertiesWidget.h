/*  Copyright (C) 2010-2013 National Institute For Space Research (INPE) - Brazil.

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
  \file  terralib/qt/widgets/externalTable/DataPropertiesWidget.h

  \brief A class used to configure the properties of a new textual file based layer
*/

#ifndef __TERRALIB_QT_WIDGETS_INTERNAL_DATAPROPERTIESWIDGET_H
#define __TERRALIB_QT_WIDGETS_INTERNAL_DATAPROPERTIESWIDGET_H

//TerraLib
#include "../Config.h"

// Qt
#include <QtGui/QWidget>
#include <QtGui/QComboBox>

namespace Ui { class dataPropertiesWidgetForm; }

namespace te
{

  namespace da {class DataSet;
                class DataSetAdapter;
                class DataSetType;
                class DataSetTypeConverter;
                class DataSource;}

  namespace qt
  {
    namespace widgets
    {

      class DataSetTableView;

      /*!
        \class DatapPropertiesWidget

        \brief A class to represent a chart display
      */
      class TEQTWIDGETSEXPORT DatapPropertiesWidget : public QWidget
      {

        Q_OBJECT

        public:

      /*!
            \brief Constructor

            It constructs a DatapPropertiesWidget.

            \param parent this widget's parent
            \param f Window flags used to configure this widget

          */
          DatapPropertiesWidget(QWidget* parent = 0,  Qt::WindowFlags f = 0);

          /*!
            \brief Destructor
          */
          ~DatapPropertiesWidget();

          /*!
            \brief Returns a pointer to the generated DataSetAdapter

            \return A DataSetAdapter pointer.
            \note The caller will take ownership of the returned pointer. 
          */
          te::da::DataSetAdapter* getAdapter();

          /*!
            \brief Returns a pointer to the generated DataSource

            \return A DataSource pointer.
            \note The caller will take ownership of the returned pointer. 
          */
          te::da::DataSource* getDataSource();

        protected slots:

          void onInputDataToolButtonTriggered();
          void onSridPushButtonCLicked();

        private:

          std::map<int, std::string>                   m_typeMap;      //!< A map that correlates a terralib type with a label.
          te::da::DataSetTypeConverter*                m_dsConverter;  //!< The DataSetConverter that will be configured by this widget.
          std::auto_ptr<te::da::DataSet>               m_dataSet;      //!< The DataSet that will be generated by this widget;
          std::auto_ptr<te::da::DataSetType>           m_dataType;     //!< The DataSetType that will be generated by this widget;
          std::auto_ptr<te::da::DataSource>            m_dataSource;   //!< The DataSource that will be generated by this widget;
          std::auto_ptr<DataSetTableView>              m_tblView;      //!< The widget used to preview the data of the new dataset.
          std::auto_ptr<Ui::dataPropertiesWidgetForm>  m_ui;           //!< The widget's form.

      };
    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_INTERNAL_DATAPROPERTIESWIDGET_H