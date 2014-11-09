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
#include <QtConcurrent>
#include <QWhatsThis>
#include <QDesktopWidget>
#include "boost/scoped_ptr.hpp"
#include "boost/bind.hpp"
#include "cxTime.h"
#include "cxReporter.h"
#include "cxDataManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxToolManager.h"
#include "cxStatusBar.h"
#include "cxVolumePropertiesWidget.h"
#include "cxNavigationWidget.h"
#include "cxTabbedWidget.h"
#include "cxToolPropertiesWidget.h"
#include "cxViewGroup.h"
#include "cxPreferencesDialog.h"
#include "cxSlicePropertiesWidget.h"
#include "cxPatientData.h"
#include "cxDataLocations.h"
#include "cxMeshInfoWidget.h"
#include "cxFrameForest.h"
#include "cxFrameTreeWidget.h"
#include "cxImportDataDialog.h"
#include "cxTrackPadWidget.h"
#include "cxCameraControl.h"
#include "cxSecondaryMainWindow.h"
#include "cxVideoConnectionWidget.h"
#include "cxAudioImpl.h"
#include "cxSettings.h"
#include "cxVideoConnectionManager.h"
#include "cxToolManagerWidget.h"
#include "cxVideoServiceOld.h"
#include "cxExportDataDialog.h"
#include "cxGPUImageBuffer.h"
#include "cxData.h"
#include "cxConsoleWidget.h"
#include "cxViewManager.h"
#include "cxStateService.h"
#include "cxPatientService.h"
#include "cxMetricWidget.h"
#include "cxViewWrapper.h"
#include "cxPlaybackWidget.h"
#include "cxEraserWidget.h"
#include "cxSamplerWidget.h"
#include "cxDataAdapterHelper.h"
#include "cxVideoConnection.h"
#include "cxWorkflowStateMachine.h"
#include "cxApplicationStateMachine.h"
#include "cxConfig.h"
#include "cxVLCRecorder.h"
#include "cxSecondaryViewLayoutWindow.h"
//#include "cxRegistrationHistoryWidget.h"
#include "cxLogger.h"
#include "cxLayoutInteractor.h"
#include "cxNavigation.h"
#include "cxPluginFrameworkWidget.h"

#include "ctkServiceTracker.h"
#include "cxLogicManager.h"
#include "cxPluginFramework.h"
#include "ctkPluginContext.h"
#include "cxDockWidgets.h"
#include "cxPatientModelServiceProxy.h"
#include "cxVisualizationServiceProxy.h"
#include "cxVideoServiceProxy.h"

