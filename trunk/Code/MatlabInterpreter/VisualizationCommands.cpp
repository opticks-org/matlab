/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "ColorMap.h"
#include "MatlabFunctions.h"
#include "MatlabInterpreter.h"
#include "RasterLayer.h"
#include "SpatialDataView.h"
#include "StringUtilities.h"
#include "TypesFile.h"
#include "VisualizationCommands.h"

#include <string>

// GetStretchUnitsCommand
GetStretchUnitsCommand::GetStretchUnitsCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetStretchUnitsCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string layerName = getOrDefault(strCmds, 1);
   std::string channelName = getOrDefault(strCmds, 2, "Gray");
   std::string windowName = getOrDefault(strCmds, 3);
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   RasterChannelType channel = MatlabFunctions::getChannelByName(channelName);
   if (channel.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to determine which channel was requested";
      return std::string();
   }

   RasterLayer* pLayer = dynamic_cast<RasterLayer*>(MatlabFunctions::getLayerByName(windowName, layerName));
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   RegionUnits units = pLayer->getStretchUnits(channel);
   if (units.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to find stretch units for the requested channel";
      return std::string();
   }

   bool error = false;
   std::string varValue = StringUtilities::toDisplayString<RegionUnits>(units, &error);
   if (varValue.empty() == true || error == true)
   {
      outputIsError = true;
      output = "Unable to convert stretch units to a string";
      return std::string();
   }

   outputIsError = false;
   return varName + MatlabFunctions::toMatlabString(varValue);
}

// GetStretchTypeCommand
GetStretchTypeCommand::GetStretchTypeCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetStretchTypeCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string layerName = getOrDefault(strCmds, 1);
   std::string displayModeName = getOrDefault(strCmds, 2, "grayscale");
   std::string windowName = getOrDefault(strCmds, 3);
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   DisplayMode displayMode = MatlabFunctions::getDisplayModeByName(displayModeName);
   if (displayMode.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to determine the display mode";
      return std::string();
   }

   RasterLayer* pLayer = dynamic_cast<RasterLayer*>(MatlabFunctions::getLayerByName(windowName, layerName));
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   StretchType stretchType = pLayer->getStretchType(displayMode);
   if (stretchType.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to find stretch type for the requested mode";
      return std::string();
   }

   bool error = false;
   std::string varValue = StringUtilities::toDisplayString<StretchType>(stretchType, &error);
   if (varValue.empty() == true || error == true)
   {
      outputIsError = true;
      output = "Unable to convert stretch type to a string";
      return std::string();
   }

   outputIsError = false;
   return varName + MatlabFunctions::toMatlabString(varValue);
}

// GetStretchValuesCommand
GetStretchValuesCommand::GetStretchValuesCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetStretchValuesCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string stretchMax = getOrDefault(strVars, 0, "stretch_max");
   std::string stretchMin = getOrDefault(strVars, 1, "stretch_min");
   std::string layerName = getOrDefault(strCmds, 1);
   std::string channelName = getOrDefault(strCmds, 2, "Gray");
   std::string windowName = getOrDefault(strCmds, 3);

   RasterChannelType channel = MatlabFunctions::getChannelByName(channelName);
   if (channel.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to determine which channel was requested";
      return std::string();
   }

   RasterLayer* pLayer = dynamic_cast<RasterLayer*>(MatlabFunctions::getLayerByName(windowName, layerName));
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   double dLower;
   double dUpper;
   pLayer->getStretchValues(channel, dLower, dUpper);

   // Determine whether to use comma or semicolon for output.
   const char delim = *(matlabInterpreter.getCurrentCommand().crbegin()) == ';' ? ';' : ',';

   outputIsError = false;
   return stretchMax + " = " + StringUtilities::toDisplayString(dUpper) + delim +
      stretchMin + " = " + StringUtilities::toDisplayString(dLower);
}

