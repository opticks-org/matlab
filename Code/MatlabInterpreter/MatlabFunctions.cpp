/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "DataElement.h"
#include "DesktopServices.h"
#include "Layer.h"
#include "LayerList.h"
#include "MatlabFunctions.h"
#include "ModelServices.h"
#include "RasterElement.h"
#include "RasterLayer.h"
#include "SpatialDataView.h"
#include "SpatialDataWindow.h"
#include "View.h"
#include "Window.h"
#include "WizardItem.h"
#include "WizardNode.h"
#include "WizardObject.h"
#include "xmlreader.h"

#include <QtCore/QString>
#include <QtCore/QStringList>

std::string MatlabFunctions::toMatlabString(const std::string& value)
{
   // Single quote characters are escaped by using two single quotes, and the entire name is wrapped in single quotes.
   // This is done so that controllers with single quotes in their names are displayed properly in MATLAB.
   QString valueTemp(QString::fromStdString(value));
   valueTemp = valueTemp.replace('\'', "\'\'");
   return '\'' + valueTemp.toStdString() + '\'';
}

std::string MatlabFunctions::toMatlabString(const std::vector<std::string>& values)
{
   QStringList valueList;
   for (std::vector<std::string>::const_iterator iter = values.begin(); iter != values.end(); ++iter)
   {
      valueList.append(QString::fromStdString(toMatlabString(*iter)));
   }

   return "char(" + valueList.join(", ").toStdString() + ')';
}

DataElement* MatlabFunctions::getDataset(const std::string& name)
{
   DataElement* pElement = NULL;
   if (name.empty())
   {
      SpatialDataWindow* pWindow = dynamic_cast<SpatialDataWindow*>(
         Service<DesktopServices>()->getCurrentWorkspaceWindow());
      SpatialDataView* pView = (pWindow == NULL) ? NULL : pWindow->getSpatialDataView();
      if (pView == NULL)
      {
         return NULL;
      }

      LayerList* pList = pView->getLayerList();
      pElement = (pList == NULL) ? NULL : pList->getPrimaryRasterElement();
   }
   else
   {
      QString str = QString::fromStdString(name);
      QStringList list = str.split(QString("=>"));

      bool first = true;
      Service<ModelServices> pModel;
      while (!list.isEmpty())
      {
         QString partName = list.front();
         list.pop_front();

         pElement = pModel->getElement(partName.toStdString(), "", pElement);
         if (pElement == NULL && first)
         {
            //the element was not found to be top level
            //check for an element with the => in the name
            std::vector<DataElement*> elements = pModel->getElements("");
            for (std::vector<DataElement*>::iterator element = elements.begin();
               element != elements.end(); ++element)
            {
               if ((*element) != NULL && ((*element)->getName() == name || (*element)->getDisplayName() == name))
               {
                  pElement = *element;
                  break;
               }
            }
         }
         first = false;
      }
   }
   return pElement;
}

SpatialDataWindow* MatlabFunctions::getWindowByRaster(RasterElement* pRasterElement)
{
   std::vector<Window*> windows;
   Service<DesktopServices>()->getWindows(SPATIAL_DATA_WINDOW, windows);

   for (std::vector<Window*>::const_iterator windowIter = windows.begin();
      windowIter != windows.end();
      ++windowIter)
   {
      SpatialDataWindow* pSpatialDataWindow = dynamic_cast<SpatialDataWindow*>(*windowIter);
      if (pSpatialDataWindow != NULL)
      {
         SpatialDataView* pSpatialDataView = dynamic_cast<SpatialDataView*>(pSpatialDataWindow->getView());
         if (pSpatialDataView != NULL)
         {
            LayerList* pLayerList = pSpatialDataView->getLayerList();
            if (pLayerList != NULL)
            {
               std::vector<Layer*> layers;
               pLayerList->getLayers(RASTER, layers);
               for (std::vector<Layer*>::const_iterator layerIter = layers.begin();
                  layerIter != layers.end();
                  ++layerIter)
               {
                  RasterLayer* pRasterLayer = dynamic_cast<RasterLayer*>(*layerIter);
                  if (pRasterLayer != NULL && pRasterLayer->getDataElement() == pRasterElement)
                  {
                     return pSpatialDataWindow;
                  }
               }
            }
         }
      }
   }
   return NULL;

}

