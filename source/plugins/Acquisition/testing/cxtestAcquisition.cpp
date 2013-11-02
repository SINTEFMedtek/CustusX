#include <QTimer>
#include "cxtestAcquisitionFixture.h"
#include <QApplication>

#include "catch.hpp"

TEST_CASE_METHOD(cxtest::AcquisitionFixture, "Acquisition: Create fixture", "[integration][plugins][Acquisition]")
{
	CHECK(true);
}

/** Run a full acquisition from MHD source local server using OpenIGTLink.
  * Save data and evaluate results.
  */
TEST_CASE_METHOD(cxtest::AcquisitionFixture, "Acquisition: Stream one MDHSource from LocalServer and save to disk", "[integration][plugins][Acquisition]")
{
	this->mConnectionMethod = "Local Server";
	this->mNumberOfExpectedStreams = 1;
	this->initialize();
	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
	qApp->exec();
	this->verify();
}

/** Run a full acquisition from MHD source direct link.
  * Save data and evaluate results.
  */
TEST_CASE_METHOD(cxtest::AcquisitionFixture, "Acquisition: Stream one MDHSource from DirectLink and save to disk", "[integration][plugins][Acquisition]")
{
	this->mConnectionMethod = "Direct Link";
	this->mNumberOfExpectedStreams = 1;
	this->initialize();
	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
	qApp->exec();
	this->verify();
}

/** Test the MHD file source using the --secondary option,
  * i.e. sending two streams and saving them to disk.
  */
TEST_CASE_METHOD(cxtest::AcquisitionFixture, "Acquisition: Stream two MDHSources from DirectLink and save to disk", "[integration][plugins][Acquisition]")
{
	this->mConnectionMethod = "Direct Link";
	this->mAdditionalGrabberArg = "--secondary";
	this->mNumberOfExpectedStreams = 2;
	this->initialize();
	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
	qApp->exec();
	this->verify();
}
