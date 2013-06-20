#include <QApplication>

#include "cppunit/extensions/TestFactoryRegistry.h"
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestResult.h>
#include <cppunit/BriefTestProgressListener.h>

//#define RUN_ALL_TESTS

#ifndef RUN_ALL_TESTS
#include "testing/cxTestTubeSegmentationFramework.h"
#include "testing/cxTestTSFPresets.h"
#endif

/**main function for CX unit tests.
 * Perform system-wide init/deinit,
 * run all registered tests.
 */
int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	CppUnit::TextUi::TestRunner runner;

	CppUnit::BriefTestProgressListener listener;
	runner.eventManager().addListener( &listener );

#ifdef RUN_ALL_TESTS
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	runner.addTest( registry.makeTest() );
#else
//	runner.addTest( new CppUnit::TestCaller<cx::TestTSFPresets>(
//									   "TestTSFPresets::testNewPresets",
//										&cx::TestTSFPresets::testNewPresets ) );

	runner.addTest(TestTubeSegmentationFramework::suite());
	runner.addTest(cx::TestTSFPresets::suite());
#endif


	bool failed = runner.run();
	return !failed;
}

