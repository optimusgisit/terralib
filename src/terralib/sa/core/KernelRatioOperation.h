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
  \file terralib/sa/core/KernelRatioOperation.h

  \brief This file contains a class that represents the kernel ratio operation.

  \reference Adapted from TerraLib4.
*/

#ifndef __TERRALIB_SA_INTERNAL_KERNELRATIOOPERATION_H
#define __TERRALIB_SA_INTERNAL_KERNELRATIOOPERATION_H

// Terralib Includes
#include "../Config.h"
#include "KernelFunctions.h"
#include "KernelOperation.h"
#include "KernelParams.h"

// STL Includes
#include <map>
#include <memory>


namespace te
{
  namespace sa
  {
    /*!
      \class KernelRatioOperation

      \brief Class used to calculate the kernel ratio of a datasets

      \sa KernelOperation
    */
    class TESAEXPORT KernelRatioOperation : public te::sa::KernelOperation
    {
      public:

        /*! \brief Default constructor. */
        KernelRatioOperation();

        /*! \brief Virtual destructor. */
        ~KernelRatioOperation();

      public:

        /*! \brief Function to execute the kernel operation. */
        virtual void execute();

        void setInputParameters(te::sa::KernelInputParams* inParamsA, te::sa::KernelInputParams* inParamsB);

      protected:

        /*! Function used to build the tree with data set information */
        virtual void buildTree();

      protected:

        te::sa::KernelMap m_kMapA;                                     //!< Kernel map with input data A.
        te::sa::KernelMap m_kMapB;                                     //!< Kernel map with input data B.

        std::auto_ptr<te::sa::KernelInputParams> m_inputParamsA;      //!< Kernel input parameters A.
        std::auto_ptr<te::sa::KernelInputParams> m_inputParamsB;      //!< Kernel input parameters B.
    };
  } // end namespace sa
} // end namespace te

#endif //__TERRALIB_SA_INTERNAL_KERNELRATIOOPERATION_H