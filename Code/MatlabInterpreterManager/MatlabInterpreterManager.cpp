/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "AppConfig.h"
#include "ConfigurationSettings.h"
#include "DynamicModule.h"
#include "External.h"
#include "MatlabVersion.h"
#include "MatlabInterpreterManager.h"
#include "ObjectResource.h"
#include "PlugInManagerServices.h"
#include "PlugInRegistration.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QRegExp>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>

REGISTER_PLUGIN_BASIC(Matlab, MatlabInterpreterManager);

QStringList MatlabInterpreterManager::getAvailableMatlabInterpreterVersions()
{
   QDir interpreterDir(QString::fromStdString(Service<ConfigurationSettings>()->getPlugInPath() + "/MATLAB"));
   QStringList interpreters(interpreterDir.entryList(QStringList("MatlabInterpreter*.dll"), QDir::Files, QDir::Name));
   QStringList versions;
   foreach (QString interpreter, interpreters)
   {
      // Strip "MatlabInterpreter" and ".dll" from the name.
      versions.append(interpreter.mid(17, interpreter.length() - (17 + 4)));
   }

   return versions;
}

MatlabInterpreterManager::MatlabInterpreterManager() :
   mpApplicationServices(Service<ApplicationServices>().get(), SIGNAL_NAME(ApplicationServices, SessionClosed),
      Slot(this, &MatlabInterpreterManager::sessionClosed)),
   mSessionClosed(false),
   mpModule(NULL),
   mStartupMessage("Unknown error occurred during startup")
{
   setName("MATLAB");
   setDescription("Provides command line utilities to execute MATLAB commands.");
   setDescriptorId("{AC9FA3C8-754A-4FCD-9544-3200FD07ED72}");
   setCopyright(MATLAB_COPYRIGHT);
   setVersion(MATLAB_VERSION_NUMBER);
   setProductionStatus(MATLAB_IS_PRODUCTION_RELEASE);
   allowMultipleInstances(false);
   setWizardSupported(false);
   setFileExtensions("MATLAB Files (*.m)");
   setInteractiveEnabled(MatlabInterpreter::getSettingInteractiveAvailable());
   addDependencyCopyright("MATLAB", "<pre>Copyright 1994 The Mathworks, Inc.\n"
      "* The user is permitted to use this software only together with Opticks for the sole purpose of calling a "
      "fully-licensed copy of MATLAB(R) software. Any other use is expressly prohibited.</pre>");
}

MatlabInterpreterManager::~MatlabInterpreterManager()
{
   notify(SIGNAL_NAME(Subject, Deleted));
   unloadModule();
}

bool MatlabInterpreterManager::execute(PlugInArgList* pInArgList, PlugInArgList* pOutArgList)
{
   // Starting the interpreter here would cause a MATLAB license to be consumed during application start-up.
   // This can lock a MATLAB floating license even though the user is not using the MATLAB Scripting Tab, and they
   // are not executing a wizard with MATLAB code. Therefore, we are intentionally delaying start of the MATLAB
   // interpreter until the application requests it to be started.
   return true;
}

bool MatlabInterpreterManager::isStarted() const
{
   return mpInterpreter.get() != NULL && mpInterpreter->isMatlabRunning();
}

