/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "ApplicationServices.h"
#include "ConfigurationSettings.h"
#include "Filename.h"
#include "MatlabCommands.h"
#include "MatlabInterpreter.h"

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtGui/QFileDialog>

// HideCommandWindowCommand
HideCommandWindowCommand::HideCommandWindowCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string HideCommandWindowCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (matlabInterpreter.hideCommandWindow() == false)
   {
      outputIsError = true;
      output = "Unable to hide the command window.";
      return std::string();
   }

   outputIsError = false;
   return std::string();
}

// RunCommand
RunCommand::RunCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string RunCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   QString filename = QString::fromStdString(getOrDefault(strCmds, 1));
   if (filename.isEmpty() == true && Service<ApplicationServices>()->isBatch() == false)
   {
      QString directory;
      const Filename* pSupportFiles = ConfigurationSettings::getSettingSupportFilesPath();
      if (pSupportFiles != NULL)
      {
         directory = QString::fromStdString(pSupportFiles->getFullPathAndName() + "/MATLAB/");
      }

      filename = QFileDialog::getOpenFileName(NULL, "Load MATLAB script", directory, "MATLAB Files (*.m *.mi)");
   }

   if (filename.isEmpty() == true)
   {
      outputIsError = true;
      output = "No file specified";
      return std::string();
   }

   QFile scriptFile(filename);
   if (scriptFile.open(QIODevice::ReadOnly | QIODevice::Text) == false)
   {
      outputIsError = true;
      output = "Unable to open the input file";
      return std::string();
   }

   unsigned int lineNumber = 0;
   while (scriptFile.atEnd() == false)
   {
      ++lineNumber;
      QString line(scriptFile.readLine());
      if (line.isEmpty() == true)
      {
         continue;
      }

      if (matlabInterpreter.executeCommand(line.toStdString()) == false)
      {
         scriptFile.close();
         outputIsError = true;
         QString error("\n\nError running script: \"%1\", Line %2: \"%3\"");
         output = error.arg(filename).arg(lineNumber).arg(line.trimmed()).toStdString();
         return std::string();
      }

      // Make the script stop if a "return" is encountered.
      QString commandLower = line.toLower().trimmed();
      if (commandLower == "return" || commandLower.startsWith("return;") || commandLower.startsWith("return,"))
      {
         break;
      }
   }

   scriptFile.close();
   outputIsError = false;
   return std::string();
}

// ShowCommandWindowCommand
ShowCommandWindowCommand::ShowCommandWindowCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string ShowCommandWindowCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (matlabInterpreter.showCommandWindow() == false)
   {
      outputIsError = true;
      output = "Unable to show the command window.";
      return std::string();
   }

   outputIsError = false;
   return std::string();
}
