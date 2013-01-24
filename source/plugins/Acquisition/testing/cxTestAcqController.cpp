/*
 * cxTestCustusXController.cpp
 *
 *  \date Oct 19, 2010
 *      \author christiana
 */

#include "cxTestAcqController.h"

#include <sstream>
#include <QTextEdit>
#include <QTimer>

#include "sscDataManager.h"
#include "sscDummyTool.h"
#include "sscTypeConversions.h"
#include "sscData.h"
#include "sscConsoleWidget.h"
#include "sscImage.h"
#include "cxPatientData.h"
#include "cxRenderTimer.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxStateService.h"
#include "cxPatientService.h"
#include "cxDataLocations.h"
#include "cxOpenIGTLinkRTSource.h"
#include "cxVideoService.h"
#include "cxVideoConnection.h"
#include "sscReconstructManager.h"
#include "sscTime.h"

TestAcqController::TestAcqController(QObject* parent) : QObject(parent)
{
//  mTestData += "Test Results:\n";
//  mMainWindow = NULL;
//  mBaseTime = 1000;
}

ssc::ReconstructManagerPtr TestAcqController::createReconstructionManager()
{
	//	std::cout << "testAngioReconstruction running" << std::endl;
	ssc::XmlOptionFile settings;
	ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(settings,""));

	reconstructer->setOutputBasePath(cx::DataLocations::getTestDataPath() + "/temp/");
	reconstructer->setOutputRelativePath("Images");

	return reconstructer;
}

void TestAcqController::initialize()
{
	QString filename = cx::DataLocations::getTestDataPath() +
			"/testing/"
			"2012-10-24_12-39_Angio_i_US3.cx3/US_Acq/US-Acq_03_20121024T132330.mhd";


	cx::patientService()->getPatientData()->newPatient(cx::DataLocations::getTestDataPath() + "/temp/Acquisition/");

	cx::videoService()->getIGTLinkVideoConnection()->setLocalServerArguments(QString("--type MHDFile --filename %1").arg(filename));
	mVideoSource = cx::videoService()->getIGTLinkVideoConnection()->getVideoSource();
	connect(mVideoSource.get(), SIGNAL(newFrame()), this, SLOT(newFrameSlot()));
	cx::videoService()->getIGTLinkVideoConnection()->launchAndConnectServer();

	mAcquisitionData.reset(new cx::AcquisitionData(this->createReconstructionManager()));

	mAcquisition.reset(new cx::USAcquisition(mAcquisitionData));
	connect(mAcquisition.get(), SIGNAL(ready(bool,QString)), this, SLOT(setReady(bool,QString)));
	connect(mAcquisition.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(saveDataCompletedSlot(QString)));
	connect(mAcquisition.get(), SIGNAL(acquisitionDataReady()), this, SLOT(acquisitionDataReadySlot()));
}

void TestAcqController::start()
{
	double startTime = ssc::getMilliSecondsSinceEpoch();
	mRecordSession.reset(new cx::RecordSession(mAcquisitionData->getNewUid(), startTime, startTime, "test_acq"));
    mAcquisitionData->addRecordSession(mRecordSession);

	mAcquisition->startRecord(mRecordSession->getUid());

	QTimer::singleShot(3000, this, SLOT(stop()));
}

void TestAcqController::stop()
{
    mRecordSession->setStopTime(ssc::getMilliSecondsSinceEpoch());
	mAcquisition->stopRecord(false);
	mAcquisition->saveSession(mRecordSession->getUid(), true);
//  delete mMainWindow;
//  cx::MainWindow::shutdown(); // shutdown all global resources, _after_ gui is deleted.
}

void TestAcqController::newFrameSlot()
{
	if (!mRecordSession)
		this->start();
//	std::cout << "TestAcqController::newFrameSlot()" << std::endl;
}

void TestAcqController::setReady(bool ok, QString text)
{
	std::cout << QString("Ready %1:\n %2").arg(ok).arg(text) << std::endl;
}

void TestAcqController::saveDataCompletedSlot(QString path)
{
	std::cout << "TestAcqController::saveDataCompletedSlot() " << path << std::endl;
	QTimer::singleShot(100,   qApp, SLOT(quit()) );

	// convert path to path + file - needed by reader
	QStringList splitPath = path.split("/");
	QString filename = splitPath.join("/") + "/" + splitPath.back() + ".fts";

	// read file and print info - this is the result of the file pathway
	cx::UsReconstructionFileReaderPtr fileReader(new cx::UsReconstructionFileReader());
	ssc::USReconstructInputData fileData = fileReader->readAllFiles(filename, "calFilesPath""");
	std::cout << "resulting file content:" << std::endl;
	std::cout << "filename: " << fileData.mFilename << std::endl;
	std::cout << "frame count " << fileData.mFrames.size() << std::endl;
	if (!fileData.mFrames.empty())
		std::cout << "time: " << fileData.mFrames.back().mTime - fileData.mFrames.front().mTime << std::endl;

}

void TestAcqController::acquisitionDataReadySlot()
{
	std::cout << "TestAcqController::acquisitionDataReadySlot() " << std::endl;

	// read data and print info - this if the result of the memory pathway
	ssc::USReconstructInputData fileData = mAcquisitionData->getReconstructer()->getSelectedFileData();
	std::cout << "resulting memory content:" << std::endl;
	std::cout << "filename: " << fileData.mFilename << std::endl;
	std::cout << "frame count " << fileData.mFrames.size() << std::endl;
	if (!fileData.mFrames.empty())
		std::cout << "time: " << fileData.mFrames.back().mTime - fileData.mFrames.front().mTime << std::endl;
}


//void TestAcqController::loadPatientSlot()
//{
//  cx::patientService()->getPatientData()->loadPatient(mPatientFolder);
//  cx::stateService()->getWorkflow()->setActiveState("NavigationUid");
//  mMainWindow->setGeometry( 10, 10, 1200, 800);

//  if (!ssc::DataManager::getInstance()->getImages().size())
//    return;

//  ssc::ImagePtr image = ssc::DataManager::getInstance()->getImages().begin()->second;
//  ssc::DoubleBoundingBox3D bb_r = transform(image->get_rMd(), image->boundingBox());

//  ssc::DataManager::getInstance()->setCenter(bb_r.center());

//  ssc::DummyToolPtr dummyTool(new ssc::DummyTool(cx::ToolManager::getInstance()));
//  dummyTool->setToolPositionMovement(dummyTool->createToolPositionMovementTranslationOnly(bb_r));
//  cx::ToolManager::getInstance()->runDummyTool(dummyTool);
//}
