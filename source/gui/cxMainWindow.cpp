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

#include "cxMainWindow.h"

#include <QtWidgets>
#include "boost/bind.hpp"

#include "cxConfig.h"
#include "cxDataLocations.h"
#include "cxProfile.h"
#include "cxLogicManager.h"
#include "cxTrackingService.h"
#include "cxSettings.h"
#include "cxVideoService.h"
#include "cxStateService.h"
#include "cxNavigation.h"
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxViewGroupData.h"
#include "cxSessionStorageService.h"
#include "cxVisServices.h"
#include "cxAudioImpl.h"
#include "cxLayoutInteractor.h"
#include "cxVLCRecorder.h"
#include "cxCameraControl.h"

#include "cxDynamicMainWindowWidgets.h"
#include "cxStatusBar.h"
#include "cxImportDataDialog.h"
#include "cxExportDataDialog.h"
#include "cxSecondaryMainWindow.h"
#include "cxSecondaryViewLayoutWindow.h"
#include "cxSamplerWidget.h"
#include "cxHelperWidgets.h"

#include "cxStreamPropertiesWidget.h"
#include "cxVideoConnectionWidget.h"
#include "cxToolManagerWidget.h"
#include "cxFrameTreeWidget.h"
#include "cxNavigationWidget.h"
#include "cxVolumePropertiesWidget.h"
#include "cxToolPropertiesWidget.h"
#include "cxPreferencesDialog.h"
#include "cxSlicePropertiesWidget.h"
#include "cxMeshInfoWidget.h"
#include "cxTrackPadWidget.h"
#include "cxConsoleWidget.h"
#include "cxMetricWidget.h"
#include "cxPlaybackWidget.h"
#include "cxEraserWidget.h"
#include "cxAllFiltersWidget.h"
#include "cxPluginFrameworkWidget.h"


