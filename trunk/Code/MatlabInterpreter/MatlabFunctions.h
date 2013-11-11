/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef MATLABFUNCTIONS_H
#define MATLABFUNCTIONS_H

#include "DataAccessor.h"
#include "DataAccessorImpl.h"
#include "DataRequest.h"
#include "DesktopServices.h"
#include "ObjectResource.h"
#include "RasterDataDescriptor.h"
#include "RasterElement.h"
#include "RasterLayer.h"
#include "RasterUtilities.h"
#include "SpatialDataView.h"
#include "SpatialDataWindow.h"
#include "StringUtilities.h"
#include "TypesFile.h"
#include "Undo.h"
#include "Units.h"

#include <matrix.h>

#include <string>
#include <vector>

class DataElement;
class Layer;
class WizardObject;

// Largely copied from the existing IdlFunctions namespace.
namespace MatlabFunctions
{
   static std::vector<WizardObject*> spWizards;
   std::string toMatlabString(const std::string& value);
   std::string toMatlabString(const std::vector<std::string>& values);
   DataElement* getDataset(const std::string& name);
   Layer* getLayerByRaster(RasterElement* pParentElement);
   SpatialDataWindow* getWindowByRaster(RasterElement* pRasterElement);
   SpatialDataView* getViewByWindowName(const std::string& windowName);
   Layer* getLayerByName(const std::string& windowName,
      const std::string& layerName, bool onlyRasterLayers = true);
   Layer* getLayerByIndex(const std::string& windowName, int index);
   RasterChannelType getChannelByName(const std::string& channelName);
   DisplayMode getDisplayModeByName(const std::string& name);
   StretchType getStretchTypeByName(const std::string& name);
   RegionUnits getRegionUnitsByName(const std::string& name);
   AnimationCycle getAnimationCycleByName(const std::string& name);
   AnimationState getAnimationStateByName(const std::string& name);
   WindowType getWindowTypeByName(const std::string& name);
   InterleaveFormatType getInterleaveByName(const std::string& name);
   WizardObject* getWizardObject(const std::string& wizardName);
   DataVariant getWizardObjectValue(const WizardObject* pObject, const std::string& name);
   bool clearWizardObject(const std::string& wizardName);
   bool setWizardObjectValue(WizardObject* pObject, const std::string& name, const DataVariant& value);

