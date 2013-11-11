/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef LAYERCOMMANDS_H
#define LAYERCOMMANDS_H

#include "MatlabInternalCommand.h"

#include <string>
#include <vector>

class GetLayerNameCommand : public MatlabInternalCommand
{
public:
   GetLayerNameCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetLayerOffsetCommand : public MatlabInternalCommand
{
public:
   GetLayerOffsetCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetLayerPositionCommand : public MatlabInternalCommand
{
public:
   GetLayerPositionCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class GetNumLayersCommand : public MatlabInternalCommand
{
public:
   GetNumLayersCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class HideLayerCommand : public MatlabInternalCommand
{
public:
   HideLayerCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class SetLayerOffsetCommand : public MatlabInternalCommand
{
public:
   SetLayerOffsetCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class SetLayerPositionCommand : public MatlabInternalCommand
{
public:
   SetLayerPositionCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

class ShowLayerCommand : public MatlabInternalCommand
{
public:
   ShowLayerCommand(const std::string& name);
   std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError);
};

#endif
