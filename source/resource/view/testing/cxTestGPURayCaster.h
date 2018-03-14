/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTGPURAYCASTER_H_
#define CXTESTGPURAYCASTER_H_

#ifndef WIN32

#include "cxResourceVisualizationTestUtilitiesExport.h"

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "sscGPURayCastVolumeRep.h"
/**Unit tests for class GPURayCasterVolumeRep
 */
class cxResourceVisualizationTestUtilities_EXPORT TestGPURayCaster : public CppUnit::TestFixture
{
public:
	void setUp();
	void testSetImages();
	void testParameters();
	
public:
	CPPUNIT_TEST_SUITE( TestGPURayCaster );
		CPPUNIT_TEST( testSetImages );
		CPPUNIT_TEST( testParameters );
	CPPUNIT_TEST_SUITE_END();
private:
	cx::GPURayCastVolumeRepPtr mRep;

};

#endif //WIN32
#endif /*CXTESTGPURAYCASTER_H_*/
