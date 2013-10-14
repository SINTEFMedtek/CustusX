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
#include "sscMessageManager.h"

TEST_CASE("ProcessWrapper can be constructed", "[unit][resource][core][ProcessWrapper]")
{
	cx::ProcessWrapperPtr exe(new cx::ProcessWrapper());
	CHECK(exe.get());
}

TEST_CASE("ProcessWrapper can handle launching of not existing executables", "[unit][resource][core][ProcessWrapper]")
{
	cx::MessageManager::initialize();

	cx::ProcessWrapperPtr exe(new cx::ProcessWrapper());
	exe->launch("dummy");

	REQUIRE(exe->getProcess());
	exe->getProcess()->waitForFinished();

	CHECK(exe->getProcess()->exitCode() == 0);

	cx::MessageManager::shutdown();
}

TEST_CASE("ProcessWrapper can check git version", "[unit][resource][core][ProcessWrapper]")
{
	cx::MessageManager::initialize();

	cx::ProcessWrapperPtr exe(new cx::ProcessWrapper());
	//TODO win/linux
	//Windows: exe->launch("\"C:\\Program Files (x86)\\Git\\cmd\\git.cmd\"", QStringList("--version"));
	//Linux: exe->launch("", QStringList("--version"));
	exe->launch("/opt/local/bin/git", QStringList("--version"));

	REQUIRE(exe->getProcess());
	exe->getProcess()->waitForFinished();

	CHECK(exe->getProcess()->exitStatus() == 0);

	cx::MessageManager::shutdown();
}
