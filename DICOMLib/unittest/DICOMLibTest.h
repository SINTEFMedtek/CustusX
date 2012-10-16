#ifndef DICOMLIBTEST_H_
#define DICOMLIBTEST_H_
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include "DICOMLib.h"
class TestDICOMLib : public QObject, public CppUnit::TestFixture
{
	Q_OBJECT;
public:
	void setUp();
	void tearDown();
	void testDummy();
	void testDump();
public:
	CPPUNIT_TEST_SUITE( TestDICOMLib );
		CPPUNIT_TEST(testDummy);
		CPPUNIT_TEST(testDump);
	CPPUNIT_TEST_SUITE_END();
private:
	struct study_t *studyList;
};

#endif
