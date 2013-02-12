#include "testing/cxTestTubeSegmentationFramework.h"

#include <fstream>
#include "sscData.h"
#include "cxLogicManager.h"
#include "cxDataInterface.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxTubeSegmentationFilter.h"
#include "cxDataLocations.h"
#include "cxFilterTimedAlgorithm.h"

#ifdef CX_USE_TSF
#include "commons.hpp"
#include "parameters.hpp"
#include "tsf-config.h"
#include "SIPL/Exceptions.hpp"
#endif //CX_USE_TSF

void TestTubeSegmentationFramework::setUp()
{
	cx::LogicManager::initialize();
}

void TestTubeSegmentationFramework::tearDown()
{
	cx::LogicManager::shutdown();
}

void TestTubeSegmentationFramework::testConstructor()
{
}

void TestTubeSegmentationFramework::testFilter()
{
	//create the filter
	cx::TubeSegmentationFilterPtr tsf = cx::TubeSegmentationFilterPtr(new cx::TubeSegmentationFilter());
	CPPUNIT_ASSERT(tsf);

	//initialize the filter
	tsf->getInputTypes();
	tsf->getOutputTypes();
	tsf->getOptions();

	//check the basics
	CPPUNIT_ASSERT_MESSAGE("Type has changed.", (tsf->getType().compare("TubeSegmentationFilter") == 0));
	CPPUNIT_ASSERT_MESSAGE("Name has changed.", (tsf->getName().compare("Tube-Segmentation Filter") == 0));
	CPPUNIT_ASSERT_MESSAGE("Help has changed.", (tsf->getHelp().compare("<html>"
	        "<h3>Tube-Segmentation.</h3>"
	        "<p><i>Extracts the centerline and creates a segementation. </br>GPU-base algorithm wrtiten by Erik Smistad (NTNU).</i></p>"
	        "</html>") == 0));

	//create a new patient and load in testdata
	QString filename = cx::DataLocations::getTestDataPath() +
			"/testing/"
			"TubeSegmentationFramework/helix.mhd";
	cx::patientService()->getPatientData()->newPatient(cx::DataLocations::getTestDataPath() + "/temp/TubeSegmentationFramework/");
	QString info;
	ssc::DataPtr data = cx::patientService()->getPatientData()->importData(filename, info);
	CPPUNIT_ASSERT(data);

	//set which data to process
	std::vector<cx::SelectDataStringDataAdapterBasePtr> input = tsf->getInputTypes();
	CPPUNIT_ASSERT_MESSAGE("Number of inputs has changed.", input.size() == 1);
	CPPUNIT_ASSERT(input[0]->setValue(data->getUid()));
	CPPUNIT_ASSERT(input[0]->getData()->getName() == "helix");

	//run the filter
	CPPUNIT_ASSERT_MESSAGE("Preprocessing TubeSegmentationFilter failed.", tsf->preProcess());
	CPPUNIT_ASSERT_MESSAGE("Executed TSF on helix with default parameters failed.", tsf->execute());
	CPPUNIT_ASSERT_MESSAGE("Post processing data from TSF failed.", tsf->postProcess());

	//check output
	std::vector<cx::SelectDataStringDataAdapterBasePtr> output = tsf->getOutputTypes();
	CPPUNIT_ASSERT_MESSAGE("Number of outputs has changed.", output.size() == 3);
	CPPUNIT_ASSERT_MESSAGE("Centerline volume not generated.",output[0]->getData());
	CPPUNIT_ASSERT_MESSAGE("Centerline vtkPolydata not generated.",output[1]->getData());
	CPPUNIT_ASSERT_MESSAGE("Segmentation not generated.",output[2]->getData());

	//delete patient to prevent overflow of files
	//TODO
}

void TestTubeSegmentationFramework::testParameters()
{
	std::string path = std::string(PARAMETERS_DIR)+"/parameters";
	CPPUNIT_ASSERT_MESSAGE("Could not find parameter file.", ifstream(path.c_str()));

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("Inititalizing parameters with default values failed.", initParameters(path));
	paramList defaultParameters = initParameters(path);

	CPPUNIT_ASSERT_MESSAGE("No default string parameters found.", !defaultParameters.strings.empty());
	CPPUNIT_ASSERT_MESSAGE("No default bool parameters found.", !defaultParameters.bools.empty());
	CPPUNIT_ASSERT_MESSAGE("No default numeric parameters found.", !defaultParameters.numerics.empty());

	CPPUNIT_ASSERT_MESSAGE("Gpu not validated as device.", defaultParameters.strings["device"].validate("gpu"));
	CPPUNIT_ASSERT_MESSAGE("250 not a valid gvf-iterations value.", defaultParameters.numerics["gvf-iterations"].validate(250));

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("Set parameter parameter to airway failed.", setParameter(defaultParameters, "parameters", "airway"));
	paramList presetParameter = setParameter(defaultParameters, "parameters", "airway");
	CPPUNIT_ASSERT_NO_THROW_MESSAGE("Load presets failed.", loadParameterPreset(presetParameter, path));


}
