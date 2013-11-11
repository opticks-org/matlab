/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef MATLABINTERPRETER_H
#define MATLABINTERPRETER_H

#include "ConfigurationSettings.h"
#include "Interpreter.h"

#include <string>
#include <vector>

class MatlabInternalCommand;

// Forward declaration of mxArray and its associated struct.
struct mxArray_tag;
typedef struct mxArray_tag mxArray;

class MatlabInterpreter : public Interpreter
{
public:
   SETTING(AutomaticConfiguration, MatlabInterpreter, bool, true);
   SETTING_PTR(DLL, MatlabInterpreter, Filename);
   SETTING(Version, MatlabInterpreter, std::string, std::string());
   SETTING(InteractiveAvailable, MatlabInterpreter, bool, true);
   SETTING(CheckErrors, MatlabInterpreter, bool, false);
   SETTING(ClearErrors, MatlabInterpreter, bool, false);
   SETTING(OutputBufferSize, MatlabInterpreter, int, 16384);

   virtual const std::string& getCurrentCommand() const = NULL;

   virtual const std::vector<MatlabInternalCommand*>& getInternalCommands() const = NULL;

   virtual bool isMatlabRunning() const = 0;
   virtual bool startMatlab() = 0;
   virtual std::string getStartupMessage() const = 0;
   virtual bool hideCommandWindow() = 0;
   virtual bool showCommandWindow() = 0;

   virtual bool getMatlabVariableAsString(const std::string& name, std::string& value) = NULL;
   virtual mxArray* getMatlabVariable(const std::string& name) = NULL;
   virtual bool setMatlabVariable(const std::string& name, const mxArray* pArray) = NULL;
};

#endif
