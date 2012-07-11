#include "cxMainWindow.h"

#include <QtGui>
#include <QWhatsThis>
#include "boost/scoped_ptr.hpp"
#include "boost/bind.hpp"
#include "sscTime.h"
#include "sscMessageManager.h"
#include "cxDataManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxToolManager.h"
#include "cxStatusBar.h"
#include "cxVolumePropertiesWidget.h"
#include "cxBrowserWidget.h"
#include "cxNavigationWidget.h"
#include "cxTabbedWidget.h"
#include "cxToolPropertiesWidget.h"
#include "cxViewGroup.h"
#include "cxPreferencesDialog.h"
#include "cxImagePropertiesWidget.h"
//#include "cxPointSamplingWidget.h"
#include "cxPatientData.h"
#include "cxDataLocations.h"
#include "cxMeshPropertiesWidget.h"
#include "cxLayoutEditorWidget.h"
#include "cxFrameForest.h"
#include "cxFrameTreeWidget.h"
#include "cxImportDataDialog.h"
#include "cxTrackPadWidget.h"
#include "cxCameraControl.h"
#include "cxSecondaryMainWindow.h"
#include "cxIGTLinkWidget.h"
#include "cxAudio.h"
#include "cxSettings.h"
#include "cxVideoConnection.h"
#include "cxToolManagerWidget.h"
#include "cxVideoService.h"
#include "cxLogicManager.h"

#include "sscGPUImageBuffer.h"
#include "sscData.h"
#include "sscConsoleWidget.h"
#include "cxViewManager.h"
#include "cxStateService.h"
#include "cxPatientService.h"
#include "cxMetricWidget.h"
#include "cxViewWrapper.h"
#include "sscDICOMWidget.h"
#include "cxPlaybackWidget.h"
#include "cxEraserWidget.h"

#include "sscDICOMLibConfig.h"

namespace cx
{

MainWindow::MainWindow(std::vector<PluginBasePtr> plugins) :
	mFullScreenAction(NULL), mStandard3DViewActions(NULL), mControlPanel(NULL)
{
	QFile stylesheet(":/cxStyleSheet.ss");
	stylesheet.open(QIODevice::ReadOnly);
//	std::cout << QString(stylesheet.readAll()) << std::endl;
	qApp->setStyleSheet(stylesheet.readAll());

	// insert all widgets from all plugins
	for (unsigned i = 0; i < plugins.size(); ++i)
	{
		std::vector<PluginBase::PluginWidget> widgets = plugins[i]->createWidgets();
		for (unsigned i = 0; i < widgets.size(); ++i)
		{
			this->addAsDockWidget(widgets[i].mWidget, widgets[i].mCategory);
		}

	}

	mCameraControl.reset(new CameraControl(this));

	this->createActions();
	this->createMenus();
	this->createToolBars();
	this->setStatusBar(new StatusBar());

	ssc::messageManager()->setLoggingFolder(DataLocations::getRootConfigPath());
	ssc::messageManager()->setAudioSource(ssc::AudioPtr(new Audio()));

	connect(stateService()->getApplication().get(), SIGNAL(activeStateChanged()), this,
		SLOT(onApplicationStateChangedSlot()));
	connect(stateService()->getWorkflow().get(), SIGNAL(activeStateChanged()), this, SLOT(onWorkflowStateChangedSlot()));
	connect(stateService()->getWorkflow().get(), SIGNAL(activeStateAboutToChange()), this, SLOT(saveDesktopSlot()));

	mLayoutActionGroup = NULL;
	this->updateWindowTitle();

	this->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);


