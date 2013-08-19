#include <iostream>
#include <string>
#include <vector>

#include <vtkPolyData.h>

#include "sscTestUtilities.h"

#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"

//namespace ssc
//{

int main(int argc, char **argv)
{
	ssc::MessageManager::initialize();

	std::cout << "DataFolder: " << ssc::TestUtilities::GetDataRoot() << std::endl;

	QString stlFileName1 = ssc::TestUtilities::ExpandDataFileName("../source/resource/ssc/Sandbox/Models/01-117-0329_Planning-Navigator.stl");

	ssc::MeshPtr mesh1 = ssc::DataManager::getInstance()->loadMesh(stlFileName1, stlFileName1, ssc::rtSTL);
	if(!mesh1)
	{
		std::cout << "Cannot read mesh" << std::endl;
		return 1;
	}

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

	if(numPoints <= 0)
	{
		std::cout << "No points in test mesh" << std::endl;
		return 1;
	}
	if(numPolys <= 0)
	{
		std::cout << "No polys in test mesh" << std::endl;
		return 1;
	}
	if(numVerts != 0)
	{
		std::cout << "Not expecting verts in test mesh" << std::endl;
		return 1;
	}
	if(numLines != 0)
	{
		std::cout << "Not expecting lines in test mesh" << std::endl;
		return 1;
	}
	if(numStrips != 0)
	{
		std::cout << "Not expecting strips in test mesh" << std::endl;
		return 1;
	}




	//std::vector<QString> names = ssc::DataManager::instance()->getImageNames();

	ssc::MessageManager::shutdown();

	return 0;
}

//}//namespace ssc
