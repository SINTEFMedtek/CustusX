/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <QTimer>
#include "cxtestAcquisitionFixture.h"
#include <QApplication>

#include "catch.hpp"
#include "cxProperty.h"

TEST_CASE_METHOD(cxtest::AcquisitionFixture, "Acquisition: Create fixture", "[integration][modules][Acquisition][not_win32]")
{
	CHECK(true);
}

/** Run a full acquisition from MHD source local server using OpenIGTLink.
  * Save data and evaluate results.
  */
TEST_CASE_METHOD(cxtest::AcquisitionFixture, "Acquisition: Stream one MDHSource from LocalServer and save to disk", "[integration][modules][Acquisition][not_win32]")
{
	this->mNumberOfExpectedStreams = 1;
	this->initialize();
	this->getOption("runlocalserver")->setValueFromVariant(true);
	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
	qApp->exec();
	this->verify();
}

/** Run a full acquisition from MHD source direct link.
  * Save data and evaluate results.
  */
TEST_CASE_METHOD(cxtest::AcquisitionFixture, "Acquisition: Stream one MDHSource from DirectLink and save to disk", "[integration][modules][Acquisition][not_win32]")
{
	this->mNumberOfExpectedStreams = 1;
	this->initialize();
	this->getOption("runlocalserver")->setValueFromVariant(false);

	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
	qApp->exec();
	this->verify();
}

/** Test the MHD file source using the --secondary option,
  * i.e. sending two streams and saving them to disk.
  */
TEST_CASE_METHOD(cxtest::AcquisitionFixture, "Acquisition: Stream two MDHSources from DirectLink and save to disk", "[integration][modules][Acquisition][not_win32]")
{
	this->mNumberOfExpectedStreams = 2;
	this->initialize();
	this->getOption("runlocalserver")->setValueFromVariant(false);
	this->getOption("secondary")->setValueFromVariant(true);

	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
	qApp->exec();
	this->verify();
}