namespace cx
{

MainWindow::MainWindow(std::vector<GUIExtenderServicePtr> guiExtenders) :
	mFullScreenAction(NULL), mStandard3DViewActions(NULL), mControlPanel(NULL), mDockWidgets(new DockWidgets(this))
{
	this->setObjectName("MainWindow");

	QFile stylesheet(":/cxStyleSheet.ss");
	stylesheet.open(QIODevice::ReadOnly);
	qApp->setStyleSheet(stylesheet.readAll());

	mCameraControl = viewManager()->getCameraControl();
	mLayoutInteractor.reset(new LayoutInteractor());

	viewManager()->initialize();
	this->setCentralWidget(viewManager()->getLayoutWidget(0));

	this->createActions();
	this->createMenus();
	this->createToolBars();
	this->setStatusBar(new StatusBar());

	reporter()->setLoggingFolder(DataLocations::getRootConfigPath());
	reporter()->setAudioSource(AudioPtr(new AudioImpl()));

	connect(stateService()->getApplication().get(), SIGNAL(activeStateChanged()), this,
		SLOT(onApplicationStateChangedSlot()));
	connect(stateService()->getWorkflow().get(), SIGNAL(activeStateChanged()), this, SLOT(onWorkflowStateChangedSlot()));
	connect(stateService()->getWorkflow().get(), SIGNAL(activeStateAboutToChange()), this, SLOT(saveDesktopSlot()));

	this->updateWindowTitle();

	this->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

	ctkPluginContext *pluginContext = LogicManager::getInstance()->getPluginContext();

	mPatientModelService = PatientModelServicePtr(new PatientModelServiceProxy(pluginContext));
	mVisualizationService = VisualizationServicePtr(new VisualizationServiceProxy(pluginContext));
	mVideoService = VideoServicePtr(new VideoServiceProxy(pluginContext));

	this->addAsDockWidget(new PlaybackWidget(this), "Browsing");
	this->addAsDockWidget(new VideoConnectionWidget(mVisualizationService, mPatientModelService, mVideoService, this), "Utility");
	this->addAsDockWidget(new EraserWidget(this), "Properties");
	this->addAsDockWidget(new MetricWidget(mVisualizationService, mPatientModelService, this), "Utility");
	this->addAsDockWidget(new SlicePropertiesWidget(mPatientModelService, mVisualizationService, this), "Properties");
	this->addAsDockWidget(new VolumePropertiesWidget(mPatientModelService, mVisualizationService, this), "Properties");
	this->addAsDockWidget(new MeshInfoWidget(mPatientModelService, mVisualizationService, this), "Properties");
	this->addAsDockWidget(new TrackPadWidget(this), "Utility");
	this->addAsDockWidget(new ToolPropertiesWidget(this), "Properties");
	this->addAsDockWidget(new NavigationWidget(this), "Properties");
	this->addAsDockWidget(new ConsoleWidget(this), "Utility");
	this->addAsDockWidget(new FrameTreeWidget(this), "Browsing");
	this->addAsDockWidget(new ToolManagerWidget(this), "Debugging");
	this->addAsDockWidget(new PluginFrameworkWidget(this), "Browsing");

	connect(patientService()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));
	connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));

	// insert all widgets from all guiExtenders
	for (unsigned i = 0; i < guiExtenders.size(); ++i)
		this->addGUIExtender(guiExtenders[i].get());

	this->setupGUIExtenders();

	// window menu must be created after all dock widgets are created
	QMenu* popupMenu = this->createPopupMenu();
	popupMenu->setTitle("Window");
	this->menuBar()->insertMenu(mHelpMenuAction, popupMenu);

	// Restore saved window states
	// Must be done after all DockWidgets are created
	if (!restoreGeometry(settings()->value("mainWindow/geometry").toByteArray()))
	{
		this->showMaximized();
	}
	else
	{
		this->show();
	}

	if (settings()->value("gui/fullscreen").toBool())
		this->setWindowState(this->windowState() | Qt::WindowFullScreen);

	QTimer::singleShot(0, this, SLOT(startupLoadPatient())); // make sure this is called after application state change
	this->toggleDebugModeSlot(mDebugModeAction->isChecked());
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
							   boost::bind(&MainWindow::onPluginBaseAdded, this, _1),
							   boost::bind(&MainWindow::onPluginBaseModified, this, _1),
								 boost::bind(&MainWindow::onPluginBaseRemoved, this, _1)
							   ));
	mServiceListener->open();
}

void MainWindow::addGUIExtender(GUIExtenderService* service)
{
	std::vector<GUIExtenderService::CategorizedWidget> widgets = service->createWidgets();
	for (unsigned j = 0; j < widgets.size(); ++j)
	{
		QWidget* widget = this->addCategorizedWidget(widgets[j]);
		mWidgetsByPlugin[service].push_back(widget);
	}
}

QWidget *MainWindow::addCategorizedWidget(GUIExtenderService::CategorizedWidget categorizedWidget)
{
	QWidget* retval;
	retval = this->addAsDockWidget(categorizedWidget.mWidget, categorizedWidget.mCategory);
	return retval;
}

void MainWindow::removeGUIExtender(GUIExtenderService* service)
{
	while (!mWidgetsByPlugin[service].empty())
	{
		// TODO: must remove widget from several difference data structures: simplify!
		QWidget* widget = mWidgetsByPlugin[service].back();
		mWidgetsByPlugin[service].pop_back();

		QDockWidget* dockWidget = dynamic_cast<QDockWidget*>(widget);
		this->removeDockWidget(dockWidget);

		mDockWidgets->erase(dockWidget);

		if (dockWidget)
		{
			for (std::map<QString, QActionGroup*>::iterator iter=mWidgetGroupsMap.begin(); iter!=mWidgetGroupsMap.end(); ++iter)
			{
				iter->second->removeAction(dockWidget->toggleViewAction());
			}
		}
	}
}

void MainWindow::onPluginBaseAdded(GUIExtenderService* service)
{
	this->addGUIExtender(service);
}

void MainWindow::onPluginBaseModified(GUIExtenderService* service)
{
}

void MainWindow::onPluginBaseRemoved(GUIExtenderService* service)
{
	this->removeGUIExtender(service);
}

