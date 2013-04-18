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
	void testStreams();///< Test init of cxImageSenderGE, and Test GE all streams streams. CPU scanconversion
	void testAllStreamsGPU();///< Test all GE streams. GPU scanconversion if possible
	void testScanConvertedStreamGPU();///< Test GE scanconverted stream. GPU scanconversion if possible
	void testTissueStreamGPU();///< Test GE tissue stream. GPU scanconversion if possible
	void testFrequencyStreamGPU();///< Test GE frequency stream. GPU scanconversion if possible
	void testBandwidthStreamGPU();///< Test GE bandwidth stream. GPU scanconversion if possible
	void testVelocityStreamGPU();///< Test GE velocity stream. GPU scanconversion if possible
	void testAllStreamsGPUConsecutively();
	void testGEStreamer();///< Test GEStreamer directly. Don't use cxImageSenderGE

	int getValue(vtkSmartPointer<vtkImageData> img, int x, int y, int z);
	void validateData(vtkSmartPointer<vtkImageData> img);
	void validateBMode3D(vtkSmartPointer<vtkImageData> img);
	void testStream(cx::StringMap args);///< Grab a short stream

public:
	CPPUNIT_TEST_SUITE( TestGEInterface );
//		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testStreams );
		CPPUNIT_TEST( testAllStreamsGPU );
		CPPUNIT_TEST( testScanConvertedStreamGPU );
		CPPUNIT_TEST( testTissueStreamGPU );
		CPPUNIT_TEST( testFrequencyStreamGPU );
		CPPUNIT_TEST( testBandwidthStreamGPU );
		CPPUNIT_TEST( testVelocityStreamGPU );
		CPPUNIT_TEST( testAllStreamsGPUConsecutively );
		CPPUNIT_TEST( testGEStreamer );
	CPPUNIT_TEST_SUITE_END();
private:
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestGEInterface );


#endif //CX_USE_ISB_GE
#endif /*CXTESTGEINTERFACE_H_*/
