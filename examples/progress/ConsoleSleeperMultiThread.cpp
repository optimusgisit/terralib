#include "ProgressExamples.h"
#include "terralib/common/progress/ProgressManager.h"

#include <windows.h>
#include <iostream>

// OpenMP
#include <omp.h>

#define TOTAL_STEPS 15000

void ConsoleSleeperMultiThread()
{
	te::common::ProgressManager::getInstance().getProgress()->reset();
	te::common::ProgressManager::getInstance().getProgress()->setTotalSteps(TOTAL_STEPS);
	te::common::ProgressManager::getInstance().getProgress()->setMessage("Console Progress Test");
  te::common::ProgressManager::getInstance().getProgress()->useProgressTimer(false);


	std::cout << std::endl << "Console progress bar test..." << std::endl;

  omp_set_num_threads( 4 );

  #pragma omp parallel for
	for(int i = 0; i < TOTAL_STEPS; ++i)
	{
		if(te::common::ProgressManager::getInstance().getProgress()->isActive())
		{
      Sleep(5);

		  te::common::ProgressManager::getInstance().getProgress()->pulse();
		}
	}
}