/**Parse the command line and load a patient if the switch --patient is found
 */
void MainWindow::startupLoadPatient()
{
	patientService()->getPatientData()->startupLoadPatient();
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

//	std::cout << "**** new widget focus: " << sa->widget()->objectName() << std::endl;
}


void MainWindow::addToWidgetGroupMap(QAction* action, QString groupname)
{
	action->setMenuRole(QAction::NoRole);
	if (mWidgetGroupsMap.find(groupname) != mWidgetGroupsMap.end())
	{
		mWidgetGroupsMap[groupname]->addAction(action);
	}
	else
	{
		QActionGroup* group = new QActionGroup(this);
		group->setExclusive(false);
		mWidgetGroupsMap[groupname] = group;
		QAction* heading = new QAction(groupname, this);
		heading->setDisabled(true);
		mWidgetGroupsMap[groupname]->addAction(heading);
		mWidgetGroupsMap[groupname]->addAction(action);
	}
}

MainWindow::~MainWindow()
{
	mServiceListener.reset();
}

QMenu* MainWindow::createPopupMenu()
{
	QMenu* popupMenu = new QMenu(this);
	std::map<QString, QActionGroup*>::iterator it = mWidgetGroupsMap.begin();
	for (; it != mWidgetGroupsMap.end(); ++it)
	{
		popupMenu->addSeparator();
		popupMenu->addActions(it->second->actions());
	}

	return popupMenu;
}

