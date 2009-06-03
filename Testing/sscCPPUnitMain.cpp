#include <QApplication>
//#include <boost/scoped_ptr.hpp>

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestResult.h> 
#include <cppunit/BriefTestProgressListener.h> 

//#include "sscTestVisualRendering.h"


//#include "SonoWand.h"

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
	
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	runner.addTest(registry.makeTest() );
	bool failed = runner.run();
	return !failed;
} 


