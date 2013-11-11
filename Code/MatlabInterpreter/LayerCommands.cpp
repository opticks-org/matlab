/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "Layer.h"
#include "LayerList.h"
#include "LayerCommands.h"
#include "MatlabFunctions.h"
#include "MatlabInterpreter.h"
#include "SpatialDataView.h"

#include <QtCore/QString>

// GetLayerNameCommand
GetLayerNameCommand::GetLayerNameCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetLayerNameCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(index, opt:window)";
      return std::string();
   }

   std::string layerIndex = getOrDefault(strCmds, 1);
   std::string windowName = getOrDefault(strCmds, 2);
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   bool ok = true;
   int index = QString::fromStdString(layerIndex).toInt(&ok);
   if (ok == false)
   {
      outputIsError = true;
      output = "Unable to determine the requested layer index";
      return std::string();
   }

   Layer* pLayer = MatlabFunctions::getLayerByIndex(windowName, index);
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   outputIsError = false;
   return varName + MatlabFunctions::toMatlabString(pLayer->getDisplayName(true));
}

// GetLayerOffsetCommand
GetLayerOffsetCommand::GetLayerOffsetCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetLayerOffsetCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string layerName = getOrDefault(strCmds, 1);
   std::string windowName = getOrDefault(strCmds, 2);
   std::string xOffsetName = getOrDefault(strVars, 0, "offset_x");
   std::string yOffsetName = getOrDefault(strVars, 1, "offset_y");

   Layer* pLayer = MatlabFunctions::getLayerByName(windowName, layerName, false);
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   // Determine whether to use comma or semicolon for output.
   const char delim = QString::fromStdString(matlabInterpreter.getCurrentCommand()).endsWith(';') ? ';' : ',';

   outputIsError = false;
   return xOffsetName + " = " + QString::number(pLayer->getXOffset()).toStdString() + delim +
      yOffsetName + " = " + QString::number(pLayer->getYOffset()).toStdString();
}

// GetLayerPositionCommand
GetLayerPositionCommand::GetLayerPositionCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetLayerPositionCommand::execute(MatlabInterpreter& matlabInterpreter,
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

   Layer* pLayer = MatlabFunctions::getLayerByName(windowName, layerName, false);
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   SpatialDataView* pView = dynamic_cast<SpatialDataView*>(pLayer->getView());
   if (pView == NULL)
   {
      outputIsError = true;
      output = "Unable to find the view";
      return std::string();
   }

   outputIsError = false;
   return varName + QString::number(pView->getLayerDisplayIndex(pLayer)).toStdString();
}

// GetNumLayersCommand
GetNumLayersCommand::GetNumLayersCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetNumLayersCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string windowName = getOrDefault(strCmds, 1);
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   SpatialDataView* pView = dynamic_cast<SpatialDataView*>(MatlabFunctions::getViewByWindowName(windowName));
   if (pView == NULL)
   {
      outputIsError = true;
      output = "Unable to find the view";
      return std::string();
   }

   LayerList* pLayerList = pView->getLayerList();
   if (pLayerList == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer list";
      return std::string();
   }

   outputIsError = false;
   return varName + QString::number(pLayerList->getNumLayers()).toStdString();
}

// HideLayerCommand
HideLayerCommand::HideLayerCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string HideLayerCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string layerName = getOrDefault(strCmds, 1);
   std::string windowName = getOrDefault(strCmds, 2);

   Layer* pLayer = MatlabFunctions::getLayerByName(windowName, layerName, false);
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   SpatialDataView* pView = dynamic_cast<SpatialDataView*>(pLayer->getView());
   if (pView == NULL)
   {
      outputIsError = true;
      output = "Unable to find the view";
      return std::string();
   }

   if (pView->hideLayer(pLayer) == false)
   {
      outputIsError = true;
      output = "Unable to hide the layer";
      return std::string();
   }

   outputIsError = false;
   return std::string();
}

// SetLayerOffsetCommand
SetLayerOffsetCommand::SetLayerOffsetCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetLayerOffsetCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1 || strCmds.size() == 2)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(x_offset, y_offset, opt:layer, opt:window)";
      return std::string();
   }

   std::string xOffsetValue = getOrDefault(strCmds, 1);
   std::string yOffsetValue = getOrDefault(strCmds, 2);
   std::string layerName = getOrDefault(strCmds, 3);
   std::string windowName = getOrDefault(strCmds, 4);

   Layer* pLayer = MatlabFunctions::getLayerByName(windowName, layerName, false);
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   bool ok = true;
   double xOffset = QString::fromStdString(xOffsetValue).toDouble(&ok);
   if (ok == false)
   {
      outputIsError = true;
      output = "Unable to determine the requested x offset";
      return std::string();
   }

   double yOffset = QString::fromStdString(yOffsetValue).toDouble(&ok);
   if (ok == false)
   {
      outputIsError = true;
      output = "Unable to determine the requested y offset";
      return std::string();
   }

   pLayer->setXOffset(xOffset);
   pLayer->setYOffset(yOffset);
   outputIsError = false;
   return std::string();
}

// SetLayerPositionCommand
SetLayerPositionCommand::SetLayerPositionCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetLayerPositionCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1 || strCmds.size() == 2)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(layer, index, opt:window)";
      return std::string();
   }

   std::string layerName = getOrDefault(strCmds, 1);
   std::string layerIndex = getOrDefault(strCmds, 2);
   std::string windowName = getOrDefault(strCmds, 3);

   Layer* pLayer = MatlabFunctions::getLayerByName(windowName, layerName, false);
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   SpatialDataView* pView = dynamic_cast<SpatialDataView*>(pLayer->getView());
   if (pView == NULL)
   {
      outputIsError = true;
      output = "Unable to find the view";
      return std::string();
   }

   bool ok = true;
   int index = QString::fromStdString(layerIndex).toInt(&ok);
   if (ok == false)
   {
      outputIsError = true;
      output = "Unable to determine the requested layer index";
      return std::string();
   }

   if (pView->setLayerDisplayIndex(pLayer, index) == false)
   {
      outputIsError = true;
      output = "Unable to set the layer display index";
      return std::string();
   }

   outputIsError = false;
   return std::string();
}

// ShowLayerCommand
ShowLayerCommand::ShowLayerCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string ShowLayerCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(layer)";
      return std::string();
   }

   std::string layerName = getOrDefault(strCmds, 1);
   std::string windowName = getOrDefault(strCmds, 2);

   Layer* pLayer = MatlabFunctions::getLayerByName(windowName, layerName, false);
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   SpatialDataView* pView = dynamic_cast<SpatialDataView*>(pLayer->getView());
   if (pView == NULL)
   {
      outputIsError = true;
      output = "Unable to find the view";
      return std::string();
   }

   if (pView->showLayer(pLayer) == false)
   {
      outputIsError = true;
      output = "Unable to show the layer";
      return std::string();
   }

   outputIsError = false;
   return std::string();
}
