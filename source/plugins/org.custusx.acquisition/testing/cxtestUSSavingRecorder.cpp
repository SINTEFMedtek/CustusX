/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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



