/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "Animation.h"
#include "AnimationCommands.h"
#include "AnimationController.h"
#include "AnimationFrame.h"
#include "AnimationServices.h"
#include "DesktopServices.h"
#include "MatlabFunctions.h"
#include "RasterDataDescriptor.h"
#include "RasterElement.h"
#include "RasterLayer.h"
#include "SpatialDataView.h"
#include "StringUtilities.h"
#include "TypesFile.h"

#include <QtCore/QString>
#include <QtCore/QStringList>

namespace
{
   AnimationController* getAnimationController(const std::string& controllerName)
   {
      if (controllerName.empty())
      {
         return Service<AnimationServices>()->getCurrentAnimationController();
      }

      return Service<AnimationServices>()->getAnimationController(controllerName);
   }
}

// CreateAnimationCommand
CreateAnimationCommand::CreateAnimationCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string CreateAnimationCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   SpatialDataView* pView = dynamic_cast<SpatialDataView*>(MatlabFunctions::getViewByWindowName(std::string()));
   if (pView == NULL)
   {
      outputIsError = true;
      output = "Unable to find an active spatial data view";
      return std::string();
   }

   if (pView->createDefaultAnimation() == NULL)
   {
      outputIsError = true;
      output = "Could not create an animation from the active spatial data view";
      return std::string();
   }

   Service<AnimationServices>()->setCurrentAnimationController(pView->getAnimationController());
   outputIsError = false;
   return std::string();
}

// DisableCanDropFramesCommand
DisableCanDropFramesCommand::DisableCanDropFramesCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string DisableCanDropFramesCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   AnimationController* pController = getAnimationController(getOrDefault(strCmds, 1));
   if (pController == NULL)
   {
      outputIsError = true;
      output = "Unable to find the animation controller";
      return std::string();
   }

   pController->setCanDropFrames(false);
   outputIsError = false;
   return std::string();
}

// EnableCanDropFramesCommand
EnableCanDropFramesCommand::EnableCanDropFramesCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string EnableCanDropFramesCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   AnimationController* pController = getAnimationController(getOrDefault(strCmds, 1));
   if (pController == NULL)
   {
      outputIsError = true;
      output = "Unable to find the animation controller";
      return std::string();
   }

   pController->setCanDropFrames(true);
   outputIsError = false;
   return std::string();
}

// GetAnimationControllerNamesCommand
GetAnimationControllerNamesCommand::GetAnimationControllerNamesCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetAnimationControllerNamesCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string windowName = getOrDefault(strCmds, 1);
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   // List of animation controller names where single quote characters are escaped by using two single quotes
   // and the entire name is wrapped in single quotes.
   // This is done so that controllers with single quotes in their names are displayed properly in MATLAB.
   std::vector<std::string> names;
   if (windowName.empty())
   {
      std::vector<AnimationController*> controllers = Service<AnimationServices>()->getAnimationControllers();
      for (std::vector<AnimationController*>::const_iterator iter = controllers.begin();
         iter != controllers.end();
         ++iter)
      {
         AnimationController* pController = *iter;
         if (pController == NULL)
         {
            outputIsError = true;
            output = "Unable to find an animation controller";
            return std::string();
         }

         names.push_back(pController->getName());
      }
   }
   else
   {
      SpatialDataView* pSpatialDataView = MatlabFunctions::getViewByWindowName(windowName);
      if (pSpatialDataView == NULL)
      {
         outputIsError = true;
         output = "Unable to find the view";
         return std::string();
      }

      AnimationController* pController = pSpatialDataView->getAnimationController();
      if (pController == NULL)
      {
         outputIsError = true;
         output = "Unable to find the animation controller";
         return std::string();
      }

      names.push_back(pController->getName());
   }

   if (names.empty() == true)
   {
      outputIsError = true;
      output = "No animation controllers found";
      return std::string();
   }

   outputIsError = false;
   return varName + MatlabFunctions::toMatlabString(names);
}

// GetAnimationCycleCommand
GetAnimationCycleCommand::GetAnimationCycleCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetAnimationCycleCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string controllerName = getOrDefault(strCmds, 1);
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   AnimationController *pController = getAnimationController(controllerName);
   if (pController == NULL)
   {
      outputIsError = true;
      output = "Unable to find the animation controller";
      return std::string();
   }

   outputIsError = false;
   return varName + MatlabFunctions::toMatlabString(StringUtilities::toDisplayString(pController->getAnimationCycle()));
}

