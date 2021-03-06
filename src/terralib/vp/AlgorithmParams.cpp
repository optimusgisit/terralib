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
 \file AlgorithmParams.cpp
 */

#include "AlgorithmParams.h"

te::vp::AlgorithmParams::AlgorithmParams():
  m_inputParams()
{
}

te::vp::AlgorithmParams::AlgorithmParams( const std::vector<te::vp::InputParams>& inputParams,
                                          te::da::DataSourcePtr outputDataSource,
                                          const std::string& outputDataSetName,
                                          const int& outputSRID,
                                          const std::map<std::string, te::dt::AbstractData*>& specificParams)
  : m_inputParams(inputParams),
  m_outputDataSource(outputDataSource),
  m_outputDataSetName(outputDataSetName),
  m_outputSRID(outputSRID),
  m_specificParams(specificParams)
{
}

te::vp::AlgorithmParams::~AlgorithmParams()
{
}


std::vector<te::vp::InputParams> te::vp::AlgorithmParams::getInputParams()
{
  return m_inputParams;
}

void te::vp::AlgorithmParams::setInputParams(const std::vector<te::vp::InputParams>& inputParams)
{
  m_inputParams = inputParams;
}


te::da::DataSourcePtr te::vp::AlgorithmParams::getOutputDataSource()
{
  return m_outputDataSource;
}

void te::vp::AlgorithmParams::setOutputDataSource(te::da::DataSourcePtr outputDataSource)
{
  m_outputDataSource = outputDataSource;
}


const std::string& te::vp::AlgorithmParams::getOutputDataSetName()
{
  return m_outputDataSetName;
}

void te::vp::AlgorithmParams::setOutputDataSetName(const std::string& outputDataSetName)
{
  m_outputDataSetName = outputDataSetName;
}


const int& te::vp::AlgorithmParams::getOutputSRID()
{
  return m_outputSRID;
}

void te::vp::AlgorithmParams::setOutputSRID(const int& outputSRID)
{
  m_outputSRID = outputSRID;
}


const std::map<std::string, te::dt::AbstractData*>& te::vp::AlgorithmParams::getSpecificParams()
{
  return m_specificParams;
}

void te::vp::AlgorithmParams::setSpecificParams(const std::map<std::string, te::dt::AbstractData*>& specificParams)
{
  m_specificParams = specificParams;
}


const std::vector<std::string>& te::vp::AlgorithmParams::getWarnings()
{
  return m_warnings;
}

void te::vp::AlgorithmParams::addWarning(const std::string& warning, const bool& appendIfExists)
{
  if (!appendIfExists)
  {
    std::vector<std::string>::iterator it = std::find(m_warnings.begin(), m_warnings.end(), warning);
    if (it == m_warnings.end())
      m_warnings.push_back(warning);
  }
  else
  {
    m_warnings.push_back(warning);
  }
  
}
