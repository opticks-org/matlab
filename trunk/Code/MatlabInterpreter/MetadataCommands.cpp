/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "DynamicObject.h"
#include "MatlabFunctions.h"
#include "MatlabInterpreter.h"
#include "MetadataCommands.h"
#include "WizardObject.h"

#include <matrix.h>

#include <QtCore/QString>
#include <QtCore/QStringList>

namespace
{
   template<typename T>
   mxArray* createMxDoubleMatrixFromVector(const std::vector<T>& src)
   {
      mxArray* pArray = mxCreateDoubleMatrix(1, src.size(), mxREAL);
      if (pArray == NULL)
      {
         return NULL;
      }

      double* pDst = mxGetPr(pArray);
      if (pDst == NULL)
      {
         mxDestroyArray(pArray);
         return NULL;
      }

      for (std::vector<T>::const_iterator iter = src.begin(); iter != src.end(); ++iter)
      {
         *pDst = static_cast<double>(*iter);
         ++pDst;
      }

      return pArray;
   }

   template<typename T>
   DataVariant copyToDataVariant(size_t total, double* pValue)
   {
      T* pValReal = reinterpret_cast<T*>(pValue);
      if (total == 1)
      {
         return DataVariant(pValReal[0]);
      }
      else
      {
         std::vector<T> tmpVal;
         for (size_t i = 0; i < total; ++i)
         {
            tmpVal.push_back(pValReal[i]);
         }
         return DataVariant(tmpVal);
      }
   }

   template<>
   DataVariant copyToDataVariant<bool>(size_t total, double* pValue)
   {
      // special case since bool isn't always 1 byte
      if (total == 1)
      {
         return DataVariant((pValue[0] == 0) ? false : true);
      }
      else
      {
         std::vector<bool> tmpVal;
         for (size_t i = 0; i < total; ++i)
         {
            tmpVal.push_back((pValue[i] == 0) ? false : true);
         }
         return DataVariant(tmpVal);
      }
   }
}

// CopyMetadataCommand
CopyMetadataCommand::CopyMetadataCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string CopyMetadataCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1 || strCmds.size() == 2)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(source, destination)";
      return std::string();
   }

   std::string source = getOrDefault(strCmds, 1);
   std::string destination = getOrDefault(strCmds, 2);

   DataElement* pSourceElement = MatlabFunctions::getDataset(source);
   if (pSourceElement == NULL)
   {
      outputIsError = true;
      output = "Unable to locate source element.";
      return std::string();
   }

   DynamicObject* pSourceMetadata = pSourceElement->getMetadata();
   if (pSourceMetadata == NULL)
   {
      outputIsError = true;
      output = "Unable to locate source metadata.";
      return std::string();
   }

   DataElement* pDestinationElement = MatlabFunctions::getDataset(destination);
   if (pDestinationElement == NULL)
   {
      outputIsError = true;
      output = "Unable to locate destination element.";
      return std::string();
   }

   DynamicObject* pDestinationMetadata = pDestinationElement->getMetadata();
   if (pDestinationMetadata == NULL)
   {
      outputIsError = true;
      output = "Unable to locate destination metadata.";
      return std::string();
   }

   pDestinationMetadata->merge(pSourceMetadata);
   outputIsError = false;
   return std::string();
}

// GetMetadataCommand
GetMetadataCommand::GetMetadataCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetMetadataCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1 || strCmds.size() == 2 || strCmds.size() == 3)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(raster, metadata_attribute_name, matlab_variable_name, opt:wizard_file)";
      return std::string();
   }

   std::string rasterName = getOrDefault(strCmds, 1);
   std::string attributePath = getOrDefault(strCmds, 2);
   std::string arrayName = getOrDefault(strCmds, 3);
   std::string wizardName = getOrDefault(strCmds, 4);

