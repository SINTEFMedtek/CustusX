/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QProcess>
#include "cxProcessWrapper.h"
#include "cxtestProcessWrapperFixture.h"


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