namespace cx
{

MainWindow::MainWindow() :
	mFullScreenAction(NULL), mStandard3DViewActions(new QActionGroup(this)), mControlPanel(NULL), mDockWidgets(new DynamicMainWindowWidgets(this))
{
	this->setObjectName("MainWindow");

	mServices = VisServices::create(logicManager()->getPluginContext());
	mLayoutInteractor.reset(new LayoutInteractor());

	this->setCentralWidget(viewService()->getLayoutWidget(this, 0));

	this->createActions();
	this->createMenus();
	this->createToolBars();
	this->setStatusBar(new StatusBar());

	reporter()->setAudioSource(AudioPtr(new AudioImpl()));

	connect(stateService().get(), &StateService::applicationStateChanged, this, &MainWindow::onApplicationStateChangedSlot);
	connect(stateService().get(), &StateService::workflowStateChanged, this, &MainWindow::onWorkflowStateChangedSlot);
	connect(stateService().get(), &StateService::workflowStateAboutToChange, this, &MainWindow::saveDesktopSlot);

	this->updateWindowTitle();

	this->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

	this->addAsDockWidget(new PlaybackWidget(this), "Browsing");
	this->addAsDockWidget(new VideoConnectionWidget(mServices, this), "Utility");
	this->addAsDockWidget(new EraserWidget(this), "Properties");
	this->addAsDockWidget(new MetricWidget(mServices->visualizationService, mServices->patientModelService, this), "Utility");
	this->addAsDockWidget(new SlicePropertiesWidget(mServices->patientModelService, mServices->visualizationService, this), "Properties");
	this->addAsDockWidget(new VolumePropertiesWidget(mServices->patientModelService, mServices->visualizationService, this), "Properties");
	this->addAsDockWidget(new MeshInfoWidget(mServices->patientModelService, mServices->visualizationService, this), "Properties");
	this->addAsDockWidget(new StreamPropertiesWidget(mServices->patientModelService, mServices->visualizationService, this), "Properties");
	this->addAsDockWidget(new TrackPadWidget(this), "Utility");
	this->addAsDockWidget(new ToolPropertiesWidget(this), "Properties");
	this->addAsDockWidget(new NavigationWidget(this), "Properties");
	this->addAsDockWidget(new ConsoleWidget(this, "ConsoleWidget", "Console"), "Utility");
	this->addAsDockWidget(new ConsoleWidget(this, "ConsoleWidget2", "Extra Console"), "Utility");
//	this->addAsDockWidget(new ConsoleWidgetCollection(this, "ConsoleWidgets", "Consoles"), "Utility");
	this->addAsDockWidget(new FrameTreeWidget(mServices->patientModelService, this), "Browsing");
	this->addAsDockWidget(new ToolManagerWidget(this), "Debugging");
	this->addAsDockWidget(new PluginFrameworkWidget(this), "Browsing");
	this->addAsDockWidget(new AllFiltersWidget(VisServices::create(logicManager()->getPluginContext()), this), "Algorithms");

	connect(patientService().get(), &PatientModelService::patientChanged, this, &MainWindow::patientChangedSlot);
	connect(qApp, &QApplication::focusChanged, this, &MainWindow::focusChanged);

	this->setupGUIExtenders();

	// window menu must be created after all dock widgets are created
	QMenu* popupMenu = this->createPopupMenu();
	popupMenu->setTitle("Window");
	this->menuBar()->insertMenu(mHelpMenuAction, popupMenu);

	// Restore saved window states
	// Must be done after all DockWidgets are created
	if (restoreGeometry(settings()->value("mainWindow/geometry").toByteArray()))
	{
		this->show();
	}
	else
	{
		this->showMaximized();
	}

	if (settings()->value("gui/fullscreen").toBool())
		this->setWindowState(this->windowState() | Qt::WindowFullScreen);
}

void MainWindow::changeEvent(QEvent * event)
{
	QMainWindow::changeEvent(event);

	if (event->type() == QEvent::WindowStateChange)
	{
		if (mFullScreenAction)
			mFullScreenAction->setChecked(this->windowState() & Qt::WindowFullScreen);
	}
}

void MainWindow::setupGUIExtenders()
{
	mServiceListener.reset(new ServiceTrackerListener<GUIExtenderService>(
							   LogicManager::getInstance()->getPluginContext(),
							   boost::bind(&MainWindow::onGUIExtenderServiceAdded, this, _1),
							   boost::bind(&MainWindow::onGUIExtenderServiceModified, this, _1),
							   boost::bind(&MainWindow::onGUIExtenderServiceRemoved, this, _1)
							   ));
	mServiceListener->open();
}

void MainWindow::onGUIExtenderServiceAdded(GUIExtenderService* service)
{
	std::vector<GUIExtenderService::CategorizedWidget> widgets = service->createWidgets();
	for (unsigned j = 0; j < widgets.size(); ++j)
	{
		mDockWidgets->addAsDockWidget(widgets[j].mWidget, widgets[j].mCategory, service);
	}

	std::vector<QToolBar*> toolBars = service->createToolBars();
	for(unsigned j = 0; j < toolBars.size(); ++j)
	{
		mDockWidgets->registerToolBar(toolBars[j]);
	}
}

void MainWindow::onGUIExtenderServiceModified(GUIExtenderService* service)
{
}

void MainWindow::onGUIExtenderServiceRemoved(GUIExtenderService* service)
{
	mDockWidgets->owningServiceRemoved(service);
}

void MainWindow::dockWidgetVisibilityChanged(bool val)
{
	if (val)
		this->focusInsideDockWidget(sender());
}

void MainWindow::focusChanged(QWidget * old, QWidget * now)
{
	this->focusInsideDockWidget(now);
}

void MainWindow::focusInsideDockWidget(QObject *dockWidget)
{
	// focusing to docked widgets is required by the help system

	// Assume structure: QDockWidget->QScrollArea->QWidget,
	// as defined in MainWindow::addAsDockWidget()
	QDockWidget* dw = dynamic_cast<QDockWidget*>(dockWidget);
	if (!dw)
		return;
	if (dw->parent()!=this) // avoid events from other mainwindows
		return;
	QScrollArea* sa = dynamic_cast<QScrollArea*>(dw->widget());
	if (!sa)
		return;
	QTimer::singleShot(0, sa->widget(), SLOT(setFocus())); // avoid loops etc by send async event.
}

MainWindow::~MainWindow()
{
	reporter()->setAudioSource(AudioPtr()); // important! QSound::play fires a thread, causes segfault during shutdown
	mServiceListener.reset();
}

QMenu* MainWindow::createPopupMenu()
{
	return mDockWidgets->createPopupMenu();
}

void MainWindow::createActions()
{
	CameraControlPtr cameraControl = viewService()->getCameraControl();
	if (cameraControl)
		mStandard3DViewActions = cameraControl->createStandard3DViewActions();

	// File
	mNewPatientAction = new QAction(QIcon(":/icons/open_icon_library/document-new-8.png"), tr(
		"&New patient"), this);
	mNewPatientAction->setShortcut(tr("Ctrl+N"));
	mNewPatientAction->setStatusTip(tr("Create a new patient file"));
	mSaveFileAction = new QAction(QIcon(":/icons/open_icon_library/document-save-5.png"), tr(
		"&Save Patient"), this);
	mSaveFileAction->setShortcut(tr("Ctrl+S"));
	mSaveFileAction->setStatusTip(tr("Save patient file"));
	mLoadFileAction = new QAction(QIcon(":/icons/open_icon_library/document-open-7.png"), tr(
		"&Load Patient"), this);
	mLoadFileAction->setShortcut(tr("Ctrl+L"));
	mLoadFileAction->setStatusTip(tr("Load patient file"));
	mClearPatientAction = new QAction(tr("&Clear Patient"), this);
	mExportPatientAction = new QAction(tr("&Export Patient"), this);

	mGotoDocumentationAction = new QAction(QIcon(":/icons/open_icon_library/applications-internet.png"),
												  "Web Documentation", this);
	connect(mGotoDocumentationAction, &QAction::triggered, this, &MainWindow::onGotoDocumentation);

	mShowContextSensitiveHelpAction = new QAction(QIcon(":/icons/open_icon_library/help-contents-5.png"),
												  "Context-sensitive help", this);
	connect(mShowContextSensitiveHelpAction, &QAction::triggered, this, &MainWindow::onShowContextSentitiveHelp);

	connect(mNewPatientAction, &QAction::triggered, this, &MainWindow::newPatientSlot);
	connect(mLoadFileAction, &QAction::triggered, this, &MainWindow::loadPatientFileSlot);
	connect(mSaveFileAction, &QAction::triggered, this, &MainWindow::savePatientFileSlot);
	connect(mSaveFileAction, &QAction::triggered, this, &MainWindow::saveDesktopSlot);
	connect(mExportPatientAction, &QAction::triggered, this, &MainWindow::exportDataSlot);
	connect(mClearPatientAction, &QAction::triggered, this, &MainWindow::clearPatientSlot);

	mShowControlPanelAction = new QAction("Show Control Panel", this);
	connect(mShowControlPanelAction, &QAction::triggered, this, &MainWindow::showControlPanelActionSlot);
	mSecondaryViewLayoutWindowAction = new QAction("Show Secondary View Layout Window", this);
	connect(mSecondaryViewLayoutWindowAction, &QAction::triggered, this, &MainWindow::showSecondaryViewLayoutWindowActionSlot);

	// Application
	mAboutAction = new QAction(tr("About"), this);
	mAboutAction->setStatusTip(tr("Show the application's About box"));
	mPreferencesAction = new QAction(tr("Preferences"), this);
	mPreferencesAction->setShortcut(tr("Ctrl+,"));
	mPreferencesAction->setStatusTip(tr("Show the preferences dialog"));

	mStartLogConsoleAction = new QAction(tr("&Start Log Console"), this);
	mStartLogConsoleAction->setShortcut(tr("Ctrl+D"));
	mStartLogConsoleAction->setStatusTip(tr("Open Log Console as external application"));
	connect(mStartLogConsoleAction, &QAction::triggered, this, &MainWindow::onStartLogConsole);

	mFullScreenAction = new QAction(tr("Fullscreen"), this);
	mFullScreenAction->setShortcut(tr("F11"));
	mFullScreenAction->setStatusTip(tr("Toggle full screen"));
	mFullScreenAction->setCheckable(true);
	mFullScreenAction->setChecked(this->windowState() & Qt::WindowFullScreen);
	connect(mFullScreenAction, &QAction::triggered, this, &MainWindow::toggleFullScreenSlot);

	mQuitAction = new QAction(tr("&Quit"), this);
	mQuitAction->setShortcut(tr("Ctrl+Q"));
	mQuitAction->setStatusTip(tr("Exit the application"));

	connect(mAboutAction, &QAction::triggered, this, &MainWindow::aboutSlot);
	connect(mPreferencesAction, &QAction::triggered, this, &MainWindow::preferencesSlot);
	connect(mQuitAction, &QAction::triggered, this, &MainWindow::quitSlot);

	mShootScreenAction = new QAction(tr("Shoot Screen"), this);
	mShootScreenAction->setIcon(QIcon(":/icons/screenshot-screen.png"));
	mShootScreenAction->setShortcut(tr("Ctrl+f"));
	mShootScreenAction->setStatusTip(tr("Save a screenshot to the patient folder."));
	connect(mShootScreenAction, &QAction::triggered, this, &MainWindow::shootScreen);

	mShootWindowAction = new QAction(tr("Shoot Window"), this);
	mShootWindowAction->setIcon(QIcon(":/icons/screenshot-window.png"));
	mShootWindowAction->setShortcut(tr("Ctrl+Shift+f"));
	mShootWindowAction->setStatusTip(tr("Save an image of the application to the patient folder."));
	connect(mShootWindowAction, &QAction::triggered, this, &MainWindow::shootWindow);

	mRecordFullscreenAction = new QAction(tr("Record Fullscreen"), this);
	mRecordFullscreenAction->setShortcut(tr("F8"));
	mRecordFullscreenAction->setStatusTip(tr("Record a video of the full screen."));
	connect(mRecordFullscreenAction, &QAction::triggered, this, &MainWindow::recordFullscreen);

	//data
	mImportDataAction = new QAction(QIcon(":/icons/open_icon_library/document-import-2.png"), tr("&Import data"), this);
	mImportDataAction->setShortcut(tr("Ctrl+I"));
	mImportDataAction->setStatusTip(tr("Import image data"));

	mDeleteDataAction = new QAction(tr("Delete current image"), this);
	mDeleteDataAction->setStatusTip(tr("Delete selected volume"));

	connect(mImportDataAction, &QAction::triggered, this, &MainWindow::importDataSlot);
	connect(mDeleteDataAction, &QAction::triggered, this, &MainWindow::deleteDataSlot);

	mShowPointPickerAction = new QAction(tr("Point Picker"), this);
	mShowPointPickerAction->setCheckable(true);
	mShowPointPickerAction->setToolTip("Activate the 3D Point Picker Probe");
	mShowPointPickerAction->setIcon(QIcon(":/icons/point_picker.png"));
	connect(mShowPointPickerAction, &QAction::triggered, this, &MainWindow::togglePointPickerActionSlot);

	if (viewService()->getGroup(0))
		connect(viewService()->getGroup(0).get(), &ViewGroupData::optionsChanged, this, &MainWindow::updatePointPickerActionSlot);
	this->updatePointPickerActionSlot();

	//tool
	mToolsActionGroup = new QActionGroup(this);
	mConfigureToolsAction = new QAction(tr("Tool configuration"), mToolsActionGroup);
	mInitializeToolsAction = new QAction(tr("Initialize"), mToolsActionGroup);
	mTrackingToolsAction = new QAction(tr("Start tracking"), mToolsActionGroup);
	mTrackingToolsAction->setShortcut(tr("Ctrl+T"));

	mToolsActionGroup->setExclusive(false); // must turn off to get the checkbox independent.

	mStartStreamingAction = new QAction(tr("Start Streaming"), mToolsActionGroup);
	mStartStreamingAction->setShortcut(tr("Ctrl+V"));
	connect(mStartStreamingAction, &QAction::triggered, this, &MainWindow::toggleStreamingSlot);
	connect(videoService().get(), &VideoService::connected, this, &MainWindow::updateStreamingActionSlot);
	this->updateStreamingActionSlot();

	mConfigureToolsAction->setChecked(true);

	connect(mConfigureToolsAction, &QAction::triggered, this, &MainWindow::configureSlot);
	boost::function<void()> finit = boost::bind(&TrackingService::setState, trackingService(), Tool::tsINITIALIZED);
	connect(mInitializeToolsAction, &QAction::triggered, finit);
	connect(mTrackingToolsAction, &QAction::triggered, this, &MainWindow::toggleTrackingSlot);
	connect(trackingService().get(), &TrackingService::stateChanged, this, &MainWindow::updateTrackingActionSlot);
	connect(trackingService().get(), &TrackingService::stateChanged, this, &MainWindow::updateTrackingActionSlot);
	this->updateTrackingActionSlot();

	mCenterToImageCenterAction = new QAction(tr("Center Image"), this);
	mCenterToImageCenterAction->setIcon(QIcon(":/icons/center_image.png"));
	connect(mCenterToImageCenterAction, &QAction::triggered, this, &MainWindow::centerToImageCenterSlot);
	mCenterToTooltipAction = new QAction(tr("Center Tool"), this);
	mCenterToTooltipAction->setIcon(QIcon(":/icons/center_tool.png"));
	connect(mCenterToTooltipAction, &QAction::triggered, this, &MainWindow::centerToTooltipSlot);

	mSaveDesktopAction = new QAction(QIcon(":/icons/workflow_state_save.png"), tr("Save desktop"), this);
	mSaveDesktopAction->setToolTip("Save desktop for workflow step");
	connect(mSaveDesktopAction, &QAction::triggered, this, &MainWindow::saveDesktopSlot);
	mResetDesktopAction = new QAction(QIcon(":/icons/workflow_state_revert.png"), tr("Reset desktop"), this);
	mResetDesktopAction->setToolTip("Reset desktop for workflow step");
	connect(mResetDesktopAction, &QAction::triggered, this, &MainWindow::resetDesktopSlot);

	mInteractorStyleActionGroup = viewService()->createInteractorStyleActionGroup();
}

void MainWindow::toggleFullScreenSlot()
{
	this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);

