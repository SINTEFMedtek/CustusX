#ifndef CXTESTACQUISITION_H_
#define CXTESTACQUISITION_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "vtkImageData.h"
#include "sscImage.h"

/**Unit tests that test IGTLink utilities
 */
class TestIGTLinkUtilities : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testConstructor();
	/**
	  * Test encode/decode of a simple grayscale image.
	  */
	void testDecodeEncodeImage();
	/**
	  * Test encode/decode of a ssc::ProbeData.
	  */
	void testDecodeEncodeProbeData();
	/**
	  * Test encode/decode of a RGBA color image.
	  * The output should be a RGBX image.
	  */
	void testDecodeEncodeColorImageRGBA();
	/**
	  * Test encode/decode of a BGR color image.
	  * The output should be a RGBX image.
	  */
	void testDecodeEncodeColorImageBGR();
	/**
	  * Test encode/decode of a ARGB color image.
	  * The output should be a RGBX image.
	  */
	void testDecodeEncodeColorImageARGB();

public:
	CPPUNIT_TEST_SUITE( TestIGTLinkUtilities );
		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testDecodeEncodeImage );
		CPPUNIT_TEST( testDecodeEncodeColorImageRGBA );
		CPPUNIT_TEST( testDecodeEncodeColorImageBGR );
		CPPUNIT_TEST( testDecodeEncodeColorImageARGB );
		CPPUNIT_TEST( testDecodeEncodeProbeData );
	CPPUNIT_TEST_SUITE_END();
private:
	typedef std::vector<std::pair<Eigen::Array3i, Eigen::Array3i> > Val3VectorType;
	int getValue(ssc::ImagePtr data, int x, int y, int z);
	void setValue(vtkImageDataPtr data, int x, int y, int z, unsigned char val);
	Eigen::Array3i getValue3i(ssc::ImagePtr data, int x, int y, int z);
	void testDecodeEncodeColorImage(Val3VectorType samples, QString colorFormat);
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestIGTLinkUtilities );


#endif /*CXTESTACQUISITION_H_*/