void MainWindow::createActions()
{
	mStandard3DViewActions = mCameraControl->createStandard3DViewActions();

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

	connect(mNewPatientAction, SIGNAL(triggered()), this, SLOT(newPatientSlot()));
	connect(mLoadFileAction, SIGNAL(triggered()), this, SLOT(loadPatientFileSlot()));
	connect(mSaveFileAction, SIGNAL(triggered()), this, SLOT(savePatientFileSlot()));
	connect(mSaveFileAction, SIGNAL(triggered()), this, SLOT(saveDesktopSlot()));
	connect(mExportPatientAction, SIGNAL(triggered()), this, SLOT(exportDataSlot()));
	connect(mClearPatientAction, SIGNAL(triggered()), this, SLOT(clearPatientSlot()));

	mShowControlPanelAction = new QAction("Show Control Panel", this);
	connect(mShowControlPanelAction, SIGNAL(triggered()), this, SLOT(showControlPanelActionSlot()));
	mSecondaryViewLayoutWindowAction = new QAction("Show Secondary View Layout Window", this);
	connect(mSecondaryViewLayoutWindowAction, SIGNAL(triggered()), this, SLOT(showSecondaryViewLayoutWindowActionSlot()));

	// Application
	mAboutAction = new QAction(tr("About"), this);
	mAboutAction->setStatusTip(tr("Show the application's About box"));
	mPreferencesAction = new QAction(tr("Preferences"), this);
	mPreferencesAction->setStatusTip(tr("Show the preferences dialog"));

	mDebugModeAction = new QAction(tr("&Debug Mode"), this);
	mDebugModeAction->setShortcut(tr("Ctrl+D"));
	mDebugModeAction->setCheckable(true);
	mDebugModeAction->setChecked(dataManager()->getDebugMode());
	mDebugModeAction->setStatusTip(tr("Set debug mode, this enables lots of weird stuff."));
	connect(mDebugModeAction, SIGNAL(triggered(bool)), dataManager(), SLOT(setDebugMode(bool)));
	connect(dataManager(), SIGNAL(debugModeChanged(bool)), mDebugModeAction, SLOT(setChecked(bool)));
	connect(mDebugModeAction, SIGNAL(toggled(bool)), this, SLOT(toggleDebugModeSlot(bool)));

	mFullScreenAction = new QAction(tr("Fullscreen"), this);
	mFullScreenAction->setShortcut(tr("F11"));
	mFullScreenAction->setStatusTip(tr("Toggle full screen"));
	mFullScreenAction->setCheckable(true);
	mFullScreenAction->setChecked(this->windowState() & Qt::WindowFullScreen);
	connect(mFullScreenAction, SIGNAL(triggered()), this, SLOT(toggleFullScreenSlot()));

	mQuitAction = new QAction(tr("&Quit"), this);
	mQuitAction->setShortcut(tr("Ctrl+Q"));
	mQuitAction->setStatusTip(tr("Exit the application"));

	connect(mAboutAction, SIGNAL(triggered()), this, SLOT(aboutSlot()));
	connect(mPreferencesAction, SIGNAL(triggered()), this, SLOT(preferencesSlot()));
	connect(mQuitAction, SIGNAL(triggered()), this, SLOT(quitSlot()));

	mShootScreenAction = new QAction(tr("Shoot Screen"), this);
	mShootScreenAction->setIcon(QIcon(":/icons/screenshot-screen.png"));
	mShootScreenAction->setShortcut(tr("Ctrl+f"));
	mShootScreenAction->setStatusTip(tr("Save a screenshot to the patient folder."));
	connect(mShootScreenAction, SIGNAL(triggered()), this, SLOT(shootScreen()));

	mShootWindowAction = new QAction(tr("Shoot Window"), this);
	mShootWindowAction->setIcon(QIcon(":/icons/screenshot-window.png"));
	mShootWindowAction->setShortcut(tr("Ctrl+Shift+f"));
	mShootWindowAction->setStatusTip(tr("Save an image of the application to the patient folder."));
	connect(mShootWindowAction, SIGNAL(triggered()), this, SLOT(shootWindow()));

	mRecordFullscreenAction = new QAction(tr("Record Fullscreen"), this);
	mRecordFullscreenAction->setShortcut(tr("F8"));
	mRecordFullscreenAction->setStatusTip(tr("Record a video of the full screen."));
	connect(mRecordFullscreenAction, SIGNAL(triggered()), this, SLOT(recordFullscreen()));

	//data
	mImportDataAction = new QAction(QIcon(":/icons/open_icon_library/document-import-2.png"), tr("&Import data"), this);
	mImportDataAction->setShortcut(tr("Ctrl+I"));
	mImportDataAction->setStatusTip(tr("Import image data"));

	mDeleteDataAction = new QAction(tr("Delete current image"), this);
	mDeleteDataAction->setStatusTip(tr("Delete selected volume"));

	connect(mImportDataAction, SIGNAL(triggered()), this, SLOT(importDataSlot()));
	connect(mDeleteDataAction, SIGNAL(triggered()), this, SLOT(deleteDataSlot()));

	mShowPointPickerAction = new QAction(tr("Point Picker"), this);
	mShowPointPickerAction->setCheckable(true);
	mShowPointPickerAction->setToolTip("Activate the 3D Point Picker Probe");
	mShowPointPickerAction->setIcon(QIcon(":/icons/point_picker.png"));
	connect(mShowPointPickerAction, SIGNAL(triggered()), this, SLOT(togglePointPickerActionSlot()));

	connect(viewManager()->getViewGroups()[0]->getData().get(), SIGNAL(optionsChanged()), this,
//	connect(mVisualizationService->getViewGroupData(0).get(), SIGNAL(optionsChanged()), this, //Too early?
		SLOT(updatePointPickerActionSlot()));
	this->updatePointPickerActionSlot();

	//tool
	mToolsActionGroup = new QActionGroup(this);
	mConfigureToolsAction = new QAction(tr("Tool configuration"), mToolsActionGroup);
	mInitializeToolsAction = new QAction(tr("Initialize"), mToolsActionGroup);
	mTrackingToolsAction = new QAction(tr("Start tracking"), mToolsActionGroup);
	mTrackingToolsAction->setShortcut(tr("Ctrl+T"));
	mSaveToolsPositionsAction = new QAction(tr("Save positions"), this);

	mToolsActionGroup->setExclusive(false); // must turn off to get the checkbox independent.

	mStartStreamingAction = new QAction(tr("Start Streaming"), mToolsActionGroup);
	mStartStreamingAction->setShortcut(tr("Ctrl+V"));
	connect(mStartStreamingAction, SIGNAL(triggered()), this, SLOT(toggleStreamingSlot()));
	connect(videoService()->getVideoConnection().get(), SIGNAL(connected(bool)), this, SLOT(updateStreamingActionSlot()));
	this->updateStreamingActionSlot();

	mConfigureToolsAction->setChecked(true);

//	connect(mConfigureToolsAction, &QAction::triggered, this, boost::bind(&MainWindow::setState, this, Tool::tsCONFIGURED));
	connect(mConfigureToolsAction, SIGNAL(triggered()), this, SLOT(configureSlot()));
	boost::function<void()> finit = boost::bind(&ToolManager::setState, trackingService(), Tool::tsINITIALIZED);
	connect(mInitializeToolsAction, &QAction::triggered, finit);
//	connect(mTrackingToolsAction, SIGNAL(triggered()), this, SLOT(toggleTrackingSlot()));
	boost::function<void()> fsavetools = boost::bind(&ToolManager::savePositionHistory, trackingService());
	connect(mSaveToolsPositionsAction, &QAction::triggered, fsavetools);
	connect(trackingService().get(), SIGNAL(stateChanged()), this, SLOT(updateTrackingActionSlot()));
	connect(trackingService().get(), SIGNAL(stateChanged()), this, SLOT(updateTrackingActionSlot()));
	this->updateTrackingActionSlot();

	mCenterToImageCenterAction = new QAction(tr("Center Image"), this);
	mCenterToImageCenterAction->setIcon(QIcon(":/icons/center_image.png"));
	connect(mCenterToImageCenterAction, SIGNAL(triggered()), this, SLOT(centerToImageCenterSlot()));
	mCenterToTooltipAction = new QAction(tr("Center Tool"), this);
	mCenterToTooltipAction->setIcon(QIcon(":/icons/center_tool.png"));
	connect(mCenterToTooltipAction, SIGNAL(triggered()), this, SLOT(centerToTooltipSlot()));

	mSaveDesktopAction = new QAction(QIcon(":/icons/workflow_state_save.png"), tr("Save desktop"), this);
	mSaveDesktopAction->setToolTip("Save desktop for workflow step");
	connect(mSaveDesktopAction, SIGNAL(triggered()), this, SLOT(saveDesktopSlot()));
	mResetDesktopAction = new QAction(QIcon(":/icons/workflow_state_revert.png"), tr("Reset desktop"), this);
	mResetDesktopAction->setToolTip("Reset desktop for workflow step");
	connect(mResetDesktopAction, SIGNAL(triggered()), this, SLOT(resetDesktopSlot()));

	mInteractorStyleActionGroup = viewManager()->createInteractorStyleActionGroup();
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
//		std::cout << "screen id " << screens[i]->name() << std::endl;
//		std::cout << "screen get " << geo.left() <<","<< geo.top() <<","<< geo.width() <<","<< geo.height() << std::endl;
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
	QString path = patientService()->getPatientData()->generateFilePath("Screenshots", "mp4");

	if(vlc()->isRecording())
		vlc()->stopRecording();
	else
		vlc()->startRecording(path);

}

