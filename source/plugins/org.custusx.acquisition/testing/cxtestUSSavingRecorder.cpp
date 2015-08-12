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

#include "cxtestUSSavingRecorderFixture.h"

#include "boost/bind.hpp"
#include "catch.hpp"
#include "cxDummyToolManager.h"

TEST_CASE_METHOD(cxtest::USSavingRecorderFixture, "USSavingRecorder: Use one VideoSource", "[integration][modules][Acquisition]")
{
	this->setTool(cx::ToolPtr());
	this->addVideoSource(80, 40);

	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::startRecord, this));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::wait, this, 1000));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::stopRecord, this));

	qApp->exec();

	this->verifyMemData("videoSource0");
}

TEST_CASE_METHOD(cxtest::USSavingRecorderFixture, "USSavingRecorder: Use one VideoSource with Tool", "[integration][modules][Acquisition]")
{
	cx::DummyToolPtr tool = cx::DummyToolTestUtilities::createDummyTool(cx::DummyToolTestUtilities::createProbeDefinitionLinear());
	this->setTool(tool);
	this->addVideoSource(80, 40);

	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::startRecord, this));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::wait, this, 1000));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::stopRecord, this));

	qApp->exec();

	this->verifyMemData("videoSource0");
}

TEST_CASE_METHOD(cxtest::USSavingRecorderFixture, "USSavingRecorder: Use one VideoSource with Tool and save", "[integration][modules][Acquisition]")
{
	cx::DummyToolPtr tool = cx::DummyToolTestUtilities::createDummyTool(cx::DummyToolTestUtilities::createProbeDefinitionLinear());
	this->setTool(tool);
	this->addVideoSource(80, 40);

	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::startRecord, this));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::wait, this, 1000));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::stopRecord, this));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::saveAndWaitForCompleted, this));

	qApp->exec();

	this->verifySaveData();
}

TEST_CASE_METHOD(cxtest::USSavingRecorderFixture, "USSavingRecorder: Use 4 VideoSources", "[integration][modules][Acquisition][unstable]")
{
	this->setTool(cx::ToolPtr());
	for (unsigned i=0; i<4; ++i)
		this->addVideoSource(80, 40);

	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::startRecord, this));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::wait, this, 1000));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::stopRecord, this));

	qApp->exec();

	for (unsigned i=0; i<4; ++i)
		this->verifyMemData(QString("videoSource%1").arg(i));
}

TEST_CASE_METHOD(cxtest::USSavingRecorderFixture, "'USSavingRecorder: Use 4 VideoSources with Tool and save", "[integration][modules][Acquisition][unstable]")
{
	cx::DummyToolPtr tool = cx::DummyToolTestUtilities::createDummyTool(cx::DummyToolTestUtilities::createProbeDefinitionLinear());
	this->setTool(tool);
	for (unsigned i=0; i<4; ++i)
		this->addVideoSource(80, 40);

	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::startRecord, this));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::wait, this, 1000));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::stopRecord, this));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::saveAndWaitForCompleted, this));

	qApp->exec();

	this->verifySaveData();
}



