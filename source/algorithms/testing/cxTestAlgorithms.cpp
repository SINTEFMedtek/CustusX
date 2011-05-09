#include "cxTestAlgorithms.h"

//#include "cxTestCustusXController.h"
//#include "cxConfig.h"
#include "cxDataLocations.h"
#include "cxTemporalCalibration.h"

void TestAlgorithms::testTemporalCalibration()
{
  cx::TemporalCalibration calibrator;
  QString filename = cx::DataLocations::getTestDataPath() + "/testing/temporal_calib.cx3/US_Acq/temporal_cal_01_20110505T155645/temporal_cal_01_20110505T155645.mhd";
  calibrator.selectData(filename);
  calibrator.setDebugFile("/home/christiana/christiana/test_temp2.txt");
  double shift = calibrator.calibrate();

  double testValue = 2156.86; // shift found on data set during first tests.

  CPPUNIT_ASSERT( ssc::similar(shift, testValue, 10));
}



