#ifdef CX_USE_LEVEL_SET
#include "catch.hpp"
#include "cxLevelSetFilter.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscToolManager.h"
#include "cxToolManager.h"
#include "sscManualTool.h"
#include "sscData.h"

namespace cxtest {

/*
DataPtr createDummyData(void * data, int size_x, int size_y, int size_z) {
	vtkImageImportPtr imageImport = vtkImageImportPtr::New();

	imageImport->SetWholeExtent(0, size_x - 1, 0, size_y - 1, 0, size_z - 1);
	imageImport->SetDataExtentToWholeExtent();
	imageImport->SetDataScalarType(type);
	imageImport->SetNumberOfScalarComponents(1);
	imageImport->SetImportVoidPointer(data);
	imageImport->GetOutput()->Update();
	imageImport->Modified();

	vtkImageDataPtr retval = vtkImageDataPtr::New();
	retval->DeepCopy(imageImport->GetOutput());
}
*/

TEST_CASE("LevelSetFilter: getSeedPointFromTool", "[unit][plugins][Algorithm][LevelSetFilter]") 
{
    cx::cxToolManager::initializeObject();
    cx::cxToolManager * toolmanager = cx::cxToolManager::getInstance();
    cx::ManualToolPtr tool = toolmanager->getManualTool();
    std::cout << " test" << std::endl;
    cx::Vector3D toolTipPoint;
    toolTipPoint.setRandom();
    tool->set_prMt(cx::createTransformTranslate(toolTipPoint));

    cx::DataPtr data;
    cx::Vector3D point = cx::LevelSetFilter::getSeedPointFromTool(data);
    REQUIRE(toolTipPoint(0) == point(0));
    REQUIRE(toolTipPoint(1) == point(1));
    REQUIRE(toolTipPoint(2) == point(2));

}

TEST_CASE("LevelSetFilter: isSeedPointInsideImage", "[unit][plugins][Algorithm][LevelSetFilter]")
{

}

}; // end cxtest namespace
#endif // end ifdef CX_USE_LEVEL_SET
