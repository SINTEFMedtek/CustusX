#include "catch.hpp"

#include "cxMessageManager.h"
#include "cxtestTestToolMesh.h"

namespace cxtest
{
TEST_CASE("Sonowand tools' STL files are readable", "[unit][tool]")
{
	cx::MessageManager::initialize();

	TestToolMesh *meshTester = new TestToolMesh();
	meshTester->setToolPath("Neurology/SW-Invite/SW-Intraoperative-Navigator-07-081-0223_POLARIS/");
	CHECK(meshTester->canLoadMesh("07-081-0223_New_Intraoperative-Navigator.stl"));

	meshTester->setToolPath("Neurology/SW-Invite/SW-Planning-Navigator_01-117-0329_POLARIS/");
	REQUIRE(meshTester->canLoadMesh("01-117-0329_Planning-Navigator.stl"));
	delete meshTester;

	cx::MessageManager::shutdown();
}
} //namespace cxtest
