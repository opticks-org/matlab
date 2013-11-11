/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "AppConfig.h"
#include "AppVerify.h"
#include "ConfigurationSettings.h"
#include "MatlabFunctions.h"
#include "MatlabInterpreterEngine.h"
#include "MatlabInterpreterOptions.h"
#include "MatlabVersion.h"
#include "InterpreterUtilities.h"

#include <engine.h>

#include <QtCore/QString>

MatlabInterpreterEngine::MatlabInterpreterEngine() :
   mpMatlabEngine(NULL),
   mGlobalOutputShown(false),
   mScopedCommandDepth(0)
{}

MatlabInterpreterEngine::~MatlabInterpreterEngine()
{
   notify(SIGNAL_NAME(Subject, Deleted));

   if (mpMatlabEngine != NULL)
   {
      engClose(mpMatlabEngine);
      mpMatlabEngine = NULL;
   }
}

const std::string& MatlabInterpreterEngine::getCurrentCommand() const
{
   return mCurrentCommand;
}

const std::vector<MatlabInternalCommand*>& MatlabInterpreterEngine::getInternalCommands() const
{
   return mParser.getInternalCommands();
}

bool MatlabInterpreterEngine::isMatlabRunning() const
{
   return mpMatlabEngine != NULL;
}

std::string MatlabInterpreterEngine::getStartupMessage() const
{
   return mStartupMessage;
}

bool MatlabInterpreterEngine::startMatlab()
{
   if (isMatlabRunning() == true)
   {
      return true;
   }

   mpMatlabEngine = engOpenSingleUse(NULL, NULL, NULL);
   if (mpMatlabEngine == NULL)
   {
      mStartupMessage = "Unable to start the MATLAB engine. "
         "Please check your platform, configuration, and license settings and try again.";
      return false;
   }

   // Suppress extra whitespace in results.
   executeCommandInMatlab("format compact");

   // Add the SupportFiles directory to the MATLAB path so that the internal command .m files can be located.
   // This ensures that help(internal_command) will function appropriately.
   const Filename* pSupportFiles = ConfigurationSettings::getSettingSupportFilesPath();
   if (pSupportFiles != NULL)
   {
      executeCommandInMatlab("path('" + pSupportFiles->getFullPathAndName() + "/MATLAB/', path)");
   }

   // Hide the command window on startup.
   hideCommandWindow();

   // Get the version number and description (e.g.: "Service Pack 1") for the startup message.
   std::string version;
   mStartupMessage = "MATLAB";
   if (getMatlabVariableAsString("version", version) == true && version.empty() == false)
   {
      mStartupMessage.append(" " + version);
   }

   if (MatlabInterpreter::getSettingInteractiveAvailable() == false)
   {
      mStartupMessage.append("\nThe ability to type MATLAB commands into the Scripting Window has been disabled.");
   }

   return isMatlabRunning();
}

std::string MatlabInterpreterEngine::getPrompt() const
{
   const unsigned int depth = mParser.getCommandDepth() + mParser.getCommentDepth();
   return depth > 0 ? std::string(depth * 3, '.') : ">> ";
}

bool MatlabInterpreterEngine::executeScopedCommand(const std::string& command, const Slot& output, const Slot& error, Progress* pProgress)
{
   if (++mScopedCommandDepth == 1)
   {
      attach(SIGNAL_NAME(MatlabInterpreterEngine, ScopedOutputText), output);
      attach(SIGNAL_NAME(MatlabInterpreterEngine, ScopedErrorText), error);
   }

   bool retValue = executeCommand(command);

   if (--mScopedCommandDepth == 0)
   {
      detach(SIGNAL_NAME(MatlabInterpreterEngine, ScopedErrorText), error);
      detach(SIGNAL_NAME(MatlabInterpreterEngine, ScopedOutputText), output);
   }

   return retValue;
}

