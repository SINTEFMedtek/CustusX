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

#ifndef CXTESTPROBEFIXTURE_H
#define CXTESTPROBEFIXTURE_H

#include "sscTool.h"
#include "cxProbeImpl.h"

namespace cxtest
{

/*
 * \class TestProbe
 *
 * \Brief Unit tests for the cxProbe class4
 *
 * \date Jun 3, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class ProbeFixture
{
public:
	ProbeFixture();
	~ProbeFixture();

protected:
	void createTestProbe();
	void createParameters();
	cx::ProbeDefinition createProbeData();
	QString mProbeName;
	QString mScannerName;
	QString mDefaultProbeDataUid;
	QString mProbeDataUid;
	QString mDefaultRtSourceName;
	cx::cxProbePtr mProbe;
	double mDefaultTemporalCalibration;
	double mTemporalCalibration;
};

} //namespace cxtest

#endif // CXTESTPROBEFIXTURE_H
