/*
 * cxTestRegistration.h
 *
 *  \date Oct 24, 2011
 *      \author christiana
 */

#ifndef CXTEST_REGISTRATION_H_
#define CXTEST_REGISTRATION_H_

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Message.h>

#include "sscTransform3D.h"

/**
 */
class TestRegistration : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testVessel2VesselRegistration();

public:
	CPPUNIT_TEST_SUITE( TestRegistration );
		CPPUNIT_TEST( testVessel2VesselRegistration );
	CPPUNIT_TEST_SUITE_END();
private:
	void doTestVessel2VesselRegistration(ssc::Transform3D perturbation, QString filenameSource, QString filenameTarget, double tol_dist, double tol_angle);
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestRegistration );


#endif /* CXTEST_REGISTRATION_H_ */
