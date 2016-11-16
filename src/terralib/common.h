/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled applications.

    TerraLib is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraLib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TerraLib. See COPYING. If not, write to
    TerraLib Team at <terralib-team@terralib.org>.
 */

/*!
  \file common.h

  \brief This file contains include headers for the TerraLib Common Runtime module.
 */

#ifndef __TERRALIB_INTERNAL_COMMON_H
#define __TERRALIB_INTERNAL_COMMON_H

// TerraLib
#include "common/AbstractFactory.h"
#include "common/BaseVisitable.h"
#include "common/BaseVisitor.h"
#include "common/BoostUtils.h"
#include "common/ByteSwapUtils.h"
#include "common/Comparators.h"
#include "common/Config.h"
#include "common/Distance.h"
#include "common/Enums.h"
#include "common/Exception.h"
#include "common/FactoryDictionary.h"
#include "common/Globals.h"
#include "common/LoggedException.h"
#include "core/logger/Logger.h"
#include "common/Module.h"
#include "common/ParameterizedAbstractFactory.h"
#include "common/PlatformUtils.h"
#include "common/Singleton.h"
#include "common/STLUtils.h"
#include "common/StringUtils.h"
#include "common/TerraLib.h"
#include "common/ThreadingPolicies.h"
#include "common/TreeItem.h"
#include "common/UnitOfMeasure.h"
#include "common/UnitsOfMeasureManager.h"
#include "common/Visitor.h"
#include "common/progress/AbstractProgressViewer.h"
#include "common/progress/ConsoleProgressViewer.h"
#include "common/progress/ProgressManager.h"
#include "common/progress/ProgressTimer.h"
#include "common/progress/TaskProgress.h"

/*!
  \defgroup common Common

  \brief The common module contains the foundation classes and functions for all TerraLib modules.

  The TerraLib Common Module contains the foundation classes and functions that makes the basic support for other TerraLib modules.
  It provides portable and cross platform code not focus exclusively in the geo-spatial domain.
  On the contrary, the helper classes, utilitary functions, generic class templates and generic design pattern classes
  are domain application neutral and can be used by any application.

  The TerraLib modules has made use of Boost libraries but some functionalities are not found in Boost, so the common runtime module fills this gap
  and add some low-level code used by TerraLib applications. You will find in this module classes for: abstract factories, singletons, visitors, support for i18n,
  message logging, shared library management, threading policies and much more.
  
  Besides the classes you will find useful routines for: byte swap, hex-encoding/decoding, platform specific routines, auxiliary functions for STL containers.

  The main classes/concepts in this module are listed here. The namespace associated to the Common module is te::common.
  To know more about it, see the te::common namespace documentation.
*/

namespace te
{
  /*!
    \brief Namespace for the Common module of TerraLib.
  */
  namespace common
  {
  } // end namespace common
}   // end namesp

#endif  // __TERRALIB_INTERNAL_COMMON_H

