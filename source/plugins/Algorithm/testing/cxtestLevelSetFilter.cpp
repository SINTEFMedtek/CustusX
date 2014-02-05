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
#include "sscDataAdapter.h"
#include "sscDoubleDataAdapter.h"
#include "sscRegistrationTransform.h"
#include "sscDoubleDataAdapterXml.h"

namespace cxtest {

void setSeedPoint(cx::Vector3D point) {
    cx::cxToolManager::initializeObject();
    cx::cxToolManager * toolmanager = cx::cxToolManager::getInstance();
    cx::ManualToolPtr tool = toolmanager->getManualTool();
    tool->set_prMt(cx::createTransformTranslate(point));

}

TEST_CASE("LevelSetFilter: getSeedPointFromTool", "[unit][plugins][Algorithm][LevelSetFilter]")
{
    cx::Vector3D toolTipPoint;
    toolTipPoint.setRandom();
    setSeedPoint(toolTipPoint);
    cx::ImagePtr image = cxtest::Utilities::create3DImage();
    cx::Vector3D point = cx::LevelSetFilter::getSeedPointFromTool(image);
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

TEST_CASE("LevelSetFilter: execute", "[integration][plugins][Algorithm][LevelSetFilter][hide][broken]")
{
    cx::LogicManager::initialize();
	//setup filter
	cx::LevelSetFilterPtr lsf = cx::LevelSetFilterPtr(new cx::LevelSetFilter());
	REQUIRE(lsf);
	lsf->getInputTypes();
	lsf->getOutputTypes();
	lsf->getOptions();

    //create a new patient
	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/Default.mhd";
	cx::patientService()->getPatientData()->newPatient(cx::DataLocations::getTestDataPath()+ "/temp/LevelSetFilter/");
	QString info;
	cx::DataPtr data = cx::patientService()->getPatientData()->importData(filename, info);
	data->get_rMd_History()->setRegistration(cx::Transform3D::Identity());
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
		REQUIRE(input[0]->getData()->getName() == "Default");
	}

	// set seedpoint, threshold, epsilon and alpha
	cx::Vector3D seedPoint;
	seedPoint(0) = 34;
	seedPoint(1) = 29;
	seedPoint(2) = 50;
	setSeedPoint(seedPoint);
	// TODO: the three lines below are not working properly. The options are not set!
	lsf->getThresholdOption(lsf->getmOptions())->setValue(60);
	lsf->getEpsilonOption(lsf->getmOptions())->setValue(100);
	lsf->getAlphaOption(lsf->getmOptions())->setValue(0.1);
	// Execute
	{
		INFO("Preprocessing LevelSetFilter failed.");
		REQUIRE(lsf->preProcess());
	}

	{
		REQUIRE(lsf->execute());
	}
	{
		INFO("Post processing data from Level Set Filter failed.");
		REQUIRE(lsf->postProcess());
	}

	// Check output
	std::vector < cx::SelectDataStringDataAdapterBasePtr > output = lsf->getOutputTypes();
	{
		INFO("Number of outputs has changed.");
		REQUIRE(output.size() == 2);
	}
	{
		INFO("Segmentation volume not generated.");
		REQUIRE(output[0]->getData());
	}
	{
		INFO("Surface/contour not generated.");
		REQUIRE(output[1]->getData());
	}
    cx::LogicManager::shutdown();
}

}; // end cxtest namespace
#endif // end ifdef CX_USE_LEVEL_SET
