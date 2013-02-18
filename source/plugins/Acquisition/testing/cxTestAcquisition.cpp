#include "cxTestAcquisition.h"

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

void TestAcquisition::setUp()
{
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
	controller.initialize();
	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
	qApp->exec();
	controller.verify();

}

void TestAcquisition::testStoreMHDSourceDirectLink()
{
	TestAcqController controller(NULL);
	controller.mConnectionMethod = "Direct Link";
	controller.initialize();
	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
	qApp->exec();
	controller.verify();

}
