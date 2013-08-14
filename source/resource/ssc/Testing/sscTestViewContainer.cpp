#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <QtGui>

//#include <vtkImageData.h>
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

#include "sscContainerWindow.h"

using ssc::Vector3D;
using ssc::Transform3D;

#include "sscTestViewContainer.h"

void TestViewContainer::setUp()
{
	widget = new ContainerWindow("Verify that the volumes are rendered correctly.", false);
	widget->mDumpSpeedData = false;
	image.push_back("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
//	image.push_back("Person5/person5_t2_byte.mhd");
//	image.push_back("Person5/person5_mra_byte.mhd");
	image.push_back("Person5/person5_t2_unsigned.mhd");
	image.push_back("Person5/person5_angio_unsigned.mhd");
	image.push_back("DTI/dti_eigenvector_rgb.mhd");
}

void TestViewContainer::tearDown()
{
	delete widget;
}

void TestViewContainer::testInitialize()
{
	CPPUNIT_ASSERT(1);
}

/**show/render/execute input widget.
 * Return success of execution.
 */
bool TestViewContainer::runWidget()
{
	widget->show();
#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
	widget->activateWindow();
#endif
	widget->raise();
	widget->updateRender();
	return !qApp->exec() && widget->accepted();
}

void TestViewContainer::testEmptyViewContainer()
{
	widget->setDescription("Empty view container");
	ssc::ViewContainer *viewContainer = new ssc::ViewContainer(widget->centralWidget());
	widget->setupViewContainer(viewContainer, "dummy", "none", 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestViewContainer::test_ACS_3D_Tool_Container()
{
	widget->mDumpSpeedData = true;

	widget->setDescription("ACS+3D, moving tool in view container");
	ssc::ViewContainer *viewContainer = new ssc::ViewContainer(widget->centralWidget());
	widget->containerGPUSlice(viewContainer->addView("Slice A", 0, 0), "A", image[0], ssc::ptAXIAL);
	widget->containerGPUSlice(viewContainer->addView("Slice C", 0, 1), "C", image[0], ssc::ptCORONAL);
	widget->containerGPUSlice(viewContainer->addView("Slice S", 1, 0), "S", image[0], ssc::ptSAGITTAL);
	widget->container3D(viewContainer->addView("3D", 1, 1), image[0]);

	widget->setupViewContainer(viewContainer, "dummy", "none", 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestViewContainer::test_AnyDual_3D_Tool_Container()
{
	widget->mDumpSpeedData = true;

	widget->setDescription("2 x Any + Side + 3D, moving tool in view container");
	ssc::ViewContainer *viewContainer = new ssc::ViewContainer(widget->centralWidget());
	widget->containerGPUSlice(viewContainer->addView("Slice Any1", 0, 0), "Any", image[0], ssc::ptANYPLANE);
	widget->containerGPUSlice(viewContainer->addView("Slice Dual1", 1, 0), "Dual", image[0], ssc::ptSIDEPLANE);
	widget->containerGPUSlice(viewContainer->addView("Slice Any2", 0, 1), "Any", image[0], ssc::ptANYPLANE);
	widget->containerGPUSlice(viewContainer->addView("Slice Dual2", 1, 1), "Dual", image[0], ssc::ptSIDEPLANE);
	widget->container3D(viewContainer->addView("3D", 0, 2, 2, 1), image[0]);

	widget->setupViewContainer(viewContainer, "dummy", "none", 0, 0);

	CPPUNIT_ASSERT(runWidget());
}

void TestViewContainer::test_3D_Composite_Views_GPU_Container()
{
	widget->setDescription("3D Composites (2 volumes) with ACS, moving tool (in viewcontainer)");
	ssc::ViewContainer *viewContainer = new ssc::ViewContainer(widget->centralWidget());

	QStringList images; images << image[1] << image[2];
	ImageParameters parameters[2];

	parameters[0].llr = 35;
	parameters[0].alpha = .1;

	parameters[1].llr = 55;
	parameters[1].alpha = .7;
	parameters[1].lut = getCreateLut(0, 200, .67, .68, 0, 1, .4, .8);

	widget->containerGPUSlice(viewContainer->addView("Slice A", 0, 0), "A", image[0], ssc::ptAXIAL);
	widget->containerGPUSlice(viewContainer->addView("Slice C", 0, 1), "C", image[0], ssc::ptCORONAL);
	widget->containerGPUSlice(viewContainer->addView("Slice S", 1, 0), "S", image[0], ssc::ptSAGITTAL);
	widget->containerGPU3D(viewContainer->addView("3D", 1, 1), images, parameters);

	widget->setupViewContainer(viewContainer, "dummy", "none", 0, 0);

	CPPUNIT_ASSERT(runWidget());
}