bool MatlabInterpreterEngine::executeCommand(const std::string& command)
{
   // The parseLine method does not handle commands spanning multiple lines.
   // Split the lines here, returning immediately if an error occurs.
   QStringList commandList = QString::fromStdString(command).split("\n");
   foreach(QString currentCommand, commandList)
   {
      // Parse the input to determine if there is a command to run in MATLAB.
      std::string output;
      bool outputIsError = false;
      mCurrentCommand = currentCommand.toStdString();
      std::string actualCommand = mParser.parseLine(*this, mCurrentCommand, output, outputIsError);
      mCurrentCommand.clear();
      if (outputIsError == true)
      {
         if (output.empty() == true)
         {
            output = "Unknown parse error.";
         }

         sendError(output);
         return false;
      }

      if (output.empty() == false)
      {
         sendOutput(output);
         output.clear();
      }

      // Run the command in MATLAB.
      if (actualCommand.empty() == false)
      {
         // Suppress output from actualCommand by appending a semicolon if the original command ended with one.
         if (currentCommand.endsWith(';') && actualCommand[actualCommand.size() - 1] != ';')
         {
            actualCommand += ';';
         }

         // This checks for output before returning, even in the case where there were errors.
         bool outputTruncated = false;
         bool success = executeCommandInMatlab(actualCommand, output, outputIsError, outputTruncated);
         if (outputIsError == true && output.empty() == true)
         {
            output = "Unknown MATLAB error.";
         }

         if (output.empty() == false)
         {
            outputIsError ? sendError(output) : sendOutput(output);
         }

         if (outputTruncated == true)
         {
            // Prepend a newline to the error to prevent it from showing up on the same line as the output.
            // Normally this is not a problem, however, since the output was truncated, it is unlikely that
            // the last character was a newline.
            sendError("\nThe previous output was truncated. Please increase the MATLAB output buffer size and try again.");
         }

         if (success == false)
         {
            sendError("MATLAB is no longer running.");
            return false;
         }

         if (outputIsError == true)
         {
            return false;
         }
      }
   }

   return true;
}

bool MatlabInterpreterEngine::isGlobalOutputShown() const
{
   return mGlobalOutputShown;
}

void MatlabInterpreterEngine::showGlobalOutput(bool newValue)
{
   mGlobalOutputShown = newValue;
}

const std::string& MatlabInterpreterEngine::getObjectType() const
{
   static std::string sType("MatlabInterpreterEngine");
   return sType;
}

bool MatlabInterpreterEngine::isKindOf(const std::string& className) const
{
   if (className == getObjectType())
   {
      return true;
   }

   return SubjectImp::isKindOf(className);
}

void MatlabInterpreterEngine::sendOutput(const std::string& text)
{
   if (text.empty())
   {
      return;
   }

   if (mScopedCommandDepth > 0)
   {
      notify(SIGNAL_NAME(MatlabInterpreterEngine, ScopedOutputText), text);
   }

   if (mScopedCommandDepth == 0|| mGlobalOutputShown)
   {
      notify(SIGNAL_NAME(Interpreter, OutputText), text);
   } 
}

void MatlabInterpreterEngine::sendError(const std::string& text)
{
   if (text.empty())
   {
      return;
   }

   if (mScopedCommandDepth > 0)
   {
      notify(SIGNAL_NAME(MatlabInterpreterEngine, ScopedErrorText), text);
   }

   if (mScopedCommandDepth == 0 || mGlobalOutputShown)
   {
      notify(SIGNAL_NAME(Interpreter, ErrorText), text);
   } 
}

bool MatlabInterpreterEngine::executeCommandInMatlab(const std::string& command)
{
   return engOutputBuffer(mpMatlabEngine, NULL, 0) == 0 &&
      engEvalString(mpMatlabEngine, command.c_str()) == 0;
}

