/*
 * cxTestRegistrationElastix2V.h
 *
 *  \date Oct 24, 2011
 *      \author christiana
 */

#ifndef CXTEST_REGISTRATIONELASTIX_H_
#define CXTEST_REGISTRATIONELASTIX_H_

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Message.h>

#include <vector>
#include "sscTransform3D.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

/**
 */
class TestRegistrationElastix : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testElastix();

public:
	CPPUNIT_TEST_SUITE( TestRegistrationElastix );
		CPPUNIT_TEST( testElastix );
	CPPUNIT_TEST_SUITE_END();
private:
	bool compareTransforms(ssc::Transform3D result, ssc::Transform3D solution);
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestRegistrationElastix );


#endif /* CXTEST_REGISTRATIONELASTIX_H_ */
