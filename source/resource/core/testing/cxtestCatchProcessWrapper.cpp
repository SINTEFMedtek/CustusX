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

#include "catch.hpp"

#include <QProcess>
#include "cxProcessWrapper.h"
#include "cxtestProcessWrapperFixture.h"
#include "sscMessageManager.h"

namespace cxtest
{

TEST_CASE("ProcessWrapper can be constructed", "[unit][resource][core][ProcessWrapper]")
{
	cx::ProcessWrapperPtr exe(new cx::ProcessWrapper());
	CHECK(exe.get());
}

TEST_CASE("ProcessWrapper can handle launching of not existing executables", "[unit][resource][core][ProcessWrapper]")
{
	bool success = ProcessWrapperFixture::canLaunchNotExistingExecutable();
	CHECK_FALSE(success);
}

TEST_CASE("ProcessWrapper can check git -version", "[unit][resource][core][ProcessWrapper][git]")
{
	bool success = ProcessWrapperFixture::canLaunchGit_Version();
	CHECK(success);
}

TEST_CASE("ProcessWrapper can run VLC -version", "[unit][resource][core][ProcessWrapper][VLC][not_win32][not_win64]")
{
	bool success = ProcessWrapperFixture::canLaunchVLC_Version();
	CHECK(success);
}

} //namespace cxtest