	settings()->setValue("gui/fullscreen", (this->windowState() & Qt::WindowFullScreen)!=0);
}

void MainWindow::shootScreen()
{
	QDesktopWidget* desktop = qApp->desktop();
	QList<QScreen*> screens = qApp->screens();

	for (int i=0; i<desktop->screenCount(); ++i)
	{
		QWidget* screenWidget = desktop->screen(i);
		WId screenWinId = screenWidget->winId();
		QRect geo = desktop->screenGeometry(i);
		QString name = "";
		if (desktop->screenCount()>1)
			name = screens[i]->name().split(" ").join("");
		this->saveScreenShot(screens[i]->grabWindow(screenWinId, geo.left(), geo.top(), geo.width(), geo.height()), name);
	}
}

void MainWindow::shootWindow()
{
	QScreen* screen = qApp->primaryScreen();
	this->saveScreenShot(screen->grabWindow(this->winId()));
}

void MainWindow::recordFullscreen()
{
	QString path = patientService()->generateFilePath("Screenshots", "mp4");

	if(vlc()->isRecording())
		vlc()->stopRecording();
	else
		vlc()->startRecording(path);

}

void MainWindow::onStartLogConsole()
{
	QString fullname = DataLocations::findExecutableInStandardLocations("LogConsole");
//	std::cout << "MainWindow::onStartLogConsole() " << fullname << std::endl;
	mLocalVideoServerProcess.reset(new ProcessWrapper(QString("LogConsole")));
	mLocalVideoServerProcess->launchWithRelativePath(fullname, QStringList());
}