	this->addAsDockWidget(new PlaybackWidget(this), "Browsing");
	this->addAsDockWidget(new IGTLinkWidget(this), "Utility");
	this->addAsDockWidget(new EraserWidget(this), "Properties");
	this->addAsDockWidget(new BrowserWidget(this), "Browsing");
//	this->addAsDockWidget(new PointSamplingWidget(this), "Utility");
	this->addAsDockWidget(new MetricWidget(this), "Utility");
	this->addAsDockWidget(new ImagePropertiesWidget(this), "Properties");
	this->addAsDockWidget(new VolumePropertiesWidget(this), "Properties");
	this->addAsDockWidget(new MeshPropertiesWidget(this), "Properties");
#ifdef SSC_USE_DCMTK
	this->addAsDockWidget(new ssc::DICOMWidget(this), "Utility");
#endif // SSC_USE_DCMTK
	this->addAsDockWidget(new TrackPadWidget(this), "Utility");
	this->addAsDockWidget(new ToolPropertiesWidget(this), "Properties");
	this->addAsDockWidget(new NavigationWidget(this), "Properties");
	this->addAsDockWidget(new ssc::ConsoleWidget(this), "Utility");
	this->addAsDockWidget(new FrameTreeWidget(this), "Browsing");
	this->addAsDockWidget(new ToolManagerWidget(this), "Debugging");

	this->setCentralWidget(viewManager()->stealCentralWidget());

	connect(patientService()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));

	connect(viewManager(), SIGNAL(activeLayoutChanged()), this, SLOT(layoutChangedSlot()));
	this->layoutChangedSlot();

	// Restore saved window states
	// Must be done after all DockWidgets are created
	if (!restoreGeometry(settings()->value("mainWindow/geometry").toByteArray()))
		this->showMaximized();
	else
		this->show();

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

/**Parse the command line and load a patient if the switch --patient is found
 */
void MainWindow::startupLoadPatient()
{
	int doLoad = QApplication::arguments().indexOf("--load");
	if (doLoad < 0)
		return;
//	std::cout << "!!!!!!!!!!!!!! load " << doLoad << std::endl;
	if (doLoad + 1 >= QApplication::arguments().size())
		return;

	QString folder = QApplication::arguments()[doLoad + 1];
	ssc::messageManager()->sendInfo("Startup Load patient: " + folder);
	patientService()->getPatientData()->loadPatient(folder);
}

void MainWindow::addAsDockWidget(QWidget* widget, QString groupname)
{
	QDockWidget* dockWidget = new QDockWidget(widget->windowTitle(), this);
	dockWidget->setObjectName(widget->objectName() + "DockWidget");
	dockWidget->setWidget(widget);

	QMainWindow::addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

	// tabify the widget onto one of the left widgets.
	for (std::set<QDockWidget*>::iterator iter = mDockWidgets.begin(); iter != mDockWidgets.end(); ++iter)
	{
		if (this->dockWidgetArea(*iter) == Qt::LeftDockWidgetArea)
		{
			this->tabifyDockWidget(*iter, dockWidget);
			break;
		}
	}

	mDockWidgets.insert(dockWidget);
	dockWidget->setVisible(false); // default visibility

	this->addToWidgetGroupMap(dockWidget->toggleViewAction(), groupname);
}

void MainWindow::addToWidgetGroupMap(QAction* action, QString groupname)
{
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
}

void MainWindow::initialize()
{
	// resources layer
	ssc::MessageManager::initialize();

	// services layer
	cx::VideoService::initialize();
	cx::DataManager::initialize();
	cx::ToolManager::initializeObject();

	// logic layer
	cx::LogicManager::initialize();

	// gui layer:
	// inited by mainwindow construction in main()
}

/** deallocate all global resources. Assumes MainWindow already has been destroyed and the mainloop is exited
 */
void MainWindow::shutdown()
{
	// gui layer
	// already destroyed by mainwindow

	// old stuff - high level
	StateService::destroyInstance();
	ViewManager::destroyInstance();
	//  RegistrationManager::shutdown();
	RepManager::destroyInstance();

	// logic layer
	cx::LogicManager::shutdown();

	// service layer
	cx::ToolManager::shutdown();
	cx::DataManager::shutdown();
	cx::VideoService::shutdown();

	ssc::GPUImageBufferRepository::shutdown();
	ssc::MessageManager::shutdown();
}

