#include "catch.hpp"
#include "cxLevelSetFilterService.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxToolManager.h"
#include "cxToolManager.h"
#include "cxManualTool.h"
#include "cxData.h"
#include "cxImage.h"
#include "cxtestUtilities.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxDataLocations.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxLogicManager.h"
#include "cxDataAdapter.h"
#include "cxDoubleDataAdapter.h"
#include "cxRegistrationTransform.h"
#include "cxDoubleDataAdapterXml.h"

namespace cxtest {

void setSeedPoint(cx::Vector3D point)
{
	cx::ManualToolPtr tool = cx::trackingService()->getManualTool();
    tool->set_prMt(cx::createTransformTranslate(point));

}

TEST_CASE("LevelSetFilter: getSeedPointFromTool", "[unit][modules][Algorithm][LevelSetFilter]")
{
	cx::LogicManager::initialize();
	cx::Vector3D toolTipPoint;
    toolTipPoint.setRandom();
	setSeedPoint(toolTipPoint);
	cx::ImagePtr image = cxtest::Utilities::create3DImage();
	cx::Vector3D point = cx::LevelSetFilter::getSeedPointFromTool(image);
	REQUIRE(toolTipPoint(0) == point(0));
    REQUIRE(toolTipPoint(1) == point(1));
    REQUIRE(toolTipPoint(2) == point(2));

	cx::LogicManager::shutdown();
}

TEST_CASE("LevelSetFilter: isSeedPointInsideImage", "[unit][modules][Algorithm][LevelSetFilter]")
{
	cx::LogicManager::initialize();
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(10,10,10), 1);
    cx::Vector3D point;
    point.setOnes();

    CHECK(cx::LevelSetFilter::isSeedPointInsideImage(point, image));

    point(1) = 12;
    CHECK_FALSE(cx::LevelSetFilter::isSeedPointInsideImage(point, image));
	cx::LogicManager::shutdown();
}

TEST_CASE("LevelSetFilter: execute", "[integration][modules][Algorithm][LevelSetFilter][hide][broken]")
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
