//#include <QApplication>
//#include <boost/scoped_ptr.hpp>

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

//#include "SonoWand.h"

/**main function for SSC unit tests. 
 * Perform system-wide init/deinit,
 * run all registered tests.
 */
int main(int argc, char **argv)
{
//	QApplication app(argc, argv);
//	SW_AppPath("/TestData/Data0/");
//	SW_Init("com.sonowand.navigation.unittest");
	CppUnit::TextUi::TestRunner runner;
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	runner.addTest(registry.makeTest() );
	bool failed = runner.run();
//	SW_Done();
	return !failed;
} 

