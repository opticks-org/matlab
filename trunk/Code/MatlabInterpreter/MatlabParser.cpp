/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "AnimationCommands.h"
#include "ArrayCommands.h"
#include "GpuCommands.h"
#include "LayerCommands.h"
#include "MatlabCommands.h"
#include "MatlabInterpreter.h"
#include "MatlabParser.h"
#include "MetadataCommands.h"
#include "MiscCommands.h"
#include "VisualizationCommands.h"
#include "WindowCommands.h"

#include <QtCore/QString>
#include <QtCore/QStringList>

MatlabParser::MatlabParser() :
   mCommandDepth(0),
   mCommentDepth(0)
{
   mInternalCommands.push_back(new ArraySizeCommand("array_size"));
   mInternalCommands.push_back(new ArrayToMatlabCommand("array_to_matlab"));
   mInternalCommands.push_back(new ArrayToOpticksCommand("array_to_opticks"));
   mInternalCommands.push_back(new CloseWindowCommand("close_window"));
   mInternalCommands.push_back(new CopyMetadataCommand("copy_metadata"));
   mInternalCommands.push_back(new CreateAnimationCommand("create_animation"));
   mInternalCommands.push_back(new DisableCanDropFramesCommand("disable_can_drop_frames"));
   mInternalCommands.push_back(new DisableFilterCommand("disable_filter"));
   mInternalCommands.push_back(new DisableGpuCommand("disable_gpu"));
   mInternalCommands.push_back(new EnableCanDropFramesCommand("enable_can_drop_frames"));
   mInternalCommands.push_back(new EnableFilterCommand("enable_filter"));
   mInternalCommands.push_back(new EnableGpuCommand("enable_gpu"));
   mInternalCommands.push_back(new ExecuteWizardCommand("execute_wizard"));
   mInternalCommands.push_back(new GetAnimationControllerNamesCommand("get_animation_controller_names"));
   mInternalCommands.push_back(new GetAnimationCycleCommand("get_animation_cycle"));
   mInternalCommands.push_back(new GetAnimationStateCommand("get_animation_state"));
   mInternalCommands.push_back(new GetConfigurationSettingCommand("get_configuration_setting"));
   mInternalCommands.push_back(new GetCurrentNameCommand("get_current_name"));
   mInternalCommands.push_back(new GetDataElementNamesCommand("get_data_element_names"));
   mInternalCommands.push_back(new GetDataNameCommand("get_data_name"));
   mInternalCommands.push_back(new GetFilterNamesCommand("get_filter_names"));
   mInternalCommands.push_back(new GetIntervalMultiplierCommand("get_interval_multiplier"));
   mInternalCommands.push_back(new GetLayerNameCommand("get_layer_name"));
   mInternalCommands.push_back(new GetLayerOffsetCommand("get_layer_offset"));
   mInternalCommands.push_back(new GetLayerPositionCommand("get_layer_position"));
   mInternalCommands.push_back(new GetMetadataCommand("get_metadata"));
   mInternalCommands.push_back(new GetNumLayersCommand("get_num_layers"));
   mInternalCommands.push_back(new GetStretchUnitsCommand("get_stretch_units"));
   mInternalCommands.push_back(new GetStretchTypeCommand("get_stretch_type"));
   mInternalCommands.push_back(new GetStretchValuesCommand("get_stretch_values"));
   mInternalCommands.push_back(new GetWindowLabelCommand("get_window_label"));
   mInternalCommands.push_back(new GetWindowNameCommand("get_window_name"));
   mInternalCommands.push_back(new GetWindowPositionCommand("get_window_position"));
   mInternalCommands.push_back(new HideCommandWindowCommand("hide_command_window"));
   mInternalCommands.push_back(new HideLayerCommand("hide_layer"));
   mInternalCommands.push_back(new OpticksCommand("opticks"));
   mInternalCommands.push_back(new RefreshDisplayCommand("refresh_display"));
   mInternalCommands.push_back(new ReloadWizardCommand("reload_wizard"));
   mInternalCommands.push_back(new RunCommand("run"));
   mInternalCommands.push_back(new SetAnimationControllerCommand("set_animation_controller"));
   mInternalCommands.push_back(new SetAnimationCycleCommand("set_animation_cycle"));
   mInternalCommands.push_back(new SetAnimationStateCommand("set_animation_state"));
   mInternalCommands.push_back(new SetColorMapCommand("set_colormap"));
   mInternalCommands.push_back(new SetCurrentWindowCommand("set_current_window"));
   mInternalCommands.push_back(new SetIntervalMultiplierCommand("set_interval_multiplier"));
   mInternalCommands.push_back(new SetLayerOffsetCommand("set_layer_offset"));
   mInternalCommands.push_back(new SetLayerPositionCommand("set_layer_position"));
   mInternalCommands.push_back(new SetMetadataCommand("set_metadata"));
   mInternalCommands.push_back(new SetStretchUnitsCommand("set_stretch_units"));
   mInternalCommands.push_back(new SetStretchTypeCommand("set_stretch_type"));
   mInternalCommands.push_back(new SetStretchValuesCommand("set_stretch_values"));
   mInternalCommands.push_back(new SetWindowLabelCommand("set_window_label"));
   mInternalCommands.push_back(new SetWindowPositionCommand("set_window_position"));
   mInternalCommands.push_back(new ShowCommandWindowCommand("show_command_window"));
   mInternalCommands.push_back(new ShowLayerCommand("show_layer"));
}

