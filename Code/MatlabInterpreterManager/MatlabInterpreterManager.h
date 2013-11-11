/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef MATLABINTERPRETERMANAGER_H
#define MATLABINTERPRETERMANAGER_H

#include "ApplicationServices.h"
#include "AttachmentPtr.h"
#include "DynamicModule.h"
#include "Interpreter.h"
#include "InterpreterManagerShell.h"
#include "MatlabInterpreter.h"
#include "SubjectImp.h"

#include <QtCore/QStringList>

#include <string>

class QString;

class MatlabInterpreterManager : public InterpreterManagerShell, public SubjectImp
{
public:
   static QStringList getAvailableMatlabInterpreterVersions();

   MatlabInterpreterManager();
   virtual ~MatlabInterpreterManager();

   virtual bool execute(PlugInArgList* pInArgList, PlugInArgList* pOutArgList);

   virtual bool isStarted() const;
   virtual bool start();
   virtual std::string getStartupMessage() const;
   virtual Interpreter* getInterpreter() const;

   virtual const std::string& getObjectType() const;
   virtual bool isKindOf(const std::string& className) const;

   SUBJECTADAPTER_METHODS(SubjectImp)

private:
   AttachmentPtr<ApplicationServices> mpApplicationServices;
   static QString prepareToLoadModule();
   static QString getPathForRegisteredMatlabServer();
   static QString getMatlabVersionAndDirectoryFromPath(const QString& path, QString& version, QString& directory);

   void unloadModule();
   void sessionClosed(Subject& subject, const std::string& signal, const boost::any& data);

   bool mSessionClosed;
   DynamicModule* mpModule;
   std::string mStartupMessage;
   AttachmentPtr<MatlabInterpreter> mpInterpreter;
};

#endif
