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
#include <cppunit/extensions/HelperMacros.h>

TestAcqController::TestAcqController(QObject* parent) : QObject(parent)
{
}

ssc::ReconstructManagerPtr TestAcqController::createReconstructionManager()
{
	mRecordDuration = 3000;
	//	std::cout << "testAngioReconstruction running" << std::endl;
	ssc::XmlOptionFile settings;
	ssc::ReconstructManagerPtr reconstructer(new ssc::ReconstructManager(settings,""));

	reconstructer->setOutputBasePath(cx::DataLocations::getTestDataPath() + "/temp/");
	reconstructer->setOutputRelativePath("Images");

	return reconstructer;
}

void TestAcqController::setupVideo(QString framesFile)
{
	std::cout << "TestAcqController::initialize() init video" << std::endl;
	cx::videoService()->getIGTLinkVideoConnection()->getConnectionMethod()->setValue("Direct Link");
	cx::videoService()->getIGTLinkVideoConnection()->setLocalServerArguments(QString("--type MHDFile --filename %1").arg(framesFile));
	mVideoSource = cx::videoService()->getIGTLinkVideoConnection()->getVideoSource();
	connect(mVideoSource.get(), SIGNAL(newFrame()), this, SLOT(newFrameSlot()));
	cx::videoService()->getIGTLinkVideoConnection()->launchAndConnectServer();
}

void TestAcqController::setupProbe(QString probedefinition)
{
	std::cout << "TestAcqController::initialize() init tool" << std::endl;
	ssc::DummyToolPtr dummyTool(new ssc::DummyTool(cx::ToolManager::getInstance()));
	dummyTool->setToolPositionMovement(dummyTool->createToolPositionMovementTranslationOnly(ssc::DoubleBoundingBox3D(0,0,0,10,10,10)));
	std::pair<QString, ssc::ProbeData> probedata = cx::UsReconstructionFileReader::readProbeDataFromFile(probedefinition);
	dummyTool->setProbeSector(probedata.second);
	// TODO should be auto, but doesnt, might because tooman is not initialized
	dummyTool->getProbe()->setRTSource(mVideoSource);
	CPPUNIT_ASSERT(dummyTool->getProbe());
	CPPUNIT_ASSERT(dummyTool->getProbe()->isValid());
	dummyTool->setVisible(true);
	// TODO: refactor toolmanager to be runnable in dummy mode (playback might benefit from this too)
	cx::ToolManager::getInstance()->runDummyTool(dummyTool);
	CPPUNIT_ASSERT(dummyTool->getProbe()->getRTSource());
}



void TestAcqController::initialize()
{
	QString filename = cx::DataLocations::getTestDataPath() +
			"/testing/"
			"2012-10-24_12-39_Angio_i_US3.cx3/US_Acq/US-Acq_03_20121024T132330.mhd";

	cx::patientService()->getPatientData()->newPatient(cx::DataLocations::getTestDataPath() + "/temp/Acquisition/");

	this->setupVideo(filename);
	this->setupProbe(filename);

	mAcquisitionData.reset(new cx::AcquisitionData(this->createReconstructionManager()));

	mAcquisitionBase.reset(new cx::Acquisition(mAcquisitionData));
	mAcquisition.reset(new cx::USAcquisition(mAcquisitionBase));
	connect(mAcquisitionBase.get(), SIGNAL(readinessChanged()), this, SLOT(readinessChangedSlot()));
	connect(mAcquisition.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(saveDataCompletedSlot(QString)));
	connect(mAcquisition.get(), SIGNAL(acquisitionDataReady()), this, SLOT(acquisitionDataReadySlot()));
}

void TestAcqController::start()
{
	mAcquisitionBase->startRecord();
	QTimer::singleShot(mRecordDuration, this, SLOT(stop()));
}

void TestAcqController::stop()
{
	mAcquisitionBase->stopRecord();
}

void TestAcqController::newFrameSlot()
{
	if (!mAcquisitionBase->getLatestSession())
		this->start();
}

void TestAcqController::readinessChangedSlot()
{
	std::cout << QString("Acquisition Ready Status %1: %2")
	             .arg(mAcquisitionBase->isReady())
	             .arg(mAcquisitionBase->getInfoText()) << std::endl;
}

void TestAcqController::acquisitionDataReadySlot()
{
	// read data and print info - this if the result of the memory pathway
	mMemOutputData = mAcquisitionData->getReconstructer()->getSelectedFileData();
}

void TestAcqController::saveDataCompletedSlot(QString path)
{
	QTimer::singleShot(100,   qApp, SLOT(quit()) );

//	// convert path to path + file - needed by reader
//	QStringList splitPath = path.split("/");
//	QString filename = splitPath.join("/") + "/" + splitPath.back() + ".fts";
	QString filename = path;

	// read file and print info - this is the result of the file pathway
	cx::UsReconstructionFileReaderPtr fileReader(new cx::UsReconstructionFileReader());
	mFileOutputData = fileReader->readAllFiles(filename, "calFilesPath""");
}

void TestAcqController::verifyFileData(ssc::USReconstructInputData fileData)
{
	CPPUNIT_ASSERT(!fileData.mFilename.isEmpty());
	// check for enough received image frames
	int framesPerSecond = 20; // minimum frame rate
	CPPUNIT_ASSERT(fileData.mFrames.size() > framesPerSecond*mRecordDuration/1000);
	// check for duration equal to input duration
	double frame_time_ms = fileData.mFrames.back().mTime - fileData.mFrames.front().mTime;
	CPPUNIT_ASSERT(ssc::similar(frame_time_ms, mRecordDuration, 0.05*mRecordDuration));

	int positionsPerSecond = 10; // minimum tracker pos rate
	CPPUNIT_ASSERT(fileData.mPositions.size() > framesPerSecond*mRecordDuration/1000);
	// check for duration equal to input duration
	double pos_time_ms = fileData.mPositions.back().mTime - fileData.mPositions.front().mTime;
	CPPUNIT_ASSERT(ssc::similar(pos_time_ms, mRecordDuration, 0.05*mRecordDuration));

	std::cout << "\tfilename: " << fileData.mFilename << std::endl;
	std::cout << "\tframe count " << fileData.mFrames.size() << std::endl;
	if (!fileData.mFrames.empty())
		std::cout << "\ttime: " << fileData.mFrames.back().mTime - fileData.mFrames.front().mTime << std::endl;

	CPPUNIT_ASSERT(fileData.mProbeData.mData.getType()!=ssc::ProbeData::tNONE);

	// check content of images
	cx::ImageDataContainerPtr images = fileData.mUsRaw->getImageContainer();
	CPPUNIT_ASSERT(images->size() == fileData.mFrames.size());
	for (unsigned i=0; i<images->size(); ++i)
	{
		CPPUNIT_ASSERT(images->get(i));
		Eigen::Array3i dim(images->get(i)->GetDimensions());
		CPPUNIT_ASSERT(dim[0]==fileData.mProbeData.mData.getImage().mSize.width());
		CPPUNIT_ASSERT(dim[1]==fileData.mProbeData.mData.getImage().mSize.height());
	}
}

void TestAcqController::verify()
{
	std::cout << " ** Resulting ssc::USReconstructInputData memory content:" << std::endl;
	this->verifyFileData(mMemOutputData);
	std::cout << " ** Resulting ssc::USReconstructInputData file content:" << std::endl;
	this->verifyFileData(mFileOutputData);
}

