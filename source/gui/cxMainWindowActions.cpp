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
#include <QDockWidget>
#include <QInputDialog>
#include <QDir>

#include "boost/bind/bind.hpp"
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
#include "cxFileHelpers.h"
#include "cxFileManagerService.h"
#include "cxFileReaderWriterService.h"
#include "cxApplication.h"
#include "cxStyles.h"

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
					   Styles::screenshotIcon("S"),
					   QKeySequence("Ctrl+f"), "Save a screenshot to the patient folder.",
					   &MainWindowActions::shootScreen);

	this->createAction("ShootWindow", "Shoot Window",
					   Styles::screenshotIcon("W"),
					   QKeySequence("Ctrl+Shift+f"), "Save an image of the application to the patient folder.",
					   &MainWindowActions::shootWindow);

	mRecordFullscreenStreamingAction = this->createAction("RecordFullscreen", "Record Fullscreen video with VLC (needs to be at least 7 sec to work)",
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

	if (mServices->view()->getGroup(0))
		connect(mServices->view()->getGroup(0).get(), &ViewGroupData::optionsChanged, this, &MainWindowActions::updatePointPickerActionSlot);
	this->updatePointPickerActionSlot();

	mStartStreamingAction = this->createAction("StartStreaming", "Start Streaming",
											   QIcon(":/icons/open_icon_library/.png"),
											   QKeySequence("Ctrl+V"), "",
											   &MainWindowActions::toggleStreamingSlot);
	connect(mServices->video().get(), &VideoService::connected, this, &MainWindowActions::updateStreamingActionSlot);
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
	this->createAction("ReconfigureTools", "Reconfigure tracking",
					   QIcon(), QKeySequence(), "Deconfigure tools, and start tracking again. Useful when changing tool configuration",
					   &MainWindowActions::reconfigureTrackingSlot);

	mTrackingToolsAction = this->createAction("TrackingTools", "Start tracking",
											  QIcon(), QKeySequence("Ctrl+T"), "",
											  &MainWindowActions::toggleTrackingSlot);

	connect(mServices->tracking().get(), &TrackingService::stateChanged, this, &MainWindowActions::updateTrackingActionSlot);
	this->updateTrackingActionSlot();
}

void MainWindowActions::createPatientActions()
{
	this->createAction("NewPatient", "New Patient",
					   QIcon(":/icons/open_icon_library/document-new-8.png"),
					   QKeySequence("Ctrl+N"),
					   "Create a new patient file",
						 [=](){this->newPatientSlot(true);});

	this->createAction("CreatePatientWithoutDialog", "New Patient",
						 QIcon(),
						 QKeySequence(),
						 "Create a new patient file",
						 [=](){this->newPatientSlot(false);});

	this->createAction("CreatePatientWithPatientName", "New Patient",
						 QIcon(),
						 QKeySequence(),
						 "Create a new patient file",
						 [=](){this->newPatientSlot(false, true);});

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

	this->createAction("LoadFileWithSimpleDialog", "Load Patient",
						 QIcon(":/icons/open_icon_library/document-open-7.png"),
						 QKeySequence("Ctrl+L"),
						 "Load patient file",
						 [=](){this->loadPatientFileSlot(true);});

	this->createAction("LoadFileCopy", "Load from Patient template",
					   QIcon(":/icons/open_icon_library/document-open-7.png"),
					   QKeySequence(),
					   "Create a new patient based on a template",
					   &MainWindowActions::loadPatientFileCopySlot);

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

	//Action "AddFilesForImport" was previously called "ImportData"
	this->createAction("AddFilesForImport", "Add files for import",
										 QIcon(":/icons/open_icon_library/document-import-2.png"),
										 QKeySequence("Ctrl+I"),
										 "Add files to be imported",
										 [=](){this->importDataSlot("AddMoreFilesButtonClickedAction");});

	this->createAction("ImportSelectedData", "Import selected data",
										 QIcon(),
										 QKeySequence(),
										 "Import all selected data files",
										 [=](){this->importDataSlot("ImportButtonClickedAction");});

	this->createAction("AddFilesForImportWithDialog", "Add files for import (Dialog)",
										 QIcon(),
										 QKeySequence(),
										 "Add files to be imported",
										 [=](){this->importDataSlot("AddFilesForImportWithDialogAction");});

	this->createAction("AddFilesForImportWithDialogCT", "Add CT files for import (Dialog)",
										 QIcon(),
										 QKeySequence(),
										 "Add CT files to be imported",
										 [=](){this->importDataSlot("AddFilesForImportWithDialogActionCT");});

	this->createAction("AddFilesForImportFromUSB", "Add CT files for import from USB",
										 QIcon(),
										 QKeySequence(),
										 "Add CT files to be imported",
										 [=](){this->importDataSlot("AddFilesForImportFromUSBCT");});

	this->createAction("AddFilesForImportWithDialogPET", "Add PET files for import (Dialog)",
										 QIcon(),
										 QKeySequence(),
										 "Add PET files to be imported",
										 [=](){this->importDataSlot("AddFilesForImportWithDialogActionPET");});
}