bool MatlabInterpreterEngine::executeCommandInMatlab(const std::string& command, std::string& output,
   bool& outputIsError, bool& outputTruncated)
{
   // Setup the output buffer.
   // Check the buffer size before running each command so settings do not require a restart.
   // The mOutputBuffer is only a member to avoid dynamic memory allocation on the heap.
   int outputBufferSize = MatlabInterpreter::getSettingOutputBufferSize();
   if (outputBufferSize > 0 && mOutputBuffer.size() != outputBufferSize)
   {
      try
      {
         // This can throw if there is not enough memory available.
         mOutputBuffer.resize(outputBufferSize);
      }
      catch (const std::bad_alloc&)
      {
         outputBufferSize = 0;
      }

      if (outputBufferSize == 0)
      {
         QString errorMessage = QString("Unable to allocate buffer of %1 bytes").arg(
            MatlabInterpreter::getSettingOutputBufferSize());
         sendError(errorMessage.toStdString());
      }
   }

   if (outputBufferSize <= 0)
   {
      mOutputBuffer.clear();
      engOutputBuffer(mpMatlabEngine, NULL, 0);
   }
   else
   {
      // Always initialize the buffer to all NULL characters since MATLAB does not NULL-terminate its output.
      memset(&mOutputBuffer[0], 0, outputBufferSize);
      engOutputBuffer(mpMatlabEngine, &mOutputBuffer[0], outputBufferSize);
   }

   // Run the command.
   int retVal = engEvalString(mpMatlabEngine, command.c_str());

   // Gather the output from the buffer.
   if (mOutputBuffer.empty() == true || mOutputBuffer[0] == 0)
   {
      // Either no buffer is being used or there was no output, so return an empty string.
      output = std::string();
      outputTruncated = false;
   }
   else
   {
      // Check whether the command filled the buffer.
      if (mOutputBuffer.back() != 0)
      {
         mOutputBuffer.back() = 0;
         outputTruncated = true;
      }
      else
      {
         outputTruncated = false;
      }

      // Copy the contents of the output buffer into the string.
      output.assign(&mOutputBuffer[0]);
   }

   // Check whether the command resulted in an error.
   outputIsError = false;
   if (retVal == 0 && mOutputBuffer.empty() == false && MatlabInterpreter::getSettingCheckErrors() == true)
   {
      memset(&mOutputBuffer[0], 0, outputBufferSize);
      if (engEvalString(mpMatlabEngine, "lasterror") == 0)
      {
         // Check whether the command filled the buffer.
         bool errorTruncated = false;
         if (mOutputBuffer.back() != 0)
         {
            mOutputBuffer.back() = 0;
            errorTruncated = true;
         }

         // Copy the contents of the output buffer into the string.
         QString error(&mOutputBuffer[0]);

         // Check for the message. Note: messageIndex may be -1 if the user has reassigned the lasterror variable.
         int messageIndex = error.indexOf(QRegExp("message:"));
         if (messageIndex >= 0)
         {
            int startIndex = error.indexOf('\'', messageIndex);
            if (startIndex >= 0)
            {
               int endIndex = error.indexOf('\'', startIndex + 1);
               if (endIndex >= 0)
               {
                  int length = endIndex - (startIndex + 1);
                  if (length > 0)
                  {
                     error = error.mid(startIndex + 1, length).trimmed();
                     if (error.isEmpty() == false)
                     {
                        output = error.toStdString();
                        outputIsError = true;
                        outputTruncated = errorTruncated;
                     }
                  }
               }
            }
         }
      }
   }

   if (retVal == 0 && MatlabInterpreter::getSettingClearErrors() == true)
   {
      engEvalString(mpMatlabEngine, "lasterror('reset');");
   }

   return retVal == 0;
}

bool MatlabInterpreterEngine::hideCommandWindow()
{
   return engSetVisible(mpMatlabEngine, false) == 0;
}

bool MatlabInterpreterEngine::showCommandWindow()
{
   return engSetVisible(mpMatlabEngine, true) == 0;
}

bool MatlabInterpreterEngine::getMatlabVariableAsString(const std::string& name, std::string& value)
{
   std::string output;
   bool outputIsError = false;
   bool outputTruncated = false;
   if (executeCommandInMatlab(name, output, outputIsError, outputTruncated) == false ||
      outputIsError == true || outputTruncated == true || output.empty() == true)
   {
      return false;
   }

   std::string isCharOutput;
   if (executeCommandInMatlab("ischar(" + name + ")", isCharOutput, outputIsError, outputTruncated) == false ||
      outputIsError == true || outputTruncated == true || isCharOutput.empty() == true)
   {
      return false;
   }

   // Massage the output into something more readable.
   //    - Remove leading and trailing spaces.
   //    - For all non-character types, remove intermediate spaces as well.
   QString formattedOutput = QString::fromStdString(output);
   formattedOutput = formattedOutput.remove('\n');
   isCharOutput = QString::fromStdString(isCharOutput).remove('\n').remove(' ').trimmed().toStdString();
   if (isCharOutput.find("ans=1") == std::string::npos)
   {
      formattedOutput = formattedOutput.remove(' ');
   }
   else
   {
      // If it is a string, check whether it is empty.
      // MATLAB outputs '' for empty strings, but that is not safe to check since a string containing
      // two apostrophes ('''''') is equivalent once whitespace has been removed.
      std::string isEmptyOutput;
      if (executeCommandInMatlab("isempty(" + name + ")", isEmptyOutput, outputIsError, outputTruncated) == false ||
         outputIsError == true || outputTruncated == true || isEmptyOutput.empty() == true)
      {
         return false;
      }

      isEmptyOutput = QString::fromStdString(isEmptyOutput).remove('\n').remove(' ').trimmed().toStdString();
      if (isEmptyOutput.find("ans=1") != std::string::npos)
      {
         value.clear();
         return true;
      }
   }

   formattedOutput = formattedOutput.trimmed();
   value = formattedOutput.toStdString();
   std::string::size_type pos = value.find('=');
   if (pos != std::string::npos)
   {
      value = value.substr(pos + 1, std::string::npos);
   }

   return true;
}

mxArray* MatlabInterpreterEngine::getMatlabVariable(const std::string& name)
{
   return engGetVariable(mpMatlabEngine, name.c_str());
}

bool MatlabInterpreterEngine::setMatlabVariable(const std::string& name, const mxArray* pArray)
{
   return engPutVariable(mpMatlabEngine, name.c_str(), pArray) == 0;
}
