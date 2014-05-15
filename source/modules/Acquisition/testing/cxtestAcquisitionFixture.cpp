/*
 * cxTestCustusXController.cpp
 *
 *  \date Oct 19, 2010
 *      \author christiana
 */

#include "cxtestAcquisitionFixture.h"

#include <vtkImageData.h>
#include "catch.hpp"
#include <QApplication>

#include "cxDummyTool.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "cxDataLocations.h"
#include "cxVideoService.h"
#include "cxVideoConnectionManager.h"
#include "cxReconstructionManager.h"
#include "cxLogger.h"
#include "cxUSFrameData.h"
#include "cxUsReconstructionFileReader.h"
#include "cxImageDataContainer.h"
#include "cxStringDataAdapterXml.h"
#include "cxProbeImpl.h"
#include "cxToolManager.h"
#include "cxLogicManager.h"
#include "cxStateService.h"
#include "cxLegacySingletons.h"

namespace cxtest
{

void AcquisitionFixture::setUp()
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
}

void AcquisitionFixture::tearDown()
{
	cx::LogicManager::shutdown();
}

AcquisitionFixture::AcquisitionFixture(QObject* parent) :
	QObject(parent),
	mRecordDuration(3000)
{
	this->setUp();
	mNumberOfExpectedStreams = 1;
}

AcquisitionFixture::~AcquisitionFixture()
{
	this->tearDown();
}

cx::ReconstructManagerPtr AcquisitionFixture::createReconstructionManager()
{
	mRecordDuration = 3000;
	cx::XmlOptionFile settings;
	cx::ReconstructionManagerPtr reconstructer(new cx::ReconstructionManager(settings,""));

	reconstructer->setOutputBasePath(cx::DataLocations::getTestDataPath() + "/temp/");
	reconstructer->setOutputRelativePath("Images");

	return reconstructer;
}

void AcquisitionFixture::setupVideo()
{
	SSC_LOG("");
	cx::videoService()->getVideoConnection()->getConnectionMethod()->setValue(mConnectionMethod);
	INFO("bundle path: "+cx::DataLocations::getBundlePath());
	REQUIRE(!cx::stateService()->getOpenIGTLinkServer().isEmpty());
	cx::videoService()->getVideoConnection()->setLocalServerExecutable(cx::stateService()->getOpenIGTLinkServer()[0]);
	cx::videoService()->getVideoConnection()->setLocalServerArguments(
				QString("%1 --type MHDFile --filename %2 %3").arg(cx::stateService()->getOpenIGTLinkServer()[1]).arg(mAcqDataFilename).arg(mAdditionalGrabberArg));
	mVideoSource = cx::videoService()->getActiveVideoSource();
	connect(mVideoSource.get(), SIGNAL(newFrame()), this, SLOT(newFrameSlot()));

	cx::videoService()->getVideoConnection()->setReconnectInterval(3000); // on slow build servers, a long delay is necessary.
	cx::videoService()->getVideoConnection()->launchAndConnectServer();
}

void AcquisitionFixture::setupProbe()
{
	SSC_LOG("");
	cx::DummyToolPtr dummyTool(new cx::DummyTool(cx::trackingService()));
	dummyTool->setToolPositionMovement(dummyTool->createToolPositionMovementTranslationOnly(cx::DoubleBoundingBox3D(0,0,0,10,10,10)));
	std::pair<QString, cx::ProbeDefinition> probedata = cx::UsReconstructionFileReader::readProbeDataFromFile(mAcqDataFilename);
	cx::ProbeImplPtr probe = cx::ProbeImpl::New("","");
	probe->setProbeSector(probedata.second);
	dummyTool->setProbeSector(probe);
	CHECK(dummyTool->getProbe());
	CHECK(dummyTool->getProbe()->isValid());
	dummyTool->setVisible(true);
	// TODO: refactor toolmanager to be runnable in dummy mode (playback might benefit from this too)
	cx::trackingService()->runDummyTool(dummyTool);
	CHECK(dummyTool->getProbe()->getRTSource());
}

void AcquisitionFixture::initialize()
{
	// select video source: use a small one because old machines cannot handle RT streaming of 1024x768 color data.
//	mAcqDataFilename = cx::DataLocations::getTestDataPath() +
//			"/testing/"
//			"2012-10-24_12-39_Angio_i_US3.cx3/US_Acq/US-Acq_03_20121024T132330.mhd";
//	mAcqDataFilename = cx::DataLocations::getTestDataPath() + "/testing/us_videos/acq_512x384.mhd";
	mAcqDataFilename = cx::DataLocations::getTestDataPath() + "/testing/us_videos/acq_256x192.mhd";

	qApp->processEvents(); // wait for stateservice to finish init of application states - needed before load patient.
	cx::patientService()->getPatientData()->newPatient(cx::DataLocations::getTestDataPath() + "/temp/Acquisition/");
	mAcquisitionData.reset(new cx::AcquisitionData(this->createReconstructionManager()));

	mAcquisitionBase.reset(new cx::Acquisition(mAcquisitionData));
	mAcquisition.reset(new cx::USAcquisition(mAcquisitionBase));
	connect(mAcquisitionBase.get(), SIGNAL(readinessChanged()), this, SLOT(readinessChangedSlot()));
	connect(mAcquisition.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(saveDataCompletedSlot(QString)));
	connect(mAcquisition.get(), SIGNAL(acquisitionDataReady()), this, SLOT(acquisitionDataReadySlot()));

	// run setup of video, probe and start acquisition in series, each depending on the success of the previous:
	QTimer::singleShot(0, this, SLOT(setupVideo()));
	connect(cx::videoService()->getVideoConnection().get(), SIGNAL(connected(bool)), this, SLOT(videoConnectedSlot()));
	connect(cx::toolManager(), SIGNAL(trackingStarted()), this, SLOT(start()));
}

