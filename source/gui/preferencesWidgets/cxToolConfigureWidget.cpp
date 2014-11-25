/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include <cxToolConfigureWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDir>
#include <QLineEdit>
#include "cxEnumConverter.h"
#include "cxReporter.h"
#include "cxStateService.h"
#include "cxTrackingService.h"
#include "cxSelectionGroupBox.h"
#include "cxToolListWidget.h"
#include "cxDataLocations.h"
#include "cxTrackerConfiguration.h"

namespace cx
{

ToolConfigureGroupBox::ToolConfigureGroupBox(QWidget* parent) :
    QGroupBox(parent),
    mClinicalApplication(mdCOUNT),
    mConfigFilesComboBox(new QComboBox()),
    mConfigFilePathLineEdit(new QLineEdit()),
    mConfigFileLineEdit(new QLineEdit()),
    mReferenceComboBox(new QComboBox())
{
  Q_PROPERTY("userEdited")

  mConfigFilesComboBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Expanding);
  //mConfigFilesComboBox->setMinimumSize(200, 0);
  //mConfigFilesComboBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  mApplicationGroupBox = new SelectionGroupBox("Applications", stateService()->getAllApplicationStateNames(), Qt::Vertical, true, NULL);
  mApplicationGroupBox->setEnabledButtons(false); //< application application is determined by the application state chosen elsewhere in the system
  mApplicationGroupBox->hide(); // large and redundant box - info is only used for path generation, which can be found in the "Save Path" box
  mApplicationGroupBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Expanding);
  TrackerConfigurationPtr config = trackingService()->getConfiguration();
  mTrackingSystemGroupBox = new SelectionGroupBox("Tracking systems", config->getSupportedTrackingSystems(), Qt::Horizontal, true, NULL);
  mToolListWidget = new ConfigToolListWidget(NULL);

  this->setClinicalApplicationSlot(string2enum<CLINICAL_APPLICATION>(stateService()->getApplicationStateName()));

  QGroupBox* toolGroupBox = new QGroupBox();
  toolGroupBox->setTitle("Tools");
  QVBoxLayout* toolLayout = new QVBoxLayout();
  toolGroupBox->setLayout(toolLayout);
  toolLayout->addWidget(mToolListWidget);

  QGridLayout* layout = new QGridLayout(this);
  int row=0;
  layout->addWidget(new QLabel("Selected config: "), row, 0, 1, 1);
  layout->addWidget(mConfigFilesComboBox, row, 1, 1, 1);
  row++;
  layout->addWidget(new QLabel("Save path: "), row, 0, 1, 1);
  layout->addWidget(mConfigFilePathLineEdit, row, 1, 1, 1);
  row++;
  layout->addWidget(new QLabel("File name: "), row, 0, 1, 1);
  layout->addWidget(mConfigFileLineEdit, row, 1, 1, 1);
  row++;
  layout->addWidget(mApplicationGroupBox, row, 0, 1, 2);
  row++;
  layout->addWidget(mTrackingSystemGroupBox, row, 0, 1, 2);
  row++;
  layout->addWidget(toolGroupBox, row, 0, 1, 2);
  layout->setRowStretch(row, 1);
  row++;
  layout->addWidget(new QLabel("Reference: "), row, 0, 1, 1);
  layout->addWidget(mReferenceComboBox, row, 1, 1, 1);

  //changes due to programming actions
  connect(mConfigFilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(configChangedSlot()));
  connect(mToolListWidget, SIGNAL(listSizeChanged()), this, SLOT(toolsChangedSlot()));
  connect(mTrackingSystemGroupBox, SIGNAL(selectionChanged()), this, SLOT(filterToolsSlot()));
  connect(mToolListWidget, SIGNAL(toolSelected(QString)), this, SIGNAL(toolSelected(QString)));

  //changes due to user actions
  connect(mConfigFilePathLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(pathEditedSlot()));
  connect(mConfigFilePathLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(configEditedSlot()));
  connect(mConfigFileLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(fileNameEditedSlot()));
  connect(mConfigFileLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(configEditedSlot()));
  connect(mConfigFileLineEdit, SIGNAL(editingFinished()), this, SLOT(filenameDoneEditingSlot()));
  connect(mApplicationGroupBox, SIGNAL(userClicked()), this, SLOT(configEditedSlot()));
  connect(mTrackingSystemGroupBox, SIGNAL(userClicked()), this, SLOT(configEditedSlot()));
  connect(mToolListWidget, SIGNAL(userChangedList()), this, SLOT(configEditedSlot()));
  connect(mReferenceComboBox, SIGNAL(activated(int)), this, SLOT(configEditedSlot()));

  this->populateConfigurations();
}

