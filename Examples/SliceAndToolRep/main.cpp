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

#include "sscExWindow.h"

using ssc::Vector3D;
using ssc::Transform3D;

/** Test app for SSC
 */
int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	std::vector<std::string> image;
	image.push_back("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	image.push_back("MetaImage/20070309T105136_MRT1.mhd");
	image.push_back("MetaImage/20070309T102309_MRA.mhd");

	ViewsWindow widget;

	  widget.defineSlice("A",image[0], ssc::ptAXIAL, 0, 0);
	  widget.defineSlice("C",image[0], ssc::ptSAGITTAL, 0, 1);
	  widget.defineSlice("S",image[0], ssc::ptCORONAL, 0, 2);
//  widget.define3D(image[0], 0, 0);
  //widget.define3D(image[1], 0, 1);

  widget.show();
#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
	widget.activateWindow();
#endif
	widget.raise();

  widget.updateRender();

  int val = qApp->exec();
	return val;
}

//void TestVisualRendering::test_ACS_3D_Tool()
//{
//	widget->setDescription("ACS+3D, moving tool");
//	widget->define3D(image[0], 1, 1);
//	widget->defineSlice("A", image[0], ssc::ptAXIAL, 0, 0);
//	widget->defineSlice("C", image[0], ssc::ptCORONAL, 1, 0);
//	widget->defineSlice("S", image[0], ssc::ptSAGITTAL, 0, 1);
//	
//	widget->updateRender();
//	int val = qApp->exec();
//	CPPUNIT_ASSERT(!val && widget->accepted());	
//}

