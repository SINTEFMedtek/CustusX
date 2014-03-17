#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <QtGui>

#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkImagePlaneWidget.h>
#include <vtkRenderer.h>
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "cxDataManager.h"
#include "cxImage.h"
#include "cxAxesRep.h"
#include "cxVolumetricRep.h"
#include "cxSliceComputer.h"
#include "cxVector3D.h"
#include "cxTransform3D.h"
#include "cxToolRep3D.h"
#include "cxDummyToolManager.h"
#include "cxDummyTool.h"
#include "cxSliceProxy.h"
#include "cxSlicerRepSW.h"

#include "cxViewsFixture.h"
#include "catch.hpp"
#include "cxtestRenderTester.h"
#include "cxReporter.h"
#include "cxImageLUT2D.h"

using cx::Vector3D;
using cx::Transform3D;

class ImageTestList
{
public:
	ImageTestList()
	{
		image.push_back("ssc/Person5/person5_t1_unsigned.mhd");
		image.push_back("ssc/Person5/person5_t2_unsigned.mhd");
		image.push_back("ssc/Person5/person5_flair_unsigned.mhd");
		image.push_back("ssc/DTI/dti_eigenvector_rgb.mhd");
	}
	QStringList image;
};

TEST_CASE("Visual rendering: Init view",
		  "[unit][resource][visualization]")
{
	cxtest::ViewsFixture fixture;
	REQUIRE(true);
}

TEST_CASE("Visual rendering: Empty view",
		  "[unit][resource][visualization]")
{
	cxtest::ViewsFixture fixture;

	cx::ViewWidget* view = fixture.addView("empty", 0, 0);
	REQUIRE(fixture.quickRunWidget());

	fixture.dumpDebugViewToDisk("emptyview", 0);
	REQUIRE(fixture.getFractionOfBrightPixelsInView(0,0) == Approx(0));
}

TEST_CASE("Visual rendering: Several empty views in a sequence.",
		  "[unit][resource][visualization]")
{
	int numberOfIterations = 2;
	for (unsigned i=0; i<numberOfIterations; ++i)
	{
		cxtest::ViewsFixture fixture;

		cx::ViewWidget* view = fixture.addView("empty", 0, 0);
		REQUIRE(fixture.quickRunWidget());

		fixture.dumpDebugViewToDisk("emptyview", 0);
		REQUIRE(fixture.getFractionOfBrightPixelsInView(0,0) == Approx(0));
	}
}

TEST_CASE("Visual rendering: Show 3D volume - vtkGPU render",
		  "[unit][resource][visualization][not_win32][not_win64]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	fixture.define3D(imagenames.image[0], NULL, 0, 0);

	REQUIRE(fixture.quickRunWidget());
	fixture.dumpDebugViewToDisk("3DvtkGPU", 0);
	REQUIRE(fixture.getFractionOfBrightPixelsInView(0,0) > 0.01);
}

TEST_CASE("Visual rendering: Show ACS+3D, centered hidden tool",
		  "[unit][resource][visualization][not_win32][not_win64]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	fixture.define3D(imagenames.image[0], NULL, 1, 1);
	fixture.defineSlice("A", imagenames.image[0], cx::ptAXIAL, 0, 0);
	fixture.defineSlice("C", imagenames.image[0], cx::ptCORONAL, 1, 0);
	fixture.defineSlice("S", imagenames.image[0], cx::ptSAGITTAL, 0, 1);
	REQUIRE(fixture.quickRunWidget());

	fixture.dumpDebugViewToDisk("acs3d0", 0);
	fixture.dumpDebugViewToDisk("acs3d1", 1);
	fixture.dumpDebugViewToDisk("acs3d2", 2);
	fixture.dumpDebugViewToDisk("acs3d3", 3);
	CHECK(fixture.getFractionOfBrightPixelsInView(0,0) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(1,20) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(2,20) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(3,20) > 0.02);
}

