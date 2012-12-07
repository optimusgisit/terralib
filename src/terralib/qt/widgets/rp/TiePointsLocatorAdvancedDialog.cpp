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
  \file terralib/widgets/rp/TiePointsLocatorAdvancedDialog.cpp

  \brief A dialog used to execute tie points location advanced options.
*/


#include "TiePointsLocatorAdvancedDialog.h"
#include "../Exception.h"

#include <ui_TiePointsLocatorAdvancedForm.h>

#include <QtCore/QString>

namespace te
{
  namespace qt
  {
    namespace widgets
    {
      namespace rp
      {
        TiePointsLocatorAdvancedDialog::TiePointsLocatorAdvancedDialog(
          QWidget* parent, Qt::WindowFlags f )
          : QDialog( parent, f )
        {
          m_uiPtr = new Ui::TiePointsLocatorAdvancedForm;
          m_uiPtr->setupUi(this);
          
          // Signals & slots
          connect(m_uiPtr->m_okPushButton, SIGNAL(clicked()), this, SLOT(on_okPushButton_clicked()));   
          
          // fill form
          

        }

        TiePointsLocatorAdvancedDialog::~TiePointsLocatorAdvancedDialog()
        {
          delete m_uiPtr;
        }


        void TiePointsLocatorAdvancedDialog::on_okPushButton_clicked()
        {

        }
      }
    }
  }
}
