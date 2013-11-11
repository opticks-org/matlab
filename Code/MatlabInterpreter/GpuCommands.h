/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef GPUCOMMANDS_H
#define GPUCOMMANDS_H

#include "MatlabInternalCommand.h"

#include <string>
#include <vector>

class DisableFilterCommand : public MatlabInternalCommand
{
public:
   DisableFilterCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class DisableGpuCommand : public MatlabInternalCommand
{
public:
   DisableGpuCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class EnableFilterCommand : public MatlabInternalCommand
{
public:
   EnableFilterCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class EnableGpuCommand : public MatlabInternalCommand
{
public:
   EnableGpuCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetFilterNamesCommand : public MatlabInternalCommand
{
public:
   GetFilterNamesCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

#endif