   template<typename T>
   void arrayToOpticks(T* pArray, std::string& error, const std::string& name,
      unsigned int columnCount, unsigned int rowCount, unsigned int bandCount, const std::string& unit,
      EncodingType type, InterleaveFormatType interleave, bool inMemory, const std::string& filename,
      bool displayResults, bool newWindow)
   {
      if (pArray == NULL)
      {
         error = "no source data.";
         return;
      }

      // Get the specified view and sensor data.
      RasterElement* pParentElement = NULL;
      if (filename.empty() == false)
      {
         pParentElement = dynamic_cast<RasterElement*>(getDataset(filename));
      }

      //it doesn't exist, so we can make a new one
      ModelResource<RasterElement> pNewElement(RasterUtilities::createRasterElement(name,
         rowCount, columnCount, bandCount, type, interleave, inMemory, pParentElement));
      if (pNewElement.get() == NULL)
      {
         error = "Unable to create new raster element.";
         return;
      }

      RasterDataDescriptor* pDescriptor = dynamic_cast<RasterDataDescriptor*>(pNewElement->getDataDescriptor());
      if (pDescriptor == NULL)
      {
         error = "Unable to obtain the raster data descriptor.";
         return;
      }

      if (interleave == BSQ)
      {
         for (unsigned int band = 0; band < bandCount; ++band)
         {
            FactoryResource<DataRequest> pRequest;
            pRequest->setWritable(true);
            pRequest->setInterleaveFormat(BSQ);
            pRequest->setRows(pDescriptor->getActiveRow(0), pDescriptor->getActiveRow(rowCount - 1), 1);
            pRequest->setColumns(pDescriptor->getActiveColumn(0), pDescriptor->getActiveColumn(columnCount - 1));
            pRequest->setBands(pDescriptor->getActiveBand(band), pDescriptor->getActiveBand(band), 1);
            DataAccessor daImage = pNewElement->getDataAccessor(pRequest.release());

            for (unsigned int row = 0; row < rowCount; ++row)
            {
               if (!daImage.isValid())
               {
                  error = "Unable to access the raster data.";
                  return;
               }

               T* pPixel = reinterpret_cast<T*>(daImage->getRow());
               for (unsigned int column = 0; column < columnCount; ++column)
               {
                  // Transpose the data during the copy.
                  unsigned int dataOffset = columnCount * rowCount * band + column * rowCount + row;
                  pPixel[column] = pArray[dataOffset];
               }

               daImage->nextRow();
            }
         }
      }
      else if (interleave == BIL)
      {
         FactoryResource<DataRequest> pRequest;
         pRequest->setWritable(true);
         pRequest->setInterleaveFormat(BIL);
         pRequest->setRows(pDescriptor->getActiveRow(0), pDescriptor->getActiveRow(rowCount - 1), 1);
         pRequest->setColumns(pDescriptor->getActiveColumn(0), pDescriptor->getActiveColumn(columnCount - 1));
         pRequest->setBands(pDescriptor->getActiveBand(0), pDescriptor->getActiveBand(bandCount - 1));
         DataAccessor daImage = pNewElement->getDataAccessor(pRequest.release());

         for (unsigned int row = 0; row < rowCount; ++row)
         {
            if (!daImage.isValid())
            {
               error = "Unable to access the raster data.";
               return;
            }

            T* pPixel = reinterpret_cast<T*>(daImage->getRow());
            for (unsigned int band = 0; band < bandCount; ++band)
            {
               for (unsigned int column = 0; column < columnCount; ++column)
               {
                  // Transpose the data during the copy.
                  unsigned int dataOffset = columnCount * rowCount * band + column * rowCount + row;
                  pPixel[column + band * columnCount] = pArray[dataOffset];
               }
            }

            daImage->nextRow();
         }
      }
      else if (interleave == BIP)
      {
         FactoryResource<DataRequest> pRequest;
         pRequest->setWritable(true);
         pRequest->setInterleaveFormat(BIP);
         pRequest->setRows(pDescriptor->getActiveRow(0), pDescriptor->getActiveRow(rowCount - 1), 1);
         pRequest->setColumns(pDescriptor->getActiveColumn(0), pDescriptor->getActiveColumn(columnCount - 1));
         pRequest->setBands(pDescriptor->getActiveBand(0), pDescriptor->getActiveBand(bandCount - 1));
         DataAccessor daImage = pNewElement->getDataAccessor(pRequest.release());

         for (unsigned int row = 0; row < rowCount; ++row)
         {
            if (!daImage.isValid())
            {
               error = "Unable to access the raster data.";
               return;
            }

            T* pPixel = reinterpret_cast<T*>(daImage->getRow());
            for (unsigned int column = 0; column < columnCount; ++column)
            {
               for (unsigned int band = 0; band < bandCount; ++band)
               {
                  // Transpose the data during the copy.
                  unsigned int dataOffset = columnCount * rowCount * band + column * rowCount + row;
                  pPixel[band + column * bandCount] = pArray[dataOffset];
               }
            }

            daImage->nextRow();
         }
      }
      else
      {
         error = "Unrecognized interleave.";
         return;
      }

      if (unit.empty() == false)
      {
         Units* pScale = pDescriptor->getUnits();
         if (pScale != NULL)
         {
            bool bError = false;
            UnitType uType = StringUtilities::fromDisplayString<UnitType>(unit, &bError);
            if (bError)
            {
               pScale->setUnitType(CUSTOM_UNIT);
            }
            else
            {
               pScale->setUnitType(uType);
            }
            pScale->setUnitName(unit);
            pDescriptor->setUnits(pScale);
         }
      }

      if (displayResults == true)
      {
         //the matrix is now created, so we should add it to the current view or create a new one
         //note that this will create a window when newWindow is false if there is no current window
         SpatialDataView* pView = NULL;
         if (newWindow == false)
         {
            WorkspaceWindow* pData = Service<DesktopServices>()->getCurrentWorkspaceWindow();
            if (pData != NULL)
            {
               pView = dynamic_cast<SpatialDataView*>(pData->getView());
            }
         }

         if (pView != NULL)
         {
            UndoLock undo(pView);
            pView->createLayer(RASTER, pNewElement.get(), name);
         }
         else
         {
            SpatialDataWindow* pWindow = static_cast<SpatialDataWindow*>(
               Service<DesktopServices>()->createWindow(name, SPATIAL_DATA_WINDOW));

            if (pWindow == NULL)
            {
               error = "Unable to create the window.";
               return;
            }

            SpatialDataView* pView = pWindow->getSpatialDataView();
            if (pView == NULL)
            {
               error = "Unable to create the view.";
               return;
            }

            UndoLock undo(pView);
            if (pView->setPrimaryRasterElement(pNewElement.get()) == false)
            {
               error = "Unable to set the primary raster element into the view";
               return;
            }

            RasterLayer* pLayer = static_cast<RasterLayer*>(pView->createLayer(RASTER, pNewElement.get()));
            if (pLayer == NULL)
            {
               error = "Unable to create the layer";
               return;
            }
         }
      }

      pNewElement.release();
   }

   template<typename T>
   void arrayToMatlab(T* pArray, std::string& error, RasterElement* pParentElement,
      unsigned int heightStart, unsigned int heightEnd,
      unsigned int widthStart, unsigned int widthEnd, unsigned int bandStart, unsigned int bandEnd)
   {
      unsigned int rowCount = heightEnd - heightStart + 1;
      unsigned int columnCount = widthEnd - widthStart + 1;
      unsigned int bandCount = bandEnd - bandStart + 1;

      if (pParentElement == NULL)
      {
         error = "No raster element provided";
         return;
      }

      RasterDataDescriptor* pDescriptor = dynamic_cast<RasterDataDescriptor*>(pParentElement->getDataDescriptor());
      for (unsigned int band = bandStart; band <= bandEnd; ++band)
      {
         FactoryResource<DataRequest> pRequest;
         pRequest->setInterleaveFormat(BSQ);
         pRequest->setRows(pDescriptor->getActiveRow(heightStart), pDescriptor->getActiveRow(heightEnd), 1);
         pRequest->setColumns(pDescriptor->getActiveColumn(widthStart), pDescriptor->getActiveColumn(widthEnd));
         pRequest->setBands(pDescriptor->getActiveBand(band), pDescriptor->getActiveBand(band), 1);
         DataAccessor daImage = pParentElement->getDataAccessor(pRequest.release());

         for (unsigned int row = 0; row < rowCount; ++row)
         {
            if (!daImage.isValid())
            {
               error = "error copying array values to MATLAB.";
               return;
            }

            T* pPixel = reinterpret_cast<T*>(daImage->getRow());
            for (unsigned int column = 0; column < columnCount; ++column)
            {
               // Transpose the data during the copy.
               unsigned int dataOffset = columnCount * rowCount * (band - bandStart) + column * rowCount + row;
               pArray[dataOffset] = pPixel[column];
            }

            daImage->nextRow();
         }
      }
   }
};

#endif