ToolConfigureGroupBox::~ToolConfigureGroupBox()
{}

void ToolConfigureGroupBox::setCurrentlySelectedCofiguration(QString configAbsoluteFilePath)
{
	QString cleanPath = QDir(configAbsoluteFilePath).absolutePath();
  int currentIndex = mConfigFilesComboBox->findData(cleanPath, Qt::ToolTipRole);
  if(currentIndex < 0)
  {
    currentIndex = 0;
    reportWarning("Tool configuration doesn't exist: " + cleanPath);
  }
  mConfigFilesComboBox->setCurrentIndex(currentIndex);
}

QString ToolConfigureGroupBox::getCurrenctlySelectedConfiguration() const
{
  QString retval;
  retval = mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex(), Qt::ToolTipRole).toString();
  return retval;
}

QString ToolConfigureGroupBox::requestSaveConfigurationSlot()
{
  QString retval;

  if(!mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex(), sEdited).toBool())
    return retval;

  // deconfigure toolmanager in order to be able to reread config data
  trackingService()->setState(Tool::tsNONE);

  TrackerConfiguration::Configuration current = this->getCurrentConfiguration();
  TrackerConfigurationPtr config = trackingService()->getConfiguration();
  config->saveConfiguration(current);

  retval = current.mUid;

  this->populateConfigurations();

  return retval;
}

void ToolConfigureGroupBox::setClinicalApplicationSlot(CLINICAL_APPLICATION clinicalApplication)
{
  mClinicalApplication = clinicalApplication;
  this->setTitle("Tool configurations for "+enum2string(mClinicalApplication));

  this->populateConfigurations();
}

void ToolConfigureGroupBox::configChangedSlot()
{
	QStringList selectedApplications;
	QStringList selectedTrackingSystems;
	QStringList selectedTools;
	QString absoluteConfigFilePath = mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex(),
					Qt::ToolTipRole).toString();
	bool suggestDefaultNames;
	TrackerConfigurationPtr config = trackingService()->getConfiguration();

	if (mConfigFilesComboBox->currentText().contains("<new config>"))
	{
		selectedApplications << enum2string(mClinicalApplication); // just want a default
		selectedTrackingSystems << enum2string(tsPOLARIS); //just want a default
		suggestDefaultNames = true;

		absoluteConfigFilePath = config->getConfigurationApplicationsPath(enum2string(mClinicalApplication));
	}
	else
	{
		TrackerConfiguration::Configuration data = config->getConfiguration(absoluteConfigFilePath);

		selectedApplications << data.mClinicalApplication;
		selectedTrackingSystems << data.mTrackingSystem;
		selectedTools = data.mTools;

		suggestDefaultNames = false;
	}

	QFile file(absoluteConfigFilePath);
	QFileInfo info(file);
	QString filePath = info.path();
	QString fileName = info.fileName();

	mConfigFilePathLineEdit->setText(filePath);
	this->setState(mConfigFilePathLineEdit, !suggestDefaultNames);

	mConfigFileLineEdit->setText(fileName);
	this->setState(mConfigFileLineEdit, !suggestDefaultNames);

	mApplicationGroupBox->setSelected(selectedApplications);
	mTrackingSystemGroupBox->setSelected(selectedTrackingSystems);
	mToolListWidget->configSlot(selectedTools);
}

void ToolConfigureGroupBox::configEditedSlot()
{
  this->setState(mConfigFilesComboBox, mConfigFilesComboBox->currentIndex(), true);
}

void ToolConfigureGroupBox::toolsChangedSlot()
{
  this->populateReference();
  this->filterToolsSlot();
}

void ToolConfigureGroupBox::filterToolsSlot()
{
  QStringList trackingsystemFilter = mTrackingSystemGroupBox->getSelected();
  mToolListWidget->filterSlot(trackingsystemFilter);
}

void ToolConfigureGroupBox::pathEditedSlot()
{
  this->setState(mConfigFilePathLineEdit, true);
}

void ToolConfigureGroupBox::fileNameEditedSlot()
{
  this->setState(mConfigFileLineEdit, true);
}

void ToolConfigureGroupBox::filenameDoneEditingSlot()
{
  if(mConfigFileLineEdit->text().contains(".xml", Qt::CaseInsensitive))
    mConfigFileLineEdit->setText(mConfigFileLineEdit->text().remove(".xml"));

  if(!mConfigFileLineEdit->text().endsWith(".xml", Qt::CaseInsensitive))
    mConfigFileLineEdit->setText(mConfigFileLineEdit->text()+".xml");
}