bool MatlabInterpreterManager::start()
{
   if (mSessionClosed == false && mpInterpreter.get() == NULL)
   {
      if (mpModule == NULL)
      {
         // Perform automatic configuration and update the path before attempting to load the module.
         QString error = prepareToLoadModule();
         if (error.isEmpty() == false)
         {
            mStartupMessage = error.toStdString();
            return false;
         }

         // Find the version of the MatlabInterpreter corresponding to the settings.
         std::string modulePath = Service<ConfigurationSettings>()->getPlugInPath() + "/MATLAB/MatlabInterpreter" +
            MatlabInterpreter::getSettingVersion() + ".dll";
         mpModule = Service<PlugInManagerServices>()->getDynamicModule(modulePath);
         if (mpModule == NULL)
         {
            mStartupMessage = "Unknown problem occurred loading " + modulePath + ". Please check that the file exists.";
            return false;
         }
      }

      MatlabInterpreter* (*init_matlab_interpreter)(External*) =
         reinterpret_cast<MatlabInterpreter* (*)(External*)>(mpModule->getProcedureAddress("init_matlab_interpreter"));
      if (init_matlab_interpreter == NULL)
      {
         if (mpModule->isLoaded())
         {
            // If the library is loaded, then there is no init_matlab_interpreter
            // function with the correct signature. This is probably a bad installation.
            mStartupMessage = "An invalid interpreter is being used. "
               "Please check your installation and try again.";
         }
         else
         {
            // The library could not be loaded, which probably means that the MATLAB libraries could
            // not be found. This is probably a machine without the required version of MATLAB.
            if (MatlabInterpreter::getSettingVersion().empty())
            {
               mStartupMessage = "No MATLAB version was set. Please check your settings and try again.";
            }
            else
            {
               mStartupMessage = "A registered installation of MATLAB " + MatlabInterpreter::getSettingVersion() +
                  " could not be located. Please check your settings and try again.";
            }
         }

         Service<PlugInManagerServices>()->destroyDynamicModule(mpModule);
         mpModule = NULL;
         return false;
      }
      else
      {
         External* pExternal = ModuleManager::instance()->getService();
         mpInterpreter.reset(init_matlab_interpreter(pExternal));
         if (mpInterpreter.get() == NULL)
         {
            // All the files look good, but the initialization still failed for some reason.
            mStartupMessage = "Unable to start the MATLAB interpreter.";
            Service<PlugInManagerServices>()->destroyDynamicModule(mpModule);
            mpModule = NULL;
            return false;
         }
      }
   }

   if (mpInterpreter.get() != NULL && mpInterpreter->isMatlabRunning() == false && mpInterpreter->startMatlab() == true)
   {
      notify(SIGNAL_NAME(InterpreterManager, InterpreterStarted));
   }

   return isStarted();
}

std::string MatlabInterpreterManager::getStartupMessage() const
{
   return mpInterpreter.get() == NULL ? mStartupMessage : mpInterpreter->getStartupMessage();
}

Interpreter* MatlabInterpreterManager::getInterpreter() const
{
   // Per the InterpreterManager documentation, only return the interpreter if it has been started.
   return isStarted() ? const_cast<MatlabInterpreter*>(mpInterpreter.get()) : NULL;
}

const std::string& MatlabInterpreterManager::getObjectType() const
{
   static std::string sType("MatlabInterpreterManager");
   return sType;
}

bool MatlabInterpreterManager::isKindOf(const std::string& className) const
{
   if (className == getObjectType())
   {
      return true;
   }

   return SubjectImp::isKindOf(className);
}

QString MatlabInterpreterManager::prepareToLoadModule()
{
   // Perform automatic configuration to set the version and path appropriately.
   if (MatlabInterpreter::getSettingAutomaticConfiguration() == true)
   {
      QString path = getPathForRegisteredMatlabServer();
      if (path.isEmpty() == true)
      {
         return "Unable to locate MATLAB COM server installation.\n"
         #if defined(_WIN64)
            "Try running \"matlab.exe -regserver\" from the command line.";
         #else
            "Please install either 64-bit Opticks or 32-bit MATLAB and try again.";
         #endif
      }

      QString version;
      QString directory;
      QString error = getMatlabVersionAndDirectoryFromPath(path, version, directory);
      if (error.isEmpty() == false)
      {
         return error;
      }

      FactoryResource<Filename> pPath;
      pPath->setFullPathAndName(directory.toStdString());
      MatlabInterpreter::setSettingDLL(pPath.get());
      MatlabInterpreter::setSettingVersion(version.toStdString());
   }

   // Change the path here to ensure that the interpreter plug-ins will load.
   // This is required in order to support multiple versions of MATLAB from a single AEB.
   // It is also important to note that this code prepends to the path cf. appending to it.
   // This is to ensure that the appropriate dll files will be loaded from the MATLAB directory
   // and not from some other place (e.g.: a different MATLAB installation).
   const Filename* pDll = MatlabInterpreter::getSettingDLL();
   if (pDll != NULL)
   {
      std::string dllPath = pDll->getPath();
      if (!dllPath.empty())
      {
         std::string oldPath;
         char* pPath = getenv("PATH");
         if (pPath != NULL)
         {
            oldPath = std::string(pPath);
         }

         std::string newPath = "PATH=" + dllPath;
         if (oldPath.empty() == false)
         {
            newPath.append(";" + oldPath);
         }

         _putenv(newPath.c_str());
      }
   }

   return QString();
}

