// Examples
#include "GraphExamples.h"

// TerraLib
#include <terralib/common/progress/ProgressManager.h>
#include <terralib/common/progress/ConsoleProgressViewer.h>
#include <terralib/common/TerraLib.h>
#include <terralib/core/plugin.h>


// STL
#include <cstdlib>
#include <iostream>
#include <locale>


int main()
{
  setlocale(LC_ALL,"C");

  try
  {
    //startup terralib
    TerraLib::getInstance().initialize();
    te::core::plugin::InitializePluginSystem();

    //create a progress bar
    std::auto_ptr<te::common::ConsoleProgressViewer> cpv(new te::common::ConsoleProgressViewer());
    int idViewer = te::common::ProgressManager::getInstance().addViewer(cpv.get());

    //load all necessary modules
    LoadModules();

    //-----------------------------------------------------------------------------------------------------
    bool draw = true;

    //create ldd graph
    CreateMSTGraph(draw);

    //-----------------------------------------------------------------------------------------------------

    //remove progress bar
    te::common::ProgressManager::getInstance().removeViewer(idViewer);

    //unload modules
    te::core::PluginManager::instance().clear();
    te::core::plugin::FinalizePluginSystem();
    //finalize terralib
    TerraLib::getInstance().finalize();
  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurred in the Graph examples: " << e.what() << std::endl;

    return EXIT_FAILURE;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred in the Graph examples!" << std::endl;

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
