/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSharedMemory.h"
#include "catch.hpp"

using namespace cx;

TEST_CASE("SharedMemory works", "[unit][resource][core]")
{
	for (int i = 1; i < 10; i++)
	{
		SharedMemoryServer srv("test_", i, 100 * i);
		SharedMemoryClient cli;

		bool result = cli.attach(srv.key());
		CHECK( result );
		CHECK( cli.key() == srv.key() );
		CHECK( cli.size() == srv.size() );
		CHECK( cli.buffers() == srv.buffers() );
		void *dst = srv.buffer();
        CHECK( dst );
		strcpy((char *)dst, "text");
		srv.release();
		srv.release();
		const void *src = cli.buffer();
        CHECK( src );
		CHECK( strncmp((char *)src, "text", 4) == 0 );
		cli.release();
		cli.release();
	}
}