Layer* MatlabFunctions::getLayerByRaster(RasterElement* pElement)
{
   VERIFYRV(pElement != NULL, NULL);
   Layer* pDatasetLayer = NULL;
   std::vector<Window*> windows;
   Service<DesktopServices>()->getWindows(SPATIAL_DATA_WINDOW, windows);

   for (std::vector<Window*>::const_iterator windowIter = windows.begin();
      windowIter != windows.end(); ++windowIter)
   {
      SpatialDataWindow* pSpatialDataWindow = dynamic_cast<SpatialDataWindow*>(*windowIter);
      if (pSpatialDataWindow != NULL)
      {
         SpatialDataView* pSpatialDataView = dynamic_cast<SpatialDataView*>(pSpatialDataWindow->getView());
         VERIFYRV(pSpatialDataView != NULL, NULL);

         LayerList* pLayerList = pSpatialDataView->getLayerList();
         VERIFYRV(pLayerList != NULL, NULL);

         // Retrieve all the raster layers in the current window
         std::vector<Layer*> layers;
         pLayerList->getLayers(RASTER, layers);
         for (std::vector<Layer*>::const_iterator layerIter = layers.begin();
            layerIter != layers.end(); ++layerIter)
         {
            RasterLayer* pRasterLayer = dynamic_cast<RasterLayer*>(*layerIter);
            VERIFYRV(pRasterLayer != NULL, NULL);

            DataElement* pDataElement = pRasterLayer->getDataElement();
            if (pDataElement == pElement)
            {
               pDatasetLayer = pRasterLayer;
               break;
            }
         }
      }
   }
   return pDatasetLayer;
}

SpatialDataView* MatlabFunctions::getViewByWindowName(const std::string& windowName)
{
   Service<DesktopServices> pDesktop;
   SpatialDataWindow* pWindow = NULL;
   if (windowName.empty())
   {
      pWindow = dynamic_cast<SpatialDataWindow*>(pDesktop->getCurrentWorkspaceWindow());
   }
   else 
   {
      pWindow = dynamic_cast<SpatialDataWindow*>(pDesktop->getWindow(windowName, SPATIAL_DATA_WINDOW));
   }

   if (pWindow == NULL)
   {
      return NULL;
   }

   return pWindow->getSpatialDataView();
}

Layer* MatlabFunctions::getLayerByName(const std::string& windowName,
   const std::string& layerName, bool onlyRasterLayers)
{
   SpatialDataView* pView = dynamic_cast<SpatialDataView*>(getViewByWindowName(windowName));
   if (pView == NULL)
   {
      return NULL;
   }

   LayerList* pList = pView->getLayerList();
   if (pList == NULL)
   {
      return NULL;
   }

   if (layerName.empty() == true)
   {
      if (onlyRasterLayers)
      {
         return pView->getTopMostLayer(RASTER);
      }

      return pView->getTopMostLayer();
   }

   std::vector<Layer*> layers;
   if (onlyRasterLayers)
   {
      layers = pList->getLayers(RASTER);
   }
   else
   {
      pList->getLayers(layers);
   }

   for (std::vector<Layer*>::const_iterator iter = layers.begin(); iter != layers.end(); ++iter)
   {
      Layer* pLayer = *iter;
      if (pLayer != NULL && (layerName == pLayer->getName() || layerName == pLayer->getDisplayName()))
      {
         return pLayer;
      }
   }

   return NULL;
}

Layer* MatlabFunctions::getLayerByIndex(const std::string& windowName, int index)
{
   SpatialDataView* pView = dynamic_cast<SpatialDataView*>(getViewByWindowName(windowName));
   if (pView == NULL)
   {
      return NULL;
   }

   // Get the current layer if the index is invalid.
   if (index < 0)
   {
      return pView->getTopMostLayer();
   }

   LayerList* pList = pView->getLayerList();
   if (pList == NULL)
   {
      return NULL;
   }

   std::vector<Layer*> layers;
   pList->getLayers(layers);
   if (static_cast<std::vector<Layer*>::size_type>(index) >= layers.size())
   {
      return NULL;
   }

   return layers[index];
}

