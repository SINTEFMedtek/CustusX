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

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "sscApplication.h"
#include "cxtestCatchImpl.h"

namespace cxtest
{

int CatchImpl::runCatchMain(int argc, char* argv[])
{
	ssc::Application app( argc, argv );

	int result = Catch::Main( argc, argv );

	return result;
}

} /* namespace cxtest */
