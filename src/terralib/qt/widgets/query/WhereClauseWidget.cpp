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
  \file terralib/qt/widgets/query/WhereClauseWidget.cpp

  \brief This file has the WhereClauseWidget class.
*/

// TerraLib
#include "../../../dataaccess/dataset/DataSet.h"
#include "../../../dataaccess/datasource/DataSourceTransactor.h"
#include "../../../dataaccess/query/DataSetName.h"
#include "../../../dataaccess/query/Distinct.h"
#include "../../../dataaccess/query/Field.h"
#include "../../../dataaccess/query/Fields.h"
#include "../../../dataaccess/query/From.h"
#include "../../../dataaccess/query/PropertyName.h"
#include "../../../dataaccess/query/Select.h"
#include "ui_WhereClauseWidgetForm.h"
#include "WhereClauseWidget.h"

// Qt
#include <QtGui/QIcon>
#include <QtGui/QMessageBox>


te::qt::widgets::WhereClauseWidget::WhereClauseWidget(QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f),
    m_ui(new Ui::WhereClauseWidgetForm)
{
  m_ui->setupUi(this);

  m_ds.reset();

  // set icons
  m_ui->m_addWhereClausePushButton->setIcon(QIcon::fromTheme("list-add"));
  m_ui->m_removeWhereClausePushButton->setIcon(QIcon::fromTheme("list-remove"));

  //connects
  connect(m_ui->m_addWhereClausePushButton, SIGNAL(clicked()), this, SLOT(onAddWhereClausePushButtonClicked()));
  connect(m_ui->m_removeWhereClausePushButton, SIGNAL(clicked()), this, SLOT(onRemoveWhereClausePushButtonClicked()));
  connect(m_ui->m_valueValueRadioButton, SIGNAL(clicked()), this, SLOT(onValuePropertyRadioButtonClicked()));
}

te::qt::widgets::WhereClauseWidget::~WhereClauseWidget()
{

}

Ui::WhereClauseWidgetForm* te::qt::widgets::WhereClauseWidget::getForm() const
{
  return m_ui.get();
}

void te::qt::widgets::WhereClauseWidget::setDataSource(const te::da::DataSourcePtr& ds)
{
  m_ds = ds;
}

void te::qt::widgets::WhereClauseWidget::setFromItens(std::vector<std::pair<std::string, std::string> > vec)
{
  m_fromItens = vec;
}

void te::qt::widgets::WhereClauseWidget::setAttributeList(const std::vector<std::string>& vec)
{
  m_ui->m_restrictValueComboBox->clear();
  m_ui->m_valuePropertyComboBox->clear();

  for(size_t t = 0; t <vec.size(); ++t)
  {
    m_ui->m_restrictValueComboBox->addItem(vec[t].c_str());
    m_ui->m_valuePropertyComboBox->addItem(vec[t].c_str());
  }
}

void te::qt::widgets::WhereClauseWidget::setOperatorsList(const std::vector<std::string>& vec)
{
  m_ui->m_OperatorComboBox->clear();

  for(size_t t = 0; t <vec.size(); ++t)
  {
    m_ui->m_OperatorComboBox->addItem(vec[t].c_str());
  }
}

void te::qt::widgets::WhereClauseWidget::setConnectorsList(const std::vector<std::string>& vec)
{
  m_ui->m_connectorComboBox->clear();

  for(size_t t = 0; t <vec.size(); ++t)
  {
    m_ui->m_connectorComboBox->addItem(vec[t].c_str());
  }
}

