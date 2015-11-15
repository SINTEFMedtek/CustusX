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
#include "cxLogicManager.h"
#include "cxSettings.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxSessionStorageService.h"
#include "cxAudioImpl.h"
#include "cxLayoutInteractor.h"
#include "cxCameraControl.h"

#include "cxDynamicMainWindowWidgets.h"
#include "cxStatusBar.h"
#include "cxSecondaryMainWindow.h"
#include "cxSecondaryViewLayoutWindow.h"
#include "cxSamplerWidget.h"
#include "cxHelperWidgets.h"
#include "cxMainWindowActions.h"

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
#include "cxFiltersWidget.h"
#include "cxPluginFrameworkWidget.h"
#include "cxManageClippersWidget.h"

namespace cx
{

MainWindow::MainWindow() :
	mFullScreenAction(NULL),
	mStandard3DViewActions(new QActionGroup(this)),
	mControlPanel(NULL),
	mDockWidgets(new DynamicMainWindowWidgets(this)),
	mActions(NULL)
{
	this->setObjectName("MainWindow");

	mServices = VisServices::create(logicManager()->getPluginContext());
	mLayoutInteractor.reset(new LayoutInteractor());

    this->setCentralWidget(viewService()->createLayoutWidget(this, 0));

	mActions = new MainWindowActions(mServices, this);

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
	this->addAsDockWidget(new EraserWidget(mServices->patient(), mServices->view(), this), "Properties");
	this->addAsDockWidget(new MetricWidget(mServices->view(), mServices->patient(), this), "Utility");
	this->addAsDockWidget(new SlicePropertiesWidget(mServices->patient(), mServices->view(), this), "Properties");
	this->addAsDockWidget(new VolumePropertiesWidget(mServices, this), "Properties");
	this->addAsDockWidget(new MeshInfoWidget(mServices->patient(), mServices->view(), this), "Properties");
	this->addAsDockWidget(new StreamPropertiesWidget(mServices->patient(), mServices->view(), this), "Properties");
	this->addAsDockWidget(new TrackPadWidget(this), "Utility");
	this->addAsDockWidget(new ToolPropertiesWidget(this), "Properties");
	this->addAsDockWidget(new NavigationWidget(this), "Properties");
	this->addAsDockWidget(new ConsoleWidget(this, "ConsoleWidget", "Console"), "Utility");
	this->addAsDockWidget(new ConsoleWidget(this, "ConsoleWidget2", "Extra Console"), "Utility");
//	this->addAsDockWidget(new ConsoleWidgetCollection(this, "ConsoleWidgets", "Consoles"), "Utility");
	this->addAsDockWidget(new FrameTreeWidget(mServices->patient(), this), "Browsing");
	this->addAsDockWidget(new ToolManagerWidget(this), "Debugging");
	this->addAsDockWidget(new PluginFrameworkWidget(this), "Browsing");
    this->addAsDockWidget(new FiltersWidget(VisServices::create(logicManager()->getPluginContext()), this), "Algorithms");
	this->addAsDockWidget(new ClippingPropertiesWidget(mServices, this), "Properties");

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
	if (!dockWidget)
		return;

//	CX_LOG_CHANNEL_DEBUG("HELP_DB") << QString("    try mw::focus [%1](%2)")
//									   .arg(dockWidget->objectName())
//									   .arg(dockWidget->metaObject()->className());

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

	if (!sa->widget())
		return;

//	CX_LOG_CHANNEL_DEBUG("HELP_DB") << QString("    do mw::focus [%1](%2)")
//									   .arg(sa->widget()->objectName())
//									   .arg(sa->widget()->metaObject()->className());

	QTimer::singleShot(0, sa->widget(), SLOT(setFocus())); // avoid loops etc by send async event.
}

MainWindow::~MainWindow()
{
	viewService()->deactivateLayout();
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

	mShowContextSensitiveHelpAction = new QAction(QIcon(":/icons/open_icon_library/help-contents-5.png"),
												  "Context-sensitive help", this);
	connect(mShowContextSensitiveHelpAction, &QAction::triggered, this, &MainWindow::onShowContextSentitiveHelp);

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
	connect(mQuitAction, &QAction::triggered, qApp, &QApplication::quit);

	mSaveDesktopAction = new QAction(QIcon(":/icons/workflow_state_save.png"), tr("Save desktop"), this);
	mSaveDesktopAction->setToolTip("Save desktop for workflow step");
	connect(mSaveDesktopAction, &QAction::triggered, this, &MainWindow::saveDesktopSlot);
	mResetDesktopAction = new QAction(QIcon(":/icons/workflow_state_revert.png"), tr("Reset desktop"), this);
	mResetDesktopAction->setToolTip("Reset desktop for workflow step");
	connect(mResetDesktopAction, &QAction::triggered, this, &MainWindow::resetDesktopSlot);

	mInteractorStyleActionGroup = viewService()->createInteractorStyleActionGroup();

	// cross-connect save patient to save session
	connect(mServices->session().get(), &SessionStorageService::isSaving, this, &MainWindow::saveDesktopSlot);
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

	// save to settings file in addition
	settings()->setValue("mainWindow/geometry", saveGeometry());
	settings()->setValue("mainWindow/windowState", saveState());
	settings()->sync();
}

void MainWindow::resetDesktopSlot()
{
	stateService()->resetDesktop();
	this->onWorkflowStateChangedSlot();
}

void MainWindow::onShowContextSentitiveHelp()
{
	mDockWidgets->showWidget("Help");
}

void MainWindow::toggleFullScreenSlot()
{
	this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);

