#include <iostream>
#include <string>
#include <vector>

#include <vtkPolyData.h>
#include <vtkRenderer.h>

#include <QtGui>


#include "sscTestUtilities.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscGeometricRep.h"
#include "sscAxesRep.h"
#include "sscView.h"
#include "sscTypeConversions.h"

//namespace cx
//{

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	std::cout << "DataFolder: " << cx::TestUtilities::GetDataRoot() << std::endl;

	QString vtkFileName1 = cx::TestUtilities::ExpandDataFileName("MetaImage/20070309T105136_MRT1.vtk");
	QString vtkFileName2 = cx::TestUtilities::ExpandDataFileName("MetaImage/20070309T102309_MRA.vtk");

	QString stlFileName1 = cx::TestUtilities::ExpandDataFileName("../Sandbox/Models/5S_joinedpart.STL");
	QString stlFileName2 = cx::TestUtilities::ExpandDataFileName("../Sandbox/Models/5S_probeMSector.STL");
	QString stlFileName3 = cx::TestUtilities::ExpandDataFileName("../Sandbox/Models/8L_12L Probe.STL");
	QString stlFileName4 = cx::TestUtilities::ExpandDataFileName("../Sandbox/Models/Intra_operativ_navigator.STL");
	QString stlFileName5 = cx::TestUtilities::ExpandDataFileName("../Sandbox/Models/Planning_navigator.STL");
	QString stlFileName6 = cx::TestUtilities::ExpandDataFileName("../Sandbox/Models/Vermon Phase Probe.STL");

	// .vtk
	//MeshPtr mesh1 = DataManager::instance()->loadMesh(vtkFileName1, mrtPOLYDATA);
	// .stl
	// OK: f.eks. 2 eller 4,    Ikke ok: 5S_joinedpart.STL
	cx::MeshPtr mesh1 = cx::DataManager::getInstance()->loadMesh(stlFileName2, stlFileName2, cx::rtSTL);

	std::cout << "UID     : " << mesh1->getUid() << std::endl;
	std::cout << "Name    : " << mesh1->getName() << std::endl;
	std::cout << "RegStat : " << mesh1->getRegistrationStatus() << std::endl;

	int numPoints = mesh1->getVtkPolyData()->GetNumberOfPoints();
	int numVerts = mesh1->getVtkPolyData()->GetNumberOfVerts();
	int numLines = mesh1->getVtkPolyData()->GetNumberOfLines();
	int numPolys = mesh1->getVtkPolyData()->GetNumberOfPolys();
	int numStrips = mesh1->getVtkPolyData()->GetNumberOfStrips();

	std::cout << "numPoints : " << numPoints << std::endl;
	std::cout << "numVerts  : " << numVerts << std::endl;
	std::cout << "numLines  : " << numLines << std::endl;
	std::cout << "numPolys  : " << numPolys << std::endl;
	std::cout << "numStrips : " << numStrips << std::endl;

	cx::ViewWidget* view = new cx::ViewWidget();

	cx::GeometricRepPtr rep = cx::GeometricRep::New(mesh1->getUid());
	rep->setMesh(mesh1);
	view->setRep(rep);

	cx::AxesRepPtr axesRep = cx::AxesRep::New("AxesRepUID");
	view->addRep(axesRep);

	QMainWindow mainWindow;
	mainWindow.setCentralWidget(view);
	mainWindow.resize(QSize(500,500));
	mainWindow.show();
	view->getRenderer()->ResetCamera();
	QTimer::singleShot(SSC_DEFAULT_TEST_TIMEOUT_SECS*1000, &app, SLOT(quit())); // terminate app after some seconds - this is an automated test!!
	app.exec();

	return 0;
}

//}//namespace cx
