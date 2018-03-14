/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxDataLocations.h"
#include "cxTemporalCalibration.h"

TEST_CASE("TemporalCalibration reproduces old results on a test data set", "[unit][modules][calibration]")
{
  cx::TemporalCalibration calibrator;
  QString filename = cx::DataLocations::getTestDataPath() + "/testing/20110511T092103_temporal_calib_mac.cx3/US_Acq/US-Acq_01_20110511T092317/US-Acq_01_20110511T092317.mhd";
  calibrator.selectData(filename);
  bool success = false;
  double shift = calibrator.calibrate(&success);

  double testValue = 115; // shift found on data set during first tests.

  CHECK( success );
  CHECK( cx::similar(shift, testValue, 1));
}