void MainWindow::toggleDebugModeSlot(bool checked)
{
	QActionGroup* debugActionGroup;
	if (mWidgetGroupsMap.find("Debugging") != mWidgetGroupsMap.end())
	{
		debugActionGroup = mWidgetGroupsMap["Debugging"];
	}
	else
		return;

	QList<QAction*> debugActions = debugActionGroup->actions();
	QAction* action;
	foreach(action, debugActions)
		{
			action->setVisible(checked);
			this->mDockWidgets->toggleDebug(action, checked);
		}
}
void MainWindow::saveScreenShot(QPixmap pixmap, QString id)
{
	QString ending = "png";
	if (!id.isEmpty())
		ending = id + "." + ending;
	QString path = patientService()->getPatientData()->generateFilePath("Screenshots", ending);
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
	if (videoService()->getVideoConnection()->isConnected())
		videoService()->getVideoConnection()->disconnectServer();
	else
		videoService()->getVideoConnection()->launchAndConnectServer(mVideoService);
}

void MainWindow::updateStreamingActionSlot()
{
	if (videoService()->getVideoConnection()->isConnected())
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
	NavigationPtr nav = viewManager()->getNavigation();

	if (dataManager()->getActiveImage())
		nav->centerToData(dataManager()->getActiveImage());
	else if (!viewManager()->getViewGroups().empty())
		nav->centerToView(viewManager()->getViewGroups()[0]->getData()->getData());
//		nav->centerToView(mVisualizationService->getViewGroupData(0)->getData());//Too early?
	else
		nav->centerToGlobalDataCenter();
}

void MainWindow::centerToTooltipSlot()
{
	NavigationPtr nav = viewManager()->getNavigation();
	nav->centerToTooltip();
}