TEST_CASE("Visual rendering: Show AnyDual+3D, centered hidden tool",
		  "[unit][resource][visualization][not_win32][not_win64]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	fixture.define3D(imagenames.image[0], NULL, 0, 2);
	fixture.defineSlice("Any", imagenames.image[0], cx::ptANYPLANE, 0, 0);
	fixture.defineSlice("Dua", imagenames.image[0], cx::ptSIDEPLANE, 0, 1);
	REQUIRE(fixture.quickRunWidget());

	fixture.dumpDebugViewToDisk("anydual3d0", 0);
	fixture.dumpDebugViewToDisk("anydual3d1", 1);
	fixture.dumpDebugViewToDisk("anydual3d2", 2);
	CHECK(fixture.getFractionOfBrightPixelsInView(0,0) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(1,20) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(2,20) > 0.02);
}

TEST_CASE("Visual rendering: Show ACS, 3 volumes, centered hidden tool",
		  "[ca_test][unit][resource][visualization]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	for (unsigned i = 0; i < 3; ++i)
	{
		fixture.defineSlice("A", imagenames.image[i], cx::ptAXIAL, 0, i);
		fixture.defineSlice("C", imagenames.image[i], cx::ptCORONAL, 1, i);
		fixture.defineSlice("S", imagenames.image[i], cx::ptSAGITTAL, 2, i);
	}
	REQUIRE(fixture.quickRunWidget());

	for (unsigned i = 0; i < 3*3; ++i)
	{
		CHECK(fixture.getFractionOfBrightPixelsInView(i,20) > 0.02);
	}
}

TEST_CASE("Visual rendering: Show Axial GPU slice, 1 volume",
		  "[unit][resource][visualization][not_win32][not_win64]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	std::vector<cx::ImagePtr> images(1);
	images[0] = fixture.loadImage(imagenames.image[0]);

	cx::ImageLUT2DPtr lut0 = images[0]->getLookupTable2D();
	lut0->addColorPoint(images[0]->getMax(), QColor::fromRgbF(0,0,1,1));

	REQUIRE(fixture.defineGPUSlice("A", images, cx::ptAXIAL, 0, 0));
	REQUIRE(fixture.quickRunWidget());

	CHECK(fixture.getFractionOfBrightPixelsInView(0,20,2) > 0.02);
}

TEST_CASE("Visual rendering: Show Axial GPU slice, 2 volumes",
		  "[unit][resource][visualization][not_win32][not_win64]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	std::vector<cx::ImagePtr> images(2);
	images[0] = fixture.loadImage(imagenames.image[0]);
	images[1] = fixture.loadImage(imagenames.image[1]);

	cx::ImageLUT2DPtr lut0 = images[0]->getLookupTable2D();
	//std::cout << "setting llr for vol1 = " << llr << std::endl;
	lut0->addColorPoint(images[0]->getMax(), QColor::fromRgbF(0,1,0,1));

	cx::ImageLUT2DPtr lut1 = images[1]->getLookupTable2D();
	//std::cout << "setting llr for vol1 = " << llr << std::endl;
	lut1->addColorPoint(images[1]->getMax(), QColor::fromRgbF(0,0,1,1));
	double llr = images[1]->getMin() + (images[1]->getMax()-images[1]->getMin())*0.25;
	lut1->setLLR(llr);

	REQUIRE(fixture.defineGPUSlice("A", images, cx::ptAXIAL, 0, 0));
	REQUIRE(fixture.quickRunWidget());

	CHECK(fixture.getFractionOfBrightPixelsInView(0,20,1) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(0,20,2) > 0.02);
}

TEST_CASE("Visual rendering: Show ACS, 3 GPU volumes, moving tool",
		  "[unit][resource][visualization][not_win32][not_win64]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	for (unsigned i = 0; i < 3; ++i)
	{
		REQUIRE(fixture.defineGPUSlice("A", imagenames.image[i], cx::ptAXIAL, 0, i));
		REQUIRE(fixture.defineGPUSlice("C", imagenames.image[i], cx::ptCORONAL, 1, i));
		REQUIRE(fixture.defineGPUSlice("S", imagenames.image[i], cx::ptSAGITTAL, 2, i));
    }
	REQUIRE(fixture.quickRunWidget());

	for (unsigned i = 0; i < 3*3; ++i)
	{
		CHECK(fixture.getFractionOfBrightPixelsInView(i,20) > 0.02);
	}
}
