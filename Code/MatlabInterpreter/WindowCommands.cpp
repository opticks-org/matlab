/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "DesktopServices.h"
#include "MatlabInterpreter.h"
#include "MatlabFunctions.h"
#include "RasterElement.h"
#include "SpatialDataWindow.h"
#include "View.h"
#include "Window.h"
#include "WindowCommands.h"
#include "WorkspaceWindow.h"

#include <QtGui/QWidget>

namespace
{
   Window* getWindow(const std::string& windowName, const std::string& windowType)
   {
      if (windowName.empty())
      {
         return Service<DesktopServices>()->getCurrentWorkspaceWindow();
      }

      return Service<DesktopServices>()->getWindow(windowName, MatlabFunctions::getWindowTypeByName(windowType));
   }
}

// CloseWindowCommand
CloseWindowCommand::CloseWindowCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string CloseWindowCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string windowName = getOrDefault(strCmds, 1);
   std::string windowType = getOrDefault(strCmds, 2, "spatialdatawindow");

   Window* pWindow = getWindow(windowName, windowType);
   if (pWindow == NULL)
   {
      outputIsError = true;
      output = "Unable to find the window";
      return std::string();
   }

   if (Service<DesktopServices>()->deleteWindow(pWindow) == false)
   {
      outputIsError = true;
      output = "Unable to close the window";
      return std::string();
   }

   outputIsError = false;
   return std::string();
}

// GetWindowLabelCommand
GetWindowLabelCommand::GetWindowLabelCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetWindowLabelCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string windowName = getOrDefault(strCmds, 1);
   std::string windowType = getOrDefault(strCmds, 2, "spatialdatawindow");
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   Window* pWindow = getWindow(windowName, windowType);
   if (pWindow == NULL)
   {
      outputIsError = true;
      output = "Unable to find the window";
      return std::string();
   }

   outputIsError = false;
   return varName + MatlabFunctions::toMatlabString(pWindow->getDisplayName(true));
}

// GetWindowNameCommand
GetWindowNameCommand::GetWindowNameCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetWindowNameCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string rasterName = getOrDefault(strCmds, 1);
   std::string varName = getOrDefault(strVars, 0);
   if (varName.empty() == false)
   {
      varName += " = ";
   }

   RasterElement* pElement = dynamic_cast<RasterElement*>(MatlabFunctions::getDataset(rasterName));
   if (pElement == NULL)
   {
      outputIsError = true;
      output = "Unable to find the raster element";
      return std::string();
   }

   SpatialDataWindow* pWindow = MatlabFunctions::getWindowByRaster(pElement);
   if (pWindow == NULL)
   {
      outputIsError = true;
      output = "Unable to find the window";
      return std::string();
   }

   outputIsError = false;
   return varName + MatlabFunctions::toMatlabString(pWindow->getName());
}

// GetWindowPositionCommand
GetWindowPositionCommand::GetWindowPositionCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string GetWindowPositionCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string windowName = getOrDefault(strCmds, 1);
   std::string windowType = getOrDefault(strCmds, 2, "spatialdatawindow");

   std::string xPosition = getOrDefault(strVars, 0, "pos_x");
   std::string yPosition = getOrDefault(strVars, 1, "pos_y");

   Window* pWindow = getWindow(windowName, windowType);
   if (pWindow == NULL)
   {
      outputIsError = true;
      output = "Unable to find the window";
      return std::string();
   }

   QWidget* pWidget = dynamic_cast<QWidget*>(pWindow);
   if (pWidget == NULL)
   {
      outputIsError = true;
      output = "Unable to find the widget";
      return std::string();
   }

   // Determine whether to use comma or semicolon for output.
   const char delim = QString::fromStdString(matlabInterpreter.getCurrentCommand()).endsWith(';') ? ';' : ',';

   outputIsError = false;
   output = xPosition + " = " + QString::number(pWidget->x()).toStdString() + delim +
      yPosition + " = " + QString::number(pWidget->y()).toStdString();
   return std::string();
}

