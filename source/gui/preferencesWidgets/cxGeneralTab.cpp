/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxGeneralTab.h"

#include <QLabel>
#include <QToolButton>
#include <QGridLayout>
#include <QFileDialog>
#include "cxSettings.h"
#include "cxStateService.h"
#include "cxVLCRecorder.h"
#include "cxHelperWidgets.h"
#include <QAction>
#include <QInputDialog>
#include <QGroupBox>
#include "cxProfile.h"
#include "cxLogicManager.h"

namespace cx
{

GeneralTab::GeneralTab(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget *parent) :
	PreferenceTab(parent), mVLCPath(""),
	mViewService(viewService),
	mPatientModelService(patientModelService)
{
	this->setObjectName("preferences_general_widget");
	mPatientDataFolderComboBox = NULL;
	mVLCPathComboBox = NULL;
	mToolConfigFolderComboBox = NULL;
//	mChooseApplicationComboBox = NULL;
}

void GeneralTab::init()
{
  mGlobalPatientDataFolder = profile()->getSessionRootFolder();
  mVLCPath = settings()->value("vlcPath").toString();
  if(!QFile::exists(mVLCPath))
	  this->searchForVLC();

//  connect(stateService().get(), &StateService::applicationStateChanged, this, &GeneralTab::applicationStateChangedSlot);

  // patientDataFolder
  QLabel* patientDataFolderLabel = new QLabel(tr("Patient data folder:"));
  mPatientDataFolderComboBox = new QComboBox;
  mPatientDataFolderComboBox->addItem( mGlobalPatientDataFolder);
  QAction* browsePatientFolderAction = new QAction(QIcon(":/icons/open.png"), tr("B&rowse..."), this);
  connect(browsePatientFolderAction, SIGNAL(triggered()), this, SLOT(browsePatientDataFolderSlot()));
  QToolButton* browsePatientFolderButton = new QToolButton(this);
  browsePatientFolderButton->setDefaultAction(browsePatientFolderAction);

  // VLC
  QLabel* vlcPathLabel = new QLabel(tr("VLC path:"));
  mVLCPathComboBox = new QComboBox();
  mVLCPathComboBox->addItem( mVLCPath);
  QAction* browseVLCPathAction = new QAction(QIcon(":/icons/open.png"), tr("Browse for VLC..."), this);
  connect(browseVLCPathAction, SIGNAL(triggered()), this, SLOT(browseVLCPathSlot()));
  QToolButton* browseVLCPathButton = new QToolButton(this);
  browseVLCPathButton->setDefaultAction(browseVLCPathAction);

  // Choose application name

  StringPropertyPtr profileSelector = this->getProfileSelector();

//  QLabel* chooseApplicationLabel = new QLabel(tr("Choose application:"));
//  mChooseApplicationComboBox = new QComboBox();
//  setApplicationComboBox();
//  connect(mChooseApplicationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentApplicationChangedSlot(int)));
//  this->applicationStateChangedSlot();

  bool filterToolPositions = settings()->value("TrackingPositionFilter/enabled").value<bool>();
  mFilterToolPositions = BoolProperty::initialize("Tool smoothing", "",
												 "Smooth the tool tracking positions using a low-pass filter.",
												 filterToolPositions);


  double filterToolPositionsCutoff = settings()->value("TrackingPositionFilter/cutoffFrequency").value<double>();
  mFilterToolPositionsCutoff
	 = DoubleProperty::initialize("filterToolPositionsCutoff", "Cutoff Frequency",
								  "If filtering enabled,\nfilter out tool movements faster than this frequency (Hz)",
								  filterToolPositionsCutoff, DoubleRange(0.1, 10, 0.1), 1);

  QToolButton* addProfileButton = this->createAddProfileButton();

  // Layout
  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(patientDataFolderLabel, 0, 0);
  mainLayout->addWidget(mPatientDataFolderComboBox, 0, 1);
  mainLayout->addWidget(browsePatientFolderButton, 0, 2);

//  mainLayout->addWidget(chooseApplicationLabel, 1, 0);
//  mainLayout->addWidget(mChooseApplicationComboBox, 1, 1);
  createDataWidget(mViewService, mPatientModelService, this, profileSelector, mainLayout, 1);
  mainLayout->addWidget(addProfileButton, 1, 2);

  mainLayout->addWidget(vlcPathLabel, 2, 0);
  mainLayout->addWidget(mVLCPathComboBox, 2, 1);
  mainLayout->addWidget(browseVLCPathButton, 2, 2);

  QGroupBox* filterToolPositionsGroupBox = new QGroupBox("Filter Tool Positions");
  QGridLayout* filterToolPositionsLayout = new QGridLayout(filterToolPositionsGroupBox);
  createDataWidget(mViewService, mPatientModelService, this, mFilterToolPositions, filterToolPositionsLayout, 0);
  createDataWidget(mViewService, mPatientModelService, this, mFilterToolPositionsCutoff, filterToolPositionsLayout, 1);
  mainLayout->addWidget(filterToolPositionsGroupBox, 3, 0, 1, 3 );

  mTopLayout->addLayout(mainLayout);
}

GeneralTab::~GeneralTab()
{}

QToolButton* GeneralTab::createAddProfileButton()
{
	QString tip = "Create a new profile based on the current";
	QAction* action = new QAction(QIcon(":/icons/preset_save.png"), "Add", this);
	action->setStatusTip(tip);
	action->setWhatsThis(tip);
	action->setToolTip(tip);
	connect(action, &QAction::triggered, this, &GeneralTab::onAddProfile);

	QToolButton* button = new QToolButton();
	button->setDefaultAction(action);
	return button;
}

void GeneralTab::onAddProfile()
{
	QString current = ProfileManager::getInstance()->activeProfile()->getUid();
	QStringList profiles = ProfileManager::getInstance()->getProfiles();
	QString name;
	for(int i=0; ; ++i)
	{
		name = QString("%1(%2)").arg(current).arg(i);
		if (!profiles.contains(name, Qt::CaseInsensitive))
			break;
	}

	bool ok = true;
	QString text = QInputDialog::getText(this, "Select profile name",
			"Name", QLineEdit::Normal, name, &ok);
	if (!ok || text.isEmpty())
		return;

	this->selectProfile(text);
}

void GeneralTab::browsePatientDataFolderSlot()
{
  mGlobalPatientDataFolder = QFileDialog::getExistingDirectory(this,
                                                     tr("Find Patient Data Folder"),
                                                     mGlobalPatientDataFolder,
                                                     QFileDialog::ShowDirsOnly);
  if( !mGlobalPatientDataFolder.isEmpty() ) {
    mPatientDataFolderComboBox->addItem( mGlobalPatientDataFolder );
    mPatientDataFolderComboBox->setCurrentIndex( mPatientDataFolderComboBox->currentIndex() + 1 );
  }
}

void GeneralTab::browseVLCPathSlot()
{
	mVLCPath = QFileDialog::getOpenFileName(this, tr("Find VLC executable"));

	if(!mVLCPath.isEmpty())
	{
		mVLCPathComboBox->addItem( mVLCPath );
		mVLCPathComboBox->setCurrentIndex( mVLCPathComboBox->currentIndex() + 1 );
	}
}

StringPropertyPtr GeneralTab::getProfileSelector()
{
	if (!mSelector)
	{
		ProfileManager* manager = ProfileManager::getInstance();
		QString defaultValue = profile()->getUid();
		mSelector = StringProperty::initialize("profile", "Profile",
											"Choose profile, containing settings and configuration",
											defaultValue, manager->getProfiles(), QDomNode());

		connect(mSelector.get(), &StringProperty::valueWasSet, this, &GeneralTab::onProfileSelected);
		connect(manager, &ProfileManager::activeProfileChanged, this, &GeneralTab::onProfileChanged);
	}

	return mSelector;
}

void GeneralTab::onProfileChanged()
{
	mSelector->setValueRange(ProfileManager::getInstance()->getProfiles());
	mSelector->setValue(profile()->getUid());
}

void GeneralTab::onProfileSelected()
{
	this->selectProfile(mSelector->getValue());
}

void GeneralTab::selectProfile(QString uid)
{
	this->rejectDialog();
	LogicManager::getInstance()->restartWithNewProfile(uid);
}

void GeneralTab::rejectDialog()
{
	QObject* item = this;
	while (item)
	{
		QDialog* dialog = dynamic_cast<QDialog*>(item);
		if (dialog)
		{
			dialog->reject();
			break;
		}
		item = item->parent();
	}
}

void GeneralTab::searchForVLC(QStringList searchPaths)
{
	vlc()->findVLCApplication(searchPaths);
	if(vlc()->hasVLCApplication())
	  mVLCPath = vlc()->getVLCPath();
}

void GeneralTab::saveParametersSlot()
{
	profile()->setSessionRootFolder(mGlobalPatientDataFolder);
  settings()->setValue("vlcPath", mVLCPath);
  settings()->setValue("TrackingPositionFilter/enabled", mFilterToolPositions->getValue());
  settings()->setValue("TrackingPositionFilter/cutoffFrequency", mFilterToolPositionsCutoff->getValue());

  settings()->sync();

  emit savedParameters();
}

} /* namespace cx */
