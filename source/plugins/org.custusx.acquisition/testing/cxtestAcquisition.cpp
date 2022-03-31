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

//These tests sometimes fails with seg. faults. Often on Ubuntu 20.04, more rarely on macOS, and seldom on Ubuntu 16.04
//The problem may seem to be related to disk access with VTK, and separate programs run serially writing to the same file,
//but keeping only one test also may fail.
TEST_CASE_METHOD(cxtest::AcquisitionFixture, "Acquisition: Create fixture", "[integration][modules][Acquisition][not_win32]")
{
	CHECK(true);
}

/** Run a full acquisition from MHD source local server using OpenIGTLink.
  * Save data and evaluate results.
  */
TEST_CASE_METHOD(cxtest::AcquisitionFixture, "Acquisition: Stream one MDHSource from LocalServer and save to disk", "[hide][integration][modules][Acquisition][not_win32][not_win64]")
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
TEST_CASE_METHOD(cxtest::AcquisitionFixture, "Acquisition: Stream one MDHSource from DirectLink and save to disk", "[hide][integration][modules][Acquisition][not_win32]")
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
TEST_CASE_METHOD(cxtest::AcquisitionFixture, "Acquisition: Stream two MDHSources from DirectLink and save to disk", "[hide][integration][modules][Acquisition][not_win32]")
{
	this->mNumberOfExpectedStreams = 2;
	this->initialize();
	this->getOption("runlocalserver")->setValueFromVariant(false);
	this->getOption("secondary")->setValueFromVariant(true);

	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
	qApp->exec();
	this->verify();
}
