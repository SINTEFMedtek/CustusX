#include "cxTestAlgorithms.h"
#include <iostream>

//#include "cxTestCustusXController.h"
//#include "cxConfig.h"
#include "cxDataLocations.h"
#include "cxTemporalCalibration.h"

void TestAlgorithms::testTemporalCalibration()
{
  cx::TemporalCalibration calibrator;
//  QString filename = "/home/christiana/Patients/20110511T092103_temporal_calib_mac.cx3/US_Acq/US-Acq_01_20110511T092317/US-Acq_01_20110511T092317.mhd";
//  QString filename = cx::DataLocations::getTestDataPath() + "/testing/temporal_calib.cx3/US_Acq/temporal_cal_01_20110505T155645/temporal_cal_01_20110505T155645.mhd";
  QString filename = cx::DataLocations::getTestDataPath() + "/testing/20110511T092103_temporal_calib_mac.cx3/US_Acq/US-Acq_01_20110511T092317/US-Acq_01_20110511T092317.mhd";
  calibrator.selectData(filename);
//  calibrator.setDebugFolder("/home/christiana/christiana/");
  bool success = false;
  double shift = calibrator.calibrate(&success);
//  std::cout << "val " << shift << std::endl;

  double testValue = 115; // shift found on data set during first tests.

  CPPUNIT_ASSERT( success );
  CPPUNIT_ASSERT( ssc::similar(shift, testValue, 1));
}



