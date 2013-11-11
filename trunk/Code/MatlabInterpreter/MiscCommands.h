/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef MISCCOMMANDS_H
#define MISCCOMMANDS_H

#include "MatlabInternalCommand.h"

#include <string>
#include <vector>

class ExecuteWizardCommand : public MatlabInternalCommand
{
public:
   ExecuteWizardCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetConfigurationSettingCommand : public MatlabInternalCommand
{
public:
   GetConfigurationSettingCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetCurrentNameCommand : public MatlabInternalCommand
{
public:
   GetCurrentNameCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetDataElementNamesCommand : public MatlabInternalCommand
{
public:
   GetDataElementNamesCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetDataNameCommand : public MatlabInternalCommand
{
public:
   GetDataNameCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class OpticksCommand : public MatlabInternalCommand
{
public:
   OpticksCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

#endif
