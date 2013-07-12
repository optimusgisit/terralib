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
  \file terralib/rp/ClassifierKMeansStrategy.cpp

  \brief KMeans strategy for image classification.
*/

// TerraLib
#include "../classification/core/KMeans.h"
#include "../common/progress/TaskProgress.h"
#include "../geometry/Envelope.h"
#include "../raster/Grid.h"
#include "../raster/PositionIterator.h"
#include "../raster/RasterIterator.h"
#include "ClassifierKMeansStrategy.h"
#include "Macros.h"
#include "Utils.h"

// STL
#include <iostream>
#include <stdlib.h>

namespace
{
  static te::rp::ClassifierKMeansStrategyFactory classifierKMeansStrategyFactoryInstance;
}

te::rp::ClassifierKMeansStrategy::Parameters::Parameters()
{
  reset();
}

te::rp::ClassifierKMeansStrategy::Parameters::~Parameters()
{
}

const te::rp::ClassifierKMeansStrategy::Parameters& te::rp::ClassifierKMeansStrategy::Parameters::operator=(const te::rp::ClassifierKMeansStrategy::Parameters& rhs)
{
  reset();

  m_K = rhs.m_K;
  m_maxIterations = rhs.m_maxIterations;
  m_epsilon = rhs.m_epsilon;

  return *this;
}

void te::rp::ClassifierKMeansStrategy::Parameters::reset() throw(te::rp::Exception)
{
  m_K = 0;
  m_maxIterations = 0;
  m_epsilon = 0.0;
}

te::common::AbstractParameters* te::rp::ClassifierKMeansStrategy::Parameters::clone() const
{
  return new te::rp::ClassifierKMeansStrategy::Parameters( *this );
}

te::rp::ClassifierKMeansStrategy::ClassifierKMeansStrategy()
{
  m_isInitialized = false;
}

te::rp::ClassifierKMeansStrategy::~ClassifierKMeansStrategy()
{
}

bool te::rp::ClassifierKMeansStrategy::initialize(te::rp::StrategyParameters const* const strategyParams) throw(te::rp::Exception)
{
  m_isInitialized = false;

  te::rp::ClassifierKMeansStrategy::Parameters const* paramsPtr = dynamic_cast<te::rp::ClassifierKMeansStrategy::Parameters const*>(strategyParams);

  if(!paramsPtr)
    return false;

  m_parameters = *(paramsPtr);

  TERP_TRUE_OR_RETURN_FALSE(m_parameters.m_K > 1, TR_RP("The value of K (number of clusters) must be at least 2."))
  TERP_TRUE_OR_RETURN_FALSE(m_parameters.m_maxIterations > 0, TR_RP("The number of iterations must be at least 1."))
  TERP_TRUE_OR_RETURN_FALSE(m_parameters.m_epsilon > 0, TR_RP("The stop criteria must be higher than 0."))

  m_isInitialized = true;

  return true;
}

bool te::rp::ClassifierKMeansStrategy::execute(const te::rst::Raster& inputRaster, const std::vector<unsigned int>& inputRasterBands,
                                              const std::vector<te::gm::Polygon*>& inputPolygons, te::rst::Raster& outputRaster,
                                              const unsigned int outputRasterBand, const bool enableProgressInterface) throw(te::rp::Exception)
{
  TERP_TRUE_OR_RETURN_FALSE(m_isInitialized, TR_RP("Instance not initialized"))

// defining classification parameters
  te::cl::KMeans<te::rst::RasterIterator<double> >::Parameters classifierParameters;
  classifierParameters.m_K = m_parameters.m_K;
  classifierParameters.m_maxIterations = m_parameters.m_maxIterations;
  classifierParameters.m_epsilon = m_parameters.m_epsilon;
  std::vector<unsigned int> classification;

// define raster iterators
  te::rst::RasterIterator<double> rit = te::rst::RasterIterator<double>::begin((te::rst::Raster*)(&inputRaster), inputRasterBands);
  te::rst::RasterIterator<double> ritend = te::rst::RasterIterator<double>::end((te::rst::Raster*)(&inputRaster), inputRasterBands);

// execute the algorithm
  te::cl::KMeans<te::rst::RasterIterator<double> > classifier;

  if(!classifier.initialize(classifierParameters))
    throw;
  if(!classifier.train(rit, ritend, inputRasterBands, std::vector<unsigned int>(), true))
    throw;
  if(!classifier.classify(rit, ritend, inputRasterBands, classification, true))
    throw;

// classifying image
  te::common::TaskProgress task;
  task.setTotalSteps(inputRaster.getNumberOfColumns() * inputRaster.getNumberOfRows());
  task.setMessage(TR_RP("KMeans algorithm - classifying image"));
  task.setCurrentStep(0);
  unsigned int i = 0;
  while (rit != ritend)
  {
    outputRaster.setValue(rit.getCol(), rit.getRow(), classification[i], outputRasterBand);
    ++i;
    ++rit;
    task.pulse();
  }

  return true;
}

te::rp::ClassifierKMeansStrategyFactory::ClassifierKMeansStrategyFactory()
  : te::rp::ClassifierStrategyFactory("kmeans")
{
}

te::rp::ClassifierKMeansStrategyFactory::~ClassifierKMeansStrategyFactory()
{
}

te::rp::ClassifierStrategy* te::rp::ClassifierKMeansStrategyFactory::build()
{
  return new te::rp::ClassifierKMeansStrategy();
}