#pragma message(__FILE__ "(" STRING(__LINE__) ") : warning : See OPTICKS-1517 (dadkins)")
   if (attributePath.empty() == true)
   {
      outputIsError = true;
      output = "No attribute path specified";
      return std::string();
   }

   // If a wizard name is present, ignore the raster name.
   DataVariant value;
   if (wizardName.empty() == true)
   {
      DataElement* pElement = MatlabFunctions::getDataset(rasterName);
      if (pElement == NULL)
      {
         outputIsError = true;
         output = "Unable to find the data element";
         return std::string();
      }

      DynamicObject* pDynamicObject = pElement->getMetadata();
      if (pDynamicObject == NULL)
      {
         // Firewall code.
         outputIsError = true;
         output = "Unable to find the metadata";
         return std::string();
      }

      value = pDynamicObject->getAttributeByPath(attributePath);
   }
   else
   {
      WizardObject* pWizardObject = MatlabFunctions::getWizardObject(wizardName);
      if (pWizardObject == NULL)
      {
         outputIsError = true;
         output = "Unable to find the wizard";
         return std::string();
      }

      value = MatlabFunctions::getWizardObjectValue(pWizardObject, attributePath);
   }

   if (value.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to find the specified attribute";
      return std::string();
   }

   // If no array name was specified, use the attributePath (correcting for invalid MATLAB characters).
   if (arrayName.empty() == true)
   {
      QString newArrayName = QString::fromStdString(attributePath);
      newArrayName.remove(' ');
      newArrayName.replace('/', '_');
      arrayName = newArrayName.toStdString();
   }

   mxArray* pArray = NULL;
   std::string conversionType;

   std::string valType = value.getTypeName();
   if (valType == "unsigned char")
   {
      conversionType.clear(); // Use default of type double.
      pArray = createMxDoubleMatrixFromVector(std::vector<unsigned char>(1, dv_cast<unsigned char>(value)));
   }
   else if (valType == "vector<unsigned char>")
   {
      conversionType.clear(); // Use default of type double.
      pArray = createMxDoubleMatrixFromVector(dv_cast<std::vector<unsigned char> >(value));
   }
   else if (valType == "char")
   {
      conversionType = "char";
      pArray = createMxDoubleMatrixFromVector(std::vector<char>(1, dv_cast<char>(value)));
   }
   else if (valType == "vector<char>")
   {
      conversionType = "char";
      pArray = createMxDoubleMatrixFromVector(dv_cast<std::vector<char> >(value));
   }
   else if (valType == "bool")
   {
      conversionType = "logical";
      pArray = createMxDoubleMatrixFromVector(std::vector<bool>(1, dv_cast<bool>(value)));
   }
   else if (valType == "vector<bool>")
   {
      conversionType = "logical";
      pArray = createMxDoubleMatrixFromVector<bool>(dv_cast<std::vector<bool> >(value));
   }
   else if (valType == "short")
   {
      conversionType = "int16";
      pArray = createMxDoubleMatrixFromVector(std::vector<short>(1, dv_cast<short>(value)));
   }
   else if (valType == "vector<short>")
   {
      conversionType = "int16";
      pArray = createMxDoubleMatrixFromVector(dv_cast<std::vector<short> >(value));
   }
   else if (valType == "unsigned short")
   {
      conversionType = "uint16";
      pArray = createMxDoubleMatrixFromVector(std::vector<unsigned short>(1, dv_cast<unsigned short>(value)));
   }
   else if (valType == "vector<unsigned short>")
   {
      conversionType = "uint16";
      pArray = createMxDoubleMatrixFromVector(dv_cast<std::vector<unsigned short> >(value));
   }
   else if (valType == "int")
   {
      conversionType = "int32";
      pArray = createMxDoubleMatrixFromVector(std::vector<int>(1, dv_cast<int>(value)));
   }
   else if (valType == "vector<int>")
   {
      conversionType = "int32";
      pArray = createMxDoubleMatrixFromVector(dv_cast<std::vector<int> >(value));
   }
   else if (valType == "unsigned int")
   {
      conversionType = "uint32";
      pArray = createMxDoubleMatrixFromVector(std::vector<unsigned int>(1, dv_cast<unsigned int>(value)));
   }
   else if (valType == "vector<unsigned int>")
   {
      conversionType = "uint32";
      pArray = createMxDoubleMatrixFromVector(dv_cast<std::vector<unsigned int> >(value));
   }
   else if (valType == "float")
   {
      conversionType = "single";
      pArray = createMxDoubleMatrixFromVector(std::vector<float>(1, dv_cast<float>(value)));
   }
   else if (valType == "vector<float>")
   {
      conversionType = "single";
      pArray = createMxDoubleMatrixFromVector(dv_cast<std::vector<float> >(value));
   }
   else if (valType == "double")
   {
      conversionType.clear(); // Use default of type double.
      pArray = createMxDoubleMatrixFromVector(std::vector<double>(1, dv_cast<double>(value)));
   }
   else if (valType == "vector<double>")
   {
      conversionType.clear(); // Use default of type double.
      pArray = createMxDoubleMatrixFromVector(dv_cast<std::vector<double> >(value));
   }
   else if (valType == "Filename")
   {
      conversionType.clear(); // Use default of type string.
      pArray = mxCreateString(dv_cast<Filename>(value).getFullPathAndName().c_str());
   }
   else if (valType == "vector<Filename>")
   {
      conversionType.clear(); // Use default of type string.
      std::vector<Filename*> vec = dv_cast<std::vector<Filename*> >(value);
      pArray = mxCreateCellMatrix(vec.size(), 1);
      for (std::vector<Filename*>::size_type idx = 0; idx < vec.size(); ++idx)
      {
         if (vec[idx] == NULL)
         {
            // Firewall code.
            mxDestroyArray(pArray);
            outputIsError = true;
            output = "Unable to copy Filename values -- internal error.";
            return std::string();
         }

         mxSetCell(pArray, idx, mxCreateString(vec[idx]->getFullPathAndName().c_str()));
      }
   }
   else if (valType == "string")
   {
      conversionType.clear(); // Use default of type string.
      pArray = mxCreateString(dv_cast<std::string>(value).c_str());
   }
   else if (valType == "vector<string>")
   {
      conversionType.clear(); // Use default of type string.
      std::vector<std::string> vec = dv_cast<std::vector<std::string> >(value);
      pArray = mxCreateCellMatrix(vec.size(), 1);
      for (std::vector<std::string>::size_type idx = 0; idx < vec.size(); ++idx)
      {
         mxSetCell(pArray, idx, mxCreateString(vec[idx].c_str()));
      }
   }
   else
   {
      // Unrecognized data type -- try a generic string conversion routine, and hope for the best.
      DataVariant::Status status;
      std::string val = value.toDisplayString(&status);
      if (status != DataVariant::SUCCESS)
      {
         outputIsError = true;
         output = "Unable to convert metadata of type " + valType + " to MATLAB.";
         return std::string();
      }

      conversionType.clear(); // Use default of type string.
      pArray = mxCreateString(val.c_str());
   }

   // An error occurred during allocation or copying.
   if (pArray == NULL)
   {
      outputIsError = true;
      output = "Unable to copy metadata to MATLAB.";
      return std::string();
   }

   // Set the variable in MATLAB.
   if (matlabInterpreter.setMatlabVariable(arrayName, pArray) == false)
   {
      mxDestroyArray(pArray);
      outputIsError = true;
      output = "Unable to set the MATLAB variable.";
      return std::string();
   }

   // If a conversion was requested, set that up as the MATLAB command to run.
   // If no conversion was requested, run arrayName as a command to get output in the output window.
   outputIsError = false;
   return conversionType.empty() ? arrayName : arrayName + " = " + conversionType + "(" + arrayName + ")";
}

