#ifndef CXTESTGEINTERFACE_H_
#define CXTESTGEINTERFACE_H_

#ifdef CX_USE_ISB_GE
#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include "cxImageSenderFactory.h"

/**Unit tests that test the acquisition plugin
 */
class TestGEInterface : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testConstructor();///< Test constructor
	void testInit();///< Test init of cxImageSenderGE
	void testGEStreamer();///< Test GEStreamer directly. Don't use cxImageSenderGE

	int getValue(vtkSmartPointer<vtkImageData> img, int x, int y, int z);
	void validateData(vtkSmartPointer<vtkImageData> img);
	void validateBMode3D(vtkSmartPointer<vtkImageData> img);
	void testStream(cx::StringMap args);///< Grab a short stream

public:
	CPPUNIT_TEST_SUITE( TestGEInterface );
//		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testInit );
		CPPUNIT_TEST( testGEStreamer );
	CPPUNIT_TEST_SUITE_END();
private:
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestGEInterface );


#endif //CX_USE_ISB_GE
#endif /*CXTESTGEINTERFACE_H_*/
