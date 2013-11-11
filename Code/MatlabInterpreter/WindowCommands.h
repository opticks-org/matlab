/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef WINDOWCOMMANDS_H
#define WINDOWCOMMANDS_H

#include "MatlabInternalCommand.h"

#include <string>
#include <vector>

class CloseWindowCommand : public MatlabInternalCommand
{
public:
   CloseWindowCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetWindowLabelCommand : public MatlabInternalCommand
{
public:
   GetWindowLabelCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetWindowNameCommand : public MatlabInternalCommand
{
public:
   GetWindowNameCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetWindowPositionCommand : public MatlabInternalCommand
{
public:
   GetWindowPositionCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class RefreshDisplayCommand : public MatlabInternalCommand
{
public:
   RefreshDisplayCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class SetCurrentWindowCommand : public MatlabInternalCommand
{
public:
   SetCurrentWindowCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class SetWindowLabelCommand : public MatlabInternalCommand
{
public:
   SetWindowLabelCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class SetWindowPositionCommand : public MatlabInternalCommand
{
public:
   SetWindowPositionCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

#endif