void MainWindow::togglePointPickerActionSlot()
{
	ViewGroupDataPtr data = viewManager()->getViewGroups()[0]->getData();
//	ViewGroupDataPtr data = mVisualizationService->getViewGroupData(0); //Too early?
	ViewGroupData::Options options = data->getOptions();
	options.mShowPointPickerProbe = !options.mShowPointPickerProbe;
	data->setOptions(options);
}
void MainWindow::updatePointPickerActionSlot()
{
	bool show = viewManager()->getViewGroups()[0]->getData()->getOptions().mShowPointPickerProbe;
//	bool show = mVisualizationService->getViewGroupData(0)->getOptions().mShowPointPickerProbe;//TOO early?
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
	QString patientDatafolder = settings()->value("globalPatientDataFolder").toString();

	// Create folders
	if (!QDir().exists(patientDatafolder))
	{
		QDir().mkdir(patientDatafolder);
		report("Made a new patient folder: " + patientDatafolder);
	}

	QString timestamp = QDateTime::currentDateTime().toString(timestampFormatFolderFriendly()) + "_";
	QString postfix = settings()->value("globalApplicationName").toString() + "_" + settings()->value("globalPatientNumber").toString() + ".cx3";

	QString choosenDir = patientDatafolder + "/" + timestamp + postfix;

	QFileDialog dialog(this, tr("Select directory to save patient in"), patientDatafolder + "/");
	dialog.setOption(QFileDialog::DontUseNativeDialog, true);
	dialog.setOption(QFileDialog::ShowDirsOnly, true);
	dialog.selectFile(timestamp + postfix);
	if (!dialog.exec())
		return;
	choosenDir = dialog.selectedFiles().front();

	if (!choosenDir.endsWith(".cx3"))
		choosenDir += QString(".cx3");

	// Update global patient number
	int patientNumber = settings()->value("globalPatientNumber").toInt();
	settings()->setValue("globalPatientNumber", ++patientNumber);

	patientService()->getPatientData()->newPatient(choosenDir);
	patientService()->getPatientData()->writeRecentPatientData();
}

void MainWindow::clearPatientSlot()
{
	patientService()->getPatientData()->clearPatient();
	patientService()->getPatientData()->writeRecentPatientData();
	reportWarning("Cleared current patient data");
}

void MainWindow::savePatientFileSlot()
{
	if (patientService()->getPatientData()->getActivePatientFolder().isEmpty())
	{
		reportWarning("No patient selected, select or create patient before saving!");
		this->newPatientSlot();
		return;
	}

	patientService()->getPatientData()->savePatient();
	patientService()->getPatientData()->writeRecentPatientData();
}

void MainWindow::onApplicationStateChangedSlot()
{
	this->updateWindowTitle();
}

void MainWindow::updateWindowTitle()
{
	QString appName;
	if (stateService()->getApplication())
		appName = stateService()->getApplication()->getActiveStateName();

	QString versionName = stateService()->getVersionName();

	QString activePatientFolder = patientService()->getPatientData()->getActivePatientFolder();
	if (activePatientFolder.endsWith('/'))
		activePatientFolder.chop(1);
	QString patientName;

	if (!activePatientFolder.isEmpty())
	{
		QFileInfo info(activePatientFolder);
		patientName = info.completeBaseName();
	}

	this->setWindowTitle("CustusX " + versionName + " - " + appName + " - " + patientName);
}

void MainWindow::onWorkflowStateChangedSlot()
{
	Desktop desktop = stateService()->getActiveDesktop();

	this->mDockWidgets->hideAll();

	viewManager()->setActiveLayout(desktop.mLayoutUid, 0);
	viewManager()->setActiveLayout(desktop.mSecondaryLayoutUid, 1);
	this->restoreState(desktop.mMainWindowState);
	patientService()->getPatientData()->autoSave();
}

void MainWindow::saveDesktopSlot()
{
	Desktop desktop;
	desktop.mMainWindowState = this->saveState();
	desktop.mLayoutUid = viewManager()->getActiveLayout(0);
	desktop.mSecondaryLayoutUid = viewManager()->getActiveLayout(1);
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
	QString patientDatafolder = settings()->value("globalPatientDataFolder").toString();
	// Create folder
	if (!QDir().exists(patientDatafolder))
	{
		QDir().mkdir(patientDatafolder);
		report("Made a new patient folder: " + patientDatafolder);
	}
	// Open file dialog
//	std::cout << "dir: " << string_cast(patientDatafolder) << std::endl;
	QString choosenDir = QFileDialog::getExistingDirectory(this, tr("Select patient"), patientDatafolder,
		QFileDialog::ShowDirsOnly);
	if (choosenDir == QString::null)
		return; // On cancel

	patientService()->getPatientData()->loadPatient(choosenDir);
	patientService()->getPatientData()->writeRecentPatientData();
}