QMenu* MainWindow::createPopupMenu()
{
	QMenu* popupMenu = new QMenu(0);
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
	mNewPatientAction = new QAction(QIcon(":/icons/open_icon_library/png/64x64/actions/document-new-8.png"), tr(
		"&New patient"), this);
	mNewPatientAction->setShortcut(tr("Ctrl+N"));
	mNewPatientAction->setStatusTip(tr("Create a new patient file"));
	mSaveFileAction = new QAction(QIcon(":/icons/open_icon_library/png/64x64/actions/document-save-5.png"), tr(
		"&Save Patient"), this);
	mSaveFileAction->setShortcut(tr("Ctrl+S"));
	mSaveFileAction->setStatusTip(tr("Save patient file"));
	mLoadFileAction = new QAction(QIcon(":/icons/open_icon_library/png/64x64/actions/document-open-7.png"), tr(
		"&Load Patient"), this);
	mLoadFileAction->setShortcut(tr("Ctrl+L"));
	mLoadFileAction->setStatusTip(tr("Load patient file"));
	mClearPatientAction = new QAction(tr("&Clear Patient"), this);
	mExportPatientAction = new QAction(tr("&Export Patient"), this);

	connect(mNewPatientAction, SIGNAL(triggered()), this, SLOT(newPatientSlot()));
	connect(mLoadFileAction, SIGNAL(triggered()), this, SLOT(loadPatientFileSlot()));
	connect(mSaveFileAction, SIGNAL(triggered()), this, SLOT(savePatientFileSlot()));
	connect(mSaveFileAction, SIGNAL(triggered()), this, SLOT(saveDesktopSlot()));
	connect(mExportPatientAction, SIGNAL(triggered()), patientService()->getPatientData().get(), SLOT(exportPatient()));
	connect(mClearPatientAction, SIGNAL(triggered()), this, SLOT(clearPatientSlot()));

	mShowControlPanelAction = new QAction("Show Control Panel", this);
	connect(mShowControlPanelAction, SIGNAL(triggered()), this, SLOT(showControlPanelActionSlot()));

	// Application
	mAboutAction = new QAction(tr("&About"), this); // About burde gitt About CustusX, det gj√∏r det ikke av en eller annen grunn???
	mAboutAction->setShortcut(tr("Ctrl+A"));
	mAboutAction->setStatusTip(tr("Show the application's About box"));
	mPreferencesAction = new QAction(tr("&Preferences"), this);
	mPreferencesAction->setShortcut(tr("Ctrl+P"));
	mPreferencesAction->setStatusTip(tr("Show the preferences dialog"));

	mDebugModeAction = new QAction(tr("&Debug Mode"), this);
	mDebugModeAction->setShortcut(tr("Ctrl+D"));
	mDebugModeAction->setCheckable(true);
	mDebugModeAction->setChecked(DataManager::getInstance()->getDebugMode());
	mDebugModeAction->setStatusTip(tr("Set debug mode, this enables lots of weird stuff."));
	connect(mDebugModeAction, SIGNAL(triggered(bool)), DataManager::getInstance(), SLOT(setDebugMode(bool)));
	connect(DataManager::getInstance(), SIGNAL(debugModeChanged(bool)), mDebugModeAction, SLOT(setChecked(bool)));
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

	//data
	mImportDataAction = new QAction(QIcon(":/icons/open_icon_library/png/64x64/actions/document-import-2.png"), tr(
		"&Import data"), this);
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
	connect(videoService()->getIGTLinkVideoConnection()->getVideoSource().get(), SIGNAL(streaming(bool)), this,
		SLOT(updateStreamingActionSlot()));
	this->updateStreamingActionSlot();

	mConfigureToolsAction->setChecked(true);

	connect(mConfigureToolsAction, SIGNAL(triggered()), this, SLOT(configureSlot()));
	connect(mInitializeToolsAction, SIGNAL(triggered()), ssc::toolManager(), SLOT(initialize()));
	connect(mTrackingToolsAction, SIGNAL(triggered()), this, SLOT(toggleTrackingSlot()));
	connect(mSaveToolsPositionsAction, SIGNAL(triggered()), ssc::toolManager(), SLOT(saveToolsSlot()));
	connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(updateTrackingActionSlot()));
	connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(updateTrackingActionSlot()));
	this->updateTrackingActionSlot();

	mNewLayoutAction = new QAction(tr("New Layout"), this);
	mNewLayoutAction->setToolTip("Create a new Custom Layout");
	connect(mNewLayoutAction, SIGNAL(triggered()), this, SLOT(newCustomLayoutSlot()));
	mEditLayoutAction = new QAction(tr("Edit Layout"), this);
	mEditLayoutAction->setToolTip("Edit the current Custom Layout");
	connect(mEditLayoutAction, SIGNAL(triggered()), this, SLOT(editCustomLayoutSlot()));
	mDeleteLayoutAction = new QAction(tr("Delete Layout"), this);
	mDeleteLayoutAction->setToolTip("Delete the current Custom Layout");
	connect(mDeleteLayoutAction, SIGNAL(triggered()), this, SLOT(deleteCustomLayoutSlot()));

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
	this->saveScreenShot(QPixmap::grabWindow(QApplication::desktop()->winId()));
}

