/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxStreamedTimestampSynchronizer.h"

namespace cxtest
{

TEST_CASE("StreamedTimestampSynchronizer: one ts", "[unit]")
{
    cx::StreamedTimestampSynchronizer syncer;

    double tol = 10;
    int diff = 10000;
    QDateTime ts = QDateTime::currentDateTime();
    ts = ts.addMSecs(-diff);
    syncer.addTimestamp(ts);
    double shift = syncer.getShift();
    CHECK(fabs(shift-diff)<tol);
}

TEST_CASE("StreamedTimestampSynchronizer: many ts and outliers", "[unit]")
{
    cx::StreamedTimestampSynchronizer syncer;

    double tol = 10;
    int diff = 10000;
    QDateTime ts = QDateTime::currentDateTime();
    ts = ts.addMSecs(-diff);

    // initialize syncer with one value
    syncer.addTimestamp(ts);
		syncer.getShift();

    // add two outliers
    for (int i=0; i<2; ++i)
    {
        syncer.addTimestamp(ts.addMSecs(1000));
    }
		// add lots of normal values
		for (int i=0; i<18; ++i)
		{
				syncer.addTimestamp(ts.addMSecs(0));
		}

    double shift = syncer.getShift();
    CHECK(fabs(shift-diff)<tol);
}

TEST_CASE("StreamedTimestampSynchronizer: convergence to new value", "[unit]")
{
    cx::StreamedTimestampSynchronizer syncer;

	double tol = 20;
    int diff = 10000;
    QDateTime ts = QDateTime::currentDateTime();
    ts = ts.addMSecs(-diff);

    // initialize syncer with one value
    syncer.addTimestamp(ts);
    syncer.getShift();

    // add lots of varying  values (note that all the ts'es are compared to currenttime())
    for (int i=0; i<30; ++i)
    {
        syncer.addTimestamp(ts.addMSecs(10*i));
    }

    int offset = 1000;
    // add lots of stable values
    for (int i=0; i<30; ++i)
    {
        syncer.addTimestamp(ts.addMSecs(offset));
    }

    double shift = syncer.getShift();
    CHECK(fabs(shift-diff+offset)<tol);
}

TEST_CASE("StreamedTimestampSynchronizer: Outlayers", "[unit]")
{
	cx::StreamedTimestampSynchronizer syncer;

	double tol = 2;
	int diff = 1000;
	QDateTime ts = QDateTime::currentDateTime();
	ts = ts.addMSecs(-diff);

	// Initialize filter with "normal" values
	for (int i=0; i<20; ++i)
		syncer.addTimestamp(ts.addMSecs(0));

	syncer.getShift(); //Needed initialize

	syncer.addTimestamp(ts.addMSecs(+ 100));
	CHECK(fabs(syncer.getShift()-diff)<tol);

	syncer.addTimestamp(ts.addMSecs(- 200));
	CHECK(fabs(syncer.getShift()-diff)<tol);

	syncer.addTimestamp(ts.addMSecs(+ 300));
	CHECK(fabs(syncer.getShift()-diff)<tol);

	syncer.addTimestamp(ts.addMSecs(+ 400));
	CHECK(fabs(syncer.getShift()-diff)<tol);
}

} // namespace cxtest
