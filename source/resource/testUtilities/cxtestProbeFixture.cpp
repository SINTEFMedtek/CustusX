/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