void MainWindow::shootWindow()
{
	this->saveScreenShot(QPixmap::grabWindow(this->winId()));
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
			for (std::set<QDockWidget*>::iterator iter = mDockWidgets.begin(); iter != mDockWidgets.end(); ++iter)
			{
				if (action == (*iter)->toggleViewAction())
				{
					if (!checked)
						(*iter)->hide();
				}
			}
		}
}

void MainWindow::saveScreenShot(QPixmap pixmap)
{
	QString folder = patientService()->getPatientData()->getActivePatientFolder() + "/Screenshots/";
	QDir().mkpath(folder);
	QString format = ssc::timestampSecondsFormat();
	QString filename = QDateTime::currentDateTime().toString(format) + ".png";

	QtConcurrent::run(boost::bind(&MainWindow::saveScreenShotThreaded, this, pixmap.toImage(), folder + "/" + filename));
}

/**Intended to be called in a separate thread.
 * \sa saveScreenShot()
 */
void MainWindow::saveScreenShotThreaded(QImage pixmap, QString filename)
{
	pixmap.save(filename, "png");
	ssc::messageManager()->sendInfo("Saved screenshot to " + filename);
	ssc::messageManager()->playScreenShotSound();
}

void MainWindow::toggleStreamingSlot()
{
	if (videoService()->getIGTLinkVideoConnection()->getVideoSource()->isStreaming())
	{
		videoService()->getIGTLinkVideoConnection()->getVideoSource()->disconnectServer();
	}
	else
	{
		videoService()->getIGTLinkVideoConnection()->launchAndConnectServer();
	}
}

void MainWindow::updateStreamingActionSlot()
{
	if (videoService()->getIGTLinkVideoConnection()->getVideoSource()->isStreaming())
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
	if (ssc::dataManager()->getActiveImage())
	{
		std::cout << "center active image" << std::endl;
		Navigation().centerToData(ssc::dataManager()->getActiveImage());
	}
	else if (!viewManager()->getViewGroups().empty())
	{
		std::cout << "center first view group" << std::endl;
		Navigation().centerToView(viewManager()->getViewGroups()[0]->getData()->getData());
	}
	else
	{
		std::cout << "center global" << std::endl;
		Navigation().centerToGlobalDataCenter();
	}
}

void MainWindow::centerToTooltipSlot()
{
	Navigation().centerToTooltip();
}