	settings()->setValue("gui/fullscreen", (this->windowState() & Qt::WindowFullScreen)!=0);
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
	mFileMenu->addAction(mActions->getAction("NewPatient"));
	mFileMenu->addAction(mActions->getAction("SaveFile"));
	mFileMenu->addAction(mActions->getAction("LoadFile"));
	mFileMenu->addAction(mActions->getAction("ClearPatient"));
	mFileMenu->addSeparator();
	mFileMenu->addAction(mActions->getAction("ExportPatient"));
	mFileMenu->addAction(mActions->getAction("ImportData"));
	mFileMenu->addSeparator();
	mFileMenu->addAction(mFullScreenAction);
	mFileMenu->addAction(mActions->getAction("StartLogConsole"));
	mFileMenu->addAction(mActions->getAction("ShootScreen"));
	mFileMenu->addAction(mActions->getAction("ShootWindow"));
	mFileMenu->addAction(mActions->getAction("RecordFullscreen"));
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
	mToolMenu->addAction(mActions->getAction("ConfigureTools"));
	mToolMenu->addAction(mActions->getAction("InitializeTools"));
	mToolMenu->addAction(mActions->getAction("TrackingTools"));
	mToolMenu->addSeparator();
	mToolMenu->addAction(mActions->getAction("StartStreaming"));
	mToolMenu->addSeparator();

	//layout
	this->menuBar()->addMenu(mLayoutMenu);
	mLayoutInteractor->connectToMenu(mLayoutMenu);

	this->menuBar()->addMenu(mNavigationMenu);
	mNavigationMenu->addAction(mActions->getAction("CenterToImageCenter"));
	mNavigationMenu->addAction(mActions->getAction("CenterToTooltip"));
	mNavigationMenu->addAction(mActions->getAction("ShowPointPicker"));
	mNavigationMenu->addSeparator();
	mNavigationMenu->addActions(mInteractorStyleActionGroup->actions());

	mHelpMenuAction = this->menuBar()->addMenu(mHelpMenu);
	mHelpMenu->addAction(mAboutAction);
	mHelpMenu->addAction(mActions->getAction("GotoDocumentation"));
	mHelpMenu->addAction(mShowContextSensitiveHelpAction);
}

void MainWindow::createToolBars()
{
	mWorkflowToolBar = this->registerToolBar("Workflow");
	QList<QAction*> actions = stateService()->getWorkflowActions()->actions();
	for (int i=0; i<actions.size(); ++i)
		mWorkflowToolBar->addAction(actions[i]);

	mDataToolBar = this->registerToolBar("Data");
	mDataToolBar->addAction(mActions->getAction("NewPatient"));
	mDataToolBar->addAction(mActions->getAction("LoadFile"));
	mDataToolBar->addAction(mActions->getAction("SaveFile"));
	mDataToolBar->addAction(mActions->getAction("ImportData"));

	mToolToolBar = this->registerToolBar("Tools");
	mToolToolBar->addAction(mActions->getAction("TrackingTools"));
	mToolToolBar->addAction(mActions->getAction("StartStreaming"));

	mNavigationToolBar = this->registerToolBar("Navigation");
	mNavigationToolBar->addAction(mActions->getAction("CenterToImageCenter"));
	mNavigationToolBar->addAction(mActions->getAction("CenterToTooltip"));
	mNavigationToolBar->addAction(mActions->getAction("ShowPointPicker"));

	mInteractorStyleToolBar = this->registerToolBar("InteractorStyle");
	mInteractorStyleToolBar->addActions(mInteractorStyleActionGroup->actions());

	mDesktopToolBar = this->registerToolBar("Desktop");
	mDesktopToolBar->addAction(mSaveDesktopAction);
	mDesktopToolBar->addAction(mResetDesktopAction);

	mScreenshotToolBar = this->registerToolBar("Screenshot");
	mScreenshotToolBar->addAction(mActions->getAction("ShootScreen"));
	mScreenshotToolBar->addAction(mActions->getAction("RecordFullscreen"));

	QToolBar* camera3DViewToolBar = this->registerToolBar("Camera 3D Views");
	camera3DViewToolBar->addActions(mStandard3DViewActions->actions());

	QToolBar* samplerWidgetToolBar = this->registerToolBar("Sampler");
	samplerWidgetToolBar->addWidget(new SamplerWidget(this));

	QToolBar* toolOffsetToolBar = this->registerToolBar("Tool Offset");
	SpinBoxAndSliderGroupWidget* offsetWidget = new SpinBoxAndSliderGroupWidget(this, DoublePropertyActiveToolOffset::create());
	offsetWidget->showLabel(false);
	toolOffsetToolBar->addWidget(offsetWidget);

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
	QString url_license = QString("file://%1/License.txt").arg(doc_path);
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
	PreferencesDialog prefDialog(mServices->view(), mServices->patient(), this);
	prefDialog.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QMainWindow::closeEvent(event);
	qApp->quit();
}

QDockWidget* MainWindow::addAsDockWidget(QWidget* widget, QString groupname)
{
	QDockWidget* dw = mDockWidgets->addAsDockWidget(widget, groupname);
	return dw;

}

}//namespace cx
