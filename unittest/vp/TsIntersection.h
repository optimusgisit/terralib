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
  \file TsIntersection.h
 
  \brief Test suite for the Intersection class.
 */

#ifndef __TERRALIB_UNITTEST_VP_INTERNAL_INTERSECTION_H
#define __TERRALIB_UNITTEST_VP_INTERNAL_INTERSECTION_H

// STL
#include <string>
#include <vector>

// STL
#include <cstdio>
#include <cstdlib>
#include <cstring>

//TerraLib 
#include <terralib/common.h>
#include <terralib/dataaccess.h>
#include <terralib/datatype/SimpleData.h>
#include <terralib/geometry.h>
#include <terralib/vp/AlgorithmParams.h>
#include <terralib/vp/Intersection.h>

// cppUnit
#include <cppunit/extensions/HelperMacros.h>

/*!
  \class TsIntersection

  \brief Test suite for the Intersection Class.

  This test suite will check the following:
  <ul>
  <li></li>
  </ul>
 */
class TsIntersection : public CPPUNIT_NS::TestFixture
{
// It registers this class as a Test Suit
  CPPUNIT_TEST_SUITE(TsIntersection);

// It registers the class methods as Test Cases belonging to the suit 
  //CPPUNIT_TEST(tcIntersectionQuery1);
  //CPPUNIT_TEST(tcIntersectionQuery2);
  CPPUNIT_TEST(tcIntersectionMemory1);

  CPPUNIT_TEST_SUITE_END();
  
  public:

// It sets up context before running the test.
    void setUp();

// It cleann up after the test run.
    void tearDown();

  protected:

// Test Cases:

    /*! \brief Test Case: Intersection query with equals  input/output dataSource*/
    void tcIntersectionQuery1();

    /*! \brief Test Case: Intersection query with different input/output dataSource*/
    void tcIntersectionQuery2();

    /*! \brief Test Case: Intersection memory with partitioning*/
    void tcIntersectionMemory1();

  private:

    std::vector<te::vp::InputParams> m_inputParams;
    te::da::DataSourcePtr m_outputDataSource;
    te::vp::AlgorithmParams* m_params;
};

#endif  // __TERRALIB_UNITTEST_VP_INTERNAL_INTERSECTION_H

