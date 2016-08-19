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
	this->getOption("secondary")->setValueFromVariant(false);
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
	this->getOption("secondary")->setValueFromVariant(false);

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
