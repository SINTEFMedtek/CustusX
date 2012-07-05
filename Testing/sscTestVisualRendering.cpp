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

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "sscTestUtilities.h"
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscAxesRep.h"
#include "sscSliceRep.h"
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

using ssc::Vector3D;
using ssc::Transform3D;

#include "sscTestVisualRendering.h"

void TestVisualRendering::setUp()
{
	widget = new ViewsWindow("Verify that the volumes are rendered correctly.", false);
	widget->mDumpSpeedData = false;
	image.push_back("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	image.push_back("Person5/person5_t2_byte.mhd");
	image.push_back("Person5/person5_mra_byte.mhd");
	image.push_back("DTI/dti_eigenvector_rgb.mhd");
}

void TestVisualRendering::tearDown()
{
	delete widget;
}

void TestVisualRendering::testInitialize()
{
	CPPUNIT_ASSERT(1);
}

/**show/render/execute input widget.
 * Return success of execution.
 */
bool TestVisualRendering::runWidget()
{
	widget->show();
#ifdef __MACOSX__ // needed on mac for bringing to front: does the opposite on linux
	widget->activateWindow();
#endif
	widget->raise();
	widget->updateRender();
	return !qApp->exec() && widget->accepted();
}

void TestVisualRendering::testEmptyView()
{
	widget->setDescription("Empty view");
	ssc::ViewWidget* view = new ssc::ViewWidget(widget->centralWidget());
	widget->insertView(view, "dummy", "none", 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::testEmptyViewContainer()
{
	widget->setDescription("Empty view container");
	ssc::ViewContainer *viewContainer = new ssc::ViewContainer(widget->centralWidget());
	widget->setupViewContainer(viewContainer, "dummy", "none", 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_3D_Tool()
{
	widget->setDescription("3D Volume, moving tool");
	widget->define3D(image[0], NULL, 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_ACS_3D_Tool_Container()
{
	widget->mDumpSpeedData = true;

	widget->setDescription("ACS+3D, moving tool in view container");
	ssc::ViewContainer *viewContainer = new ssc::ViewContainer(widget->centralWidget());
	widget->containerGPUSlice(viewContainer->addView(0, 0), "A", image[0], ssc::ptAXIAL);
	widget->containerGPUSlice(viewContainer->addView(1, 0), "C", image[0], ssc::ptCORONAL);
	widget->containerGPUSlice(viewContainer->addView(1, 1), "S", image[0], ssc::ptSAGITTAL);
	widget->container3D(viewContainer->addView(0, 1), image[0]);

	widget->setupViewContainer(viewContainer, "dummy", "none", 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_AnyDual_3D_Tool_Container()
{
	widget->mDumpSpeedData = true;

	widget->setDescription("2 x Any + Side + 3D, moving tool in view container");
	ssc::ViewContainer *viewContainer = new ssc::ViewContainer(widget->centralWidget());
	widget->containerGPUSlice(viewContainer->addView(0, 0), "Any", image[0], ssc::ptANYPLANE);
	widget->containerGPUSlice(viewContainer->addView(1, 0), "Dual", image[0], ssc::ptSIDEPLANE);
	widget->containerGPUSlice(viewContainer->addView(0, 1), "Any", image[0], ssc::ptANYPLANE);
	widget->containerGPUSlice(viewContainer->addView(1, 1), "Dual", image[0], ssc::ptSIDEPLANE);
	widget->container3D(viewContainer->addView(0, 2, 2, 1), image[0]);

	widget->setupViewContainer(viewContainer, "dummy", "none", 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_3D_Tool_GPU()
{
	widget->setDescription("3D Volume, moving tool");
	widget->define3DGPU(QStringList(image[0]), NULL, 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_3D_Composite_GPU()
{
	widget->setDescription("3D Composites (2 volumes), moving tool");
	QStringList images; images << image[1] << image[2];
	ImageParameters parameters[2];

	parameters[0].llr = 35;
	parameters[0].alpha = .1;

	parameters[1].llr = 55;
	parameters[1].alpha = .7;
	parameters[1].lut = getCreateLut(0, 200, .67, .68, 0, 1, .4, .8);

	widget->define3DGPU(images, parameters, 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_3D_Composite_Views_GPU()
{
	widget->setDescription("3D Composites (2 volumes) with ACS, moving tool");
	QStringList images; images << image[1] << image[2];
	ImageParameters parameters[2];

	parameters[0].llr = 35;
	parameters[0].alpha = .1;

	parameters[1].llr = 55;
	parameters[1].alpha = .7;
	parameters[1].lut = getCreateLut(0, 200, .67, .68, 0, 1, .4, .8);

	widget->defineSlice("A", image[0], ssc::ptAXIAL, 0, 0);
	widget->defineSlice("C", image[0], ssc::ptCORONAL, 1, 0);
	widget->defineSlice("S", image[0], ssc::ptSAGITTAL, 0, 1);
	widget->define3DGPU(images, parameters, 1, 1);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_3D_Composite_Views_GPU_Container()
{
	ssc::ViewContainer *view = new ssc::ViewContainer(widget->centralWidget());
	view->setupViews(2, 2);
	widget->setDescription("3D Composites (2 volumes) with ACS, moving tool (in viewcontainer)");
	widget->setupViewContainer(view, "dummy", "none", 0, 0);
	QStringList images; images << image[1] << image[2];
	ImageParameters parameters[2];

	parameters[0].llr = 35;
	parameters[0].alpha = .1;

	parameters[1].llr = 55;
	parameters[1].alpha = .7;
	parameters[1].lut = getCreateLut(0, 200, .67, .68, 0, 1, .4, .8);

	widget->containerGPUSlice(view, 1, "A", image[0], ssc::ptAXIAL);
	widget->containerGPUSlice(view, 0, "C", image[0], ssc::ptCORONAL);
	widget->containerGPUSlice(view, 3, "S", image[0], ssc::ptSAGITTAL);
	widget->define3DGPUContainer(view, images, 2, parameters);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_3D_RGB_GPU()
{
	widget->setDescription("3D RGB Volume");

	ImageParameters parameters;
	parameters.llr = 35;

	widget->define3DGPU(QStringList(image[3]), &parameters, 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_3D_Lut()
{
	widget->setDescription("3D with lut, moving tool");

	ImageParameters parameters;
	parameters.llr = 75;
	parameters.lut = getCreateLut(0, 200, .67, .68, 0, 1, .4, .8);

	widget->define3D(image[0], &parameters, 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_ACS_3D_Tool()
{
	widget->setDescription("ACS+3D, moving tool");
	widget->define3D(image[0], NULL, 1, 1);
	widget->defineSlice("A", image[0], ssc::ptAXIAL, 0, 0);
	widget->defineSlice("C", image[0], ssc::ptCORONAL, 1, 0);
	widget->defineSlice("S", image[0], ssc::ptSAGITTAL, 0, 1);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_AnyDual_3D_Tool()
{
	widget->setDescription("Any+Dual+3D, moving tool");
	widget->define3D(image[0], NULL, 0, 2);
	widget->defineSlice("Any", image[0], ssc::ptANYPLANE, 0, 0);
	widget->defineSlice("Dua", image[0], ssc::ptSIDEPLANE, 0, 1);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_ACS_3Volumes()
{
	widget->setDescription("ACS 3 volumes, moving tool");

	for (unsigned i = 0; i < 3; ++i)
	{
		widget->defineSlice("A", image[i], ssc::ptAXIAL, 0, i);
		widget->defineSlice("C", image[i], ssc::ptCORONAL, 1, i);
		widget->defineSlice("S", image[i], ssc::ptSAGITTAL, 2, i);
	}

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_ACS_3Volumes_GPU()
{
	widget->setDescription("ACS 3 volumes, moving tool, GPU");

	for (unsigned i = 0; i < 3; ++i)
	{
		widget->defineGPUSlice("A", image[i], ssc::ptAXIAL, 0, i);
		widget->defineGPUSlice("C", image[i], ssc::ptCORONAL, 1, i);
		widget->defineGPUSlice("S", image[i], ssc::ptSAGITTAL, 2, i);
	}

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_AnyDual_3Volumes()
{
	widget->setDescription("Any+Dual 3 volumes, moving tool");

	for (unsigned i = 0; i < 3; ++i)
	{
		widget->defineSlice("Any", image[i], ssc::ptANYPLANE, 0, i);
		widget->defineSlice("Dua", image[i], ssc::ptSIDEPLANE, 1, i);
	}

	CPPUNIT_ASSERT(runWidget());
}
