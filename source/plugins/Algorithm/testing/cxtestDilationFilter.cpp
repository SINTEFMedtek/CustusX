#include "catch.hpp"
#include "cxDilationFilter.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxDataLocations.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxData.h"
#include "cxImage.h"

TEST_CASE("DilationFilter: execute", "[plugins][Algorithm][DilationFilter]")
{
    // Setup filter
	cx::DilationFilterPtr filter = cx::DilationFilterPtr(new cx::DilationFilter());
	REQUIRE(filter);
	filter->getInputTypes();
	filter->getOutputTypes();
	filter->getOptions();

	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/DilationFilter/helix_seg.mhd";
	cx::patientService()->getPatientData()->newPatient(cx::DataLocations::getTestDataPath()+ "/temp/DilationFilter/");
	QString info;
	cx::DataPtr data = cx::patientService()->getPatientData()->importData(filename, info);
	REQUIRE(data);

	//set input
	std::vector < cx::SelectDataStringDataAdapterBasePtr > input =filter->getInputTypes();
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
		REQUIRE(input[0]->getData()->getName() == "helix_seg");
	}
	// Execute
	{
		INFO("Preprocessing LevelSetFilter failed.");
		REQUIRE(filter->preProcess());
	}

	{
		REQUIRE(filter->execute());
	}
	{
		INFO("Post processing data from Level Set Filter failed.");
		REQUIRE(filter->postProcess());
	}

	// Check output
	std::vector < cx::SelectDataStringDataAdapterBasePtr > output = filter->getOutputTypes();
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
}