QString MatlabInterpreterManager::getPathForRegisteredMatlabServer()
{
   // Try to determine the registered installation of MATLAB.
   // MATLAB uses the Microsoft Component Object Model (COM) when used from another process.
   // The MATLAB documentation for its COM integration is here:
   //    http://www.mathworks.com/help/matlab/matlab_external/introducing-matlab-com-integration.html
   // Microsoft COM documentation and the applicable registry information can be found in the following places:
   //    COM: http://msdn.microsoft.com/en-us/library/windows/desktop/ms680573%28v=vs.85%29.aspx
   //    COM Registry Keys: http://msdn.microsoft.com/en-us/library/windows/desktop/ms678477%28v=vs.85%29.aspx
   //    HKCR: http://msdn.microsoft.com/en-us/library/windows/desktop/ms724475%28v=vs.85%29.aspx
   //    CLSID: http://msdn.microsoft.com/en-us/library/windows/desktop/ms691424%28v=vs.85%29.aspx
   //    Wow6432Node: http://msdn.microsoft.com/en-us/library/windows/desktop/ms724072%28v=vs.85%29.aspx

   // The MATLAB documentation states that only one version of MATLAB can be registered at any given time
   // on a particular machine. To determine the current version, use the following algorithm:
   //    MATLAB_CLSID = HKEY_CLASSES_ROOT/Matlab.Application/CLSID/Default (GUID stored as a string)
   //    MATLAB_EXE = HKEY_CLASSES_ROOT/CLSID/MATLAB_CLSID/LocalServer32/Default (full name and path of MATLAB)
   //    if MATLAB_EXE is empty
   //       MATLAB_EXE = HKEY_CLASSES_ROOT/Wow6432Node/CLSID/MATLAB_CLSID/LocalServer32/Default (64-bit only)
   QString path;
   QSettings settings("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
   QString clsid = settings.value("Matlab.Application/CLSID/Default").toString();
   if (clsid.isEmpty() == false)
   {
      // Note: If checking these values manually using the registry editor, be aware that 32-bit applications
      // and 64-bit applications see a different version of the registry. In other words, just because you can
      // see a value in regedit.exe, that does not mean that 32-bit applications can see it. This page has more details:
      // http://msdn.microsoft.com/en-us/library/windows/desktop/aa384129%28v=vs.85%29.aspx
      path = settings.value("CLSID/" + clsid + "/LocalServer32/Default").toString();
      if (path.isEmpty() == true)
      {
         // Check the 64-bit value just in case this is a 32-bit MATLAB installation on a 64-bit machine.
         path = settings.value("Wow6432Node/CLSID/" + clsid + "/LocalServer32/Default").toString();
      }
   }

   // This is the full name and path of the MATLAB executable which will be invoked when engOpen or engOpenSingleUse
   // is called along with any command-line arguments. If this is empty, then no server is registered.
   return path;
}

QString MatlabInterpreterManager::getMatlabVersionAndDirectoryFromPath(const QString& path,
   QString& version, QString& directory)
{
   // MATLAB is typically installed in a directory with the version string appearing somewhere inside the name.
   // Get the list of all supported versions and check for each one in the path of the MATLAB installation.
   // If a version is found, try to use that one. Note that architecture (32 or 64-bit) is not checked here and
   // could cause problems later down the road (e.g.: 64-bit MATLAB is being used with 32-bit Opticks).
   QStringList versions = getAvailableMatlabInterpreterVersions();
   foreach(QString currentVersion, versions)
   {
      int versionIndex = path.indexOf(QRegExp(currentVersion, Qt::CaseInsensitive));
      if (versionIndex >= 0)
      {
         // Retrieve the directory where the MATLAB executable resides (to set the PATH for the MatlabInterpreter library).
         int pathIndex = path.indexOf("matlab.exe", 0, Qt::CaseInsensitive);
         if (pathIndex < 0)
         {
            return "Unable to detect MATLAB DLL directory.\nTry running manual configuration.";
         }

         version = currentVersion;
         directory = path.left(pathIndex);
         return QString();
      }
   }

   return "Located MATLAB COM server but could not determine its version.\n"
      "Try running manual configuration.";
}

void MatlabInterpreterManager::sessionClosed(Subject& subject, const std::string& signal, const boost::any& data)
{
   // This must be done during session close and not application close to support session restore.
   // If this is not done, then the interpreter library will remain open and cause problems during session restore.
   unloadModule();

   #pragma message(__FILE__ "(" STRING(__LINE__) ") : warning : See OPTICKS-1549 (dadkins)")
   mSessionClosed = true;
}

void MatlabInterpreterManager::unloadModule()
{
   if (mpModule != NULL)
   {
      void (*shutdown_matlab_interpreter)() =
         reinterpret_cast<void(*)()>(mpModule->getProcedureAddress("shutdown_matlab_interpreter"));
      if (shutdown_matlab_interpreter != NULL)
      {
         shutdown_matlab_interpreter();
      }

      Service<PlugInManagerServices>()->destroyDynamicModule(mpModule);
      mpModule = NULL;
   }
}