// RefreshDisplayCommand
RefreshDisplayCommand::RefreshDisplayCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string RefreshDisplayCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   std::string rasterName = getOrDefault(strCmds, 1);

   RasterElement* pElement = dynamic_cast<RasterElement*>(MatlabFunctions::getDataset(rasterName));
   if (pElement == NULL)
   {
      outputIsError = true;
      output = "Unable to find the raster element";
      return std::string();
   }

   pElement->updateData();

   // Succeed even if no window or view is found because the raster updateData method was called successfully.
   SpatialDataWindow* pWindow = MatlabFunctions::getWindowByRaster(pElement);
   if (pWindow != NULL)
   {
      View* pView = pWindow->getView();
      if (pView != NULL)
      {
         pView->refresh();
      }
   }

   outputIsError = false;
   return std::string();
}

// SetCurrentWindowCommand
SetCurrentWindowCommand::SetCurrentWindowCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetCurrentWindowCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(raster)";
      return std::string();
   }

   std::string rasterName = getOrDefault(strCmds, 1);

   RasterElement* pElement = dynamic_cast<RasterElement*>(MatlabFunctions::getDataset(rasterName));
   if (pElement == NULL)
   {
      outputIsError = true;
      output = "Unable to find the raster element";
      return std::string();
   }

   SpatialDataWindow* pWindow = MatlabFunctions::getWindowByRaster(pElement);
   if (pWindow == NULL)
   {
      outputIsError = true;
      output = "Unable to find the window";
      return std::string();
   }

   if (Service<DesktopServices>()->setCurrentWorkspaceWindow(pWindow) == false)
   {
      outputIsError = true;
      output = "Unable to set the current window";
      return std::string();
   }

   outputIsError = false;
   return std::string();
}

// SetWindowLabelCommand
SetWindowLabelCommand::SetWindowLabelCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetWindowLabelCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(label, opt:window, opt:type)";
      return std::string();
   }

   std::string label = getOrDefault(strCmds, 1);
   std::string windowName = getOrDefault(strCmds, 2);
   std::string windowType = getOrDefault(strCmds, 3, "spatialdatawindow");

   Window* pWindow = getWindow(windowName, windowType);
   if (pWindow == NULL)
   {
      outputIsError = true;
      output = "Unable to find the window";
      return std::string();
   }

   pWindow->setName(label);
   outputIsError = false;
   return std::string();
}

// SetWindowPositionCommand
SetWindowPositionCommand::SetWindowPositionCommand(const std::string& name) :
   MatlabInternalCommand(name)
{}

std::string SetWindowPositionCommand::execute(MatlabInterpreter& matlabInterpreter,
   const std::vector<std::string>& strCmds, const std::vector<std::string>& strVars,
   std::string& output, bool& outputIsError)
{
   if (strCmds.size() == 1 || strCmds.size() == 2)
   {
      outputIsError = true;
      output = "Usage: " + strCmds[0] + "(posX, posY, opt:window, opt:type)";
      return std::string();
   }

   std::string xPosition = getOrDefault(strCmds, 1);
   std::string yPosition = getOrDefault(strCmds, 2);
   std::string windowName = getOrDefault(strCmds, 3);
   std::string windowType = getOrDefault(strCmds, 4);

   Window* pWindow = getWindow(windowName, windowType);
   if (pWindow == NULL)
   {
      outputIsError = true;
      output = "Unable to find the window";
      return std::string();
   }

   QWidget* pWidget = dynamic_cast<QWidget*>(pWindow);
   if (pWidget == NULL)
   {
      outputIsError = true;
      output = "Unable to find the widget";
      return std::string();
   }

   // Do not move the window unless it is in a normal state.
   // Doing so would cause the window to take focus, which could be problematic.
   if (pWidget->isMinimized() == true || pWidget->isMaximized() == true || pWidget->isFullScreen() == true)
   {
      outputIsError = true;
      output = "Unable to move a minimized, maximized, or fullscreen window";
      return std::string();
   }

   bool ok = true;
   int x = QString::fromStdString(xPosition).toInt(&ok);
   if (ok == false)
   {
      outputIsError = true;
      output = "Unable to determine the x position";
      return std::string();
   }

   int y = QString::fromStdString(yPosition).toInt(&ok);
   if (ok == false)
   {
      outputIsError = true;
      output = "Unable to determine the y position";
      return std::string();
   }

   pWidget->move(x, y);
   outputIsError = false;
   return std::string();
}
