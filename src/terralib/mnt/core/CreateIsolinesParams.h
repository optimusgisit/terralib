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
  \file terralib/sa/core/BayesParams.h

  \brief This file contains a class that represents the Bayes parameters.

  \reference Adapted from TerraLib4.
*/

#ifndef __TERRALIB_MNT_INTERNAL_CREATEISOLINESPARAMS_H
#define __TERRALIB_MNT_INTERNAL_CREATEISOLINESPARAMS_H

// Terralib Includes
#include "../../../../src/terralib/dataaccess/dataset/DataSet.h"
#include "../../../../src/terralib/dataaccess/dataset/DataSetType.h"
#include "../../../../src/terralib/dataaccess/datasource/DataSource.h"
#include "Config.h"
#include "Enums.h"

// STL Includes
#include <map>
#include <memory>
#include <string>

    /*!
      \class BayesInputParams

      \brief Class that represents the Bayes input parameters.

      \sa BayesGlobalOperation BayesLocalOperation
    */

	class CreateIsolinesInputParams
    {
      public:
      
        /*! \brief Default constructor. */
        CreateIsolinesInputParams() {}

        /*! \brief Virtual destructor. */
      ~CreateIsolinesInputParams() {}

      public:

        std::auto_ptr<te::da::DataSetType> m_dsType;              //!< Attribute used to access the data set metadata
        std::auto_ptr<te::da::DataSet> m_ds;                      //!< Attribute with data set
    };

    /*!
      \class BayesOutputParams
      \brief Class that represents the Bayes output parameters.
      \sa BayesGlobalOperation BayesLocalOperation
    */

	class  CreateIsolinesOutputParams
    {
      public:
      
        /*! \brief Default constructor. */
        CreateIsolinesOutputParams()
        {
          m_outputDataSetName = "";
        }

        /*! \brief Virtual destructor. */
        ~CreateIsolinesOutputParams()
        {
        }

      public:

        te::da::DataSourcePtr m_ds;                       //!< Pointer to the output datasource.

        std::string m_outputDataSetName;                  //!< Attribute that defines the output dataset name
    };

#endif //__TERRALIB_MNT_INTERNAL_CREATEISOLINESPARAMS_H
