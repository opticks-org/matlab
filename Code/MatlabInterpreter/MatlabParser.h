/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef MATLABPARSER_H
#define MATLABPARSER_H

#include "MatlabInternalCommand.h"

#include <string>
#include <vector>

class MatlabInterpreter;

// Not intended to be subclassed.
class MatlabParser
{
public:
   MatlabParser();
   ~MatlabParser();

   unsigned int getCommandDepth() const;
   unsigned int getCommentDepth() const;

   std::string parseLine(MatlabInterpreter& matlabInterpreter, const std::string& command,
      std::string& output, bool& outputIsError);

   const std::vector<MatlabInternalCommand*>& getInternalCommands() const;

private:
   unsigned int mCommandDepth;
   unsigned int mCommentDepth;
   std::string mBufferedCommand;
   std::vector<MatlabInternalCommand*> mInternalCommands;

   bool parseInputString(const std::string& strCommand, std::vector<std::string>& lhs, std::vector<std::string>& rhs);

   std::string processInternalCommand(MatlabInterpreter& matlabInterpreter,
      std::vector<std::string>& lhs, std::vector<std::string>& rhs,
      std::string& output, bool& outputIsError);

   bool isInternalCommand(const std::string& command);

   bool startsWithValidLoopKeyword(const std::string& command);
   std::vector<std::string> parseVarList(const std::string& strVars);
   void parseCommandLine(const std::string& command, std::vector<std::string>& rhs);

   // Not implemented.
   MatlabParser(const MatlabParser&);
   MatlabParser& operator=(const MatlabParser&);
};

#endif
