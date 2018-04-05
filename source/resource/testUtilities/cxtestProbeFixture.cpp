/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxtestProbeFixture.h"

#include <QString>

#include "ProbeXmlConfigParserMock.h"
#include "cxProbeDefinition.h"
#include "cxTestVideoSource.h"
#include "cxReporter.h"
#include "catch.hpp"

namespace cxtest
{

ProbeFixture::ProbeFixture() :
	mDefaultTemporalCalibration(0),
	mTemporalCalibration(0)
{
	// this stuff will be performed just before all tests in this class
	cx::Reporter::initialize();
	this->createTestProbe();
}


ProbeFixture::~ProbeFixture()
{
	// this stuff will be performed just after all tests in this class
	cx::Reporter::shutdown();
}


void ProbeFixture::createTestProbe()
{
	this->createParameters();
	QString xmlFileName = "testXmlFileName";
	ProbeXmlConfigParserPtr mXml;
	mXml.reset(new ProbeXmlConfigParserMock(xmlFileName));
	mDefaultRtSourceName = mXml->getRtSourceList(mProbeName, mScannerName)[0];
	mProbe = cx::ProbeImpl::New(mProbeName, mScannerName, mXml);
}


void ProbeFixture::testProbeWithNoRTSource()
{
	QString xmlFileName = "testXmlFileName";
	ProbeXmlConfigParserPtr mXml;
	bool provideRTSource = false;
	mXml.reset(new ProbeXmlConfigParserMock(xmlFileName, provideRTSource));
	mDefaultRtSourceName = QString();
	mProbe = cx::ProbeImpl::New(mProbeName, mScannerName, mXml);
}

void ProbeFixture::createParameters()
{
	mProbeName = "TestProbe";
	mScannerName = "TestScanner";
	cx::ProbeDefinition probeDefinition;
	mDefaultProbeDefinitionUid = probeDefinition.getUid(); //Uid set to "default" in ProbeDefinition()
	mDefaultTemporalCalibration = probeDefinition.getTemporalCalibration();
	mProbeDefinitionUid = "TestprobeDefinition";
}

cx::ProbeDefinition ProbeFixture::createProbeDefinition()
{
	cx::ProbeDefinition probeDefinition = mProbe->getProbeDefinition();
	probeDefinition.setUid(mProbeDefinitionUid);
	mTemporalCalibration = 1000.5;
	probeDefinition.setTemporalCalibration(mTemporalCalibration);
	return probeDefinition;
}

} //namespace cxtest