void MainWindow::saveScreenShot(QPixmap pixmap, QString id)
{
	QString ending = "png";
	if (!id.isEmpty())
		ending = id + "." + ending;
	QString path = patientService()->generateFilePath("Screenshots", ending);
	QtConcurrent::run(boost::bind(&MainWindow::saveScreenShotThreaded, this, pixmap.toImage(), path));
}

/**Intended to be called in a separate thread.
 * \sa saveScreenShot()
 */
void MainWindow::saveScreenShotThreaded(QImage pixmap, QString filename)
{
	pixmap.save(filename, "png");
	report("Saved screenshot to " + filename);
	reporter()->playScreenShotSound();
}

void MainWindow::toggleStreamingSlot()
{
	if (videoService()->isConnected())
		videoService()->closeConnection();
	else
		videoService()->openConnection();
}

void MainWindow::updateStreamingActionSlot()
{
	if (videoService()->isConnected())
	{
		mStartStreamingAction->setIcon(QIcon(":/icons/streaming_green.png"));
		mStartStreamingAction->setText("Stop Streaming");
	}
	else
	{
		mStartStreamingAction->setIcon(QIcon(":/icons/streaming_red.png"));
		mStartStreamingAction->setText("Start Streaming");
	}
}

void MainWindow::centerToImageCenterSlot()
{
	NavigationPtr nav = viewService()->getNavigation();

	if (patientService()->getActiveImage())
		nav->centerToData(patientService()->getActiveImage());
	else if (!viewService()->groupCount())
		nav->centerToView(viewService()->getGroup(0)->getData());
	else
		nav->centerToGlobalDataCenter();
}