RasterChannelType MatlabFunctions::getChannelByName(const std::string& name)
{
   QString nameLower = QString::fromStdString(name).toLower();
   if (nameLower == "gray" || nameLower == "grayscale" ||
      nameLower == "grey" || nameLower == "greyscale")
   {
      return GRAY;
   }

   if (nameLower == "red")
   {
      return RED;
   }

   if (nameLower == "green")
   {
      return GREEN;
   }

   if (nameLower == "blue")
   {
      return BLUE;
   }

   return RasterChannelType();
}

DisplayMode MatlabFunctions::getDisplayModeByName(const std::string& name)
{
   QString nameLower = QString::fromStdString(name).toLower();
   if (nameLower == "gray" || nameLower == "grayscale" ||
      nameLower == "grey" || nameLower == "greyscale")
   {
      return GRAYSCALE_MODE;
   }

   if (nameLower == "rgb")
   {
      return RGB_MODE;
   }

   return DisplayMode();
}

StretchType MatlabFunctions::getStretchTypeByName(const std::string& name)
{
   QString nameLower = QString::fromStdString(name).toLower();
   if (nameLower == "linear")
   {
      return LINEAR;
   }

   if (nameLower == "logarithmic")
   {
      return LOGARITHMIC;
   }

   if (nameLower == "exponential")
   {
      return EXPONENTIAL;
   }

   if (nameLower == "equalization")
   {
      return EQUALIZATION;
   }

   return StretchType();
}

RegionUnits MatlabFunctions::getRegionUnitsByName(const std::string& name)
{
   QString nameLower = QString::fromStdString(name).toLower();
   if (nameLower == "raw" || nameLower == "raw value")
   {
      return RAW_VALUE;
   }

   if (nameLower == "percentage")
   {
      return PERCENTAGE;
   }

   if (nameLower == "percentile")
   {
      return PERCENTILE;
   }

   if (nameLower == "stddev")
   {
      return STD_DEV;
   }

   return RegionUnits();
}

AnimationCycle MatlabFunctions::getAnimationCycleByName(const std::string& name)
{
   QString nameLower = QString::fromStdString(name).toLower();
   if (nameLower == "play_once" || nameLower == "play once")
   {
      return PLAY_ONCE;
   }

   if (nameLower == "bounce")
   {
      return BOUNCE;
   }

   if (nameLower == "repeat")
   {
      return REPEAT;
   }

   return AnimationCycle();
}

AnimationState MatlabFunctions::getAnimationStateByName(const std::string& name)
{
   QString nameLower = QString::fromStdString(name).toLower();
   if (nameLower == "stop")
   {
      return STOP;
   }

   if (nameLower == "play_forward")
   {
      return PLAY_FORWARD;
   }

   if (nameLower == "play_backward")
   {
      return PLAY_BACKWARD;
   }

   if (nameLower == "pause_forward")
   {
      return PAUSE_FORWARD;
   }

   if (nameLower == "pause_backward")
   {
      return PAUSE_BACKWARD;
   }

   return AnimationState();
}

WindowType MatlabFunctions::getWindowTypeByName(const std::string& name)
{
   QString nameLower = QString::fromStdString(name).toLower().remove(' ');
   if (nameLower == "workspacewindow")
   {
       return WORKSPACE_WINDOW;
   }

   if (nameLower == "spatialdatawindow")
   {
       return SPATIAL_DATA_WINDOW;
   }

   if (nameLower == "productwindow")
   {
       return PRODUCT_WINDOW;
   }

   if (nameLower == "dockwindow" || nameLower == "plotwindow")
   {
       return DOCK_WINDOW;
   }

   return WORKSPACE_WINDOW;
}

InterleaveFormatType MatlabFunctions::getInterleaveByName(const std::string& name)
{
   QString nameLower = QString::fromStdString(name).toLower().remove(' ');
   if (nameLower == "bsq")
   {
       return BSQ;
   }

   if (nameLower == "bil")
   {
       return BIL;
   }

   if (nameLower == "bip")
   {
       return BIP;
   }

   return InterleaveFormatType();
}

