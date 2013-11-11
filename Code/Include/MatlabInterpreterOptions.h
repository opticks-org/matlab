/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef MATLABINTERPRETEROPTIONS_H
#define MATLABINTERPRETEROPTIONS_H

#include "Filename.h"
#include "MatlabVersion.h"
#include "LabeledSectionGroup.h"

#include <string>
#include <vector>

class FileBrowser;
class QCheckBox;
class QComboBox;
class QRadioButton;
class QSpinBox;

class MatlabInterpreterOptions : public LabeledSectionGroup
{
   Q_OBJECT

public:

   MatlabInterpreterOptions();
   virtual ~MatlabInterpreterOptions();

   void setDll(const Filename* pDll);
   void setVersion(const QString& version);
   void applyChanges();

   static const std::string& getName()
   {
      static std::string var = "MATLAB Interpreter Options";
      return var;
   }

   static const std::string& getOptionName()
   {
      static std::string var = "Scripting/MATLAB Interpreter";
      return var;
   }

   static const std::string& getDescription()
   {
      static std::string var = "Configuration options for the MATLAB interpreter";
      return var;
   }

   static const std::string& getShortDescription()
   {
      static std::string var = "Configuration options for the MATLAB interpreter";
      return var;
   }

   static const std::string& getCreator()
   {
      static std::string var = "Ball Aerospace & Technologies Corp.";
      return var;
   }

   static const std::string& getCopyright()
   {
      static std::string var = MATLAB_COPYRIGHT;
      return var;
   }

   static const std::string& getVersion()
   {
      static std::string var = MATLAB_VERSION_NUMBER;
      return var;
   }

   static bool isProduction()
   {
      return MATLAB_IS_PRODUCTION_RELEASE;
   }

   static const std::string& getDescriptorId()
   {
      static std::string var = "{0092A5A1-5431-4798-BB34-98ED7A9D5556}";
      return var;
   }

private:
   QRadioButton* mpAutomaticConfiguration;
   FileBrowser* mpDll;
   QComboBox* mpVersion;
   QSpinBox* mpOutputBufferSize;
   QCheckBox* mpCheckErrors;
   QCheckBox* mpClearErrors;

   bool settingsRequireApplicationRestart() const;
};

#endif