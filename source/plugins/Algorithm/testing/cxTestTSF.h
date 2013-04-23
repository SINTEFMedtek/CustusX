#ifndef CXTESTTSF_H_
#define CXTESTTSF_H_

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

namespace cx {

class TestTSF  : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testConstructor();///< Test constructor

	void testInitParameters(); ///< parameters should be initialized without any trouble
	void testLoadPreset(); ///< check that the parameters in the loaded file are set as expected

private:
	std::string mParametersFolderPath;

public:
	CPPUNIT_TEST_SUITE( TestTSF );
		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testInitParameters );
		CPPUNIT_TEST( testLoadPreset );
	CPPUNIT_TEST_SUITE_END();

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestTSF );
} /* namespace cx */
#endif /* CXTESTTSF_H_ */
