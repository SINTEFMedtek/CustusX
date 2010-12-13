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
	//	image.push_back("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	//	image.push_back("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	image.push_back("MetaImage/20070309T105136_MRT1.mhd");
	image.push_back("MetaImage/20070309T102309_MRA.mhd");
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
	int val = qApp->exec();

	return !val && widget->accepted();
}

void TestVisualRendering::testEmptyView()
{
	widget->setDescription("Empty view");
	ssc::View* view = new ssc::View(widget->centralWidget());
	widget->insertView(view, "dummy", "none", 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_3D_Tool()
{
	widget->setDescription("3D Volume, moving tool");
	widget->define3D(image[0], 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_ACS_3D_Tool()
{
	widget->setDescription("ACS+3D, moving tool");
	widget->define3D(image[0], 1, 1);
	widget->defineSlice("A", image[0], ssc::ptAXIAL, 0, 0);
	widget->defineSlice("C", image[0], ssc::ptCORONAL, 1, 0);
	widget->defineSlice("S", image[0], ssc::ptSAGITTAL, 0, 1);

	CPPUNIT_ASSERT(runWidget());
}

void TestVisualRendering::test_AnyDual_3D_Tool()
{
	widget->setDescription("Any+Dual+3D, moving tool");
	widget->define3D(image[0], 0, 2);
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

//	widget->defineGPUSlice("A", image[0], ssc::ptAXIAL, 0, 0);

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