template <class T>
QAction* MainWindowActions::createAction(QString uid, QString text, QIcon icon,
										 QKeySequence shortcut, QString help,
										 T triggerSlot)
{
	QAction* retval = new QAction(icon, text, this);
    retval->setObjectName(uid);
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


void MainWindowActions::newPatientSlot(bool showDialog, bool useSimpleDialog)
{
	mServices->view()->enableRender(false);
	QString choosenDir;
	if(useSimpleDialog)
	{
		QString patientName = getPatientNameFromUser();
		if(patientName.isEmpty())
		{
			mServices->view()->enableRender(true);
			return;
		}
		choosenDir = this->selectNewPatientFolder(showDialog, patientName);
	}
	else
		choosenDir = this->selectNewPatientFolder(showDialog);

	if(!choosenDir.isEmpty())
	{
		// Update global patient number
		int patientNumber = settings()->value("globalPatientNumber").toInt();
		settings()->setValue("globalPatientNumber", ++patientNumber);

		mServices->session()->load(choosenDir);
	}
	mServices->view()->enableRender(true);
}

QString MainWindowActions::selectNewPatientFolder(bool showDialog, QString patientName)
{
	QString patientDatafolder = this->getExistingSessionFolder();

	QString timestamp = QDateTime::currentDateTime().toString(timestampFormatFolderFriendly());

	QString filename = timestamp;
	if(patientName.isEmpty())
	{
		filename.append(profile()->getName());
		filename.append(settings()->value("globalPatientNumber").toString());
	}
	else
		filename.append("_" + patientName);
	filename.append(".cx3");

	QString choosenDir = patientDatafolder + "/" + filename;

	if(showDialog)
	{
		QFileDialog dialog(this->parentWidget(), tr("Select directory to save patient in"), patientDatafolder + "/");
		dialog.setOption(QFileDialog::DontUseNativeDialog, true);
		dialog.setOption(QFileDialog::ShowDirsOnly, true);
		dialog.selectFile(filename);
		if (!dialog.exec())
			return QString();
		choosenDir = dialog.selectedFiles().front();
	}

	if (!choosenDir.endsWith(".cx3"))
		choosenDir += QString(".cx3");

	return choosenDir;
}

QString MainWindowActions::getPatientNameFromUser()
{
	bool ok;
	QString inputText = QInputDialog::getText(nullptr, "Create new patient file",
																					 "Please enter patient name or code:", QLineEdit::Normal,
																					 QString(), &ok);
	if (ok && !inputText.isEmpty())
	{
		inputText.replace(' ', '_');
		inputText.replace('.', '_');
		inputText.replace('/', '_');
		return inputText;
	}
	else
		return "";
}

QString MainWindowActions::getUserToSelectExistingPatient()
{
	QStringList patients = getAllPatientsInProfile();
	QString sessionFolder = this->getExistingSessionFolder();

	if(patients.empty())
	{
		QString message = "Found no patients in the profile " + sessionFolder.section('/', -1);
		CX_LOG_WARNING(message);
		QMessageBox::information(this->parentWidget(), "Found no patients", message);
		return {};
	}
	std::sort(patients.begin(), patients.end(), std::greater<QString>()); //Sort to get most recent dates first

	bool ok = false;
	QString choice = QInputDialog::getItem(
				this->parentWidget(),
				"Choose patient to open",
				"Choose patient to open",
				patients,
				0,
				false,
				&ok);

	if(!ok)
		return {};

	return sessionFolder + "/" + choice;

}

QStringList MainWindowActions::getAllPatientsInProfile()
{
	QString patientDatafolder = this->getExistingSessionFolder();
	QDir directory(patientDatafolder);
	QStringList patients;
	if(!directory.exists())
		return patients;

	QFileInfoList entries = directory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

	for(const QFileInfo& entry: entries)
		if(entry.fileName().endsWith(".cx3", Qt::CaseSensitive))
			patients << entry.fileName();

	return patients;
}

QString MainWindowActions::getExistingSessionFolder()
{
	return profile()->getSessionRootFolder();
}

void MainWindowActions::clearPatientSlot()
{
	mServices->view()->enableRender(false);
	mServices->session()->clear();
	mServices->view()->enableRender(true);
}

void MainWindowActions::savePatientFileSlot()
{
	mServices->view()->enableRender(false);
	if (mServices->patient()->getActivePatientFolder().isEmpty())
	{
		reportWarning("No patient selected, select or create patient before saving!");
		this->newPatientSlot(true);
		mServices->view()->enableRender(true);
		return;
	}

	mServices->session()->save();
	mServices->view()->enableRender(true);
}

void MainWindowActions::loadPatientFileSlot(bool useSimpleFileDialog)
{
	mServices->view()->enableRender(false);
	QString patientDatafolder = this->getExistingSessionFolder();

	// Open file dialog
	QString folder;
	if(useSimpleFileDialog)
		folder = getUserToSelectExistingPatient();
	else
		folder = QFileDialog::getExistingDirectory(this->parentWidget(), "Select patient", patientDatafolder, QFileDialog::ShowDirsOnly);

	if (!folder.isEmpty())
		mServices->session()->load(folder);

	mServices->view()->enableRender(true);
}

void MainWindowActions::loadPatientFileCopySlot()
{
	mServices->view()->enableRender(false);
	QString patientDatafolder = profile()->getPatientTemplatePath();

	// Open file dialog
	QString folder = QFileDialog::getExistingDirectory(this->parentWidget(), "Select template patient to copy", patientDatafolder, QFileDialog::ShowDirsOnly);
	if (!folder.isEmpty())
	{
		QString newFolder = this->selectNewPatientFolder();
		if (!newFolder.isEmpty())
		{
			if(!copyRecursively(folder, newFolder, true))
			{
				CX_LOG_WARNING() << "MainWindowActions::loadPatientFileCopySlot(): Cannot copy patient folder: " << folder;
			}
			else
				mServices->session()->load(newFolder);
		}
	}
	mServices->view()->enableRender(true);
}

void MainWindowActions::exportDataSlot()
{
	this->savePatientFileSlot();

	mServices->view()->enableRender(false);
	ExportDataDialog* wizard = new ExportDataDialog(mServices->patient(), this->parentWidget());
	wizard->exec(); //calling exec() makes the wizard dialog modal which prevents other user interaction with the system
	mServices->view()->enableRender(true);
}

void MainWindowActions::importDataSlot(QString actionText)
{
	this->savePatientFileSlot();
	mServices->view()->enableRender(false);

	QDockWidget* importDockWidget = findMainWindowChildWithObjectName<QDockWidget*>("import_widgetDockWidget");
	if(!importDockWidget)
	{
		CX_LOG_ERROR() << "Cannot find DockWidget for ImportWidget";
		mServices->view()->enableRender(true);
		return;
	}

	importDockWidget->show();
	importDockWidget->raise();

	QWidget* widget = findMainWindowChildWithObjectName<QWidget*>("import_widget");
	if(!widget)
	{
		CX_LOG_ERROR() << "Cannot find ImportWidget";
		mServices->view()->enableRender(true);
		return;
	}

	bool actionFound = false;
	QList<QAction*> actions = widget->actions();
	foreach(QAction* action, actions)
	{
		if(action->text().contains(actionText))
		{
			actionFound =  true;
			action->trigger();
		}
	}
	if(!actionFound)
		CX_LOG_ERROR() << "MainWindowActions::importDataSlot, action not found: " << actionText;
	mServices->view()->enableRender(true);
}

void MainWindowActions::shootScreen()
{
	mScreenShotWriter->grabAllScreensToFile();
}

void MainWindowActions::shootWindow()
{
//	this->shootOneLayout(0);

	int index=0;
    while (mServices->view()->getLayoutWidget(index))
		this->shootOneLayout(index++);
}

void MainWindowActions::shootOneLayout(int index)
{
    QWidget* widget = mServices->view()->getLayoutWidget(index);
	ViewCollectionWidget* vcWidget = dynamic_cast<ViewCollectionWidget*>(widget);

	ViewCollectionImageWriter grabber(vcWidget);
    QImage pm = ViewCollectionImageWriter::vtkImageData2QImage(grabber.grab());
	mScreenShotWriter->save(pm, QString("_layout%1").arg(index));
}

void MainWindowActions::recordFullscreen()
{
	QString path = mServices->patient()->generateFilePath("Screenshots", "mp4");

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
	if (mServices->video()->isConnected())
		mServices->video()->closeConnection();
	else
		mServices->video()->openConnection();
}

void MainWindowActions::updateStreamingActionSlot()
{
	if (mServices->video()->isConnected())
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
	mServices->view()->centerToImageCenterInActiveViewGroup();
}

void MainWindowActions::centerToTooltipSlot()
{
	NavigationPtr nav = mServices->view()->getNavigation();
	nav->centerToTooltip();
}

void MainWindowActions::togglePointPickerActionSlot()
{
	ViewGroupDataPtr data = mServices->view()->getGroup(0);
	ViewGroupData::Options options = data->getOptions();
	options.mShowPointPickerProbe = !options.mShowPointPickerProbe;
	data->setOptions(options);
}
void MainWindowActions::updatePointPickerActionSlot()
{
	if (!mServices->view()->getGroup(0))
		return;
	bool show = mServices->view()->getGroup(0)->getOptions().mShowPointPickerProbe;
	mShowPointPickerAction->setChecked(show);
}

void MainWindowActions::updateTrackingActionSlot()
{
	if (mServices->tracking()->getState() >= Tool::tsTRACKING)
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

void MainWindowActions::reconfigureTrackingSlot()
{
	mServices->tracking()->setState(Tool::tsNONE);
	mServices->tracking()->setState(Tool::tsTRACKING);
}

void MainWindowActions::toggleTrackingSlot()
{
	if (mServices->tracking()->getState() >= Tool::tsTRACKING)
		mServices->tracking()->setState(Tool::tsINITIALIZED);
	else
		mServices->tracking()->setState(Tool::tsTRACKING);
}

void MainWindowActions::onGotoDocumentation()
{
	QString url = DataLocations::getWebsiteUserDocumentationURL();
	QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
}



} // namespace cx