void AcquisitionFixture::videoConnectedSlot()
{
	SSC_LOG("");

	// make sure all sources have started streaming before running probe setup (there might be several sources)
	if (cx::videoService()->getVideoSources().size() < mNumberOfExpectedStreams)
	{
		// loop back to this handler
		QTimer::singleShot(50, this, SLOT(videoConnectedSlot()));
		return;
	}
	QTimer::singleShot(50, this, SLOT(setupProbe()));
}

void AcquisitionFixture::start()
{
	SSC_LOG("");
	mAcquisitionBase->startRecord();
	QTimer::singleShot(mRecordDuration, this, SLOT(stop()));
}

void AcquisitionFixture::stop()
{
	SSC_LOG("");
	mAcquisitionBase->stopRecord();
}

void AcquisitionFixture::newFrameSlot()
{
	// add debug code here if needed.
}

void AcquisitionFixture::readinessChangedSlot()
{
	std::cout << QString("Acquisition Ready Status %1: %2")
	             .arg(mAcquisitionBase->isReady())
	             .arg(mAcquisitionBase->getInfoText()) << std::endl;
}

void AcquisitionFixture::acquisitionDataReadySlot()
{
	SSC_LOG("");

	// read data and print info - this if the result of the memory pathway
	mMemOutputData = mAcquisitionData->getReconstructer()->getSelectedFileData();
}

void AcquisitionFixture::saveDataCompletedSlot(QString path)
{
	SSC_LOG("");

	// this is the last step: quit when finished
	if (!mAcquisition->getNumberOfSavingThreads())
		QTimer::singleShot(100,   qApp, SLOT(quit()) );

	// read file and print info - this is the result of the file pathway
	cx::UsReconstructionFileReaderPtr fileReader(new cx::UsReconstructionFileReader());
	mFileOutputData.push_back(fileReader->readAllFiles(path, ""));
}

void AcquisitionFixture::verifyFileData(cx::USReconstructInputData fileData)
{
	std::cout << "\tfilename: " << fileData.mFilename << std::endl;
	std::cout << "\tframe count: " << fileData.mFrames.size() << std::endl;
	std::cout << "\trecord duration: " << mRecordDuration << " ms" << std::endl;
	if (!fileData.mFrames.empty())
		std::cout << "\ttime: " << fileData.mFrames.back().mTime - fileData.mFrames.front().mTime << std::endl;

	double tolerance = 0.1;
	QString msg;

	REQUIRE(!fileData.mFilename.isEmpty());

	// check for enough received image frames
//	int framesPerSecond = 20; // minimum frame rate
	int framesPerSecond = 10; // minimum frame rate, reduced because tests on old computers have a hard time reaching 10 fps.
	msg = QString("Frames received: %1. Required: %2fps over %3ms").arg(fileData.mFrames.size()).arg(framesPerSecond).arg(mRecordDuration);
	INFO(string_cast(msg));
	CHECK(fileData.mFrames.size() > framesPerSecond*mRecordDuration/1000);

	// check for duration equal to input duration
	double frame_time_ms = fileData.mFrames.back().mTime - fileData.mFrames.front().mTime;
	msg = QString("Frames received over period: %1ms. Required: %2ms with a tolerance of %3").arg(frame_time_ms).arg(mRecordDuration).arg(tolerance);
	INFO(string_cast(msg));
	CHECK(cx::similar(frame_time_ms, mRecordDuration, tolerance*mRecordDuration));

	int positionsPerSecond = 10; // minimum tracker pos rate
	msg = QString("Tracker positions received: %1. Required: %2tps over %3ms").arg(fileData.mPositions.size()).arg(positionsPerSecond).arg(mRecordDuration);
	INFO(string_cast(msg));
	CHECK(fileData.mPositions.size() > positionsPerSecond*mRecordDuration/1000);

	// check for duration equal to input duration
	double pos_time_ms = fileData.mPositions.back().mTime - fileData.mPositions.front().mTime;
	msg = QString("Tracker positions received over period: %1ms. Required: %2ms with a tolerance of %3").arg(pos_time_ms).arg(mRecordDuration).arg(tolerance);
	INFO(string_cast(msg));
	CHECK(cx::similar(pos_time_ms, mRecordDuration, tolerance*mRecordDuration));

	CHECK(fileData.mProbeData.mData.getType()!=cx::ProbeDefinition::tNONE);

	// check content of images
	cx::ImageDataContainerPtr images = fileData.mUsRaw->getImageContainer();
	CHECK(images->size() == fileData.mFrames.size());
	for (unsigned i=0; i<images->size(); ++i)
	{
		CHECK(images->get(i));
		Eigen::Array3i dim(images->get(i)->GetDimensions());
		CHECK(dim[0]==fileData.mProbeData.mData.getSize().width());
		CHECK(dim[1]==fileData.mProbeData.mData.getSize().height());
	}
}

void AcquisitionFixture::verify()
{
	CHECK(mAcquisition->getNumberOfSavingThreads()==0);

	QString msg = QString("Got %1 streams, expected %2").arg(mFileOutputData.size()).arg(mNumberOfExpectedStreams);
	INFO(string_cast(msg));
	CHECK(mNumberOfExpectedStreams==mFileOutputData.size());

	std::cout << " ** Resulting USReconstructInputData memory content:" << std::endl;
	this->verifyFileData(mMemOutputData);

	for (unsigned i=0; i< mNumberOfExpectedStreams; ++i)
	{
		std::cout << QString(" ** Resulting USReconstructInputData file content [%1]:").arg(i) << std::endl;
		this->verifyFileData(mFileOutputData[i]);
	}
}

} // namespace cxtest