void MainWindow::centerToTooltipSlot()
{
	NavigationPtr nav = viewService()->getNavigation();
	nav->centerToTooltip();
}

void MainWindow::togglePointPickerActionSlot()
{
	ViewGroupDataPtr data = viewService()->getGroup(0);
	ViewGroupData::Options options = data->getOptions();
	options.mShowPointPickerProbe = !options.mShowPointPickerProbe;
	data->setOptions(options);
}
void MainWindow::updatePointPickerActionSlot()
{
	if (!viewService()->getGroup(0))
		return;
	bool show = viewService()->getGroup(0)->getOptions().mShowPointPickerProbe;
	mShowPointPickerAction->setChecked(show);
}

void MainWindow::updateTrackingActionSlot()
{
	if (trackingService()->getState() >= Tool::tsTRACKING)
	{
		mTrackingToolsAction->setIcon(QIcon(":/icons/polaris-green.png"));
		mTrackingToolsAction->setText("Stop Tracking");
	}
	else
	{
		mTrackingToolsAction->setIcon(QIcon(":/icons/polaris-red.png"));
		mTrackingToolsAction->setText("Start Tracking");
	}
}

void MainWindow::toggleTrackingSlot()
{
	if (trackingService()->getState() >= Tool::tsTRACKING)
		trackingService()->setState(Tool::tsINITIALIZED);
	else
		trackingService()->setState(Tool::tsTRACKING);
}

namespace
{
QString timestampFormatFolderFriendly()
{
  return QString("yyyy-MM-dd_hh-mm");
}
}

void MainWindow::newPatientSlot()
{
	QString patientDatafolder = this->getExistingSessionFolder();

	QString timestamp = QDateTime::currentDateTime().toString(timestampFormatFolderFriendly());
	QString filename = QString("%1_%2_%3.cx3")
			.arg(timestamp)
			.arg(profile()->getName())
			.arg(settings()->value("globalPatientNumber").toString());

	QString choosenDir = patientDatafolder + "/" + filename;

	QFileDialog dialog(this, tr("Select directory to save patient in"), patientDatafolder + "/");
	dialog.setOption(QFileDialog::DontUseNativeDialog, true);
	dialog.setOption(QFileDialog::ShowDirsOnly, true);
	dialog.selectFile(filename);
	if (!dialog.exec())
		return;
	choosenDir = dialog.selectedFiles().front();

	if (!choosenDir.endsWith(".cx3"))
		choosenDir += QString(".cx3");

	// Update global patient number
	int patientNumber = settings()->value("globalPatientNumber").toInt();
	settings()->setValue("globalPatientNumber", ++patientNumber);

	mServices->getSession()->load(choosenDir);
}

