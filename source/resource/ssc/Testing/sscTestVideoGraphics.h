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
#ifndef SSCTESTVIDEOGRAPHICS_H
#define SSCTESTVIDEOGRAPHICS_H

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "sscTransform3D.h"

#include "sscVideoGraphics.h"
#include "sscVtkRenderTester.h"
#include "sscProbeData.h"

/** Tests for class ssc::VideoGraphics
  *
  * \date april 29, 2013
  * \author christiana
  */
class TestVideoGraphics : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

private:
	void testRenderImage();
	void testRenderMetaHeaderImage();
	void testPassSingleMHDImage();
	void testPassSinglePNGImage();
	void testMask();
	void testSector();

public:
	CPPUNIT_TEST_SUITE( TestVideoGraphics );
	CPPUNIT_TEST( testRenderImage );
	CPPUNIT_TEST( testRenderMetaHeaderImage );
	CPPUNIT_TEST( testPassSingleMHDImage );
	CPPUNIT_TEST( testPassSinglePNGImage );
	CPPUNIT_TEST( testMask );
	CPPUNIT_TEST( testSector );
	CPPUNIT_TEST_SUITE_END();

private:
	void renderImageAndCompareToExpected(vtkImageDataPtr input, vtkImageDataPtr expected);
	vtkImageDataPtr  readImageData(QString filename, QString description);
	ssc::ProbeData readProbeData(QString filename);
	void addImageToRenderer(vtkImageDataPtr image);
	ssc::TestVtkRenderingPtr mMachine;
	ssc::VideoGraphicsPtr mVideoGraphics;
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestVideoGraphics );

#endif // SSCTESTVIDEOGRAPHICS_H
