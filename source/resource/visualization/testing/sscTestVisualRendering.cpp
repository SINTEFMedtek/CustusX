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

//#include <cppunit/extensions/TestFactoryRegistry.h>
//#include <cppunit/ui/text/TestRunner.h>
//#include <cppunit/extensions/HelperMacros.h>

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

#include "sscTestVisualRendering.h"

//void TestVisualRendering::setUp()
//{
//	widget = new ViewsWindow("Verify that the volumes are rendered correctly.", false);
//	widget->mDumpSpeedData = false;
//	image.push_back("Phantoms/Kaisa/MetaImage/Kaisa.mhd");
////	image.push_back("Person5/person5_t2_byte.mhd");
////	image.push_back("Person5/person5_mra_byte.mhd");
//	image.push_back("ssc/Person5/person5_t2_unsigned.mhd");
//	image.push_back("ssc/Person5/person5_flair_unsigned.mhd");
//	image.push_back("ssc/DTI/dti_eigenvector_rgb.mhd");
//}

//void TestVisualRendering::tearDown()
//{
//	delete widget;
//}

class VisualRenderingFixture : public ViewsWindow
{
public:
	VisualRenderingFixture() : ViewsWindow("Verify that the volumes are rendered correctly.", false)
	{
		this->mDumpSpeedData = false;
		image.push_back("Phantoms/Kaisa/MetaImage/Kaisa.mhd");
	//	image.push_back("Person5/person5_t2_byte.mhd");
	//	image.push_back("Person5/person5_mra_byte.mhd");
		image.push_back("ssc/Person5/person5_t2_unsigned.mhd");
		image.push_back("ssc/Person5/person5_flair_unsigned.mhd");
		image.push_back("ssc/DTI/dti_eigenvector_rgb.mhd");
	}

	~VisualRenderingFixture()
	{
	}