QString MainWindow::getExistingSessionFolder()
{
	QString folder = settings()->value("globalPatientDataFolder").toString();

	// Create folders
	if (!QDir().exists(folder))
	{
		QDir().mkdir(folder);
		report("Made a new patient folder: " + folder);
	}

	return folder;
}

void MainWindow::onGotoDocumentation()
{
	QString url = DataLocations::getWebsiteUserDocumentationURL();
	QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
}

void MainWindow::onShowContextSentitiveHelp()
{
	mDockWidgets->showWidget("Help");
}


void MainWindow::clearPatientSlot()
{
	mServices->getSession()->clear();
}

void MainWindow::savePatientFileSlot()
{
	if (patientService()->getActivePatientFolder().isEmpty())
	{
		reportWarning("No patient selected, select or create patient before saving!");
		this->newPatientSlot();
		return;
	}

	mServices->getSession()->save();
}

void MainWindow::onApplicationStateChangedSlot()
{
	this->updateWindowTitle();
}

void MainWindow::updateWindowTitle()
{
	QString profileName = stateService()->getApplicationStateName();
	QString versionName = stateService()->getVersionName();

	QString activePatientFolder = patientService()->getActivePatientFolder();
	if (activePatientFolder.endsWith('/'))
		activePatientFolder.chop(1);
	QString patientName;

	if (!activePatientFolder.isEmpty())
	{
		QFileInfo info(activePatientFolder);
		patientName = info.completeBaseName();
	}

	QString format("%1 %2 - %3 - %4  (not approved for medical use)");
	QString title = format
			.arg(qApp->applicationDisplayName())
			.arg(versionName)
			.arg(profileName)
			.arg(patientName);
	this->setWindowTitle(title);
}

void MainWindow::onWorkflowStateChangedSlot()
{
	Desktop desktop = stateService()->getActiveDesktop();

	mDockWidgets->restoreFrom(desktop);
	viewService()->setActiveLayout(desktop.mLayoutUid, 0);
	viewService()->setActiveLayout(desktop.mSecondaryLayoutUid, 1);
	patientService()->autoSave();

	// moved to help plugin:
//	// set initial focus to mainwindow in order to view it in the documentation
//	// this is most important when starting up.
//	QTimer::singleShot(0, this, SLOT(setFocus())); // avoid loops etc by send async event.

//#ifdef CX_APPLE
//	// HACK
//	// Toolbars are not correctly refreshed on mac 10.8,
//	// Cause is related to QVTKWidget (removing it removes the problem)
//	// The following "force refresh by resize" solves repaint, but
//	// inactive toolbars are still partly clickable.
//	QSize size = this->size();
//	this->resize(size.width()-1, size.height());
//	this->resize(size);
//#endif
}

void MainWindow::saveDesktopSlot()
{
	Desktop desktop;
	desktop.mMainWindowState = this->saveState();
	desktop.mLayoutUid = viewService()->getActiveLayout(0);
	desktop.mSecondaryLayoutUid = viewService()->getActiveLayout(1);
	stateService()->saveDesktop(desktop);
}

void MainWindow::resetDesktopSlot()
{
	stateService()->resetDesktop();
	this->onWorkflowStateChangedSlot();
}

void MainWindow::showControlPanelActionSlot()
{
	if (!mControlPanel)
		mControlPanel = new SecondaryMainWindow(this);
	mControlPanel->show();
}

void MainWindow::showSecondaryViewLayoutWindowActionSlot()
{
	if (!mSecondaryViewLayoutWindow)
		mSecondaryViewLayoutWindow = new SecondaryViewLayoutWindow(this);
	mSecondaryViewLayoutWindow->tryShowOnSecondaryScreen();
}

void MainWindow::loadPatientFileSlot()
{
	QString patientDatafolder = this->getExistingSessionFolder();

	// Open file dialog
	QString folder = QFileDialog::getExistingDirectory(this, "Select patient", patientDatafolder, QFileDialog::ShowDirsOnly);
	if (folder.isEmpty())
		return;

	mServices->getSession()->load(folder);
}

void MainWindow::exportDataSlot()
{
	this->savePatientFileSlot();

	ExportDataDialog* wizard = new ExportDataDialog(mServices->patientModelService, this);
	wizard->exec(); //calling exec() makes the wizard dialog modal which prevents other user interaction with the system
}

