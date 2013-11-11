/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "ArrayCommands.h"
#include "MatlabFunctions.h"
#include "MatlabInterpreter.h"
#include "RasterDataDescriptor.h"
#include "RasterElement.h"
#include "StringUtilities.h"
#include "switchOnEncoding.h"

#include <matrix.h>

#include <QtCore/QString>

namespace
{
   mxClassID getMxClassFromEncodingType(EncodingType type)
   {
      switch (type)
      {
         case INT1SBYTE:
            return mxINT8_CLASS;
         case INT1UBYTE:
            return mxUINT8_CLASS;
         case INT2SBYTES:
            return mxINT16_CLASS;
         case INT2UBYTES:
            return mxUINT16_CLASS;
         case INT4SBYTES:
            return mxINT32_CLASS;
         case INT4UBYTES:
            return mxUINT32_CLASS;
         case FLT4BYTES:
            return mxSINGLE_CLASS;
         case FLT8BYTES:
            return mxDOUBLE_CLASS;
         default:
            return mxUNKNOWN_CLASS;
      }
   }

   EncodingType getEncodingTypeFromMxClass(mxClassID classId)
   {
      switch (classId)
      {
         case mxINT8_CLASS:
            return INT1SBYTE;
         case mxUINT8_CLASS:
            return INT1UBYTE;
         case mxINT16_CLASS:
            return INT2SBYTES;
         case mxUINT16_CLASS:
            return INT2UBYTES;
         case mxINT32_CLASS:
            return INT4SBYTES;
         case mxUINT32_CLASS:
            return INT4UBYTES;
         case mxSINGLE_CLASS:
            return FLT4BYTES;
         case mxDOUBLE_CLASS:
            return FLT8BYTES;
         default:
            return EncodingType();
      }
   }
}

// ArraySizeCommand
ArraySizeCommand::ArraySizeCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string ArraySizeCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string datasetName = getOrDefault(strCmds, 1);
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   RasterElement* pRasterElement = dynamic_cast<RasterElement*>(MatlabFunctions::getDataset(datasetName));
   if (pRasterElement == NULL)
   {
      outputIsError = true;
      output = "Unable to find a dataset";
      return std::string();
   }

   RasterDataDescriptor* pDescriptor = dynamic_cast<RasterDataDescriptor*>(pRasterElement->getDataDescriptor());
   if (pDescriptor == NULL)
   {
      outputIsError = true;
      output = "Unable to find a data descriptor";
      return std::string();
   }

   outputIsError = false;
   return varName + "[" + QString::number(pDescriptor->getRowCount()).toStdString() + ", " +
      QString::number(pDescriptor->getColumnCount()).toStdString() + ", " +
      QString::number(pDescriptor->getBandCount()).toStdString() + "]";
}

// ArrayToMatlabCommand
ArrayToMatlabCommand::ArrayToMatlabCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string ArrayToMatlabCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string arrayName = getOrDefault(strVars, 0, "raster");
   std::string startColumnValue = getOrDefault(strCmds, 1, "0");
   std::string stopColumnValue = getOrDefault(strCmds, 2, "0");
   std::string startRowValue = getOrDefault(strCmds, 3, "0");
   std::string stopRowValue = getOrDefault(strCmds, 4, "0");
   std::string startBandValue = getOrDefault(strCmds, 5, "0");
   std::string stopBandValue = getOrDefault(strCmds, 6, "0");
   std::string rasterName = getOrDefault(strCmds, 7);

   bool ok = true;
   unsigned int startColumn = QString::fromStdString(startColumnValue).toUInt(&ok);
   if (ok == false)
   {
      outputIsError = true;
      output = "Unable to determine the requested start column";
      return std::string();
   }

   unsigned int stopColumn = QString::fromStdString(stopColumnValue).toUInt(&ok);
   if (ok == false)
   {
      outputIsError = true;
      output = "Unable to determine the requested stop column";
      return std::string();
   }

   unsigned int startRow = QString::fromStdString(startRowValue).toUInt(&ok);
   if (ok == false)
   {
      outputIsError = true;
      output = "Unable to determine the requested start row";
      return std::string();
   }

   unsigned int stopRow = QString::fromStdString(stopRowValue).toUInt(&ok);
   if (ok == false)
   {
      outputIsError = true;
      output = "Unable to determine the requested stop row";
      return std::string();
   }

   unsigned int startBand = QString::fromStdString(startBandValue).toUInt(&ok);
   if (ok == false)
   {
      outputIsError = true;
      output = "Unable to determine the requested start band";
      return std::string();
   }

   unsigned int stopBand = QString::fromStdString(stopBandValue).toUInt(&ok);
   if (ok == false)
   {
      outputIsError = true;
      output = "Unable to determine the requested stop band";
      return std::string();
   }

   RasterElement* pRasterElement = dynamic_cast<RasterElement*>(MatlabFunctions::getDataset(rasterName));
   if (pRasterElement == NULL)
   {
      outputIsError = true;
      output = "Unable to find a dataset";
      return std::string();
   }

   RasterDataDescriptor* pDescriptor = dynamic_cast<RasterDataDescriptor*>(pRasterElement->getDataDescriptor());
   if (pDescriptor == NULL)
   {
      outputIsError = true;
      output = "Unable to find a data descriptor";
      return std::string();
   }

   if (stopRow == 0)
   {
      stopRow = pDescriptor->getRowCount() - 1;
   }

   if (stopColumn == 0)
   {
      stopColumn = pDescriptor->getColumnCount() - 1;
   }

   if (stopBand == 0)
   {
      stopBand = pDescriptor->getBandCount() - 1;
   }

   // Validate values.
   if (startRow > stopRow)
   {
      std::swap(startRow, stopRow);
   }

   if (startColumn > stopColumn)
   {
      std::swap(startColumn, stopColumn);
   }

   if (startBand > stopBand)
   {
      std::swap(startBand, stopBand);
   }

   // Clamp to maximum values.
   stopRow = std::min(stopRow, pDescriptor->getRowCount() - 1);
   stopColumn = std::min(stopColumn, pDescriptor->getColumnCount() - 1);
   stopBand = std::min(stopBand, pDescriptor->getBandCount() - 1);

   // Create the mxArray which will hold the copy of data for MATLAB.
   mwSize nDims = (startBand == stopBand ? 2 : 3);
   mwSize dims[3];
   dims[0] = stopRow - startRow + 1;
   dims[1] = stopColumn - startColumn + 1;
   dims[2] = stopBand - startBand + 1;

   mxClassID classId = getMxClassFromEncodingType(pDescriptor->getDataType());
   if (classId == mxUNKNOWN_CLASS)
   {
      outputIsError = true;
      output = "Unsupported data type.";
      return std::string();
   }

   mxArray* pArray = mxCreateNumericArray(nDims, dims, classId, mxREAL);
   if (pArray == NULL)
   {
      outputIsError = true;
      output = "Unable to allocate enough memory to copy the data to MATLAB.";
      return std::string();
   }

   void* pArrayData = mxGetData(pArray);
   if (pArrayData == NULL)
   {
      mxDestroyArray(pArray);
      outputIsError = true;
      output = "Unable to copy data.";
      return std::string();
   }

   // Copy the data. This does not call getRawData because the data must be transposed during the copy
   // to account for MATLAB's column-major nature.
   std::string error;
   switchOnEncoding(pDescriptor->getDataType(), MatlabFunctions::arrayToMatlab, pArrayData, error, pRasterElement,
      startRow, stopRow, startColumn, stopColumn, startBand, stopBand);
   if (error.empty() == false)
   {
      mxDestroyArray(pArray);
      outputIsError = true;
      output = error;
      return std::string();
   }

   if (matlabInterpreter.setMatlabVariable(arrayName, pArray) == false)
   {
      mxDestroyArray(pArray);
      outputIsError = true;
      output = "Unable to set the MATLAB variable.";
      return std::string();
   }

   // This command uses mxArray to put the variable into the environment, so there is no need to return anything in the
   // std::string() since MATLAB does not need to evaluate anything more. This also avoids  potentially printing out the
   // entire raster element to MATLAB's output, which could take a very long time.
   mxDestroyArray(pArray);
   outputIsError = false;
   return std::string();
}

