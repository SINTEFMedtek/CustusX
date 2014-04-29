// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "catch.hpp"

#include "cxDataLocations.h"
#include "cxTemporalCalibration.h"

TEST_CASE("TemporalCalibration reproduces old results on a test data set", "[unit][modules][calibration]")
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

  CHECK( success );
  CHECK( cx::similar(shift, testValue, 1));
}



