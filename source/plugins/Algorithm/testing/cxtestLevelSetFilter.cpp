#ifdef CX_USE_LEVEL_SET
#include "catch.hpp"
#include "cxLevelSetFilter.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscToolManager.h"
#include "cxToolManager.h"
#include "sscManualTool.h"
#include "sscData.h"
#include "sscImage.h"
#include "cxtestUtilities.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxDataLocations.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxLogicManager.h"

namespace cxtest {

TEST_CASE("LevelSetFilter: getSeedPointFromTool", "[unit][plugins][Algorithm][LevelSetFilter]") 
{
    cx::cxToolManager::initializeObject();
    cx::cxToolManager * toolmanager = cx::cxToolManager::getInstance();
    cx::ManualToolPtr tool = toolmanager->getManualTool();
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
    cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(10,10,10), 1);
    cx::Vector3D point;
    point.setOnes();

    CHECK(cx::LevelSetFilter::isSeedPointInsideImage(point, image));

    point(1) = 12;
    CHECK_FALSE(cx::LevelSetFilter::isSeedPointInsideImage(point, image));
}

TEST_CASE("LevelSetFilter: execute", "[integration][plugins][Algorithm][LevelSetFilter]")
{
    cx::LogicManager::initialize();
	//setup filter
	cx::LevelSetFilterPtr lsf = cx::LevelSetFilterPtr(new cx::LevelSetFilter());
	REQUIRE(lsf);
	lsf->getInputTypes();
	lsf->getOutputTypes();
	lsf->getOptions();

    //create a new patient
	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/Lung-Airways-CT.mhd";
	cx::patientService()->getPatientData()->newPatient(cx::DataLocations::getTestDataPath()+ "/temp/LevelSetFilter/");
	QString info;
	cx::DataPtr data = cx::patientService()->getPatientData()->importData(filename, info);
	REQUIRE(data);

	//set input
	std::vector < cx::SelectDataStringDataAdapterBasePtr > input =lsf->getInputTypes();
	{
		INFO("Number of inputs has changed.");
		REQUIRE(input.size() == 1);
	}
	{
		INFO("Could not set input to the filter.");
		REQUIRE(input[0]->setValue(data->getUid()));
	}
	{
		INFO("The name of the input data is not as we requested.");
		REQUIRE(input[0]->getData()->getName() == "Lung-Airways-CT");
	}

	// TODO: set seedpoint, threshold, epsilon and alpha
	// TODO: execute
	// TODO: check that a volume and contour is generated
    cx::LogicManager::shutdown();
}

}; // end cxtest namespace
#endif // end ifdef CX_USE_LEVEL_SET