bool MatlabFunctions::setWizardObjectValue(WizardObject* pObject, const std::string& name, const DataVariant& value)
{
   if (pObject == NULL)
   {
      return false;
   }
   std::vector<std::string> parts = StringUtilities::split(name, '/');
   if (parts.size() != 2)
   {
      return false;
   }
   std::string itemName = parts[0];
   std::string nodeName = parts[1];

   const std::vector<WizardItem*>& items = pObject->getItems();
   for (std::vector<WizardItem*>::const_iterator item = items.begin(); item != items.end(); ++item)
   {
      if ((*item)->getName() == itemName)
      {
         WizardNode* pNode = (*item)->getOutputNode(nodeName, value.getTypeName());
         if (pNode != NULL)
         {
            pNode->setValue(value.getPointerToValueAsVoid());
            return true;
         }
         else if (value.getTypeName() == TypeConverter::toString<std::string>())
         {
            pNode = (*item)->getOutputNode(nodeName, TypeConverter::toString<Filename>());
            if (pNode != NULL)
            {
               FactoryResource<Filename> pFilename;
               pFilename->setFullPathAndName(dv_cast<std::string>(value));
               pNode->setValue(pFilename.release());
               return true;
            }
         }
         break;
      }
   }
   return false;
}

DataVariant MatlabFunctions::getWizardObjectValue(const WizardObject* pObject, const std::string& name)
{
   if (pObject == NULL)
   {
      return DataVariant();
   }
   std::vector<std::string> parts = StringUtilities::split(name, '/');
   if (parts.size() != 2)
   {
      return DataVariant();
   }
   std::string itemName = parts[0];
   std::string nodeName = parts[1];

   const std::vector<WizardItem*>& items = pObject->getItems();
   for (std::vector<WizardItem*>::const_iterator item = items.begin(); item != items.end(); ++item)
   {
      if ((*item)->getName() == itemName)
      {
         std::vector<WizardNode*> nodes = (*item)->getOutputNodes();
         for (std::vector<WizardNode*>::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
         {
            if ((*node)->getName() == nodeName)
            {
               return DataVariant((*node)->getType(), (*node)->getValue(), false);
            }
         }
      }
   }

   return DataVariant();
}

bool MatlabFunctions::clearWizardObject(const std::string& wizardName)
{
   if (wizardName.empty())
   {
      for (std::vector<WizardObject*>::iterator iter = spWizards.begin(); iter != spWizards.end(); ++iter)
      {
         FactoryResource<WizardObject> pWiz(*iter);
      }
      spWizards.clear();
      return true;
   }

   //traverse the list of items, looking for the one that matches the item name
   for (std::vector<WizardObject*>::iterator iter = spWizards.begin(); iter != spWizards.end(); ++iter)
   {
      WizardObject* pItem = *iter;
      if (pItem != NULL && pItem->getName() == wizardName)
      {
         FactoryResource<WizardObject> pWiz(pItem);
         spWizards.erase(iter);
         return true;
      }
   }
   return false;
}

WizardObject* MatlabFunctions::getWizardObject(const std::string& wizardName)
{
   WizardObject* pWizard = NULL;

   //traverse the list of items, looking for the one that matches the item name
   for (std::vector<WizardObject*>::const_iterator iter = spWizards.begin(); iter != spWizards.end(); ++iter)
   {
      WizardObject* pItem = *iter;
      if (pItem != NULL && pItem->getName() == wizardName)
      {
         pWizard = pItem;
         break;
      }
   }
   if (pWizard == NULL)
   {
      //the wizard object doesn't exist, read it from the file
      FactoryResource<WizardObject> pNewWizard;
      VERIFYRV(pNewWizard.get() != NULL, NULL);

      FactoryResource<Filename> pWizardFilename;
      pWizardFilename->setFullPathAndName(wizardName);

      bool bSuccess = false;
      XmlReader xml;
      XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* pDocument = xml.parse(pWizardFilename.get());
      if (pDocument != NULL)
      {
         XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* pRoot = pDocument->getDocumentElement();
         if (pRoot != NULL)
         {
            unsigned int version = atoi(A(pRoot->getAttribute(X("version"))));
            bSuccess = pNewWizard->fromXml(pRoot, version);
         }
      }

      if (bSuccess == false)
      {
         return NULL;
      }
      pWizard = pNewWizard.release();
      pWizard->setName(wizardName);
      spWizards.push_back(pWizard);
   }
   return pWizard;
}
