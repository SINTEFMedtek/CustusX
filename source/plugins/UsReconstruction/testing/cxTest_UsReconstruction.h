#ifndef CXTEST_USRECONSTRUCTION_H_
#define CXTEST_USRECONSTRUCTION_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

/**Unit tests that show the properties of the cxToolConfigurationParser class
 */
class TestUsReconstruction : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testAngioReconstruction();//Test reconstruction of US angio data (#318)
	void testThunderGPUReconstruction();//Test Thunder GPU reconstruction

public:
	CPPUNIT_TEST_SUITE( TestUsReconstruction );
		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testAngioReconstruction );
		CPPUNIT_TEST( testThunderGPUReconstruction );
	CPPUNIT_TEST_SUITE_END();
private:

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestUsReconstruction );


#endif /*CXTEST_USRECONSTRUCTION_H_*/