// SetColorMapCommand
SetColorMapCommand::SetColorMapCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetColorMapCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(filename, opt:layer, opt:window)";
      return std::string();
   }

   std::string filename = getOrDefault(strCmds, 1);
   std::string layerName = getOrDefault(strCmds, 2);
   std::string windowName = getOrDefault(strCmds, 3);

   ColorMap colorMap;
   if (colorMap.loadFromFile(filename) == false)
   {
      outputIsError = true;
      output = "Unable to load the color map from the file";
      return std::string();
   }

   RasterLayer* pLayer = dynamic_cast<RasterLayer*>(MatlabFunctions::getLayerByName(windowName, layerName));
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   outputIsError = false;
   pLayer->setColorMap(colorMap);
   return std::string();
}

// SetStretchUnitsCommand
SetStretchUnitsCommand::SetStretchUnitsCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetStretchUnitsCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(units, opt:layer, opt:channel, opt:window)";
      return std::string();
   }

   std::string unitsName = getOrDefault(strCmds, 1);
   std::string layerName = getOrDefault(strCmds, 2);
   std::string channelName = getOrDefault(strCmds, 3, "Gray");
   std::string windowName = getOrDefault(strCmds, 4);

   RegionUnits units = MatlabFunctions::getRegionUnitsByName(unitsName);
   if (units.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to determine which unit type was requested";
      return std::string();
   }

   RasterChannelType channel = MatlabFunctions::getChannelByName(channelName);
   if (channel.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to determine which channel was requested";
      return std::string();
   }

   RasterLayer* pLayer = dynamic_cast<RasterLayer*>(MatlabFunctions::getLayerByName(windowName, layerName));
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   pLayer->setStretchUnits(channel, units);
   outputIsError = false;
   return std::string();
}

// SetStretchTypeCommand
SetStretchTypeCommand::SetStretchTypeCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetStretchTypeCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(type, opt:layer, opt:display mode, opt:window)";
      return std::string();
   }

   std::string stretchTypeName = getOrDefault(strCmds, 1);
   std::string layerName = getOrDefault(strCmds, 2);
   std::string displayModeName = getOrDefault(strCmds, 3, "grayscale");
   std::string windowName = getOrDefault(strCmds, 4);

   RasterLayer* pLayer = dynamic_cast<RasterLayer*>(MatlabFunctions::getLayerByName(windowName, layerName));
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   DisplayMode displayMode = MatlabFunctions::getDisplayModeByName(displayModeName);
   if (displayMode.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to determine the display mode";
      return std::string();
   }

   StretchType stretchType = MatlabFunctions::getStretchTypeByName(stretchTypeName);
   if (stretchType.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to determine the stretch type";
      return std::string();
   }

   outputIsError = false;
   pLayer->setStretchType(displayMode, stretchType);
   return std::string();
}

// SetStretchValuesCommand
SetStretchValuesCommand::SetStretchValuesCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetStretchValuesCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1 || strCmds.size() == 2)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(max, min, opt:layer, opt:channel, opt:window)";
      return std::string();
   }

   std::string stretchMax = getOrDefault(strCmds, 1);
   std::string stretchMin = getOrDefault(strCmds, 2);
   std::string layerName = getOrDefault(strCmds, 3);
   std::string channelName = getOrDefault(strCmds, 4, "Gray");
   std::string windowName = getOrDefault(strCmds, 5);

   bool error = false;
   double dLower = StringUtilities::fromXmlString<double>(stretchMin, &error);
   if (error == true)
   {
      outputIsError = true;
      output = "Unable to determine lower stretch value";
      return std::string();
   }

   double dUpper = StringUtilities::fromXmlString<double>(stretchMax, &error);
   if (error == true)
   {
      outputIsError = true;
      output = "Unable to determine upper stretch value";
      return std::string();
   }

   RasterChannelType channel = MatlabFunctions::getChannelByName(channelName);
   if (channel.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to determine which channel was requested";
      return std::string();
   }

   RasterLayer* pLayer = dynamic_cast<RasterLayer*>(MatlabFunctions::getLayerByName(windowName, layerName));
   if (pLayer == NULL)
   {
      outputIsError = true;
      output = "Unable to find the layer or window";
      return std::string();
   }

   pLayer->setStretchValues(channel, dLower, dUpper);
   outputIsError = false;
   return std::string();
}
