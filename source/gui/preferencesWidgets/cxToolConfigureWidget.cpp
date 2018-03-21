/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <cxToolConfigureWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDir>
#include <QLineEdit>
#include "cxEnumConverter.h"
#include "cxLogger.h"
#include "cxStateService.h"
#include "cxTrackingService.h"
#include "cxSelectionGroupBox.h"
#include "cxToolListWidget.h"
#include "cxHelperWidgets.h"
#include "cxTrackerConfiguration.h"

namespace cx
{

ToolConfigureGroupBox::ToolConfigureGroupBox(TrackingServicePtr trackingService, StateServicePtr stateService, QWidget* parent) :
	QGroupBox(parent),
	mConfigFilesComboBox(new QComboBox()),
	mConfigFileLineEdit(new QLineEdit()),
	mReferenceComboBox(new QComboBox()),
	mModified(false),
	mTrackingService(trackingService),
	mStateService(stateService)
{
	connect(stateService.get(), &StateService::applicationStateChanged, this, &ToolConfigureGroupBox::onApplicationStateChanged);

	mConfigFilesComboBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Expanding);
	//mConfigFilesComboBox->setMinimumSize(200, 0);
	//mConfigFilesComboBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	mToolListWidget = new ConfigToolListWidget(trackingService, NULL);

	this->createTrackingSystemSelector();

//	QGroupBox* toolGroupBox = new QGroupBox();
//	toolGroupBox->setTitle("Tools");
//	QVBoxLayout* toolLayout = new QVBoxLayout();
//	toolGroupBox->setLayout(toolLayout);
//	toolLayout->addWidget(mToolListWidget);

	QGridLayout* layout = new QGridLayout(this);
	int row=0;
	layout->addWidget(new QLabel("Configuration: "), row, 0, 1, 1);
	layout->addWidget(mConfigFilesComboBox, row, 1, 1, 1);
	row++;
	layout->addWidget(new QLabel("File name: "), row, 0, 1, 1);
	layout->addWidget(mConfigFileLineEdit, row, 1, 1, 1);
	row++;
	sscCreateDataWidget(this, mTrackingSystemSelector, layout, row);
	row++;
	layout->addWidget(mToolListWidget, row, 0, 1, 2);
	layout->setRowStretch(row, 1);
	row++;
//	layout->addWidget(new QLabel("Reference: "), row, 0, 1, 1);
//	layout->addWidget(mReferenceComboBox, row, 1, 1, 1);

	QHBoxLayout* refLayout = new QHBoxLayout;
	refLayout->addWidget(new QLabel("Reference: "));
	refLayout->addWidget(mReferenceComboBox);
	refLayout->setMargin(0);
	layout->addLayout(refLayout, row, 0, 1, 2);

	//changes due to programming actions
	connect(mConfigFilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(configChangedSlot()));
	connect(mToolListWidget, SIGNAL(listSizeChanged()), this, SLOT(toolsChangedSlot()));
	connect(mToolListWidget, SIGNAL(toolSelected(QString)), this, SIGNAL(toolSelected(QString)));

	//changes due to user actions
	connect(mConfigFileLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(configEditedSlot()));
	connect(mToolListWidget, SIGNAL(userChangedList()), this, SLOT(configEditedSlot()));
	connect(mReferenceComboBox, SIGNAL(activated(int)), this, SLOT(configEditedSlot()));

	this->onApplicationStateChanged();
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
		if (!configAbsoluteFilePath.isEmpty())
			reportWarning("Tool configuration doesn't exist: " + cleanPath);
	}
	mConfigFilesComboBox->setCurrentIndex(currentIndex);

	mModified = false;
}

QString ToolConfigureGroupBox::getCurrenctlySelectedConfiguration() const
{
	QString retval;
	retval = mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex(), Qt::ToolTipRole).toString();
	return retval;
}

void ToolConfigureGroupBox::createTrackingSystemSelector()
{
	QString defaultValue = "";
	TrackerConfigurationPtr config = mTrackingService->getConfiguration();
	mTrackingSystemSelector = StringProperty::initialize("trackingsystem", "Tracking System",
															   "Select tracking system to use",
															   defaultValue,
															   config->getSupportedTrackingSystems(),
															   QDomNode());
	connect(mTrackingSystemSelector.get(), SIGNAL(changed()), this, SLOT(filterToolsSlot()));
}

StringPropertyBasePtr ToolConfigureGroupBox::getTrackingSystemSelector()
{
	return mTrackingSystemSelector;
}

QString ToolConfigureGroupBox::requestSaveConfigurationSlot()
{
	QString retval;

	if(!mModified)
		return retval;

	// deconfigure toolmanager in order to be able to reread config data
	mTrackingService->setState(Tool::tsNONE);

	TrackerConfiguration::Configuration current = this->getCurrentConfiguration();
	TrackerConfigurationPtr config = mTrackingService->getConfiguration();
	config->saveConfiguration(current);
	mModified = false;

	this->populateConfigurations();

	return current.mUid;
}