void te::qt::widgets::WhereClauseWidget::onAddWhereClausePushButtonClicked()
{
  if(m_ui->m_restrictValueComboBox->currentText().isEmpty())
  {
    QMessageBox::warning(this, tr("Query Builder"), tr("Restrict value not defined."));
    return;
  }

  if(m_ui->m_valuePropertyRadioButton->isChecked() == false &&
     m_ui->m_valueValueRadioButton->isChecked() == false)
  {
     if(m_ui->m_valueValueComboBox->currentText().isEmpty())
    {
      QMessageBox::warning(this, tr("Query Builder"), tr("Value not defined."));
      return;
    }
  }

  if(m_ui->m_valueValueRadioButton->isChecked())
  {
    if(m_ui->m_valueValueComboBox->currentText().isEmpty())
    {
      QMessageBox::warning(this, tr("Query Builder"), tr("Value not defined."));
      return;
    }
  }

  int newrow = m_ui->m_whereClauseTableWidget->rowCount();

  std::string restrictValue = m_ui->m_restrictValueComboBox->currentText().toStdString();
  std::string operatorStr = m_ui->m_OperatorComboBox->currentText().toStdString();
  
  std::string connector = "";
  if(m_ui->m_connectorCheckBox->isChecked())
    connector = m_ui->m_connectorComboBox->currentText().toStdString();
  
  std::string valueStr = "";
  if(m_ui->m_valuePropertyRadioButton->isChecked())
    valueStr = m_ui->m_valuePropertyComboBox->currentText().toStdString();
  else if(m_ui->m_valueValueRadioButton->isChecked())
    valueStr = m_ui->m_valueValueComboBox->currentText().toStdString();

//new entry
  m_ui->m_whereClauseTableWidget->insertRow(newrow);

  QTableWidgetItem* itemProperty = new QTableWidgetItem(QString::fromStdString(restrictValue));
  m_ui->m_whereClauseTableWidget->setItem(newrow, 0, itemProperty);

  QTableWidgetItem* itemOperator = new QTableWidgetItem(QString::fromStdString(operatorStr));
  m_ui->m_whereClauseTableWidget->setItem(newrow, 1, itemOperator);

  QTableWidgetItem* itemValue = new QTableWidgetItem(QString::fromStdString(valueStr));
  m_ui->m_whereClauseTableWidget->setItem(newrow, 2, itemValue);

  QTableWidgetItem* itemConnector = new QTableWidgetItem(QString::fromStdString(connector));
  m_ui->m_whereClauseTableWidget->setItem(newrow, 3, itemConnector);

  m_ui->m_whereClauseTableWidget->resizeColumnsToContents();
}

void te::qt::widgets::WhereClauseWidget::onRemoveWhereClausePushButtonClicked()
{
  int row = m_ui->m_whereClauseTableWidget->currentRow();

  if(row >= 0)
    m_ui->m_whereClauseTableWidget->removeRow(row);

  m_ui->m_whereClauseTableWidget->resizeColumnsToContents();
}

void te::qt::widgets::WhereClauseWidget::onValuePropertyRadioButtonClicked()
{
  if(m_ds.get() == 0)
    return;

  m_ui->m_valueValueComboBox->clear();

  std::string propertyName = m_ui->m_restrictValueComboBox->currentText().toStdString();

  te::da::Fields* fields = new te::da::Fields;
  te::da::Field* f = new te::da::Field(new te::da::PropertyName(propertyName));
  fields->push_back(f);

  te::da::PropertyName* name = new te::da::PropertyName(propertyName);
  te::da::Distinct* dist = new te::da::Distinct();
  dist->push_back(name);

  te::da::From* from = new te::da::From;

  for(size_t t = 0; t < m_fromItens.size(); ++t)
  {
    te::da::FromItem* fi = new te::da::DataSetName(m_fromItens[t].first, m_fromItens[t].second);

    from->push_back(fi);
  }

  te::da::Select* select = new te::da::Select();

  select->setFields(fields);
  select->setDistinct(dist);
  select->setFrom(from);

  te::da::DataSourceTransactor* transactor = m_ds->getTransactor();

  te::da::DataSet* dataset = 0;

  try
  {
    dataset = transactor->query(*select);
  }
  catch(const std::exception& e)
  {
    std::string msg =  "An exception has occuried: ";
                msg += e.what();

    QMessageBox::warning(0, "Query Example", msg.c_str());

    delete transactor;
    delete dataset;
    delete select;

    return;
  }
  catch(...)
  {
    std::string msg =  "An unexpected exception has occuried!";

    QMessageBox::warning(0, "Query Example", msg.c_str());

    delete transactor;
    delete dataset;
    delete select;

    return;
  }

  if(dataset)
  {
    while(dataset->moveNext())
    {
      std::string value = dataset->getAsString(0);

      m_ui->m_valueValueComboBox->addItem(value.c_str());
    }
  }

  delete transactor;
  delete dataset;
  delete select;
}