MatlabParser::~MatlabParser()
{
   // Delete all internal commands.
   for (std::vector<MatlabInternalCommand*>::iterator iter = mInternalCommands.begin();
      iter != mInternalCommands.end();
      ++iter)
   {
      delete *iter;
   }

   mInternalCommands.clear();
}

unsigned int MatlabParser::getCommandDepth() const
{
   return mCommandDepth;
}

unsigned int MatlabParser::getCommentDepth() const
{
   return mCommentDepth;
}

const std::vector<MatlabInternalCommand*>& MatlabParser::getInternalCommands() const
{
   return mInternalCommands;
}

std::string MatlabParser::parseLine(MatlabInterpreter& matlabInterpreter, const std::string& command,
   std::string& output, bool& outputIsError)
{
   std::string tmpCommand = command;
   if (tmpCommand.empty() == true)
   {
      return std::string();
   }

   // Check for comments. This will not work in all cases, e.g.:
   // if a string literal contains a '%' character.
   size_t commentIndex = tmpCommand.find('%');
   if (commentIndex != std::string::npos)
   {
      if (tmpCommand == "%{")
      {
         ++mCommentDepth;
      }
      else if (mCommentDepth > 0 && tmpCommand == "%}")
      {
         --mCommentDepth;
      }

      if (commentIndex == 0)
      {
         return std::string();
      }

      // Check number of single quote characters.
      // Even --> the '%' is outside of a string literal
      // Odd --> the '%' is inside of a string literal
      bool outsideString = true;
      for (size_t i = 0; i < commentIndex; ++i)
      {
         if (tmpCommand[i] == '\'')
         {
            outsideString = !outsideString;
         }
      }

      // Ignore comments during parsing.
      if (outsideString == true)
      {
         tmpCommand = tmpCommand.substr(0, commentIndex);
      }
   }

   if (mCommentDepth > 0)
   {
      return std::string();
   }

   // Split the input.
   std::vector<std::string> lhs;
   std::vector<std::string> rhs;

   // If this is an internal command and there is currently no buffering, run that command.
   if (parseInputString(tmpCommand, lhs, rhs) == true)
   {
      if (mCommandDepth > 0)
      {
         // Internal commands are not supported while buffering because the entire buffered command must be sent.
         outputIsError = true;
         output = "Unable to run this command while buffering commands for MATLAB";
         return std::string();
      }

      return processInternalCommand(matlabInterpreter, rhs, lhs, output, outputIsError);
   }

   // Buffer the command, and return it once it is ready to be executed.
   if (startsWithValidLoopKeyword(tmpCommand) == true)
   {
      ++mCommandDepth;
      mBufferedCommand += tmpCommand + "\n";
      return std::string();
   }

   if (mCommandDepth == 0)
   {
      return tmpCommand;
   }

   QString commandLower = QString::fromStdString(tmpCommand).toLower().trimmed();
   if (commandLower == "end" || commandLower.startsWith("end;") || commandLower.startsWith("end,"))
   {
      --mCommandDepth;
      if (mCommandDepth == 0)
      {
         std::string completeCommand = mBufferedCommand + tmpCommand;
         mBufferedCommand.clear();
         return completeCommand;
      }
   }

   mBufferedCommand += tmpCommand + "\n";
   return std::string();
}

