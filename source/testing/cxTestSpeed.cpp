#include "cxTestSpeed.h"

#include "cxTestCustusXController.h"
#include <QTimer>

void cxTestSpeed::setUp()
{
	// this stuff will be performed just before all tests in this class
}

void cxTestSpeed::tearDown()
{
	// this stuff will be performed just after all tests in this class
}


/**
 */
void cxTestSpeed::testInitialize()
{
  CustusXController* custusX = new CustusXController(NULL);

  custusX->start();
  custusX->mPatientFolder = "/Users/christiana/Patients/Kaisa_Speed_Test.cx3";

  QTimer::singleShot(0, custusX, SLOT(loadPatientSlot()) );
  QTimer::singleShot(30*1000, qApp, SLOT(quit()) );

  qApp->exec();
  custusX->stop();

  delete custusX;
}

void cxTestSpeed::testMore()
{

}


