/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "AppVerify.h"
#include "Filename.h"
#include "InterpreterUtilities.h"
#include "MatlabInterpreterManager.h"
#include "MatlabTests.h"
#include "MatlabVersion.h"
#include "PlugInManagerServices.h"
#include "PlugInRegistration.h"
#include "Progress.h"

REGISTER_PLUGIN_BASIC(Matlab, MatlabTests);

MatlabTests::MatlabTests()
{
   setName("MATLAB Tests");
   setDescription("MATLAB test suite");
   setDescriptorId("{6E99E724-CC20-4A2D-B140-B4DEA6A112C6}");
   setCopyright(MATLAB_COPYRIGHT);
   setVersion(MATLAB_VERSION_NUMBER);
   setProductionStatus(MATLAB_IS_PRODUCTION_RELEASE);
   setType("Testable");
}

MatlabTests::~MatlabTests()
{}

bool MatlabTests::runAllTests(Progress* pProgress, std::ostream& failure)
{
   return runOperationalTests(pProgress, failure);
}

bool MatlabTests::runOperationalTests(Progress* pProgress, std::ostream& failure)
{
   // This test simply runs the opticks_test.m script located in the SupportFiles directory.
   std::string supportFilesPath;
   const Filename* pSupportFiles = ConfigurationSettings::getSettingSupportFilesPath();
   if (pSupportFiles != NULL)
   {
      supportFilesPath = pSupportFiles->getFullPathAndName();
   }

   std::vector<PlugIn*> plugins = Service<PlugInManagerServices>()->getPlugInInstances("MATLAB");
   if (plugins.size() != 1)
   {
      failure << "Unable to locate MatlabInterpreterManager.";
      return false;
   }

   MatlabInterpreterManager* pMatlabInterpreterManager = dynamic_cast<MatlabInterpreterManager*>(plugins.front());
   VERIFY(pMatlabInterpreterManager != NULL);
   Interpreter* pInterpreter = pMatlabInterpreterManager->getInterpreter();
   if (pInterpreter == NULL)
   {
      failure << "MatlabInterpreterManager returned a NULL interpreter.";
      return false;
   }

   if (pProgress != NULL)
   {
      pProgress->updateProgress("Executing MATLAB tests.", 5, NORMAL);
   }

   // Temporarily change the output buffer size to an arbitrarily large value so commands do not get truncated.
   const int oldOutputBufferSize = MatlabInterpreter::getSettingOutputBufferSize();
   MatlabInterpreter::setSettingOutputBufferSize(32767);

   std::string command = "run('" + supportFilesPath + "/MATLAB/opticks_test.m')";
   std::string output;
   bool outputIsError = false;
   bool testsPassed = InterpreterUtilities::executeScopedCommand("MATLAB", command, output, outputIsError, pProgress);

   // Return the output buffer size to the old value.
   MatlabInterpreter::setSettingOutputBufferSize(oldOutputBufferSize);
   if (pProgress != NULL)
   {
      pProgress->updateProgress(output, 99, WARNING);
   }

   if (testsPassed == false || outputIsError == true)
   {
      if (pProgress != NULL)
      {
         pProgress->updateProgress("MATLAB tests failed.", 0, ERRORS);
      }

      return false;
   }

   if (pProgress != NULL)
   {
      pProgress->updateProgress("MATLAB tests complete.", 100, NORMAL);
   }

   return true;
}
