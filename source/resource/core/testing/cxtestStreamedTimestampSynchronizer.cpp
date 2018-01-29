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
