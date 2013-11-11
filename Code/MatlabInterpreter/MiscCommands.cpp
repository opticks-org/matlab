/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "ConfigurationSettings.h"
#include "DataVariant.h"
#include "LayerList.h"
#include "MatlabFunctions.h"
#include "MatlabInterpreter.h"
#include "MiscCommands.h"
#include "ModelServices.h"
#include "PlugInArgList.h"
#include "PlugInResource.h"
#include "Progress.h"
#include "SpatialDataView.h"

// ExecuteWizardCommand
ExecuteWizardCommand::ExecuteWizardCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string ExecuteWizardCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(wizard, opt:batch, opt:progress)";
      return std::string();
   }

   std::string wizardName = getOrDefault(strCmds, 1);
   std::string batchValue = getOrDefault(strCmds, 2, "0");
   std::string progressValue = getOrDefault(strCmds, 3, "1");

   if (wizardName.empty() == true)
   {
      outputIsError = true;
      output = "No wizard file specified";
      return std::string();
   }

   WizardObject* pWizard = MatlabFunctions::getWizardObject(wizardName);
   if (pWizard == NULL)
   {
      outputIsError = true;
      output = "Unable to find the specified wizard";
      return std::string();
   }

   bool error = false;
   bool batch = StringUtilities::fromDisplayString<bool>(batchValue, &error);
   if (error == true)
   {
      outputIsError = true;
      output = "Unable to determine whether to use batch mode or interactive mode";
      return std::string();
   }

   bool progress = StringUtilities::fromDisplayString<bool>(progressValue, &error);
   if (error == true)
   {
      outputIsError = true;
      output = "Unable to determine whether to display progress";
      return std::string();
   }

   ExecutableResource pExecutor("Wizard Executor", "", NULL, batch);
   if (pExecutor.get() == NULL)
   {
      // Firewall code.
      outputIsError = true;
      output = "Internal error running wizard";
      return std::string();
   }

   PlugInArgList& argList = pExecutor->getInArgList();
   if (argList.setPlugInArgValue("Wizard", pWizard) == false)
   {
      // Firewall code.
      outputIsError = true;
      output = "Internal error populating wizard arguments";
      return std::string();
   }

   if (progress == true)
   {
      pExecutor->createProgressDialog(true);
   }

   if (pExecutor->execute() == false)
   {
      outputIsError = true;
      output = "Error running wizard";
      Progress* pProgress = pExecutor->getProgress();
      if (pProgress != NULL)
      {
         std::string text;
         int percent;
         ReportingLevel level;
         pProgress->getProgress(text, percent, level);
         if (text.empty() == false)
         {
            output += ": " + text;
         }
      }

      return std::string();
   }

   outputIsError = false;
   return std::string();
}

// GetConfigurationSettingCommand
GetConfigurationSettingCommand::GetConfigurationSettingCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetConfigurationSettingCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(setting_name)";
      return std::string();
   }

   std::string settingName = getOrDefault(strCmds, 1);
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

#pragma message(__FILE__ "(" STRING(__LINE__) ") : warning : See OPTICKS-1517 (dadkins)")
   if (settingName.empty() == true)
   {
      outputIsError = true;
      output = "No setting specified";
      return std::string();
   }

   const DataVariant& dataVariant = Service<ConfigurationSettings>()->getSetting(settingName);
   if (dataVariant.isValid() == false)
   {
      outputIsError = true;
      output = "Unknown setting";
      return std::string();
   }

   DataVariant::Status status = DataVariant::SUCCESS;
   std::string varValue = dataVariant.toDisplayString(&status);
   if (status != DataVariant::SUCCESS)
   {
      outputIsError = true;
      output = "Unable to display the setting";
      return std::string();
   }

   outputIsError = false;
   return varName + MatlabFunctions::toMatlabString(varValue);
}

// GetCurrentNameCommand
GetCurrentNameCommand::GetCurrentNameCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetCurrentNameCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   DataElement* pElement = MatlabFunctions::getDataset(std::string());
   if (pElement == NULL)
   {
      outputIsError = true;
      output = "No current element";
      return std::string();
   }

   outputIsError = false;
   return varName + MatlabFunctions::toMatlabString(pElement->getDisplayName(true));
}

// GetDataElementNamesCommand
GetDataElementNamesCommand::GetDataElementNamesCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetDataElementNamesCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string windowName = getOrDefault(strCmds, 1);
   std::string type = getOrDefault(strCmds, 2);
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   SpatialDataView* pView = MatlabFunctions::getViewByWindowName(windowName);
   if (pView == NULL)
   {
      outputIsError = true;
      output = "Unable to find the view";
      return std::string();
   }

   LayerList* pLayerList = pView->getLayerList();
   if (pLayerList == NULL)
   {
      // Firewall code.
      outputIsError = true;
      output = "Unable to find the layer list";
      return std::string();
   }

   RasterElement* pElement = pLayerList->getPrimaryRasterElement();
   if (pElement == NULL)
   {
      // Firewall code.
      outputIsError = true;
      output = "Unable to find the primary raster element";
      return std::string();
   }

   std::vector<std::string> elementNames = Service<ModelServices>()->getElementNames(pElement, type);

   bool error = false;
   std::string varValue = StringUtilities::toDisplayString(elementNames, &error);
   if (error == true)
   {
      outputIsError = true;
      output = "Unable to display the result";
      return std::string();
   }

   outputIsError = false;
   return varName + MatlabFunctions::toMatlabString(varValue);
}

// GetDataNameCommand
GetDataNameCommand::GetDataNameCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetDataNameCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string layerName = getOrDefault(strCmds, 1);
   std::string windowName = getOrDefault(strCmds, 2);
   std::string rasterOnlyValue = getOrDefault(strCmds, 3, "1");
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   bool error = false;
   bool rasterOnly = StringUtilities::fromDisplayString<bool>(rasterOnlyValue, &error);
   if (error == true)
   {
      outputIsError = true;
      output = "Unable to determine whether to only search for raster elements or all elements";
      return std::string();
   }

   Layer* pLayer = MatlabFunctions::getLayerByName(windowName, layerName, rasterOnly);
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the specified layer";
      return std::string();
   }

   DataElement* pElement = pLayer->getDataElement();
   if (pElement == NULL)
   {
      // Firewall code.
      outputIsError = true;
      output = "Unable to find the data element for the layer";
      return std::string();
   }

   outputIsError = false;
   return varName + MatlabFunctions::toMatlabString(pElement->getDisplayName(true));
}

// OpticksCommand
OpticksCommand::OpticksCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string OpticksCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   output = "Available Opticks Commands: \n";
   const std::vector<MatlabInternalCommand*>& internalCommands = matlabInterpreter.getInternalCommands();
   for (std::vector<MatlabInternalCommand*>::const_iterator iter = internalCommands.begin();
      iter != internalCommands.end();
      ++iter)
   {
      MatlabInternalCommand* pCommand = *iter;
      if (pCommand == NULL)
      {
         // Firewall code.
         outputIsError = true;
         output = "Internal error getting the list of Opticks commands";
         return std::string();
      }

      output.append("   " + pCommand->getName() + "\n");
   }

   // This command is somewhat special in that it only displays information which should never need to be stored in a
   // MATLAB variable, so there is no need to return a string for MATLAB to execute; just set the output variable.
   output.append("Type help('command') to display help for a particular command.\n"
      "Type help('opticks') to display a brief summary of the MATLAB Scripting Extension.\n");
   outputIsError = false;
   return std::string();
}
