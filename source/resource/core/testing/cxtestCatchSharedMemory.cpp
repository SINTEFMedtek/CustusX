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

#include "sscSharedMemory.h"
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
		CHECK( dst != NULL );
		strcpy((char *)dst, "text");
		srv.release();
		srv.release();
		const void *src = cli.buffer();
		CHECK( src != NULL );
		CHECK( strncmp((char *)src, "text", 4) == 0 );
		cli.release();
		cli.release();
	}
}