// ArrayToOpticksCommand
ArrayToOpticksCommand::ArrayToOpticksCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string ArrayToOpticksCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(matlab_array, opt:parent_raster_name, opt:display_results, "
         "opt:new_window, opt:interleave, opt:units)";
      return std::string();
   }

   std::string arrayName = getOrDefault(strCmds, 1);
   std::string rasterName = getOrDefault(strCmds, 2);
   std::string displayResultsValue = getOrDefault(strCmds, 3, "1");
   std::string newWindowValue = getOrDefault(strCmds, 4, displayResultsValue);
   std::string interleaveValue = getOrDefault(strCmds, 5, "bsq");
   std::string units = getOrDefault(strCmds, 6);

   bool error = false;
   bool displayResults = StringUtilities::fromDisplayString<bool>(displayResultsValue, &error);
   if (error == true)
   {
      outputIsError = true;
      output = "Unable to determine whether to display results";
      return std::string();
   }

   bool newWindow = StringUtilities::fromDisplayString<bool>(newWindowValue, &error);
   if (error == true)
   {
      outputIsError = true;
      output = "Unable to determine whether to create a window";
      return std::string();
   }

   if (displayResults == false && newWindow == true)
   {
      outputIsError = true;
      output = "Unable to create a new window without also displaying the results";
      return std::string();
   }

   InterleaveFormatType interleave = MatlabFunctions::getInterleaveByName(interleaveValue);
   if (interleave.isValid() == false)
   {
      outputIsError = true;
      output = "Unable to determine the requested interleave";
      return std::string();
   }

   mxArray* pArray = matlabInterpreter.getMatlabVariable(arrayName);
   if (pArray == NULL)
   {
      outputIsError = true;
      output = "Unable to get the MATLAB variable";
      return std::string();
   }

   const mwSize* varSz = mxGetDimensions(pArray);
   if (varSz == NULL)
   {
      mxDestroyArray(pArray);
      outputIsError = true;
      output = "Unable to find the size of the specified array";
      return std::string();
   }

   mwSize numDims = mxGetNumberOfDimensions(pArray);
   if (numDims < 1)
   {
      mxDestroyArray(pArray);
      outputIsError = true;
      output = "Unable to find the number of dimensions of the specified array";
      return std::string();
   }

   const unsigned int rows = varSz[0];
   const unsigned int columns = numDims >= 2 ? varSz[1] : 1;
   const unsigned int bands = numDims >= 3 ? varSz[2] : 1;

   mxClassID classId = mxGetClassID(pArray);
   EncodingType type = getEncodingTypeFromMxClass(classId);
   if (type.isValid() == false)
   {
      mxDestroyArray(pArray);
      outputIsError = true;
      output = "Unsupported data type";
      return std::string();
   }

   std::string errorMessage;
   switchOnEncoding(type, MatlabFunctions::arrayToOpticks, mxGetData(pArray), errorMessage,
      arrayName, columns, rows, bands, units, type, interleave, true, rasterName, displayResults, newWindow);
   mxDestroyArray(pArray);
   if (errorMessage.empty() == false)
   {
      outputIsError = true;
      output = errorMessage;
      return std::string();
   }

   return std::string();
}