void MainWindow::togglePointPickerActionSlot()
{
	ViewGroupDataPtr data = viewManager()->getViewGroups()[0]->getData();
	ViewGroupData::Options options = data->getOptions();
	options.mShowPointPickerProbe = !options.mShowPointPickerProbe;
	data->setOptions(options);
}
void MainWindow::updatePointPickerActionSlot()
{
	bool show = viewManager()->getViewGroups()[0]->getData()->getOptions().mShowPointPickerProbe;
	mShowPointPickerAction->setChecked(show);
}

void MainWindow::updateTrackingActionSlot()
{
	if (ssc::toolManager()->isTracking())
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
	if (ssc::toolManager()->isTracking())
		ssc::toolManager()->stopTracking();
	else
		ssc::toolManager()->startTracking();
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
		ssc::messageManager()->sendInfo("Made a new patient folder: " + patientDatafolder);
	}

	QString timestamp = QDateTime::currentDateTime().toString(timestampFormatFolderFriendly()) + "_";
	QString postfix = settings()->value("globalApplicationName").toString() + "_" + settings()->value("globalPatientNumber").toString() + ".cx3";

	QString choosenDir = patientDatafolder + "/" + timestamp + postfix;

	// not necessary:
//	// if existing, revert to seconds format
//	if (QDir().exists(choosenDir))
//	{
//		timestamp = QDateTime::currentDateTime().toString(timestampSecondsFormatFolderFriendly()) + "_";
//		choosenDir = patientDatafolder + "/" + timestamp + postfix;
//	}

	choosenDir = QFileDialog::getSaveFileName(this, tr("Select directory to save patient in"), choosenDir);
	if (choosenDir == QString::null)
		return; // On cancel
	if (!choosenDir.endsWith(".cx3"))
		choosenDir += QString(".cx3");

	// Update global patient number
	int patientNumber = settings()->value("globalPatientNumber").toInt();
	settings()->setValue("globalPatientNumber", ++patientNumber);

	patientService()->getPatientData()->newPatient(choosenDir);
}

void MainWindow::clearPatientSlot()
{
	patientService()->getPatientData()->clearPatient();
	ssc::messageManager()->sendWarning("Cleared current patient data");
}

void MainWindow::savePatientFileSlot()
{
	if (patientService()->getPatientData()->getActivePatientFolder().isEmpty())
	{
		ssc::messageManager()->sendWarning("No patient selected, select or create patient before saving!");
		this->newPatientSlot();
		return;
	}

	patientService()->getPatientData()->savePatient();
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

	for (std::set<QDockWidget*>::iterator iter = mDockWidgets.begin(); iter != mDockWidgets.end(); ++iter)
	{
		(*iter)->hide();
		// this->DockWidget(*iter); // wrong: removed the dockwidget altogether
	}

	viewManager()->setActiveLayout(desktop.mLayoutUid);
	this->restoreState(desktop.mMainWindowState);
}

