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
  \file terralib/qt/widgets/rp/ClassifierWizard.cpp

  \brief A Qt dialog that allows users to run a classifier operation defined by RP module.
*/

// TerraLib 
#include "../../../dataaccess/dataset/DataSet.h"
#include "../../../raster/Raster.h"
#include "../../../rp/Classifier.h"
#include "ClassifierWizard.h"
#include "ClassifierWizardPage.h"
#include "LayerSearchWidget.h"
#include "LayerSearchWizardPage.h"
#include "RasterInfoWidget.h"
#include "RasterInfoWizardPage.h"

// STL
#include <cassert>

// Qt
#include <QtGui/QMessageBox>


te::qt::widgets::ClassifierWizard::ClassifierWizard(QWidget* parent)
  : QWizard(parent)
{
  //configure the wizard
  this->setWizardStyle(QWizard::ModernStyle);
  this->setWindowTitle(tr("Classifier"));
  this->setFixedSize(640, 480);

  addPages();
}

te::qt::widgets::ClassifierWizard::~ClassifierWizard()
{

}

bool te::qt::widgets::ClassifierWizard::validateCurrentPage()
{
  if(currentPage() ==  m_layerSearchPage.get())
  {
    std::list<te::map::AbstractLayerPtr> list = m_layerSearchPage->getSearchWidget()->getSelecteds();

    if(list.empty() == false)
    {
      te::map::AbstractLayerPtr l = *list.begin();

      m_classifierPage->set(l);
    }

    return m_layerSearchPage->isComplete();
  }
  else if(currentPage() ==  m_classifierPage.get())
  {
    return m_classifierPage->isComplete();
  }
  else if(currentPage() ==  m_rasterInfoPage.get())
  {
    return execute();
  }

  return true;
}

void te::qt::widgets::ClassifierWizard::setList(std::list<te::map::AbstractLayerPtr>& layerList)
{
  m_layerSearchPage->getSearchWidget()->setList(layerList);
}

void te::qt::widgets::ClassifierWizard::addPages()
{
  m_layerSearchPage.reset(new te::qt::widgets::LayerSearchWizardPage(this));
  m_classifierPage.reset(new te::qt::widgets::ClassifierWizardPage(this));
  m_rasterInfoPage.reset(new te::qt::widgets::RasterInfoWizardPage(this));

  addPage(m_layerSearchPage.get());
  addPage(m_classifierPage.get());
  addPage(m_rasterInfoPage.get());

  //for contrast only one layer can be selected
  m_layerSearchPage->getSearchWidget()->enableMultiSelection(false);
}

bool te::qt::widgets::ClassifierWizard::execute()
{
  //get layer
  std::list<te::map::AbstractLayerPtr> list = m_layerSearchPage->getSearchWidget()->getSelecteds();
  te::map::AbstractLayerPtr l = *list.begin();
  te::da::DataSet* ds = l->getData();
  te::rst::Raster* inputRst = ds->getRaster();

  //run contrast
  te::rp::Classifier algorithmInstance;

  te::rp::Classifier::InputParameters algoInputParams = m_classifierPage->getInputParams();


  te::rp::Classifier::OutputParameters algoOutputParams;


  if(algorithmInstance.initialize(algoInputParams))
  {
    if(algorithmInstance.execute(algoOutputParams))
    {

      QMessageBox::information(this, tr("Classifier"), tr("Classifier enhencement ended sucessfully"));
    }
    else
    {
      QMessageBox::critical(this, tr("Classifier"), tr("Classifier enhencement execution error"));

      delete ds;
      return false;
    }
  }
  else
  {
    QMessageBox::critical(this, tr("Classifier"), tr("Classifier enhencement initialization error"));

    delete ds;
    return false;
  }

  delete ds;

  return true;
}
