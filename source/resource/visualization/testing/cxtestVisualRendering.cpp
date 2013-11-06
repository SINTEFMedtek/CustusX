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

#include "sscDataManager.h"
#include "sscImage.h"
#include "sscAxesRep.h"
#include "sscVolumetricRep.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"
#include "sscToolRep3D.h"
#include "sscDummyToolManager.h"
#include "sscDummyTool.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"

#include "sscViewsWindow.h"
#include "catch.hpp"
#include "cxtestRenderTester.h"

using cx::Vector3D;
using cx::Transform3D;

class VisualRenderingFixture : public ViewsWindow
{
public:
	VisualRenderingFixture() : ViewsWindow("")
	{
		image.push_back("ssc/Person5/person5_t1_unsigned.mhd");
		image.push_back("ssc/Person5/person5_t2_unsigned.mhd");
		image.push_back("ssc/Person5/person5_flair_unsigned.mhd");
		image.push_back("ssc/DTI/dti_eigenvector_rgb.mhd");
	}
	QStringList image;
};

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Init view",
				 "[unit][resource/visualization]")
{
	REQUIRE(true);
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Empty view",
				 "[unit][resource][visualization]")
{
	this->setDescription("Empty view");
	cx::ViewWidget* view = new cx::ViewWidget(this->centralWidget());
	this->insertView(view, "dummy", "none", 0, 0);
//	REQUIRE(this->runWidget());
	REQUIRE(this->quickRunWidget());

	this->dumpDebugViewToDisk("emptyview", 0);
	REQUIRE(this->getFractionOfBrightPixelsInView(0,0) == Approx(0));
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show 3D volume - vtkGPU render",
				 "[unit][resource][visualization]")
{
	this->setDescription("3D Volume, moving tool");
	this->define3D(image[0], NULL, 0, 0);

//	REQUIRE(this->runWidget());
	REQUIRE(this->quickRunWidget());
	this->dumpDebugViewToDisk("3DvtkGPU", 0);
	REQUIRE(this->getFractionOfBrightPixelsInView(0,0) > 0.01);
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show 3D volume - snwGPU multivolume render",
				 "[unit][resource][visualization][not_apple][not_windows]")
{
	this->setDescription("3D Volume, moving tool, GPU");
	REQUIRE(this->define3DGPU(QStringList(image[0]), NULL, 0, 0));
	REQUIRE(this->runWidget());
	//REQUIRE(this->quickRunWidget());
	REQUIRE(this->getFractionOfBrightPixelsInView(0,0) > 0.01);
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show 3D 2 volumes - snwGPU multivolume render",
				 "[unit][resource][visualization][not_apple][not_windows]")
{
	this->setDescription("3D Composites (2 volumes), moving tool");
	QStringList images; images << image[1] << image[2];
	ImageParameters parameters[2];

	parameters[0].llr = 35;
	parameters[0].alpha = .1;

	parameters[1].llr = 55;
	parameters[1].alpha = .7;
	parameters[1].lut = getCreateLut(0, 200, .67, .68, 0, 1, .4, .8);

	REQUIRE(this->define3DGPU(images, parameters, 0, 0));

	REQUIRE(this->runWidget());
	//REQUIRE(this->quickRunWidget());
	REQUIRE(this->getFractionOfBrightPixelsInView(0,0) > 0.02);
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show 3D RGB GPU volume",
				 "[unit][resource][visualization][not_apple][not_windows][hide]")
{
	this->setDescription("3D RGB Volume");

	ImageParameters parameters;
	parameters.llr = 35;

	REQUIRE(this->define3DGPU(QStringList(image[3]), &parameters, 0, 0));
	//REQUIRE(this->runWidget());
	REQUIRE(this->quickRunWidget());
	CHECK(this->getFractionOfBrightPixelsInView(0,0) > 0.02);
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show 3D volume with lut",
				 "[unit][resource][visualization]")
{
	this->setDescription("3D with lut, moving tool");

	ImageParameters parameters;
	parameters.llr = 75;
	parameters.lut = getCreateLut(0, 200, .67, .68, 0, 1, .4, .8);

	this->define3D(image[0], &parameters, 0, 0);

	REQUIRE(this->quickRunWidget());
	CHECK(this->getFractionOfBrightPixelsInView(0,0) > 0.02);
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show ACS+3D, centered hidden tool",
				 "[unit][resource][visualization]")
{
	this->setDescription("ACS+3D, moving tool");
	this->define3D(image[0], NULL, 1, 1);
	this->defineSlice("A", image[0], cx::ptAXIAL, 0, 0);
	this->defineSlice("C", image[0], cx::ptCORONAL, 1, 0);
	this->defineSlice("S", image[0], cx::ptSAGITTAL, 0, 1);
	REQUIRE(this->quickRunWidget());
	this->dumpDebugViewToDisk("acs3d0", 0);
	this->dumpDebugViewToDisk("acs3d1", 1);
	this->dumpDebugViewToDisk("acs3d2", 2);
	this->dumpDebugViewToDisk("acs3d3", 3);

	CHECK(this->getFractionOfBrightPixelsInView(0,0) > 0.02);
	CHECK(this->getFractionOfBrightPixelsInView(1,20) > 0.02);
	CHECK(this->getFractionOfBrightPixelsInView(2,20) > 0.02);
	CHECK(this->getFractionOfBrightPixelsInView(3,20) > 0.02);
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show AnyDual+3D, centered hidden tool",
				 "[unit][resource][visualization]")
{
	this->setDescription("Any+Dual+3D, moving tool");
	this->define3D(image[0], NULL, 0, 2);
	this->defineSlice("Any", image[0], cx::ptANYPLANE, 0, 0);
	this->defineSlice("Dua", image[0], cx::ptSIDEPLANE, 0, 1);
	REQUIRE(this->quickRunWidget());
	this->dumpDebugViewToDisk("anydual3d0", 0);
	this->dumpDebugViewToDisk("anydual3d1", 1);
	this->dumpDebugViewToDisk("anydual3d2", 2);
	CHECK(this->getFractionOfBrightPixelsInView(0,0) > 0.02);
	CHECK(this->getFractionOfBrightPixelsInView(1,20) > 0.02);
	CHECK(this->getFractionOfBrightPixelsInView(2,20) > 0.02);
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show ACS, 3 volumes, centered hidden tool",
				 "[unit][resource][visualization]")
{
	this->setDescription("ACS 3 volumes, moving tool");

	for (unsigned i = 0; i < 3; ++i)
	{
		this->defineSlice("A", image[i], cx::ptAXIAL, 0, i);
		this->defineSlice("C", image[i], cx::ptCORONAL, 1, i);
		this->defineSlice("S", image[i], cx::ptSAGITTAL, 2, i);
	}
	REQUIRE(this->quickRunWidget());

	for (unsigned i = 0; i < 3*3; ++i)
	{
		CHECK(this->getFractionOfBrightPixelsInView(i,20) > 0.02);
	}
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show Axial GPU slice",
				 "[unit][resource][visualization][not_apple][not_windows][ca_special]")
{
	this->setDescription("A  volumes, moving tool, GPU");

	REQUIRE(this->defineGPUSlice("A", image[0], cx::ptAXIAL, 0, 0));
	REQUIRE(this->quickRunWidget());

	CHECK(this->getFractionOfBrightPixelsInView(0,20) > 0.02);
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show ACS, 3 GPU volumes, moving tool",
				 "[unit][resource][visualization][not_apple][not_windows]")
{
	this->setDescription("ACS 3 volumes, moving tool, GPU");

	for (unsigned i = 0; i < 3; ++i)
	{
		REQUIRE(this->defineGPUSlice("A", image[i], cx::ptAXIAL, 0, i));
		REQUIRE(this->defineGPUSlice("C", image[i], cx::ptCORONAL, 1, i));
		REQUIRE(this->defineGPUSlice("S", image[i], cx::ptSAGITTAL, 2, i));
	}
	//REQUIRE(this->runWidget());
	REQUIRE(this->quickRunWidget());

	for (unsigned i = 0; i < 3*3; ++i)
	{
		CHECK(this->getFractionOfBrightPixelsInView(i,20) > 0.02);
	}
}
