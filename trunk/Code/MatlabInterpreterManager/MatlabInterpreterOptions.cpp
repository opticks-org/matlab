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
#include "FileBrowser.h"
#include "Filename.h"
#include "MatlabInterpreter.h"
#include "MatlabInterpreterManager.h"
#include "MatlabInterpreterOptions.h"
#include "LabeledSection.h"
#include "OptionQWidgetWrapper.h"
#include "PlugInManagerServices.h"
#include "PlugInRegistration.h"

#include <QtCore/QDir>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QMessageBox>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>
#include <QtGui/QStyleOptionButton>
#include <QtGui/QWidget>

#include <limits>

REGISTER_PLUGIN(Matlab, MatlabInterpreterOptions, OptionQWidgetWrapper<MatlabInterpreterOptions>());

MatlabInterpreterOptions::MatlabInterpreterOptions()
{
   // Automatic/Manual Configuration Section.
   QWidget* pMatlabConfigWidget = new QWidget(this);

   mpAutomaticConfiguration = new QRadioButton("Automatic Configuration", pMatlabConfigWidget);
   QLabel* pAutomaticConfigurationLabel = new QLabel("Automatic configuration scans the system's settings before "
      "starting the MATLAB interpreter to determine the MATLAB version being used.", pMatlabConfigWidget);
   pAutomaticConfigurationLabel->setWordWrap(true);

   QRadioButton* pManualConfiguration = new QRadioButton("Manual Configuration", pMatlabConfigWidget);
   QLabel* pManualConfigurationLabel = new QLabel("The MATLAB Installation Location and MATLAB Version must be updated "
      "whenever a new version of MATLAB is installed or a different MATLAB COM server is registered. Please refer to "
      "the MATLAB documentation for more details about MATLAB COM server registration.", pMatlabConfigWidget);
   pManualConfigurationLabel->setWordWrap(true);

   QLabel* pDllLabel = new QLabel("MATLAB Installation Location", pMatlabConfigWidget);
   mpDll = new FileBrowser(pMatlabConfigWidget);
   mpDll->setBrowseCaption("Locate the MATLAB installation");
   mpDll->setBrowseExistingFile(true);
   mpDll->setBrowseFileFilters("MATLAB Engine (libeng.dll)");

   QLabel* pVersionLabel = new QLabel("MATLAB Version", pMatlabConfigWidget);
   mpVersion = new QComboBox(pMatlabConfigWidget);
   mpVersion->addItems(MatlabInterpreterManager::getAvailableMatlabInterpreterVersions());
   mpVersion->setEditable(true);
   mpVersion->setDuplicatesEnabled(false);
   mpVersion->setInsertPolicy(QComboBox::InsertAlphabetically);

   QGridLayout* pMatlabConfigLayout = new QGridLayout(pMatlabConfigWidget);
   pMatlabConfigLayout->setMargin(0);
   pMatlabConfigLayout->setSpacing(5);
   pMatlabConfigLayout->addWidget(mpAutomaticConfiguration, 0, 0, 1, 4);
   pMatlabConfigLayout->addWidget(pAutomaticConfigurationLabel, 1, 1, 1, 3);
   pMatlabConfigLayout->addWidget(pManualConfiguration, 2, 0, 1, 4);
   pMatlabConfigLayout->addWidget(pManualConfigurationLabel, 3, 1, 1, 3);
   pMatlabConfigLayout->addWidget(pDllLabel, 4, 1);
   pMatlabConfigLayout->addWidget(mpDll, 4, 2, 1, 2);
   pMatlabConfigLayout->addWidget(pVersionLabel, 5, 1);
   pMatlabConfigLayout->addWidget(mpVersion, 5, 2, 1, 2);
   QStyleOptionButton option;
   option.initFrom(mpAutomaticConfiguration);
   int radioWidth = style()->subElementRect(QStyle::SE_RadioButtonIndicator, &option).width();
   pMatlabConfigLayout->setColumnMinimumWidth(0, radioWidth);
   pMatlabConfigLayout->setColumnStretch(3, 10);
   pMatlabConfigLayout->setRowStretch(6, 10);
   LabeledSection* pMatlabConfigSection = new LabeledSection(pMatlabConfigWidget, "MATLAB Configuration", this);

   // Miscellaneous Settings.
   QWidget* pMatlabMiscWidget = new QWidget(this);

   // Output buffer size should never be 0 since that would cause some internal commands to fail.
   // It would also prevent reading the value of variables from MATLAB as well as detecting error conditions.
   QLabel* pOutputBufferSizeLabel = new QLabel("Output Buffer Size", pMatlabMiscWidget);
   mpOutputBufferSize = new QSpinBox(pMatlabMiscWidget);
   mpOutputBufferSize->setToolTip("Set the number of bytes to use for output.");
   mpOutputBufferSize->setRange(1024, std::numeric_limits<int>::max());
   mpOutputBufferSize->setSingleStep(1024);
   mpOutputBufferSize->setSuffix(" bytes");

   mpCheckErrors = new QCheckBox("Automatically Check for Errors", pMatlabMiscWidget);
   mpCheckErrors->setToolTip("Set whether to check for errors after running each command.");

   mpClearErrors = new QCheckBox("Automatically Clear Errors", pMatlabMiscWidget);
   mpClearErrors->setToolTip("Set whether to clear errors after running each command.");

   QGridLayout* pMatlabMiscLayout = new QGridLayout(pMatlabMiscWidget);
   pMatlabMiscLayout->addWidget(pOutputBufferSizeLabel, 0, 0);
   pMatlabMiscLayout->addWidget(mpOutputBufferSize, 0, 1);
   pMatlabMiscLayout->addWidget(mpCheckErrors, 1, 0, 1, 2);
   pMatlabMiscLayout->addWidget(mpClearErrors, 2, 0, 1, 2);
   pMatlabMiscLayout->setRowStretch(3, 10);
   pMatlabMiscLayout->setColumnStretch(2, 10);
   LabeledSection* pMatlabMiscSection = new LabeledSection(pMatlabMiscWidget, "Miscellaneous MATLAB Settings", this);

   // Connections/Initialization.
   VERIFYNR(connect(mpAutomaticConfiguration, SIGNAL(toggled(bool)),
      pAutomaticConfigurationLabel, SLOT(setEnabled(bool))));
   VERIFYNR(connect(pManualConfiguration, SIGNAL(toggled(bool)), pManualConfigurationLabel, SLOT(setEnabled(bool))));
   VERIFYNR(connect(pManualConfiguration, SIGNAL(toggled(bool)), pDllLabel, SLOT(setEnabled(bool))));
   VERIFYNR(connect(pManualConfiguration, SIGNAL(toggled(bool)), mpDll, SLOT(setEnabled(bool))));
   VERIFYNR(connect(pManualConfiguration, SIGNAL(toggled(bool)), pVersionLabel, SLOT(setEnabled(bool))));
   VERIFYNR(connect(pManualConfiguration, SIGNAL(toggled(bool)), mpVersion, SLOT(setEnabled(bool))));
   if (MatlabInterpreter::getSettingAutomaticConfiguration() == true)
   {
      mpAutomaticConfiguration->setChecked(true);
      pManualConfigurationLabel->setEnabled(false);
      pDllLabel->setEnabled(false);
      mpDll->setEnabled(false);
      pVersionLabel->setEnabled(false);
      mpVersion->setEnabled(false);
   }
   else
   {
      pManualConfiguration->setChecked(true);
      pAutomaticConfigurationLabel->setEnabled(false);
   }

   const Filename* pTmpFile = MatlabInterpreter::getSettingDLL();
   setDll(pTmpFile);
   setVersion(QString::fromStdString(MatlabInterpreter::getSettingVersion()));
   mpOutputBufferSize->setValue(MatlabInterpreter::getSettingOutputBufferSize());
   mpCheckErrors->setChecked(MatlabInterpreter::getSettingCheckErrors());
   mpClearErrors->setChecked(MatlabInterpreter::getSettingClearErrors());

   addSection(pMatlabConfigSection);
   addSection(pMatlabMiscSection);
   addStretch(1);
}

