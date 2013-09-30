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

#include "cxtestUSSavingRecorderFixture.h"

#include "boost/bind.hpp"
#include "catch.hpp"
#include "sscDummyToolManager.h"

TEST_CASE_METHOD(cxtest::USSavingRecorderFixture, "USSavingRecorder: Use one VideoSource", "[integration][plugins][Acquisition]")
{
	this->setTool(cx::ToolPtr());
	this->addVideoSource(80, 40);

	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::startRecord, this));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::wait, this, 1000));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::stopRecord, this));

	qApp->exec();

	this->verifyMemData("videoSource0");
}

TEST_CASE_METHOD(cxtest::USSavingRecorderFixture, "USSavingRecorder: Use one VideoSource with Tool", "[integration][plugins][Acquisition]")
{
	cx::DummyToolPtr tool = cx::DummyToolTestUtilities::createDummyTool(cx::DummyToolTestUtilities::createProbeDataLinear());
	this->setTool(tool);
	this->addVideoSource(80, 40);

	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::startRecord, this));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::wait, this, 1000));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::stopRecord, this));

	qApp->exec();

	this->verifyMemData("videoSource0");
}

TEST_CASE_METHOD(cxtest::USSavingRecorderFixture, "USSavingRecorder: Use one VideoSource with Tool and save", "[integration][plugins][Acquisition]")
{
	cx::DummyToolPtr tool = cx::DummyToolTestUtilities::createDummyTool(cx::DummyToolTestUtilities::createProbeDataLinear());
	this->setTool(tool);
	this->addVideoSource(80, 40);

	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::startRecord, this));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::wait, this, 1000));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::stopRecord, this));
	this->addOperation(boost::bind(&cxtest::USSavingRecorderFixture::saveAndWaitForCompleted, this));

	qApp->exec();

	this->verifySaveData();
}

TEST_CASE_METHOD(cxtest::USSavingRecorderFixture, "USSavingRecorder: Use 4 VideoSources", "[integration][plugins][Acquisition]")
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

TEST_CASE_METHOD(cxtest::USSavingRecorderFixture, "USSavingRecorder: Use 4 VideoSources with Tool and save", "[integration][plugins][Acquisition]")
{
	cx::DummyToolPtr tool = cx::DummyToolTestUtilities::createDummyTool(cx::DummyToolTestUtilities::createProbeDataLinear());
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



