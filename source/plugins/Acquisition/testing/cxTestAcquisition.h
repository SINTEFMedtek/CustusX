#ifndef CXTESTACQUISITION_H_
#define CXTESTACQUISITION_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

/** Unit tests for cx::USSavingRecorder
 *
 * \ingroup cxTest
 * \date april 17, 2013
 * \author christiana
 */
class TestUSSavingRecorder : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testOneVideoSource();
	void testOneVideoSourceWithTool();
	void testOneVideoSourceWithToolAndSave();
	void testFourVideoSources();
	void testFourVideoSourcesWithToolAndSave();

public:
	CPPUNIT_TEST_SUITE( TestUSSavingRecorder );
		CPPUNIT_TEST( testOneVideoSource );
		CPPUNIT_TEST( testOneVideoSourceWithTool );
		CPPUNIT_TEST( testOneVideoSourceWithToolAndSave );
		CPPUNIT_TEST( testFourVideoSources );
		CPPUNIT_TEST( testFourVideoSourcesWithToolAndSave );
	CPPUNIT_TEST_SUITE_END();
private:
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestUSSavingRecorder );

/** Unit/integration tests that test the acquisition plugin
 *
 * \ingroup cxTest
 * \date jan 1, 2012
 * \author christiana
 */
class TestAcquisition : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testConstructor();///< Test constructor
	/** Run a full acquisition from MHD source local server using OpenIGTLink.
	  * Save data and evaluate results.
	  */
	void testStoreMHDSourceLocalServer();
	/** Run a full acquisition from MHD source direct link.
	  * Save data and evaluate results.
	  */
	void testStoreMHDSourceDirectLink();
	/** Test the MHD file source using the --secondary option,
	  * i.e. sending two streams and saving them to disk.
	  */
	void testStoreMultipleMHDSourceDirectLink();

public:
	CPPUNIT_TEST_SUITE( TestAcquisition );
		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testStoreMHDSourceLocalServer );
		CPPUNIT_TEST( testStoreMHDSourceDirectLink );
		CPPUNIT_TEST( testStoreMultipleMHDSourceDirectLink );
	CPPUNIT_TEST_SUITE_END();
private:
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestAcquisition );


#endif /*CXTESTACQUISITION_H_*/