void MainWindow::exportDataSlot()
{
	this->savePatientFileSlot();

	ExportDataDialog* wizard = new ExportDataDialog(mPatientModelService, this);
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
		ImportDataDialog* wizard = new ImportDataDialog(mPatientModelService, fileName[i], this);
		wizard->exec(); //calling exec() makes the wizard dialog modal which prevents other user interaction with the system
	}
}

void MainWindow::patientChangedSlot()
{
	this->updateWindowTitle();
}

void MainWindow::createMenus()
{
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
	mFileMenu->addAction(mDebugModeAction);
	mFileMenu->addAction(mShootScreenAction);
	mFileMenu->addAction(mShootWindowAction);
	mFileMenu->addAction(mRecordFullscreenAction);
	mFileMenu->addSeparator();
	mFileMenu->addAction(mShowControlPanelAction);
	mFileMenu->addAction(mSecondaryViewLayoutWindowAction);

	mFileMenu->addAction(mQuitAction);

	//workflow
	this->menuBar()->addMenu(mWorkflowMenu);
	stateService()->getWorkflow()->fillMenu(mWorkflowMenu);

	QList<QAction *> actions = mWorkflowMenu->actions();
	for (int i = 1; i <= actions.size(); ++i)
	{
		QString shortcut = "Ctrl+" + QString::number(i);
		actions[i - 1]->setShortcut(shortcut);
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
	mToolMenu->addAction(mSaveToolsPositionsAction);
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
	mHelpMenu->addAction(QWhatsThis::createAction(this));
}

void MainWindow::createToolBars()
{
	mDataToolBar = addToolBar("Data");
	mDataToolBar->setObjectName("DataToolBar");
	mDataToolBar->addAction(mNewPatientAction);
	mDataToolBar->addAction(mLoadFileAction);
	mDataToolBar->addAction(mSaveFileAction);
	mDataToolBar->addAction(mImportDataAction);
	this->registerToolBar(mDataToolBar, "Toolbar");

	mToolToolBar = addToolBar("Tools");
	mToolToolBar->setObjectName("ToolToolBar");
	mToolToolBar->addAction(mTrackingToolsAction);
	mToolToolBar->addAction(mStartStreamingAction);
	this->registerToolBar(mToolToolBar, "Toolbar");

	mNavigationToolBar = addToolBar("Navigation");
	mNavigationToolBar->setObjectName("NavigationToolBar");
	mNavigationToolBar->addAction(mCenterToImageCenterAction);
	mNavigationToolBar->addAction(mCenterToTooltipAction);
	mNavigationToolBar->addAction(mShowPointPickerAction);
	this->registerToolBar(mNavigationToolBar, "Toolbar");

	mInteractorStyleToolBar = addToolBar("InteractorStyle");
	mInteractorStyleToolBar->setObjectName("InteractorStyleToolBar");
	mInteractorStyleToolBar->addActions(mInteractorStyleActionGroup->actions());
	this->registerToolBar(mInteractorStyleToolBar, "Toolbar");

	mWorkflowToolBar = addToolBar("Workflow");
	mWorkflowToolBar->setObjectName("WorkflowToolBar");
	stateService()->getWorkflow()->fillToolBar(mWorkflowToolBar);
	this->registerToolBar(mWorkflowToolBar, "Toolbar");

	mDesktopToolBar = addToolBar("Desktop");
	mDesktopToolBar->setObjectName("DesktopToolBar");
	mDesktopToolBar->addAction(mSaveDesktopAction);
	mDesktopToolBar->addAction(mResetDesktopAction);
	this->registerToolBar(mDesktopToolBar, "Toolbar");

	mHelpToolBar = addToolBar("Help");
	mHelpToolBar->setObjectName("HelpToolBar");
	mHelpToolBar->addAction(QWhatsThis::createAction(this));
	this->registerToolBar(mHelpToolBar, "Toolbar");

	mScreenshotToolBar = addToolBar("Screenshot");
	mScreenshotToolBar->setObjectName("ScreenshotToolBar");
	mScreenshotToolBar->addAction(mShootScreenAction);
	this->registerToolBar(mScreenshotToolBar, "Toolbar");

	QToolBar* camera3DViewToolBar = addToolBar("Camera 3D Views");
	camera3DViewToolBar->setObjectName("Camera3DViewToolBar");
	camera3DViewToolBar->addActions(mStandard3DViewActions->actions());
	this->registerToolBar(camera3DViewToolBar, "Toolbar");

	QToolBar* samplerWidgetToolBar = addToolBar("Sampler");
	samplerWidgetToolBar->setObjectName("SamplerToolBar");
	samplerWidgetToolBar->addWidget(new SamplerWidget(this));
	this->registerToolBar(samplerWidgetToolBar, "Toolbar");

	QToolBar* toolOffsetToolBar = addToolBar("Tool Offset");
	toolOffsetToolBar->setObjectName("ToolOffsetToolBar");
	toolOffsetToolBar->addWidget(createDataWidget(mVisualizationService, mPatientModelService, this, DoubleDataAdapterActiveToolOffset::create()));
	this->registerToolBar(toolOffsetToolBar, "Toolbar");

//	QToolBar* registrationHistoryToolBar = addToolBar("Registration History");
//	registrationHistoryToolBar->setObjectName("RegistrationHistoryToolBar");
//	registrationHistoryToolBar->addWidget(new RegistrationHistoryWidget(this, true));
//	this->registerToolBar(registrationHistoryToolBar, "Toolbar");
}

void MainWindow::registerToolBar(QToolBar* toolbar, QString groupname)
{
	this->addToWidgetGroupMap(toolbar->toggleViewAction(), groupname);
}

void MainWindow::aboutSlot()
{
	QString doc_path = DataLocations::getDocPath();
	QString url_github("https://github.com/SINTEFMedisinskTeknologi/CustusX3");
	QString url_license = QString("file://%1/license.txt").arg(doc_path);
	QString url_config = QString("file://%1/cxConfigDescription.txt").arg(doc_path);

	QString text(""
	"<h2>CustusX</h2>"
	"<h4>%1</h4>"
	"<p>A Research Platform for Image-Guided Therapy<p>"
	"<p>CustusX is NOT approved for medical use.<p>"
	""
	"<p><a href=%2> website </a><p>"
	"<p><a href=%3> license </a><p>"
	"<p><a href=%4> configuration </a><p>");

	QMessageBox::about(this, tr("About CustusX"), text
			.arg(CustusX_VERSION_STRING)
			.arg(url_github)
			.arg(url_license)
			.arg(url_config)
			);

//	QMessageBox::about(this, tr("About CustusX"), tr("<h2>CustusX version %1</h2> "
//		"<p>Created by SINTEF Medical Technology."
//		"<p><a href=http://www.sintef.no/Home/Technology-and-Society/Medical-technology> www.sintef.no </a>"
//		"<p>An application for Image Guided Surgery."
//		"<p>Created using Qt, VTK, ITK, IGSTK, CTK.").arg(CustusX_VERSION_STRING));
}

void MainWindow::preferencesSlot()
{
	PreferencesDialog prefDialog(mVisualizationService, mPatientModelService, this);
	prefDialog.exec();
}

void MainWindow::quitSlot()
{
	report("Shutting down CustusX");
	viewManager()->deactivateCurrentLayout();

	patientService()->getPatientData()->autoSave();

	settings()->setValue("mainWindow/geometry", saveGeometry());
	settings()->setValue("mainWindow/windowState", saveState());
	settings()->sync();
	report("Closing: Save geometry and window state");

	qApp->quit();
}

void MainWindow::deleteDataSlot()
{
	if (!dataManager()->getActiveImage())
		return;
	QString text = QString("Do you really want to delete data %1?").arg(dataManager()->getActiveImage()->getName());
	if (QMessageBox::question(this, "Data delete", text, QMessageBox::StandardButtons(QMessageBox::Ok | QMessageBox::Cancel))!=QMessageBox::Ok)
		return;
	mPatientModelService->removePatientData(dataManager()->getActiveImage()->getUid());
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
	QDockWidget* dockWidget = mDockWidgets->addAsDockWidget(widget, groupname);
	this->addToWidgetGroupMap(dockWidget->toggleViewAction(), groupname);
	QMainWindow::addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
	this->restoreDockWidget(dockWidget); // restore if added after construction
	return dockWidget;
}

}//namespace cx
