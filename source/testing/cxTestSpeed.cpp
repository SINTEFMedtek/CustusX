#include "cxTestSpeed.h"

#include "cxTestCustusXController.h"
//#include "cxConfig.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
#include <QTimer>

void cxTestSpeed::setUp()
{
	// this stuff will be performed just before all tests in this class
	mPreviousRenderingInterval = cx::settings()->value("renderingInterval").toInt();
	cx::settings()->setValue("renderingInterval", 4);
}

void cxTestSpeed::tearDown()
{
	// this stuff will be performed just after all tests in this class
	cx::settings()->setValue("renderingInterval", mPreviousRenderingInterval);
}

/**
 */
void cxTestSpeed::testKaisaMetricsSpeed()
{
  CustusXController custusX(NULL);

  custusX.mPatientFolder = cx::DataLocations::getTestDataPath() + "/Phantoms/Kaisa/CustusX/Speed_Test_Kaisa.cx3";
  custusX.start();
  //custusX.mPatientFolder = "/Users/christiana/Patients/Speed_Test_Lap_Large.cx3.cx3";

  QTimer::singleShot(      0,   &custusX, SLOT(initialBeginCheckRenderSlot()) );
  QTimer::singleShot(      0,   &custusX, SLOT(loadPatientSlot()) );
  QTimer::singleShot( 5*1000,   &custusX, SLOT(initialEndCheckRenderSlot()) );
//  QTimer::singleShot( 5*1000+1, &custusX, SLOT(initialBeginCheckRenderSlot()) );
//  QTimer::singleShot(20*1000,   &custusX, SLOT(secondEndCheckRenderSlot()) );
//  QTimer::singleShot(21*1000,   &custusX, SLOT(displayResultsSlot()) );


//  QTimer::singleShot(25*1000,   qApp, SLOT(quit()) );

  qApp->exec();
  custusX.stop();
}

//void cxTestSpeed::testKaisaMetricsSpeed()
//{
//  CustusXController custusX(NULL);

//  custusX.mPatientFolder = cx::DataLocations::getTestDataPath() + "/Phantoms/Kaisa/CustusX/Speed_Test_Kaisa_metrics.cx3";
//  custusX.start();

//  QTimer::singleShot(10*1000,   qApp, SLOT(quit()) );

//  qApp->exec();
//  custusX.stop();
//}

///**
// */
//void cxTestSpeed::testLapSpeed()
//{
//  CustusXController custusX(NULL);

//  custusX.mPatientFolder = "/Users/christiana/Patients/Speed_Test_Lap_Large.cx3";
////  custusX.mPatientFolder = cx::DataLocations::getTestDataPath() + "/Lap/2010-10-18/CustusX/Speed_Test_Lap_Large.cx3";
//  custusX.start();

////  QTimer::singleShot(      0,   &custusX, SLOT(beginCheckRenderSlot()) );
////  QTimer::singleShot(      0,   &custusX, SLOT(loadPatientSlot()) );
////  QTimer::singleShot( 5*1000,   &custusX, SLOT(endCheckRenderSlot()) );
////  QTimer::singleShot( 5*1000+1, &custusX, SLOT(beginCheckRenderSlot()) );
////  QTimer::singleShot(20*1000,   &custusX, SLOT(endCheckRenderSlot()) );
////  QTimer::singleShot(21*1000,   &custusX, SLOT(displayResultsSlot()) );


//  QTimer::singleShot(5*1000,   qApp, SLOT(quit()) );

//  qApp->exec();
//  custusX.stop();
//}