void ToolConfigureGroupBox::populateConfigurations()
{
  mConfigFilesComboBox->clear();

  TrackerConfigurationPtr config = trackingService()->getConfiguration();
  QStringList configurations = config->getConfigurationsGivenApplication(enum2string(mClinicalApplication));

  foreach(QString filename, configurations)
  {
	  TrackerConfiguration::Configuration configuration = config->getConfiguration(filename);
	  this->addConfigurationToComboBox(configuration.mName, configuration.mUid);
  }
	QString newConfig("<new config>");
	this->addConfigurationToComboBox(newConfig, this->generateConfigName());

  int currentIndex = mConfigFilesComboBox->findText(newConfig);
  mConfigFilesComboBox->setCurrentIndex(currentIndex);
}

int ToolConfigureGroupBox::addConfigurationToComboBox(QString displayName, QString absoluteFilePath)
{
  mConfigFilesComboBox->addItem(displayName);
  int index = mConfigFilesComboBox->findText(displayName);
  mConfigFilesComboBox->setItemData(index, absoluteFilePath, Qt::ToolTipRole);
  this->setState(mConfigFilesComboBox, index, false);

  return index;
}

void ToolConfigureGroupBox::setState(QComboBox* box, int index, bool edited)
{
  box->setItemData(index, edited, sEdited);

  if(edited && !mConfigFilePathLineEdit->property("userEdited").toBool() && !mConfigFileLineEdit->property("userEdited").toBool())
  {
    QString absoluteConfigPaht = this->generateConfigName();
    QFile file(absoluteConfigPaht);
    QFileInfo info(file);
    QString filename = info.fileName();
    QString filepath = info.path();

    mConfigFilePathLineEdit->setText(filepath);
    mConfigFileLineEdit->setText(filename);
  }
}

TrackerConfiguration::Configuration ToolConfigureGroupBox::getCurrentConfiguration()
{
  TrackerConfiguration::Configuration retval;
  QString filename = mConfigFileLineEdit->text();
  QString filepath = mConfigFilePathLineEdit->text();
  retval.mUid = filepath+"/"+filename;
  retval.mClinicalApplication = mApplicationGroupBox->getSelected()[0];
  retval.mTrackingSystem = mTrackingSystemGroupBox->getSelected()[0];
  retval.mTools = mToolListWidget->getTools();
  retval.mReferenceTool = mReferenceComboBox->itemData(mReferenceComboBox->currentIndex(), Qt::ToolTipRole).toString();

  return retval;
}

QString ToolConfigureGroupBox::generateConfigName()
{
	QStringList applicationFilter = mApplicationGroupBox->getSelected();
	QString app = ((applicationFilter.size() >= 1) ? applicationFilter[0] : "");
	TrackerConfigurationPtr config = trackingService()->getConfiguration();
	QString root = config->getConfigurationApplicationsPath(app);
	return root + "/MyConfig.xml";
}


void ToolConfigureGroupBox::setState(QLineEdit* line, bool userEdited)
{
  QVariant value(userEdited);
  line->setProperty("userEdited", value);
}

void ToolConfigureGroupBox::populateReference()
{
	mReferenceComboBox->clear();

	int currentIndex = -1;

	TrackerConfigurationPtr config = trackingService()->getConfiguration();

	// populate list
	QStringList selectedTools = mToolListWidget->getTools();
	foreach(QString string, selectedTools)
	{
		if (config->getTool(string).mIsReference)
			currentIndex = this->addRefrenceToComboBox(string);
	}

	// look for a selected reference
	QString configAbsoluteFilePath = mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex(), Qt::ToolTipRole).toString();
	QString reference = config->getConfiguration(configAbsoluteFilePath).mReferenceTool;
	currentIndex = this->addRefrenceToComboBox(reference);

	// always select a reference if available:
	if (currentIndex<0)
		currentIndex = 0;

	mReferenceComboBox->setCurrentIndex(currentIndex);
}

int ToolConfigureGroupBox::addRefrenceToComboBox(QString absoluteRefereneFilePath)
{
	int index;

	QFile file(absoluteRefereneFilePath);
	QFileInfo info(file);
	QString refUid = info.dir().dirName();

	QStringList selectedTools = mToolListWidget->getTools();
	if (!selectedTools.count(absoluteRefereneFilePath))
		return -1;

	if (refUid.isEmpty())
		return -1;

	if (mReferenceComboBox->findText(refUid) < 0)
		mReferenceComboBox->addItem(refUid);
	index = mReferenceComboBox->findText(refUid);
	mReferenceComboBox->setItemData(index, info.absoluteFilePath(), Qt::ToolTipRole);

	return index;
}


}//namespace cx
