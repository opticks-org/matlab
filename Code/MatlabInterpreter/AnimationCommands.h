/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef ANIMATIONCOMMANDS_H
#define ANIMATIONCOMMANDS_H

#include "MatlabInternalCommand.h"

#include <string>
#include <vector>

class CreateAnimationCommand : public MatlabInternalCommand
{
public:
   CreateAnimationCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class DisableCanDropFramesCommand : public MatlabInternalCommand
{
public:
   DisableCanDropFramesCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class EnableCanDropFramesCommand : public MatlabInternalCommand
{
public:
   EnableCanDropFramesCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetAnimationControllerNamesCommand : public MatlabInternalCommand
{
public:
   GetAnimationControllerNamesCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetAnimationCycleCommand : public MatlabInternalCommand
{
public:
   GetAnimationCycleCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetAnimationStateCommand : public MatlabInternalCommand
{
public:
   GetAnimationStateCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetIntervalMultiplierCommand : public MatlabInternalCommand
{
public:
   GetIntervalMultiplierCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class SetAnimationControllerCommand : public MatlabInternalCommand
{
public:
   SetAnimationControllerCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class SetAnimationCycleCommand : public MatlabInternalCommand
{
public:
   SetAnimationCycleCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class SetAnimationStateCommand : public MatlabInternalCommand
{
public:
   SetAnimationStateCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class SetIntervalMultiplierCommand : public MatlabInternalCommand
{
public:
   SetIntervalMultiplierCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

#endif
