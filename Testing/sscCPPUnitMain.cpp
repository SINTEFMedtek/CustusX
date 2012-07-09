#include <QApplication>

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestResult.h> 
#include <cppunit/BriefTestProgressListener.h> 

#ifndef RUN_ALL_TESTS
#include "sscTestUtilityClasses.h"
#include "sscTestSliceComputer.h"
#include "sscTestVisualRendering.h"
#include "sscTestViewContainer.h"
#include "sscTestTransform3D.h"
#include "sscTestBoundingBox3D.h"
#endif

/**main function for SSC unit tests. 
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
//	runner.addTest( new CppUnit::TestCaller<TestSliceComputer>(
//			                                 "anyplane compute",
//			                                  &TestSliceComputer::testAnyPlanes ) );
//  runner.addTest( new CppUnit::TestCaller<TestVisualRendering>(
//                                       "visual",
//                                        &TestVisualRendering::testEmptyView ) );
//  runner.addTest( new CppUnit::TestCaller<TestVisualRendering>(
//                                       "visual",
//                                        &TestVisualRendering::test_ACS_3Volumes ) );
//  runner.addTest( new CppUnit::TestCaller<TestVisualRendering>(
//                                       "visual",
//                                        &TestVisualRendering::test_ACS_3Volumes_GPU ) );
//	runner.addTest(TestSpaceOrganizer::suite());
  runner.addTest(TestTransform3D::suite());
//  runner.addTest(TestBoundingBox3D::suite());
#endif
		
	bool failed = runner.run();
	return !failed;
}