void MainWindow::importDataSlot()
{
	this->savePatientFileSlot();

	QString folder = mLastImportDataFolder;
	if (folder.isEmpty())
		folder = settings()->value("globalPatientDataFolder").toString();

	QStringList fileName = QFileDialog::getOpenFileNames(this, QString(tr("Select data file(s) for import")),
		folder, tr("Image/Mesh (*.mhd *.mha *.stl *.vtk *.mnc)"));
	if (fileName.empty())
	{
		report("Import canceled");
		return;
	}

	mLastImportDataFolder = QFileInfo(fileName[0]).absolutePath();

	for (int i=0; i<fileName.size(); ++i)
	{
		ImportDataDialog* wizard = new ImportDataDialog(mServices->patientModelService, fileName[i], this);
		wizard->exec(); //calling exec() makes the wizard dialog modal which prevents other user interaction with the system
	}
}

void MainWindow::patientChangedSlot()
{
	this->updateWindowTitle();
}

void MainWindow::createMenus()
{
#ifdef CX_LINUX
    // shortcuts defined on actions in the global menubar is not reachable on Qt5+Ubuntu14.04,
    // solve by reverting to old style.
    // This will create a double menubar: remove by autohiding menubar or using
    //   sudo apt-get autoremove appmenu-gtk appmenu-gtk3 appmenu-qt
    // and reboot
    this->menuBar()->setNativeMenuBar(false);
#endif
	mFileMenu = new QMenu(tr("File"), this);
	mWorkflowMenu = new QMenu(tr("Workflow"), this);
	mToolMenu = new QMenu(tr("Tracking"), this);
	mLayoutMenu = new QMenu(tr("Layouts"), this);
	mNavigationMenu = new QMenu(tr("Navigation"), this);
	mHelpMenu = new QMenu(tr("Help"), this);

	// File
	mFileMenu->addAction(mPreferencesAction);
	this->menuBar()->addMenu(mFileMenu);
	mFileMenu->addAction(mNewPatientAction);
	mFileMenu->addAction(mSaveFileAction);
	mFileMenu->addAction(mLoadFileAction);
	mFileMenu->addAction(mClearPatientAction);
	mFileMenu->addSeparator();
	mFileMenu->addAction(mExportPatientAction);
	mFileMenu->addAction(mImportDataAction);
	mFileMenu->addAction(mDeleteDataAction);
	mFileMenu->addSeparator();
	mFileMenu->addAction(mFullScreenAction);
	mFileMenu->addAction(mStartLogConsoleAction);
	mFileMenu->addAction(mShootScreenAction);
	mFileMenu->addAction(mShootWindowAction);
	mFileMenu->addAction(mRecordFullscreenAction);
	mFileMenu->addSeparator();
	mFileMenu->addAction(mShowControlPanelAction);
	mFileMenu->addAction(mSecondaryViewLayoutWindowAction);

	mFileMenu->addAction(mQuitAction);

	//workflow
	this->menuBar()->addMenu(mWorkflowMenu);
	QList<QAction*> actions = stateService()->getWorkflowActions()->actions();
	for (int i=0; i<actions.size(); ++i)
	{
		mWorkflowMenu->addAction(actions[i]);
	}

	mWorkflowMenu->addSeparator();
	mWorkflowMenu->addAction(mSaveDesktopAction);
	mWorkflowMenu->addAction(mResetDesktopAction);

	//tool
	this->menuBar()->addMenu(mToolMenu);
	mToolMenu->addAction(mConfigureToolsAction);
	mToolMenu->addAction(mInitializeToolsAction);
	mToolMenu->addAction(mTrackingToolsAction);
	mToolMenu->addSeparator();
	mToolMenu->addAction(mStartStreamingAction);
	mToolMenu->addSeparator();

	//layout
	this->menuBar()->addMenu(mLayoutMenu);
	mLayoutInteractor->connectToMenu(mLayoutMenu);

	this->menuBar()->addMenu(mNavigationMenu);
	mNavigationMenu->addAction(mCenterToImageCenterAction);
	mNavigationMenu->addAction(mCenterToTooltipAction);
	mNavigationMenu->addAction(mShowPointPickerAction);
	mNavigationMenu->addSeparator();
	mNavigationMenu->addActions(mInteractorStyleActionGroup->actions());

	mHelpMenuAction = this->menuBar()->addMenu(mHelpMenu);
	mHelpMenu->addAction(mAboutAction);
	mHelpMenu->addAction(mGotoDocumentationAction);
	mHelpMenu->addAction(mShowContextSensitiveHelpAction);
}