	/**show/render/execute input widget.
	 * Return success of execution.
	 */
	bool runWidget()
	{
		this->show();
	#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
		this->activateWindow();
		std::cout << "on mac!!!" << std::endl;
	#endif
		this->raise();
		this->updateRender();
		return !qApp->exec() && this->accepted();
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
				 "[unit][resource/visualization]")
{
	this->setDescription("Empty view");
	cx::ViewWidget* view = new cx::ViewWidget(this->centralWidget());
	this->insertView(view, "dummy", "none", 0, 0);
	REQUIRE(this->runWidget());

	std::set<cx::View *> views = this->getViews();
	cxtest::RenderTesterPtr renderTester = cxtest::RenderTester::create((*views.begin())->getRenderWindow());
	vtkImageDataPtr output = renderTester->renderToImage();
	unsigned int numNonZeroPixels = renderTester->getNumberOfNonZeroPixels(output);
	std::cout << "numNonZeroPixels: " << numNonZeroPixels << std::endl;
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show 3D moving tool+volume",
				 "[unit][resource/visualization]")
{
	this->setDescription("3D Volume, moving tool");
	this->define3D(image[0], NULL, 0, 0);

	REQUIRE(this->runWidget());

	std::set<cx::View *> views = this->getViews();
	cxtest::RenderTesterPtr renderTester = cxtest::RenderTester::create((*views.begin())->getRenderWindow());
	vtkImageDataPtr output = renderTester->renderToImage();
	unsigned int numNonZeroPixels = renderTester->getNumberOfNonZeroPixels(output);
	std::cout << "numNonZeroPixels: " << numNonZeroPixels << std::endl;
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show 3D moving tool + GPU volume",
				 "[unit][resource/visualization][not_apple]")
{
	this->setDescription("3D Volume, moving tool, GPU");
	REQUIRE(this->define3DGPU(QStringList(image[0]), NULL, 0, 0));
	REQUIRE(this->runWidget());
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show moving tool + 2 GPU volumes",
				 "[unit][resource/visualization][not_apple]")
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
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show moving tool + 2 volumes in ACS+3D_GPU",
				 "[unit][resource/visualization][not_apple]")
{
	this->setDescription("3D Composites (2 volumes) with ACS, moving tool");
	QStringList images; images << image[1] << image[2];
	ImageParameters parameters[2];

	parameters[0].llr = 35;
	parameters[0].alpha = .1;

	parameters[1].llr = 55;
	parameters[1].alpha = .7;
	parameters[1].lut = getCreateLut(0, 200, .67, .68, 0, 1, .4, .8);

	this->defineSlice("A", image[0], cx::ptAXIAL, 0, 0);
	this->defineSlice("C", image[0], cx::ptCORONAL, 1, 0);
	this->defineSlice("S", image[0], cx::ptSAGITTAL, 0, 1);
	REQUIRE(this->define3DGPU(images, parameters, 1, 1));
	REQUIRE(this->runWidget());
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show 3D RGB GPU volume",
				 "[unit][resource/visualization][not_apple]")
{
	this->setDescription("3D RGB Volume");

	ImageParameters parameters;
	parameters.llr = 35;

	REQUIRE(this->define3DGPU(QStringList(image[3]), &parameters, 0, 0));
	REQUIRE(this->runWidget());
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show 3D volume with lut",
				 "[unit][resource/visualization]")
{
	this->setDescription("3D with lut, moving tool");

	ImageParameters parameters;
	parameters.llr = 75;
	parameters.lut = getCreateLut(0, 200, .67, .68, 0, 1, .4, .8);

	this->define3D(image[0], &parameters, 0, 0);

	REQUIRE(this->runWidget());
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show ACS+3D, moving tool",
				 "[unit][resource/visualization]")
{
	this->setDescription("ACS+3D, moving tool");
	this->define3D(image[0], NULL, 1, 1);
	this->defineSlice("A", image[0], cx::ptAXIAL, 0, 0);
	this->defineSlice("C", image[0], cx::ptCORONAL, 1, 0);
	this->defineSlice("S", image[0], cx::ptSAGITTAL, 0, 1);
	REQUIRE(this->runWidget());
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show AnyDual+3D, moving tool",
				 "[unit][resource/visualization]")
{
	this->setDescription("Any+Dual+3D, moving tool");
	this->define3D(image[0], NULL, 0, 2);
	this->defineSlice("Any", image[0], cx::ptANYPLANE, 0, 0);
	this->defineSlice("Dua", image[0], cx::ptSIDEPLANE, 0, 1);
	REQUIRE(this->runWidget());
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show ACS, 3 volumes, moving tool",
				 "[unit][resource/visualization]")
{
	this->setDescription("ACS 3 volumes, moving tool");

	for (unsigned i = 0; i < 3; ++i)
	{
		this->defineSlice("A", image[i], cx::ptAXIAL, 0, i);
		this->defineSlice("C", image[i], cx::ptCORONAL, 1, i);
		this->defineSlice("S", image[i], cx::ptSAGITTAL, 2, i);
	}
	REQUIRE(this->runWidget());
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show ACS, 3 GPU volumes, moving tool",
				 "[unit][resource/visualization][not_apple]")
{
	this->setDescription("ACS 3 volumes, moving tool, GPU");

	for (unsigned i = 0; i < 3; ++i)
	{
		REQUIRE(this->defineGPUSlice("A", image[i], cx::ptAXIAL, 0, i));
		REQUIRE(this->defineGPUSlice("C", image[i], cx::ptCORONAL, 1, i));
		REQUIRE(this->defineGPUSlice("S", image[i], cx::ptSAGITTAL, 2, i));
	}
	REQUIRE(this->runWidget());
}

TEST_CASE_METHOD(VisualRenderingFixture,
				 "Visual rendering: Show AnyDual, 3 volumes, moving tool",
				 "[unit][resource/visualization]")
{
	this->setDescription("Any+Dual 3 volumes, moving tool");

	for (unsigned i = 0; i < 3; ++i)
	{
		this->defineSlice("Any", image[i], cx::ptANYPLANE, 0, i);
		this->defineSlice("Dua", image[i], cx::ptSIDEPLANE, 1, i);
	}
	REQUIRE(this->runWidget());
}

