// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

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
 *  \date Oct 8, 2012
 *  \author christiana
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