void MainWindow::createToolBars()
{
	mWorkflowToolBar = this->registerToolBar("Workflow");
	QList<QAction*> actions = stateService()->getWorkflowActions()->actions();
	for (int i=0; i<actions.size(); ++i)
		mWorkflowToolBar->addAction(actions[i]);

	mDataToolBar = this->registerToolBar("Data");
	mDataToolBar->addAction(mNewPatientAction);
	mDataToolBar->addAction(mLoadFileAction);
	mDataToolBar->addAction(mSaveFileAction);
	mDataToolBar->addAction(mImportDataAction);

	mToolToolBar = this->registerToolBar("Tools");
	mToolToolBar->addAction(mTrackingToolsAction);
	mToolToolBar->addAction(mStartStreamingAction);

	mNavigationToolBar = this->registerToolBar("Navigation");
	mNavigationToolBar->addAction(mCenterToImageCenterAction);
	mNavigationToolBar->addAction(mCenterToTooltipAction);
	mNavigationToolBar->addAction(mShowPointPickerAction);

	mInteractorStyleToolBar = this->registerToolBar("InteractorStyle");
	mInteractorStyleToolBar->addActions(mInteractorStyleActionGroup->actions());

	mDesktopToolBar = this->registerToolBar("Desktop");
	mDesktopToolBar->addAction(mSaveDesktopAction);
	mDesktopToolBar->addAction(mResetDesktopAction);

	mScreenshotToolBar = this->registerToolBar("Screenshot");
	mScreenshotToolBar->addAction(mShootScreenAction);

	QToolBar* camera3DViewToolBar = this->registerToolBar("Camera 3D Views");
	camera3DViewToolBar->addActions(mStandard3DViewActions->actions());

	QToolBar* samplerWidgetToolBar = this->registerToolBar("Sampler");
	samplerWidgetToolBar->addWidget(new SamplerWidget(this));

	QToolBar* toolOffsetToolBar = this->registerToolBar("Tool Offset");
	toolOffsetToolBar->addWidget(createDataWidget(mServices->visualizationService, mServices->patientModelService, this, DoublePropertyActiveToolOffset::create()));

	QToolBar* helpToolBar = this->registerToolBar("Help");
	helpToolBar->addAction(mShowContextSensitiveHelpAction);
}

QToolBar* MainWindow::registerToolBar(QString name, QString groupname)
{
	QToolBar* toolbar = new QToolBar(name);
	toolbar->setObjectName(QString(name).remove(" ")+"ToolBar");
	mDockWidgets->registerToolBar(toolbar, groupname);
	return toolbar;
}

void MainWindow::aboutSlot()
{
	QString doc_path = DataLocations::getDocPath();
	QString appName = qApp->applicationDisplayName();
	QString url_website = DataLocations::getWebsiteURL();
	QString url_license = QString("file://%1/license.txt").arg(doc_path);
	QString url_config = QString("file://%1/cxConfigDescription.txt").arg(doc_path);

	QString text(""
	"<h2>%1</h2>"
	"<h4>%2</h4>"
	"<p>A Research Platform for Image-Guided Therapy<p>"
	"<p>%1 is NOT approved for medical use.<p>"
	""
	"<p><a href=%3> website </a><p>"
	"<p><a href=%4> license </a><p>"
	"<p><a href=%5> configuration </a><p>");

	QMessageBox::about(this, tr("About %1").arg(appName), text
			.arg(appName)
			.arg(CustusX_VERSION_STRING)
			.arg(url_website)
			.arg(url_license)
			.arg(url_config)
			);
}

void MainWindow::preferencesSlot()
{
	PreferencesDialog prefDialog(mServices->visualizationService, mServices->patientModelService, this);
	prefDialog.exec();
}

void MainWindow::quitSlot()
{
	report("Shutting down CustusX");
	viewService()->deactivateLayout();

	patientService()->autoSave();

	settings()->setValue("mainWindow/geometry", saveGeometry());
	settings()->setValue("mainWindow/windowState", saveState());
	settings()->sync();
	report("Closing: Save geometry and window state");

	qApp->quit();
}

void MainWindow::deleteDataSlot()
{
	if (!patientService()->getActiveImage())
		return;
	QString text = QString("Do you really want to delete data %1?").arg(patientService()->getActiveImage()->getName());
	if (QMessageBox::question(this, "Data delete", text, QMessageBox::StandardButtons(QMessageBox::Ok | QMessageBox::Cancel))!=QMessageBox::Ok)
		return;
	mServices->patientModelService->removeData(patientService()->getActiveImageUid());
}

void MainWindow::configureSlot()
{
	trackingService()->setState(Tool::tsCONFIGURED);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QMainWindow::closeEvent(event);
	this->quitSlot();
}

QDockWidget* MainWindow::addAsDockWidget(QWidget* widget, QString groupname)
{
	return mDockWidgets->addAsDockWidget(widget, groupname);
}

}//namespace cx
