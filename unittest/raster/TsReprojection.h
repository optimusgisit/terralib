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
  \file TsReprojection.h
 
  \brief A test suit for the raster Reprojection Class.
 */

#ifndef __TERRALIB_UNITTEST_RASTER_INTERNAL_REPROJECTION_H
#define __TERRALIB_UNITTEST_RASTER_INTERNAL_REPROJECTION_H

#include <terralib/raster.h>
#include <boost/shared_ptr.hpp>

// cppUnit
#include <cppunit/extensions/HelperMacros.h>

/*!
  \class TsReprojection

  \brief A test suit for the raster Reprojection  class interface.

  ....
  <br>
  This test suite will check the raster Reprojection class
  </ul>
 */
class TsReprojection : public CPPUNIT_NS::TestFixture 
{
  CPPUNIT_TEST_SUITE( TsReprojection );
  
  CPPUNIT_TEST(tcReprojection1);
  
  CPPUNIT_TEST(tcReprojection2 );

  CPPUNIT_TEST(tcReprojection3 );

  // add other tests (tcXxxx)

  CPPUNIT_TEST_SUITE_END();

  protected :
    
    void tcReprojection1();

    void tcReprojection2();
    
    void tcReprojection3();

    // add other tests...
};

#endif  // __TERRALIB_UNITTEST_RASTER_INTERNAL_REPROJECTION_H