void MainWindow::saveDesktopSlot()
{
	Desktop desktop;
	desktop.mMainWindowState = this->saveState();
	desktop.mLayoutUid = viewManager()->getActiveLayout();
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

void MainWindow::loadPatientFileSlot()
{
	QString patientDatafolder = settings()->value("globalPatientDataFolder").toString();
	// Create folder
	if (!QDir().exists(patientDatafolder))
	{
		QDir().mkdir(patientDatafolder);
		ssc::messageManager()->sendInfo("Made a new patient folder: " + patientDatafolder);
	}
	// Open file dialog
	QString choosenDir = QFileDialog::getExistingDirectory(this, tr("Select patient"), patientDatafolder,
		QFileDialog::ShowDirsOnly);
	if (choosenDir == QString::null)
		return; // On cancel

	patientService()->getPatientData()->loadPatient(choosenDir);

	//  cx::FrameForest forest;
}

void MainWindow::importDataSlot()
{
	this->savePatientFileSlot();

	//ssc::messageManager()->sendInfo("Importing data...");
	QString fileName = QFileDialog::getOpenFileName(this, QString(tr("Select data file for import")),
		settings()->value("globalPatientDataFolder").toString(), tr("Image/Mesh (*.mhd *.mha *.stl *.vtk *.mnc)"));
	if (fileName.isEmpty())
	{
		ssc::messageManager()->sendInfo("Import canceled");
		return;
	}

	ImportDataDialog* wizard = new ImportDataDialog(fileName, this);
	wizard->exec(); //calling exec() makes the wizard dialog modal which prevents other user interaction
	//with the system
}

void MainWindow::patientChangedSlot()
{
	this->updateWindowTitle();
}

/** Called when the layout is changed: update the layout menu
 */
void MainWindow::layoutChangedSlot()
{
	// reset list of available layouts

	//Make sure all actions in the group are deleted - possibly removes a few memory leaks
	if (mLayoutActionGroup)
	{
		QList<QAction*> actionList = mLayoutActionGroup->actions();
		for (int i = 0; i < actionList.size(); i++)
			delete actionList.at(i);
	}

	//if(mLayoutActionGroup)
	delete mLayoutActionGroup;
	mLayoutActionGroup = viewManager()->createLayoutActionGroup();

	//  mLayoutMenu->clear(); // Clearing removes too much
	mLayoutMenu->addActions(mLayoutActionGroup->actions());

	bool editable = viewManager()->isCustomLayout(viewManager()->getActiveLayout());
	mEditLayoutAction->setEnabled(editable);
	mDeleteLayoutAction->setEnabled(editable);
}

/**create and execute a dialog for determining layout.
 * Return layout data, or invalid layout data if cancelled.
 */
LayoutData MainWindow::executeLayoutEditorDialog(QString title, bool createNew)
{
	boost::shared_ptr<QDialog> dialog(new QDialog(NULL, Qt::Dialog));
	dialog->setWindowTitle(title);
	QVBoxLayout* layout = new QVBoxLayout(dialog.get());
	layout->setMargin(0);

	LayoutEditorWidget* editor = new LayoutEditorWidget(dialog.get());

	LayoutData data = viewManager()->getLayoutData(viewManager()->getActiveLayout());

	if (createNew)
	{
		data.resetUid(viewManager()->generateLayoutUid());
	}
	editor->setLayoutData(data);
	layout->addWidget(editor);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), dialog.get(), SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), dialog.get(), SLOT(reject()));
	layout->addWidget(buttonBox);

	if (!dialog->exec())
		return LayoutData();

	return editor->getLayoutData();
}

void MainWindow::newCustomLayoutSlot()
{
	LayoutData data = this->executeLayoutEditorDialog("New Custom Layout", true);
	if (data.getUid().isEmpty())
		return;
	viewManager()->setLayoutData(data);
	viewManager()->setActiveLayout(data.getUid());
}

void MainWindow::editCustomLayoutSlot()
{
	LayoutData data = this->executeLayoutEditorDialog("Edit Current Layout", false);
	if (data.getUid().isEmpty())
		return;
	viewManager()->setLayoutData(data);
}

void MainWindow::deleteCustomLayoutSlot()
{
	if (QMessageBox::question(this, "Delete current layout", "Do you really want to delete the current layout?",
		QMessageBox::Cancel | QMessageBox::Ok) != QMessageBox::Ok)
		return;
	viewManager()->deleteLayoutData(viewManager()->getActiveLayout());
	viewManager()->setActiveLayout(viewManager()->getAvailableLayouts().front()); // revert to existing state
}

