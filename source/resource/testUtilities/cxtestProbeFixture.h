/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTPROBEFIXTURE_H
#define CXTESTPROBEFIXTURE_H

#include "cxtestutilities_export.h"

#include "cxTool.h"
#include "cxProbeImpl.h"

namespace cxtest
{

/*
 * \class TestProbe
 *
 * \Brief Unit tests for the cxProbe class
 *
 * \date Jun 3, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class CXTESTUTILITIES_EXPORT ProbeFixture
{
public:
	ProbeFixture();
	~ProbeFixture();

	void createTestProbe();
	void testProbeWithNoRTSource();
	void createParameters();
	cx::ProbeDefinition createProbeDefinition();
	QString mProbeName;
	QString mScannerName;
	QString mDefaultProbeDefinitionUid;
	QString mProbeDefinitionUid;
	QString mDefaultRtSourceName;
	cx::ProbeImplPtr mProbe;
	double mDefaultTemporalCalibration;
	double mTemporalCalibration;
};

} //namespace cxtest

#endif // CXTESTPROBEFIXTURE_H
