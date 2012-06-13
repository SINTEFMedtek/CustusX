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

//namespace ssc
//{

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	std::cout << "DataFolder: " << ssc::TestUtilities::GetDataRoot() << std::endl;

	QString vtkFileName1 = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T105136_MRT1.vtk");
	QString vtkFileName2 = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T102309_MRA.vtk");

	QString stlFileName1 = ssc::TestUtilities::ExpandDataFileName("../Sandbox/Models/5S_joinedpart.STL");
	QString stlFileName2 = ssc::TestUtilities::ExpandDataFileName("../Sandbox/Models/5S_probeMSector.STL");
	QString stlFileName3 = ssc::TestUtilities::ExpandDataFileName("../Sandbox/Models/8L_12L Probe.STL");
	QString stlFileName4 = ssc::TestUtilities::ExpandDataFileName("../Sandbox/Models/Intra_operativ_navigator.STL");
	QString stlFileName5 = ssc::TestUtilities::ExpandDataFileName("../Sandbox/Models/Planning_navigator.STL");
	QString stlFileName6 = ssc::TestUtilities::ExpandDataFileName("../Sandbox/Models/Vermon Phase Probe.STL");

	// .vtk
	//ssc::MeshPtr mesh1 = ssc::DataManager::instance()->loadMesh(vtkFileName1, ssc::mrtPOLYDATA);
	// .stl
	// OK: f.eks. 2 eller 4,    Ikke ok: 5S_joinedpart.STL
	ssc::MeshPtr mesh1 = ssc::DataManager::getInstance()->loadMesh(stlFileName2, stlFileName2, ssc::rtSTL);

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

	ssc::ViewWidget* view = new ssc::ViewWidget();

	ssc::GeometricRepPtr rep = ssc::GeometricRep::New(mesh1->getUid());
	rep->setMesh(mesh1);
	view->setRep(rep);

	ssc::AxesRepPtr axesRep = ssc::AxesRep::New("AxesRepUID");
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

//}//namespace ssc
