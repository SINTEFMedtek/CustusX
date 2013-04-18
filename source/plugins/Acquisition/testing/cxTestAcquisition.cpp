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
#include "cxTestAcqCoreController.h"
#include "sscLogger.h"
#include "sscDummyTool.h"

void TestUSAcquisitionCore::setUp()
{
	ssc::MessageManager::initialize();
}

void TestUSAcquisitionCore::tearDown()
{

}

void TestUSAcquisitionCore::testOneVideoSource()
{
	TestAcqCoreController controller(NULL);
//	controller.setRecordDuration(1000);
	controller.setTool(ssc::ToolPtr());
	controller.addVideoSource(80, 40);

	controller.addOperation(boost::bind(&TestAcqCoreController::startRecord, &controller));
	controller.addOperation(boost::bind(&TestAcqCoreController::wait, &controller, 1000));
	controller.addOperation(boost::bind(&TestAcqCoreController::stopRecord, &controller));

	SSC_LOG("pre exec");
	qApp->exec();
	SSC_LOG("post exec");

	controller.verifyMemData("videoSource0");
}

void TestUSAcquisitionCore::testOneVideoSourceWithTool()
{
	TestAcqCoreController controller(NULL);
	ssc::DummyToolPtr tool = ssc::DummyToolTestUtilities::createDummyTool(ssc::DummyToolTestUtilities::createProbeDataLinear());
	controller.setTool(tool);
	controller.addVideoSource(80, 40);

	controller.addOperation(boost::bind(&TestAcqCoreController::startRecord, &controller));
	controller.addOperation(boost::bind(&TestAcqCoreController::wait, &controller, 1000));
	controller.addOperation(boost::bind(&TestAcqCoreController::stopRecord, &controller));

	SSC_LOG("pre exec");
	qApp->exec();
	SSC_LOG("post exec");

	controller.verifyMemData("videoSource0");
}

void TestUSAcquisitionCore::testOneVideoSourceWithToolAndSave()
{
	TestAcqCoreController controller(NULL);
	ssc::DummyToolPtr tool = ssc::DummyToolTestUtilities::createDummyTool(ssc::DummyToolTestUtilities::createProbeDataLinear());
	controller.setTool(tool);
	controller.addVideoSource(80, 40);

	controller.addOperation(boost::bind(&TestAcqCoreController::startRecord, &controller));
	controller.addOperation(boost::bind(&TestAcqCoreController::wait, &controller, 1000));
	controller.addOperation(boost::bind(&TestAcqCoreController::stopRecord, &controller));
	controller.addOperation(boost::bind(&TestAcqCoreController::saveAndWaitForCompleted, &controller));

	SSC_LOG("pre exec");
	qApp->exec();
	SSC_LOG("post exec");

	controller.verifySaveData();
}

void TestUSAcquisitionCore::testFourVideoSources()
{
	TestAcqCoreController controller(NULL);
	controller.setTool(ssc::ToolPtr());
	for (unsigned i=0; i<4; ++i)
		controller.addVideoSource(80, 40);

	controller.addOperation(boost::bind(&TestAcqCoreController::startRecord, &controller));
	controller.addOperation(boost::bind(&TestAcqCoreController::wait, &controller, 1000));
	controller.addOperation(boost::bind(&TestAcqCoreController::stopRecord, &controller));

	SSC_LOG("pre exec");
	qApp->exec();
	SSC_LOG("post exec");

	for (unsigned i=0; i<4; ++i)
		controller.verifyMemData(QString("videoSource%1").arg(i));
}

void TestUSAcquisitionCore::testFourVideoSourcesWithToolAndSave()
{
	TestAcqCoreController controller(NULL);
	ssc::DummyToolPtr tool = ssc::DummyToolTestUtilities::createDummyTool(ssc::DummyToolTestUtilities::createProbeDataLinear());
	controller.setTool(tool);
	for (unsigned i=0; i<4; ++i)
		controller.addVideoSource(80, 40);

	controller.addOperation(boost::bind(&TestAcqCoreController::startRecord, &controller));
	controller.addOperation(boost::bind(&TestAcqCoreController::wait, &controller, 1000));
	controller.addOperation(boost::bind(&TestAcqCoreController::stopRecord, &controller));
	controller.addOperation(boost::bind(&TestAcqCoreController::saveAndWaitForCompleted, &controller));

	SSC_LOG("pre exec");
	qApp->exec();
	SSC_LOG("post exec");

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
