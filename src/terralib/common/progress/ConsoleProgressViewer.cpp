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
  \file terralib/common/progress/ConsoleProgressViewer.cpp

  \brief A progress viewer implementation for the console.
*/

// TerraLib
#include "../../core/translator/Translator.h"
#include "ConsoleProgressViewer.h"
#include "TaskProgress.h"

// STL
#include <iostream>

te::common::ConsoleProgressViewer::ConsoleProgressViewer()
  : AbstractProgressViewer(),
    m_totalSteps(0),
    m_currentStep(0),
    m_propStep(0)
{
}

te::common::ConsoleProgressViewer::~ConsoleProgressViewer()
{
}

void te::common::ConsoleProgressViewer::addTask(TaskProgress* t, int id)
{
  m_tasks.insert(std::map<int, TaskProgress*>::value_type(id, t));

  updateMessage(-1);
}

void te::common::ConsoleProgressViewer::removeTask(int taskId)
{
  cancelTask(taskId);

  std::map<int, TaskProgress*>::iterator it = m_tasks.find(taskId);

  if(it != m_tasks.end())
  {
    m_tasks.erase(it);
    updateMessage(-1);
  }

  if(m_tasks.empty())
  {
    m_totalSteps = 0;
    m_currentStep = 0;
    m_propStep = 0;
  }
}

void te::common::ConsoleProgressViewer::cancelTask(int taskId)
{
  std::map<int, TaskProgress*>::iterator it = m_tasks.find(taskId);

  if(it != m_tasks.end())
  {
    //update total and current values
    m_totalSteps -= it->second->getTotalSteps();
    m_currentStep -= it->second->getCurrentStep();

    double aux = static_cast<double>(m_currentStep) / static_cast<double>(m_totalSteps);

    m_propStep = static_cast<int>(100.0 * aux);
  }
}

void te::common::ConsoleProgressViewer::setTotalValues(int taskId)
{
  m_totalSteps += m_tasks[taskId]->getTotalSteps();
}

void te::common::ConsoleProgressViewer::updateValue(int taskId)
{
  double currentStep = 0;

  std::map<int, te::common::TaskProgress*>::iterator it;

  for (it = m_tasks.begin(); it != m_tasks.end(); ++it)
    currentStep += it->second->getCurrentStep();

  double aux = static_cast<double>(currentStep) / static_cast<double>(m_totalSteps);

  int val = static_cast<int>(100.0 * aux);

  if((val != m_propStep) && (val >= 0))
  {
    m_propStep = val;

    std::cout << m_message << TE_TR(" - Percent: ") << m_propStep << "%" <<std::endl;
  }
}

void te::common::ConsoleProgressViewer::updateMessage(int /*taskId*/)
{
  if(m_tasks.size() == 1)
  {
    m_message = m_tasks.begin()->second->getMessage();
  }
  else
  {
    m_message = TE_TR("Multi Tasks");
  }
}

