/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "GpuCommands.h"
#include "MatlabFunctions.h"
#include "RasterLayer.h"
#include "StringUtilities.h"
#include "View.h"

#include <QtCore/QString>

// DisableFilterCommand
DisableFilterCommand::DisableFilterCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string DisableFilterCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(filter, opt:layer, opt:window)";
      return std::string();
   }

   std::string filterName = getOrDefault(strCmds, 1);
   std::string layerName = getOrDefault(strCmds, 2);
   std::string windowName = getOrDefault(strCmds, 3);

   RasterLayer* pLayer = dynamic_cast<RasterLayer*>(MatlabFunctions::getLayerByName(windowName, layerName));
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   // Case-insensitive search for the filter.
   QString filterNameLower = QString::fromStdString(filterName).toLower();
   std::vector<std::string> filters = pLayer->getSupportedFilters();
   for (std::vector<std::string>::const_iterator iter = filters.begin(); iter != filters.end(); ++iter)
   {
      if (filterNameLower == QString::fromStdString(*iter).toLower())
      {
         // Disable the filter, and refresh the view.
         pLayer->disableFilter(*iter);
         View* pView = pLayer->getView();
         if (pView != NULL)
         {
            pView->refresh();
         }

         outputIsError = false;
         return std::string();
      }
   }

   outputIsError = true;
   output = "Unable to find the specified filter";
   return std::string();
}

// DisableGpuCommand
DisableGpuCommand::DisableGpuCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string DisableGpuCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string layerName = getOrDefault(strCmds, 1);
   std::string windowName = getOrDefault(strCmds, 2);

   RasterLayer* pLayer = dynamic_cast<RasterLayer*>(MatlabFunctions::getLayerByName(windowName, layerName));
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   pLayer->enableGpuImage(false);
   View* pView = pLayer->getView();
   if (pView != NULL)
   {
      pView->refresh();
   }

   outputIsError = false;
   return std::string();
}

// EnableFilterCommand
EnableFilterCommand::EnableFilterCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string EnableFilterCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(filter, opt:layer, opt:window)";
      return std::string();
   }

   std::string filterName = getOrDefault(strCmds, 1);
   std::string layerName = getOrDefault(strCmds, 2);
   std::string windowName = getOrDefault(strCmds, 3);

   RasterLayer* pLayer = dynamic_cast<RasterLayer*>(MatlabFunctions::getLayerByName(windowName, layerName));
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   // Case-insensitive search for the filter.
   QString filterNameLower = QString::fromStdString(filterName).toLower();
   std::vector<std::string> filters = pLayer->getSupportedFilters();
   for (std::vector<std::string>::const_iterator iter = filters.begin(); iter != filters.end(); ++iter)
   {
      if (filterNameLower == QString::fromStdString(*iter).toLower())
      {
         // Enable the filter, and refresh the view.
         pLayer->enableFilter(*iter);
         View* pView = pLayer->getView();
         if (pView != NULL)
         {
            pView->refresh();
         }

         outputIsError = false;
         return std::string();
      }
   }

   outputIsError = true;
   output = "Unable to find the specified filter";
   return std::string();
}

// EnableGpuCommand
EnableGpuCommand::EnableGpuCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string EnableGpuCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string layerName = getOrDefault(strCmds, 1);
   std::string windowName = getOrDefault(strCmds, 2);

   RasterLayer* pLayer = dynamic_cast<RasterLayer*>(MatlabFunctions::getLayerByName(windowName, layerName));
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   pLayer->enableGpuImage(true);
   View* pView = pLayer->getView();
   if (pView != NULL)
   {
      pView->refresh();
   }

   outputIsError = false;
   return std::string();
}

// GetFilterNamesCommand
GetFilterNamesCommand::GetFilterNamesCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetFilterNamesCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string layerName = getOrDefault(strCmds, 1);
   std::string windowName = getOrDefault(strCmds, 2);
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   RasterLayer* pLayer = dynamic_cast<RasterLayer*>(MatlabFunctions::getLayerByName(windowName, layerName));
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   std::vector<std::string> filters = pLayer->getSupportedFilters();
   if (filters.empty())
   {
      outputIsError = true;
      output = "No filters available";
      return std::string();
   }

   outputIsError = false;
   return varName + MatlabFunctions::toMatlabString(filters);
}