bool MatlabParser::parseInputString(const std::string& strCommand, std::vector<std::string>& lhs, std::vector<std::string>& rhs)
{
   int iIndex = strCommand.find("=");
   if (iIndex > 0)
   {
      std::string strSubCommand = strCommand.substr(iIndex + 1);
      parseCommandLine(strSubCommand, rhs);
      strCommand.substr(iIndex + 1);
      strSubCommand = strCommand.substr(0,iIndex);
      lhs = parseVarList(strSubCommand);
   }
   else
   {
      parseCommandLine(strCommand, rhs);
      lhs.clear();
   }

   return rhs.size() > 0 && isInternalCommand(rhs[0]);
}

bool MatlabParser::isInternalCommand(const std::string& command)
{
   for (std::vector<MatlabInternalCommand*>::iterator iter = mInternalCommands.begin();
      iter != mInternalCommands.end();
      ++iter)
   {
      if ((*iter)->getName() == command)
      {
         return true;
      }
   }

   return false;
}

bool MatlabParser::startsWithValidLoopKeyword(const std::string& command)
{
   // Check whether the command begins with a loop keyword.
   QString commandLower = QString::fromStdString(command).toLower().trimmed();
   return commandLower.startsWith("for ") ||
      commandLower.startsWith("if ") ||
      commandLower.startsWith("while ");
}

std::vector<std::string> MatlabParser::parseVarList(const std::string& strVars)
{
   // output variable list can have the form of [var1 var2 ...] or var1
   std::vector<std::string> varList;
   std::string strVarsTemp = QString(strVars.c_str()).trimmed().toStdString();
   if (strVarsTemp.find("[") == 0 && strVarsTemp.find("]")==strVarsTemp.length()-1)
   {
      strVarsTemp = strVarsTemp.substr(1,strVarsTemp.length()-2);
   }
   QString strWorking = QString(strVarsTemp.c_str()).trimmed();
   QStringList strVarListWorking = strWorking.split(" ",QString::SkipEmptyParts);
   for (int i = 0; i < strVarListWorking.size(); i++)
   {
      varList.push_back(strVarListWorking[i].toStdString());
   }
   return varList;
}

void MatlabParser::parseCommandLine(const std::string& command, std::vector<std::string>& rhs)
{
   int iPos1, iPos2, iPos3;
   std::string strTemp;

   iPos1 = command.find("(");
   if (iPos1 > 0)
   {
      iPos3 = command.find(")");
      if (iPos3 > 0)
      {
         QString strCommand = QString(command.substr(0, iPos1).c_str());
         rhs.push_back(strCommand.trimmed().toStdString());
         iPos2 = command.length() - 1;
         while (command[iPos2] != ')')
         {
            iPos2--;
         }
         if (iPos2 > 0)
         {
            if (iPos2 - iPos1 - 1 > 0)  // ignore empty parens
            {
               strTemp = command.substr(iPos1+1, iPos2-iPos1-1);
               QStringList strList = QString(strTemp.c_str()).split(",");
               for (int i = 0; i < strList.size(); i++)
               {
                  rhs.push_back(strList[i].trimmed().toStdString());
               }
            }
         }
      }
   }
   else // no "(" found in string, must be single word command
   {
      rhs.push_back(QString::fromStdString(command).trimmed().toStdString());
   }
}

std::string MatlabParser::processInternalCommand(MatlabInterpreter& matlabInterpreter,
   std::vector<std::string>& strCmds, std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.empty() == true)
   {
      outputIsError = true;
      output = "Internal error parsing the command";
      return std::string();
   }

   // Check if any arguments are matlab variables, and evaluate them.
   for (std::vector<std::string>::iterator iter = strCmds.begin() + 1; iter != strCmds.end(); ++iter)
   {
      std::string variable;
      if (matlabInterpreter.getMatlabVariableAsString(*iter, variable) == true)
      {
         *iter = variable;
      }
   }

   for (std::vector<MatlabInternalCommand*>::iterator iter = mInternalCommands.begin();
      iter != mInternalCommands.end();
      ++iter)
   {
      if ((*iter)->getName() == strCmds[0])
      {
         return (*iter)->execute(matlabInterpreter, strCmds, strVars, output, outputIsError);
      }
   }

   outputIsError = true;
   output = "Internal error running command " + strCmds[0];
   return std::string();
}
