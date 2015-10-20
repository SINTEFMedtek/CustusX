#include "cxMainWindowActions.h"

#include <QtConcurrent>
#include <QWidget>
#include <QFileDialog>
#include <QAction>
#include <QScreen>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopServices>

#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "cxReporter.h"
#include "cxLogger.h"
#include "cxConfig.h"
#include "cxDataLocations.h"
#include "cxProfile.h"
#include "cxLogicManager.h"
#include "cxTrackingService.h"
#include "cxSettings.h"
#include "cxVideoService.h"
#include "cxNavigation.h"
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxViewGroupData.h"
#include "cxSessionStorageService.h"
#include "cxVisServices.h"
#include "cxVLCRecorder.h"
#include "cxImportDataDialog.h"
#include "cxExportDataDialog.h"
#include "cxScreenShotImageWriter.h"
#include "cxViewCollectionWidget.h"
#include "cxViewCollectionImageWriter.h"

namespace cx
{

MainWindowActions::MainWindowActions(VisServicesPtr services, QWidget *parent) :
	QObject(parent),
	mServices(services),
	mScreenShotWriter(ScreenShotImageWriter::create(services->patient()))
{
	this->createActions();
}

QAction* MainWindowActions::getAction(QString uid)
{
	if (!mActions.count(uid))
		CX_LOG_ERROR() << QString("Could not find action %1 in MainWindowActions object").arg(uid);
	return mActions[uid];
}

void MainWindowActions::createActions()
{
	this->createAction("GotoDocumentation", "Web Documentation",
					   QIcon(":/icons/open_icon_library/applications-internet.png"),
					   QKeySequence(""), "",
					   &MainWindowActions::onGotoDocumentation);

	this->createAction("StartLogConsole", "Start Log Console",
					   QIcon(),
					   QKeySequence("Ctrl+D"), "Open Log Console as external application",
					   &MainWindowActions::onStartLogConsole);

	this->createAction("ShootScreen", "Shoot Screen",
					   QIcon(":/icons/screenshot-screen.png"),
					   QKeySequence("Ctrl+f"), "Save a screenshot to the patient folder.",
					   &MainWindowActions::shootScreen);

	this->createAction("ShootWindow", "Shoot Window",
					   QIcon(":/icons/screenshot-window.png"),
					   QKeySequence("Ctrl+Shift+f"), "Save an image of the application to the patient folder.",
					   &MainWindowActions::shootWindow);

	mRecordFullscreenStreamingAction = this->createAction("RecordFullscreen", "Record Fullscreen",
					   QIcon(),
					   QKeySequence("F8"), "Record a video of the full screen.",
					   &MainWindowActions::recordFullscreen);
	connect(vlc(), &VLCRecorder::stateChanged, this, &MainWindowActions::updateRecordFullscreenActionSlot);
	this->updateRecordFullscreenActionSlot();

	mShowPointPickerAction = this->createAction("ShowPointPicker", "Point Picker",
												QIcon(":/icons/point_picker.png"),
												QKeySequence(""), "Activate the 3D Point Picker Probe",
												&MainWindowActions::togglePointPickerActionSlot);
	mShowPointPickerAction->setCheckable(true);

	if (viewService()->getGroup(0))
		connect(viewService()->getGroup(0).get(), &ViewGroupData::optionsChanged, this, &MainWindowActions::updatePointPickerActionSlot);
	this->updatePointPickerActionSlot();

	mStartStreamingAction = this->createAction("StartStreaming", "Start Streaming",
											   QIcon(":/icons/open_icon_library/.png"),
											   QKeySequence("Ctrl+V"), "",
											   &MainWindowActions::toggleStreamingSlot);
	connect(videoService().get(), &VideoService::connected, this, &MainWindowActions::updateStreamingActionSlot);
	this->updateStreamingActionSlot();

	this->createAction("CenterToImageCenter", "Center Image",
					   QIcon(":/icons/center_image.png"),
					   QKeySequence(), "",
					   &MainWindowActions::centerToImageCenterSlot);

	this->createAction("CenterToTooltip", "Center Tool",
					   QIcon(":/icons/center_tool.png"),
					   QKeySequence(""), "",
					   &MainWindowActions::centerToTooltipSlot);

	this->createPatientActions();
	this->createTrackingActions();
}

void MainWindowActions::createTrackingActions()
{
	this->createAction("ConfigureTools", "Tool configuration",
					   QIcon(), QKeySequence(), "",
					   &MainWindowActions::configureSlot);

	boost::function<void()> finit = boost::bind(&TrackingService::setState, trackingService(), Tool::tsINITIALIZED);
	this->createAction("InitializeTools", "Initialize",
					   QIcon(), QKeySequence(), "",
					   finit);

	mTrackingToolsAction = this->createAction("TrackingTools", "Start tracking",
											  QIcon(), QKeySequence("Ctrl+T"), "",
											  &MainWindowActions::toggleTrackingSlot);

	connect(trackingService().get(), &TrackingService::stateChanged, this, &MainWindowActions::updateTrackingActionSlot);
	this->updateTrackingActionSlot();
}

void MainWindowActions::createPatientActions()
{
	this->createAction("NewPatient", "New Patient",
					   QIcon(":/icons/open_icon_library/document-new-8.png"),
					   QKeySequence("Ctrl+N"),
					   "Create a new patient file",
					   &MainWindowActions::newPatientSlot);

	this->createAction("SaveFile", "Save Patient",
					   QIcon(":/icons/open_icon_library/document-save-5.png"),
					   QKeySequence("Ctrl+S"),
					   "Save patient file",
					   &MainWindowActions::savePatientFileSlot);

	this->createAction("LoadFile", "Load Patient",
					   QIcon(":/icons/open_icon_library/document-open-7.png"),
					   QKeySequence("Ctrl+L"),
					   "Load patient file",
					   &MainWindowActions::loadPatientFileSlot);

	this->createAction("ClearPatient", "Clear Patient",
					   QIcon(),
					   QKeySequence(),
					   "Clear the current patient from the application",
					   &MainWindowActions::clearPatientSlot);

	this->createAction("ExportPatient", "Export Patient",
					   QIcon(),
					   QKeySequence(),
					   "Export patient data to a folder",
					   &MainWindowActions::exportDataSlot);

	this->createAction("ImportData", "Import data",
					   QIcon(":/icons/open_icon_library/document-import-2.png"),
					   QKeySequence("Ctrl+I"),
					   "Import image data",
					   &MainWindowActions::importDataSlot);
}

template <class T>
QAction* MainWindowActions::createAction(QString uid, QString text, QIcon icon,
										 QKeySequence shortcut, QString help,
										 T triggerSlot)
{
	QAction* retval = new QAction(icon, text, this);
	retval->setShortcut(shortcut);
	retval->setStatusTip(help);
	connect(retval, &QAction::triggered, this, triggerSlot);
	mActions[uid] = retval;
	return retval;
}

namespace
{
QString timestampFormatFolderFriendly()
{
  return QString("yyyy-MM-dd_hh-mm");
}
}

QWidget* MainWindowActions::parentWidget()
{
	return dynamic_cast<QWidget*>(this->parent());
}


void MainWindowActions::newPatientSlot()
{
	QString patientDatafolder = this->getExistingSessionFolder();

	QString timestamp = QDateTime::currentDateTime().toString(timestampFormatFolderFriendly());
	QString filename = QString("%1_%2_%3.cx3")
			.arg(timestamp)
			.arg(profile()->getName())
			.arg(settings()->value("globalPatientNumber").toString());

	QString choosenDir = patientDatafolder + "/" + filename;

	QFileDialog dialog(this->parentWidget(), tr("Select directory to save patient in"), patientDatafolder + "/");
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

	mServices->session()->load(choosenDir);
}

QString MainWindowActions::getExistingSessionFolder()
{
	return profile()->getSessionRootFolder();
}

void MainWindowActions::clearPatientSlot()
{
	mServices->session()->clear();
}

void MainWindowActions::savePatientFileSlot()
{
	if (patientService()->getActivePatientFolder().isEmpty())
	{
		reportWarning("No patient selected, select or create patient before saving!");
		this->newPatientSlot();
		return;
	}

	mServices->session()->save();
}

void MainWindowActions::loadPatientFileSlot()
{
	QString patientDatafolder = this->getExistingSessionFolder();

	// Open file dialog
	QString folder = QFileDialog::getExistingDirectory(this->parentWidget(), "Select patient", patientDatafolder, QFileDialog::ShowDirsOnly);
	if (folder.isEmpty())
		return;

	mServices->session()->load(folder);
}

void MainWindowActions::exportDataSlot()
{
	this->savePatientFileSlot();

	ExportDataDialog* wizard = new ExportDataDialog(mServices->patient(), this->parentWidget());
	wizard->exec(); //calling exec() makes the wizard dialog modal which prevents other user interaction with the system
}

void MainWindowActions::importDataSlot()
{
	this->savePatientFileSlot();

	QString folder = mLastImportDataFolder;
	if (folder.isEmpty())
		folder = profile()->getSessionRootFolder();

	QStringList fileName = QFileDialog::getOpenFileNames(this->parentWidget(), QString(tr("Select data file(s) for import")),
		folder, tr("Image/Mesh (*.mhd *.mha *.stl *.vtk *.mnc *.png)"));
	if (fileName.empty())
	{
		report("Import canceled");
		return;
	}

	mLastImportDataFolder = QFileInfo(fileName[0]).absolutePath();

	for (int i=0; i<fileName.size(); ++i)
	{
		ImportDataDialog* wizard = new ImportDataDialog(mServices->patient(), fileName[i], this->parentWidget());
		wizard->exec(); //calling exec() makes the wizard dialog modal which prevents other user interaction with the system
	}
}

void MainWindowActions::shootScreen()
{
	mScreenShotWriter->grabAllScreensToFile();
}

void MainWindowActions::shootWindow()
{
	this->shootOneLayout(0);

//	int index=0;
//	while (mServices->view()->getLayoutWidget(NULL, index))
//		this->shootOneLayout(index++);
}

void MainWindowActions::shootOneLayout(int index)
{
	QWidget* widget = mServices->view()->getLayoutWidget(NULL, index);
	ViewCollectionWidget* vcWidget = dynamic_cast<ViewCollectionWidget*>(widget);

	ViewCollectionImageWriter grabber(vcWidget);
	QImage pm = grabber.grab();
	mScreenShotWriter->save(pm, QString("_layout%1").arg(index));
}

void MainWindowActions::recordFullscreen()
{
	QString path = patientService()->generateFilePath("Screenshots", "mp4");

	if(vlc()->isRecording())
		vlc()->stopRecording();
	else
		vlc()->startRecording(path);
}

void MainWindowActions::updateRecordFullscreenActionSlot()
{
	mRecordFullscreenStreamingAction->setCheckable(true);
	mRecordFullscreenStreamingAction->blockSignals(true);
	mRecordFullscreenStreamingAction->setChecked(vlc()->isRecording());
	mRecordFullscreenStreamingAction->blockSignals(false);

	if(vlc()->isRecording())
	{
		mRecordFullscreenStreamingAction->setIcon(QIcon(":/icons/Video-icon_green.png"));
	}
	else
	{
		mRecordFullscreenStreamingAction->setIcon(QIcon(":/icons/Video-icon_gray.png"));
	}

}

void MainWindowActions::onStartLogConsole()
{
	QString fullname = DataLocations::findExecutableInStandardLocations("LogConsole");
//	std::cout << "MainWindowActions::onStartLogConsole() " << fullname << std::endl;
	mLocalVideoServerProcess.reset(new ProcessWrapper(QString("LogConsole")));
	mLocalVideoServerProcess->launchWithRelativePath(fullname, QStringList());
}

void MainWindowActions::toggleStreamingSlot()
{
	if (videoService()->isConnected())
		videoService()->closeConnection();
	else
		videoService()->openConnection();
}

void MainWindowActions::updateStreamingActionSlot()
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

void MainWindowActions::centerToImageCenterSlot()
{
	viewService()->getNavigation()->centerToDataInActiveViewGroup();
}

void MainWindowActions::centerToTooltipSlot()
{
	NavigationPtr nav = viewService()->getNavigation();
	nav->centerToTooltip();
}

void MainWindowActions::togglePointPickerActionSlot()
{
	ViewGroupDataPtr data = viewService()->getGroup(0);
	ViewGroupData::Options options = data->getOptions();
	options.mShowPointPickerProbe = !options.mShowPointPickerProbe;
	data->setOptions(options);
}
void MainWindowActions::updatePointPickerActionSlot()
{
	if (!viewService()->getGroup(0))
		return;
	bool show = viewService()->getGroup(0)->getOptions().mShowPointPickerProbe;
	mShowPointPickerAction->setChecked(show);
}

void MainWindowActions::updateTrackingActionSlot()
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

void MainWindowActions::configureSlot()
{
	trackingService()->setState(Tool::tsCONFIGURED);
}

void MainWindowActions::toggleTrackingSlot()
{
	if (trackingService()->getState() >= Tool::tsTRACKING)
		trackingService()->setState(Tool::tsINITIALIZED);
	else
		trackingService()->setState(Tool::tsTRACKING);
}

void MainWindowActions::onGotoDocumentation()
{
	QString url = DataLocations::getWebsiteUserDocumentationURL();
	QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
}



} // namespace cx
