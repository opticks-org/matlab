/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef MATLABINTERPRETERENGINE_H
#define MATLABINTERPRETERENGINE_H

#include "AppConfig.h"
#include "MatlabInterpreter.h"
#include "MatlabParser.h"
#include "SubjectImp.h"

#include <engine.h>

#include <stdexcept>
#include <string>
#include <vector>

class External;

extern "C" LINKAGE MatlabInterpreter* init_matlab_interpreter(External* pServices);
extern "C" LINKAGE void shutdown_matlab_interpreter();

class MatlabInterpreterEngine : public MatlabInterpreter, public SubjectImp
{
public:
   MatlabInterpreterEngine();
   virtual ~MatlabInterpreterEngine();

   // MatlabInterpreter methods.
   virtual const std::string& getCurrentCommand() const;
   virtual const std::vector<MatlabInternalCommand*>& getInternalCommands() const;
   virtual bool isMatlabRunning() const;
   virtual bool startMatlab();
   virtual std::string getStartupMessage() const;
   virtual bool hideCommandWindow();
   virtual bool showCommandWindow();
   virtual bool getMatlabVariableAsString(const std::string& name, std::string& value);
   virtual mxArray* getMatlabVariable(const std::string& name);
   virtual bool setMatlabVariable(const std::string& name, const mxArray* pArray);

   virtual std::string getPrompt() const;
   virtual bool executeCommand(const std::string& command);
   virtual bool executeScopedCommand(const std::string& command, const Slot& output,
      const Slot& error, Progress* pProgress);
   virtual bool isGlobalOutputShown() const;
   virtual void showGlobalOutput(bool newValue);

   void sendOutput(const std::string& text);
   void sendError(const std::string& text);

   virtual const std::string& getObjectType() const;
   virtual bool isKindOf(const std::string& className) const;

   SUBJECTADAPTER_METHODS(SubjectImp)

private:
   void sendOutput(const std::string& text, bool scoped);
   void sendError(const std::string& text, bool scoped);

   void gatherOutput(Subject& subject, const std::string& signal, const boost::any& data);

   SIGNAL_METHOD(MatlabInterpreterEngine, ScopedOutputText);
   SIGNAL_METHOD(MatlabInterpreterEngine, ScopedErrorText);

   bool executeCommandInMatlab(const std::string& command);
   bool executeCommandInMatlab(const std::string& command, std::string& output,
      bool& outputIsError, bool& outputTruncated);

   MatlabParser mParser;
   Engine* mpMatlabEngine;
   bool mGlobalOutputShown;
   unsigned int mScopedCommandDepth;
   std::string mStartupMessage;
   std::vector<char> mOutputBuffer;
   std::string mCurrentCommand;
};

#endif
