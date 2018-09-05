/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"


#include "cxtestTestToolMesh.h"
#include "cxReporter.h"
#include "cxLogicManager.h"
#include "cxFileManagerServiceProxy.h"

namespace cxtest
{

#ifdef CX_CUSTUS_SINTEF
TEST_CASE("Sonowand planning navigator STL file are readable", "[unit][tool][org.custus.core.tracking]")
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());
//	cx::Reporter::initialize();

	TestToolMesh meshTester;
	meshTester.setToolPath("Neurology/SW-Invite/SW-Planning-Navigator_01-117-0329_POLARIS/");
	REQUIRE(meshTester.canLoadMesh("01-117-0329_Planning-Navigator.stl", filemanager));

//	cx::Reporter::shutdown();
	cx::LogicManager::shutdown();
}

TEST_CASE("Sonowand intraop navigator STL file are readable", "[unit][tool][org.custus.core.tracking]")
{
//	cx::Reporter::initialize();
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	TestToolMesh meshTester;
	meshTester.setToolPath("Neurology/SW-Invite/SW-Intraoperative-Navigator-07-081-0223_POLARIS/");
	CHECK(meshTester.canLoadMesh("SW-Intraop-Navigator.stl", filemanager));

//	cx::Reporter::shutdown();
	cx::LogicManager::shutdown();
}
#endif

} //namespace cxtest
