#include "cxTestAcquisition.h"

#include <QTimer>
#include "boost/bind.hpp"
#include <vtkImageData.h>
#include "sscReconstructManager.h"
#include "sscImage.h"
#include "cxDataLocations.h"
#include "cxDataManager.h"

#include "cxVideoService.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxLogicManager.h"
#include "cxStateService.h"
#include "cxRepManager.h"
#include "cxTestAcqController.h"

#include "sscTestVideoSource.h"
#include "cxTestUSSavingRecorderController.h"
#include "sscLogger.h"
#include "sscDummyTool.h"
#include "cxFileHelpers.h"

void TestUSSavingRecorder::setUp()
{
	cx::removeNonemptyDirRecursively(TestUSSavingRecorderController::getDataPath());
	ssc::MessageManager::initialize();
}

void TestUSSavingRecorder::tearDown()
{
	ssc::MessageManager::shutdown();
	cx::removeNonemptyDirRecursively(TestUSSavingRecorderController::getDataPath());
}

void TestUSSavingRecorder::testOneVideoSource()
{
	TestUSSavingRecorderController controller(NULL);
	controller.setTool(ssc::ToolPtr());
	controller.addVideoSource(80, 40);

	controller.addOperation(boost::bind(&TestUSSavingRecorderController::startRecord, &controller));
	controller.addOperation(boost::bind(&TestUSSavingRecorderController::wait, &controller, 1000));
	controller.addOperation(boost::bind(&TestUSSavingRecorderController::stopRecord, &controller));

	qApp->exec();

	controller.verifyMemData("videoSource0");
}

void TestUSSavingRecorder::testOneVideoSourceWithTool()
{
	TestUSSavingRecorderController controller(NULL);
	ssc::DummyToolPtr tool = ssc::DummyToolTestUtilities::createDummyTool(ssc::DummyToolTestUtilities::createProbeDataLinear());
	controller.setTool(tool);
	controller.addVideoSource(80, 40);

	controller.addOperation(boost::bind(&TestUSSavingRecorderController::startRecord, &controller));
	controller.addOperation(boost::bind(&TestUSSavingRecorderController::wait, &controller, 1000));
	controller.addOperation(boost::bind(&TestUSSavingRecorderController::stopRecord, &controller));

	qApp->exec();

	controller.verifyMemData("videoSource0");
}

void TestUSSavingRecorder::testOneVideoSourceWithToolAndSave()
{
	TestUSSavingRecorderController controller(NULL);
	ssc::DummyToolPtr tool = ssc::DummyToolTestUtilities::createDummyTool(ssc::DummyToolTestUtilities::createProbeDataLinear());
	controller.setTool(tool);
	controller.addVideoSource(80, 40);

	controller.addOperation(boost::bind(&TestUSSavingRecorderController::startRecord, &controller));
	controller.addOperation(boost::bind(&TestUSSavingRecorderController::wait, &controller, 1000));
	controller.addOperation(boost::bind(&TestUSSavingRecorderController::stopRecord, &controller));
	controller.addOperation(boost::bind(&TestUSSavingRecorderController::saveAndWaitForCompleted, &controller));

	qApp->exec();

	controller.verifySaveData();
}

void TestUSSavingRecorder::testFourVideoSources()
{
	TestUSSavingRecorderController controller(NULL);
	controller.setTool(ssc::ToolPtr());
	for (unsigned i=0; i<4; ++i)
		controller.addVideoSource(80, 40);

	controller.addOperation(boost::bind(&TestUSSavingRecorderController::startRecord, &controller));
	controller.addOperation(boost::bind(&TestUSSavingRecorderController::wait, &controller, 1000));
	controller.addOperation(boost::bind(&TestUSSavingRecorderController::stopRecord, &controller));

	qApp->exec();

	for (unsigned i=0; i<4; ++i)
		controller.verifyMemData(QString("videoSource%1").arg(i));
}

void TestUSSavingRecorder::testFourVideoSourcesWithToolAndSave()
{
	TestUSSavingRecorderController controller(NULL);
	ssc::DummyToolPtr tool = ssc::DummyToolTestUtilities::createDummyTool(ssc::DummyToolTestUtilities::createProbeDataLinear());
	controller.setTool(tool);
	for (unsigned i=0; i<4; ++i)
		controller.addVideoSource(80, 40);

	controller.addOperation(boost::bind(&TestUSSavingRecorderController::startRecord, &controller));
	controller.addOperation(boost::bind(&TestUSSavingRecorderController::wait, &controller, 1000));
	controller.addOperation(boost::bind(&TestUSSavingRecorderController::stopRecord, &controller));
	controller.addOperation(boost::bind(&TestUSSavingRecorderController::saveAndWaitForCompleted, &controller));

	qApp->exec();

	controller.verifySaveData();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


void TestAcquisition::setUp()
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
}

void TestAcquisition::tearDown()
{
	cx::LogicManager::shutdown();
}

void TestAcquisition::testConstructor()
{
}


// start mhd file video source
// start dummy probe ?
// start acquisition
// look for saved stuff (both inmem and ondisk)
// turn on autoreconstruct and check that stuff is generated
void TestAcquisition::testStoreMHDSourceLocalServer()
{
	TestAcqController controller(NULL);
	controller.mConnectionMethod = "Local Server";
	controller.mNumberOfExpectedStreams = 1;
	controller.initialize();
	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
	qApp->exec();
	controller.verify();
}

void TestAcquisition::testStoreMHDSourceDirectLink()
{
	TestAcqController controller(NULL);
	controller.mConnectionMethod = "Direct Link";
	controller.mNumberOfExpectedStreams = 1;
	controller.initialize();
	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
	qApp->exec();
	controller.verify();
}

void TestAcquisition::testStoreMultipleMHDSourceDirectLink()
{
	TestAcqController controller(NULL);
	controller.mConnectionMethod = "Direct Link";
	controller.mAdditionalGrabberArg = "--secondary";
	controller.mNumberOfExpectedStreams = 2;
	controller.initialize();
	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
	qApp->exec();
	controller.verify();

}