// GetAnimationStateCommand
GetAnimationStateCommand::GetAnimationStateCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetAnimationStateCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string controllerName = getOrDefault(strCmds, 1);
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   AnimationController *pController = getAnimationController(controllerName);
   if (pController == NULL)
   {
      outputIsError = true;
      output = "Unable to find the animation controller";
      return std::string();
   }

   outputIsError = false;
   return varName + MatlabFunctions::toMatlabString(StringUtilities::toDisplayString(pController->getAnimationState()));
}

// GetIntervalMultiplierCommand
GetIntervalMultiplierCommand::GetIntervalMultiplierCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetIntervalMultiplierCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string controllerName = getOrDefault(strCmds, 1);
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   AnimationController *pController = getAnimationController(controllerName);
   if (pController == NULL)
   {
      outputIsError = true;
      output = "Unable to find the animation controller";
      return std::string();
   }

   outputIsError = false;
   return varName + StringUtilities::toDisplayString(pController->getIntervalMultiplier());
}

// SetAnimationControllerCommand
SetAnimationControllerCommand::SetAnimationControllerCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetAnimationControllerCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(controller)";
      return std::string();
   }

   Service<AnimationServices>()->setCurrentAnimationController(getAnimationController(getOrDefault(strCmds, 1)));
   outputIsError = false;
   return std::string();
}

// SetAnimationCycleCommand
SetAnimationCycleCommand::SetAnimationCycleCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetAnimationCycleCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(cycle, opt:controller)";
      return std::string();
   }

   std::string cycleName = getOrDefault(strCmds, 1);
   AnimationController* pController = getAnimationController(getOrDefault(strCmds, 2));
   if (pController == NULL)
   {
      outputIsError = true;
      output = "Unable to find the animation controller";
      return std::string();
   }

   AnimationCycle animationCycle = MatlabFunctions::getAnimationCycleByName(cycleName);
   if (animationCycle.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to determine the requested animation cycle";
      return std::string();
   }

   pController->setAnimationCycle(animationCycle);
   outputIsError = false;
   return std::string();
}

// SetAnimationStateCommand
SetAnimationStateCommand::SetAnimationStateCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetAnimationStateCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(state, opt:controller)";
      return std::string();
   }

   std::string stateName = getOrDefault(strCmds, 1);
   AnimationController* pController = getAnimationController(getOrDefault(strCmds, 2));
   if (pController == NULL)
   {
      outputIsError = true;
      output = "Unable to find the animation controller";
      return std::string();
   }

   // Changing the state does not affect the play/pause/stop status of the controller, so do that here.
   AnimationState animationState = MatlabFunctions::getAnimationStateByName(stateName);
   switch (animationState)
   {
      case STOP:
      {
         pController->stop();
         break;
      }

      case PAUSE_FORWARD: // Fall through.
      case PAUSE_BACKWARD:
      {
         pController->pause();
         break;
      }

      case PLAY_FORWARD: // Fall through.
      case PLAY_BACKWARD:
      {
         pController->play();
         break;
      }

      default:
      {
         outputIsError = true;
         output = "Unable to determine the requested animation state";
         return std::string();
      }
   }

   pController->setAnimationState(animationState);
   outputIsError = false;
   return std::string();
}

// SetIntervalMultiplierCommand
SetIntervalMultiplierCommand::SetIntervalMultiplierCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetIntervalMultiplierCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(interval, opt:controller)";
      return std::string();
   }

   std::string intervalValue = getOrDefault(strCmds, 1);
   AnimationController* pController = getAnimationController(getOrDefault(strCmds, 2));
   if (pController == NULL)
   {
      outputIsError = true;
      output = "Unable to find the animation controller";
      return std::string();
   }

   bool ok = true;
   double interval = QString::fromStdString(intervalValue).toDouble(&ok);
   if (ok == false)
   {
      outputIsError = true;
      output = "Unable to determine the requested interval";
      return std::string();
   }

   pController->setIntervalMultiplier(interval);
   outputIsError = false;
   return std::string();
}