// ReloadWizardCommand
ReloadWizardCommand::ReloadWizardCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string ReloadWizardCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string wizardName = getOrDefault(strCmds, 1);
   if (MatlabFunctions::clearWizardObject(wizardName) == false)
   {
      outputIsError = true;
      output = "Unable to reload the specified wizard.";
      return std::string();
   }

   outputIsError = false;
   return std::string();
}

// SetMetadataCommand
SetMetadataCommand::SetMetadataCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetMetadataCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1 || strCmds.size() == 2)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(metadata_attribute_name, matlab_variable_name, opt:force_bool_type, "
         "opt:force_filename_type, opt:wizard, opt:dataset)";
      return std::string();
   }

   std::string attributePath = getOrDefault(strCmds, 1);
   std::string arrayName = getOrDefault(strCmds, 2);
   std::string forceBoolValue = getOrDefault(strCmds, 3, "0");
   std::string forceFilenameValue = getOrDefault(strCmds, 4, "0");
   std::string wizardName = getOrDefault(strCmds, 5);
   std::string rasterName = getOrDefault(strCmds, 6);

#pragma message(__FILE__ "(" STRING(__LINE__) ") : warning : See OPTICKS-1517 (dadkins)")
   if (attributePath.empty() == true)
   {
      outputIsError = true;
      output = "No attribute path specified";
      return std::string();
   }

   bool error = false;
   bool forceBool = StringUtilities::fromDisplayString<bool>(forceBoolValue, &error);
   if (error == true)
   {
      outputIsError = true;
      output = "Unable to determine whether to force type bool";
      return std::string();
   }

   bool forceFilename = StringUtilities::fromDisplayString<bool>(forceFilenameValue, &error);
   if (error == true)
   {
      outputIsError = true;
      output = "Unable to determine whether to force type Filename";
      return std::string();
   }

   if (forceBool == true && forceFilename == true)
   {
      outputIsError = true;
      output = "Unable to force both type bool and type Filename at the same time";
      return std::string();
   }

   mxArray* pArray = matlabInterpreter.getMatlabVariable(arrayName);
   if (pArray == NULL)
   {
      outputIsError = true;
      output = "Unable to get the MATLAB variable";
      return std::string();
   }

   mwSize numDims = mxGetNumberOfDimensions(pArray);
   const mwSize* pDims = mxGetDimensions(pArray);
   if (numDims < 1 || pDims == NULL)
   {
      mxDestroyArray(pArray);
      outputIsError = true;
      output = "Unable to get the MATLAB variable size information";
      return std::string();
   }

   // Use the last dimension of n-dimensional data.
   unsigned int numValues = pDims[numDims - 1];

   double* pArrayData = mxGetPr(pArray);
   if (pArrayData == NULL)
   {
      mxDestroyArray(pArray);
      outputIsError = true;
      output = "Unable to get the MATLAB variable data";
      return std::string();
   }

   DataVariant value;
   if (forceBool == true)
   {
      value = copyToDataVariant<bool>(numValues, pArrayData);
   }
   else if (forceFilename == true)
   {
      // Shove it into a Filename object, even though it might not be a compatible type.
      FactoryResource<Filename> pFilename;
      pFilename->setFullPathAndName(reinterpret_cast<char*>(pArrayData));
      value = DataVariant(*pFilename.release());
   }
   else
   {
      mxClassID classId = mxGetClassID(pArray);
      switch (classId)
      {
         case mxLOGICAL_CLASS:
         {
            value = copyToDataVariant<bool>(numValues, pArrayData);
            break;
         }
         case mxINT8_CLASS:
         {
            value = copyToDataVariant<char>(numValues, pArrayData);
            break;
         }
         case mxUINT8_CLASS:
         {
            value = copyToDataVariant<unsigned char>(numValues, pArrayData);
            break;
         }
         case mxINT16_CLASS:
         {
            value = copyToDataVariant<short>(numValues, pArrayData);
            break;
         }
         case mxUINT16_CLASS:
         {
            value = copyToDataVariant<unsigned short>(numValues, pArrayData);
            break;
         }
         case mxINT32_CLASS:
         {
            value = copyToDataVariant<int>(numValues, pArrayData);
            break;
         }
         case mxUINT32_CLASS:
         {
            value = copyToDataVariant<unsigned int>(numValues, pArrayData);
            break;
         }
         case mxSINGLE_CLASS:
         {
            value = copyToDataVariant<float>(numValues, pArrayData);
            break;
         }
         case mxDOUBLE_CLASS:
         {
            value = copyToDataVariant<double>(numValues, pArrayData);
            break;
         }
         default:
         {
            mxDestroyArray(pArray);
            outputIsError = true;
            output = "Unsupported MATLAB data type";
            return std::string();
         }
      }
   }

   mxDestroyArray(pArray);
   if (value.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to copy MATLAB data";
      return std::string();
   }

   // If a wizard name is present, ignore the raster name.
   if (wizardName.empty() == true)
   {
      DataElement* pElement = MatlabFunctions::getDataset(rasterName);
      if (pElement == NULL)
      {
         outputIsError = true;
         output = "Unable to find the data element";
         return std::string();
      }

      DynamicObject* pDynamicObject = pElement->getMetadata();
      if (pDynamicObject == NULL)
      {
         // Firewall code.
         outputIsError = true;
         output = "Unable to find the metadata";
         return std::string();
      }

      if (pDynamicObject->adoptAttributeByPath(attributePath, value) == false)
      {
         outputIsError = true;
         output = "Unable to set the metadata";
         return std::string();
      }
   }
   else
   {
      WizardObject* pWizardObject = MatlabFunctions::getWizardObject(wizardName);
      if (pWizardObject == NULL)
      {
         outputIsError = true;
         output = "Unable to find the wizard";
         return std::string();
      }

      if (MatlabFunctions::setWizardObjectValue(pWizardObject, attributePath, value) == false)
      {
         outputIsError = true;
         output = "Unable to set wizard object value";
         return std::string();
      }
   }

   outputIsError = false;
   return std::string();
}
