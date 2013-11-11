/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "External.h"
#include "MatlabFunctions.h"
#include "MatlabInterpreterEngine.h"
#include "PlugInRegistration.h"

namespace
{
   // Singleton.
   MatlabInterpreterEngine* spEngine = NULL;
}

extern "C" LINKAGE MatlabInterpreter* init_matlab_interpreter(External* pExternal)
{
   if (pExternal == NULL)
   {
      return NULL;
   }

   ModuleManager::instance()->setService(pExternal);
   if (spEngine == NULL)
   {
      spEngine = new MatlabInterpreterEngine();
   }
   else
   {
      spEngine->showGlobalOutput(false); //reset to default
      spEngine->executeCommand("clear");
   }

   return spEngine;
}

extern "C" LINKAGE void shutdown_matlab_interpreter()
{
   MatlabFunctions::clearWizardObject(std::string());
   delete spEngine;
   spEngine = NULL;
}