void MainWindow::createMenus()
{
	mCustusXMenu = new QMenu(tr("CustusX"), this);
	mFileMenu = new QMenu(tr("File"), this);
	mWorkflowMenu = new QMenu(tr("Workflow"), this);
	mToolMenu = new QMenu(tr("Tracking"), this);
	mLayoutMenu = new QMenu(tr("Layouts"), this);
	mNavigationMenu = new QMenu(tr("Navigation"), this);
	mHelpMenu = new QMenu(tr("Help"), this);

	// Application
	this->menuBar()->addMenu(mCustusXMenu);
	mCustusXMenu->addAction(mAboutAction);
	mCustusXMenu->addAction(mPreferencesAction);

	// File
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
	mFileMenu->addSeparator();
	mFileMenu->addAction(mShowControlPanelAction);
	mFileMenu->addAction(mQuitAction);

	// window
	QMenu* popupMenu = this->createPopupMenu();
	popupMenu->setTitle("Window");
	this->menuBar()->addMenu(popupMenu);

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
	mLayoutMenu->addAction(mNewLayoutAction);
	mLayoutMenu->addAction(mEditLayoutAction);
	mLayoutMenu->addAction(mDeleteLayoutAction);
	mLayoutMenu->addSeparator();

	this->menuBar()->addMenu(mNavigationMenu);
	mNavigationMenu->addAction(mCenterToImageCenterAction);
	mNavigationMenu->addAction(mCenterToTooltipAction);
	mNavigationMenu->addAction(mShowPointPickerAction);
	mNavigationMenu->addSeparator();
	mNavigationMenu->addActions(mInteractorStyleActionGroup->actions());

	this->menuBar()->addMenu(mHelpMenu);
	mHelpMenu->addAction(QWhatsThis::createAction());

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
	mHelpToolBar->addAction(QWhatsThis::createAction());
	this->registerToolBar(mHelpToolBar, "Toolbar");

	mScreenshotToolBar = addToolBar("Screenshot");
	mScreenshotToolBar->setObjectName("ScreenshotToolBar");
	mScreenshotToolBar->addAction(mShootScreenAction);
	//  mScreenshotToolBar->addAction(mShootWindowAction);
	this->registerToolBar(mScreenshotToolBar, "Toolbar");

	QToolBar* camera3DViewToolBar = addToolBar("Camera 3D Views");
	camera3DViewToolBar->setObjectName("Camera3DViewToolBar");
	camera3DViewToolBar->addActions(mStandard3DViewActions->actions());
	this->registerToolBar(camera3DViewToolBar, "Toolbar");
}

void MainWindow::registerToolBar(QToolBar* toolbar, QString groupname)
{
	this->addToWidgetGroupMap(toolbar->toggleViewAction(), groupname);
}

void MainWindow::aboutSlot()
{
	QMessageBox::about(this, tr("About CustusX"), tr("<h2>CustusX version %1</h2> "
		"<p>Created by SINTEF Medical Technology."
		"<p><a href=http://www.sintef.no/Home/Technology-and-Society/Medical-technology> www.sintef.no </a>"
		"<p>An application for Image Guided Surgery."
		"<p>Created using Qt, VTK, ITK, IGSTK, SSC.").arg(VERSION_NUMBER_VERBOSE));
}

void MainWindow::preferencesSlot()
{
	PreferencesDialog prefDialog(this);
	prefDialog.exec();
}

void MainWindow::quitSlot()
{
	ssc::messageManager()->sendInfo("Shutting down CustusX");
	qApp->quit();
}

void MainWindow::deleteDataSlot()
{
	if (!ssc::dataManager()->getActiveImage())
		return;
	ssc::dataManager()->removeData(ssc::dataManager()->getActiveImage()->getUid());
}

void MainWindow::configureSlot()
{
	ssc::toolManager()->configure();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	settings()->setValue("mainWindow/geometry", saveGeometry());
	settings()->setValue("mainWindow/windowState", saveState());
	settings()->sync();
	ssc::messageManager()->sendInfo("Closing: Save geometry and window state");

	if (ssc::toolManager()->isTracking())
	{
		ssc::messageManager()->sendInfo("Closing: Stopping tracking");
		ssc::toolManager()->stopTracking();
	}
	QMainWindow::closeEvent(event);
}
}//namespace cx
