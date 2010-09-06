#include <iostream>
#include <string>
#include <vector>

#include <vtkPolyData.h>

#include "sscTestUtilities.h"

#include "sscDataManager.h"
#include "sscMesh.h"

//namespace ssc
//{

int main(int argc, char **argv)
{
	std::cout << "DataFolder: " << ssc::TestUtilities::GetDataRoot() << std::endl;

	std::string vtkFileName1 = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T105136_MRT1.vtk");
	std::string vtkFileName2 = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T102309_MRA.vtk");

	std::string stlFileName1 = ssc::TestUtilities::ExpandDataFileName("../Sandbox/Models/5S_joinedpart.STL");
	std::string stlFileName2 = ssc::TestUtilities::ExpandDataFileName("../Sandbox/Models/5S_probeMSector.STL");
	std::string stlFileName3 = ssc::TestUtilities::ExpandDataFileName("../Sandbox/Models/8L_12L Probe.STL");
	std::string stlFileName4 = ssc::TestUtilities::ExpandDataFileName("../Sandbox/Models/Intra_operativ_navigator.STL");
	std::string stlFileName5 = ssc::TestUtilities::ExpandDataFileName("../Sandbox/Models/Planning_navigator.STL");
	std::string stlFileName6 = ssc::TestUtilities::ExpandDataFileName("../Sandbox/Models/Vermon Phase Probe.STL");

	// .vtk
	//ssc::MeshPtr mesh1 = ssc::DataManager::instance()->loadMesh(vtkFileName1, ssc::mrtPOLYDATA);
	// .stl
	ssc::MeshPtr mesh1 = ssc::DataManager::getInstance()->loadMesh(stlFileName1, stlFileName1, ssc::rtSTL);

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


	//std::vector<std::string> names = ssc::DataManager::instance()->getImageNames();

	return 0;
}

//}//namespace ssc
