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
#include "../../../dataaccess/datasource/DataSource.h"

// Qt
#include <QSignalMapper>
#include <QtGui/QWidget>

//STL
#include <memory>

namespace Ui { class DataPropertiesWidgetForm; }

namespace te
{

  namespace da {class DataSet;
                class DataSetAdapter;
                class DataSetType;
                class DataSetTypeConverter;}

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
            \brief Returns a pointer to the generated DataSetTypeConverter

            \return A DataSetTypeConverter pointer.
            \note The caller will take ownership of the returned auto pointer. 
          */
          std::auto_ptr<te::da::DataSetTypeConverter> getConverter();

          /*!
            \brief Returns a pointer to the widget's dataSet

            \return A DataSet  pointer.
            \note The caller will not take ownership of the returned pointer. 
          */
          te::da::DataSet* getDataSet();

          /*!
            \brief Returns a pointer to the widget's dataSetType

            \return A DataSetType  pointer.
            \note The caller will not take ownership of the returned pointer. 
          */
          te::da::DataSetType* getDataSetType();

          /*!
            \brief Returns a pointer to the widget's dataSource

            \return A DataSource  pointer.
            \note The caller will not take ownership of the returned pointer. 
          */
          te::da::DataSource* getDataSource();

        protected slots:

          void onInputDataToolButtonTriggered();
          void onSridPushButtonCLicked();
          void onPropertyTypeChanged(int index);
          void onPointPropertyChanged(const QString& text);

        private:

          QSignalMapper*                               m_mapper;       //!< The mapper used to know which property was configured.
          std::map<int, std::string>                   m_typeMap;      //!< A map that correlates a terralib type with a label.
          std::auto_ptr<te::da::DataSet>               m_dataSet;      //!< The DataSet that will be generated by this widget.
          std::auto_ptr<te::da::DataSetType>           m_dataType;     //!< The DataSetType that will be generated by this widget.
          std::auto_ptr<DataSetTableView>              m_tblView;      //!< The widget used to preview the data of the new dataset.
          std::auto_ptr<Ui::DataPropertiesWidgetForm>  m_ui;           //!< The widget's form.
          std::auto_ptr<te::da::DataSetTypeConverter>  m_dsConverter;  //!< The DataSetConverter that will be configured by this widget.
          te::da::DataSourcePtr                        m_dataSource;   //!< The DataSource that will be generated by this widget.
      };
    } // end namespace widgets
  }   // end namespace qt
}     // end namespace te

#endif  // __TERRALIB_QT_WIDGETS_INTERNAL_DATAPROPERTIESWIDGET_H