void ToolConfigureGroupBox::onApplicationStateChanged()
{
	QString application = mStateService->getApplicationStateName();
	this->setTitle("Tool configurations for "+ application);
	this->populateConfigurations();
}

void ToolConfigureGroupBox::configChangedSlot()
{
	QString uid = mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex(),
												 Qt::ToolTipRole).toString();

	TrackerConfigurationPtr config = mTrackingService->getConfiguration();
	TrackerConfiguration::Configuration data = config->getConfiguration(uid);
	bool isNewConfig = 	mConfigFilesComboBox->currentText().contains("<new config>");

	if (isNewConfig)
	{
		data.mTrackingSystemName = enum2string(tsPOLARIS);
		data.mName = "MyConfig";
	}

	mConfigFileLineEdit->setText(data.mName);
	mConfigFileLineEdit->setEnabled(isNewConfig);
	mConfigFileLineEdit->setToolTip(data.mUid);
	mModified = true;
	mTrackingSystemSelector->setValue(data.mTrackingSystemName);
	mToolListWidget->configSlot(data.mTools);
	this->mTrackingSystemImplementation = data.mTrackingSystemImplementation;
}

void ToolConfigureGroupBox::configEditedSlot()
{
	mModified = true;
}

void ToolConfigureGroupBox::toolsChangedSlot()
{
	this->populateReference();
	this->filterToolsSlot();
}

void ToolConfigureGroupBox::filterToolsSlot()
{
	QString ts = mTrackingSystemSelector->getValue();
	mToolListWidget->filterSlot(QStringList() << ts);
	mModified = true;
}

void ToolConfigureGroupBox::populateConfigurations()
{
	mConfigFilesComboBox->clear();

	TrackerConfigurationPtr config = mTrackingService->getConfiguration();
	QStringList configurations = config->getConfigurationsGivenApplication();

	foreach(QString filename, configurations)
	{
		TrackerConfiguration::Configuration configuration = config->getConfiguration(filename);
		this->addConfigurationToComboBox(configuration.mName, configuration.mUid);
	}
	QString newConfig = "<new config>";
	this->addConfigurationToComboBox(newConfig, newConfig);

	int currentIndex = mConfigFilesComboBox->findText(newConfig);
	mConfigFilesComboBox->setCurrentIndex(currentIndex);
}

int ToolConfigureGroupBox::addConfigurationToComboBox(QString displayName, QString absoluteFilePath)
{
	mConfigFilesComboBox->addItem(displayName);
	int index = mConfigFilesComboBox->findText(displayName);
	mConfigFilesComboBox->setItemData(index, absoluteFilePath, Qt::ToolTipRole);

	return index;
}

TrackerConfiguration::Configuration ToolConfigureGroupBox::getCurrentConfiguration()
{
	TrackerConfiguration::Configuration retval;
	TrackerConfigurationPtr config = mTrackingService->getConfiguration();
	QString application = mStateService->getApplicationStateName();

	QString filename = mConfigFileLineEdit->text();
	QString filepath = config->getConfigurationApplicationsPath();

	retval.mUid = QString("%1/%2.xml").arg(filepath).arg(filename);
	retval.mClinicalApplication = application;
	retval.mTrackingSystemImplementation = this->mTrackingSystemImplementation;
	retval.mTrackingSystemName = mTrackingSystemSelector->getValue();
	retval.mTools = mToolListWidget->getTools();
	retval.mReferenceTool = mReferenceComboBox->itemData(mReferenceComboBox->currentIndex(), Qt::ToolTipRole).toString();

	// TODO fix retval.mToolList. See TrackerConfigurationImpl::saveConfiguration()
	// Will need to keep mOpenIGTLinkImageId and mOpenIGTLinkTransformId when creating return value
	// For now ConfigurationFileParser::saveConfiguration() refuse to save openigtlink tracking files
	// to prevent these from being destroyed.

	return retval;
}

QString ToolConfigureGroupBox::getCurrentConfigFilePath()
{
	QString configAbsoluteFilePath = mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex(), Qt::ToolTipRole).toString();
	return configAbsoluteFilePath;
}

void ToolConfigureGroupBox::populateReference()
{
	mReferenceComboBox->clear();

	int currentIndex = -1;

	TrackerConfigurationPtr config = mTrackingService->getConfiguration();//Get last config. No problem here?

	// populate list
	QStringList selectedTools = mToolListWidget->getTools();
	foreach(QString string, selectedTools)
	{
		if (config->getTool(string).mIsReference)
			currentIndex = this->addRefrenceToComboBox(string);
	}

	// look for a selected reference
	QString reference = config->getConfiguration(this->getCurrentConfigFilePath()).mReferenceTool;
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