MatlabInterpreterOptions::~MatlabInterpreterOptions()
{}

void MatlabInterpreterOptions::setDll(const Filename* pDll)
{
   if (pDll != NULL)
   {
      mpDll->setFilename(*pDll);
   }
}

void MatlabInterpreterOptions::setVersion(const QString& version)
{
   int loc = mpVersion->findText(version, Qt::MatchFixedString);
   if (loc == -1)
   {
      mpVersion->addItem(version);
      loc = mpVersion->findText(version, Qt::MatchFixedString);
   }
   mpVersion->setCurrentIndex(loc);
}

void MatlabInterpreterOptions::applyChanges()
{
   if (settingsRequireApplicationRestart() == true)
   {
      QString appName = QString::fromStdString(Service<ConfigurationSettings>()->getProduct());
      QMessageBox::warning(this, appName, "The MATLAB interpreter is already running so changes to the MATLAB "
         "installation location or version will not take effect until the application is restarted.");
   }

   MatlabInterpreter::setSettingAutomaticConfiguration(mpAutomaticConfiguration->isChecked());
   FactoryResource<Filename> pTmpDll;
   pTmpDll->setFullPathAndName(mpDll->getFilename().toStdString());
   MatlabInterpreter::setSettingDLL(pTmpDll.get());
   MatlabInterpreter::setSettingVersion(mpVersion->currentText().toStdString());
   MatlabInterpreter::setSettingOutputBufferSize(mpOutputBufferSize->value());
   MatlabInterpreter::setSettingCheckErrors(mpCheckErrors->isChecked());
   MatlabInterpreter::setSettingClearErrors(mpClearErrors->isChecked());
}

bool MatlabInterpreterOptions::settingsRequireApplicationRestart() const
{
   std::vector<PlugIn*> plugIns = Service<PlugInManagerServices>()->getPlugInInstances("MATLAB");
   if (plugIns.size() != 1)
   {
      // Sanity check -- should never happen.
      return false;
   }

   InterpreterManager* pInterpreter = dynamic_cast<InterpreterManager*>(plugIns.front());
   if ((pInterpreter == NULL) || (pInterpreter->isStarted() == false))
   {
      // No need to restart since the interpreter has not yet pulled these values.
      return false;
   }

   if (MatlabInterpreter::getSettingAutomaticConfiguration() != mpAutomaticConfiguration->isChecked())
   {
      return true;
   }

   std::string oldFilename;
   const Filename* pFilename = MatlabInterpreter::getSettingDLL();
   if (pFilename != NULL)
   {
      oldFilename = pFilename->getFullPathAndName();
   }

   if (oldFilename != mpDll->getFilename().toStdString())
   {
      return true;
   }

   if (MatlabInterpreter::getSettingVersion() != mpVersion->currentText().toStdString())
   {
      return true;
   }

   return false;
}
