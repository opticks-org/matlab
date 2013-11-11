/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef MATLABINTERNALCOMMAND_H
#define MATLABINTERNALCOMMAND_H

#include <string>
#include <vector>

class MatlabInterpreter;

class MatlabInternalCommand
{
public:
   MatlabInternalCommand(const std::string& name) :
      mName(name)
   {}

   virtual ~MatlabInternalCommand()
   {}

   virtual const std::string& getName() const
   {
      return mName;
   }

   virtual std::string execute(MatlabInterpreter& matlabInterpreter, const std::vector<std::string>& strCmds,
      const std::vector<std::string>& strVars, std::string& output, bool& outputIsError) = NULL;

   static std::string getOrDefault(const std::vector<std::string>& vec,
      unsigned int index, const std::string& defaultValue = std::string())
   {
      if (vec.size() > index)
      {
         return vec[index];
      }

      return defaultValue;
   }

private:
   const std::string mName;
};

#endif
