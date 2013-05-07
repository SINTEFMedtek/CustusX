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
#include "cxSelectDataStringDataAdapter.h"

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

void TestTubeSegmentationFramework::testParameters()
{
	std::string path = std::string(PARAMETERS_DIR);
	CPPUNIT_ASSERT_MESSAGE("Could not find parameter file.", ifstream(path.c_str()));

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("Inititalizing parameters with default values failed.", initParameters(path));
	paramList defaultParameters = initParameters(path);

	CPPUNIT_ASSERT_MESSAGE("No default string parameters found.", !defaultParameters.strings.empty());
	CPPUNIT_ASSERT_MESSAGE("No default bool parameters found.", !defaultParameters.bools.empty());
	CPPUNIT_ASSERT_MESSAGE("No default numeric parameters found.", !defaultParameters.numerics.empty());

	CPPUNIT_ASSERT_MESSAGE("Gpu not validated as device.", defaultParameters.strings["device"].validate("gpu"));
	CPPUNIT_ASSERT_MESSAGE("250 not a valid gvf-iterations value.", defaultParameters.numerics["gvf-iterations"].validate(250));

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("Set parameter parameter to Lung-Airways-CT failed.", setParameter(defaultParameters, "parameters", "Lung-Airways-CT"));

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("Load presets failed.", loadParameterPreset(defaultParameters, path));
}

void TestTubeSegmentationFramework::testDefaultPreset()
{
	runFilter(QString("Default"));
}

void TestTubeSegmentationFramework::testSyntheticVascusynthPreset()
{
	runFilter(QString("Synthetic-Vascusynth"));
}

void TestTubeSegmentationFramework::testPhantomAccUSPreset()
{
	runFilter(QString("Phantom-Acc-US"));
}

void TestTubeSegmentationFramework::testNeuroVesselsMRAPreset()
{
	runFilter(QString("Neuro-Vessels-MRA"));
}

void TestTubeSegmentationFramework::testNeuroVesselsUSAPreset()
{
	runFilter(QString("Neuro-Vessels-USA"));
}

void TestTubeSegmentationFramework::testLungAirwaysCTPreset()
{
	runFilter(QString("Lung-Airways-CT"));
}

void TestTubeSegmentationFramework::testLoadParameterFile()
{
	paramList preset = loadPreset(QString("Neuro-Vessels-USA"));

	CPPUNIT_ASSERT_MESSAGE("sphere-segmentation not set to true in Neuro-Vessels-USA", getParamBool(preset,"sphere-segmentation") == true);
}

paramList TestTubeSegmentationFramework::loadPreset(QString preset)
{
	std::string path = std::string(PARAMETERS_DIR);
	paramList parameters = initParameters(path);
	setParameter(parameters, "parameters", preset.toStdString());
	loadParameterPreset(parameters, path);
	return parameters;
}

void TestTubeSegmentationFramework::runFilter(QString preset)
{
	//setup filter
	cx::TubeSegmentationFilterPtr tsf = cx::TubeSegmentationFilterPtr(new cx::TubeSegmentationFilter());
	CPPUNIT_ASSERT(tsf);
	tsf->getInputTypes();
	tsf->getOutputTypes();
	tsf->getOptions();

	//create a new patient
	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/"+preset+".mhd";
	cx::patientService()->getPatientData()->newPatient(cx::DataLocations::getTestDataPath()+ "/temp/TubeSegmentationFramework/");
	QString info;
	ssc::DataPtr data = cx::patientService()->getPatientData()->importData(filename, info);
	CPPUNIT_ASSERT(data);

	//set input
	std::vector < cx::SelectDataStringDataAdapterBasePtr > input =tsf->getInputTypes();
	CPPUNIT_ASSERT_MESSAGE("Number of inputs has changed.", input.size() == 1);
	CPPUNIT_ASSERT_MESSAGE("Could not set input to the filter.", input[0]->setValue(data->getUid()));
	CPPUNIT_ASSERT_MESSAGE("The name of the input data is not as we requested.", input[0]->getData()->getName() == preset);
	tsf->requestSetPresetSlot(preset);

	//execute
	CPPUNIT_ASSERT_MESSAGE("Preprocessing TubeSegmentationFilter failed.", tsf->preProcess());
	CPPUNIT_ASSERT_MESSAGE( "Executed TSF on "+preset.toStdString()+" with "+preset.toStdString()+" parameters failed.", tsf->execute());
	CPPUNIT_ASSERT_MESSAGE("Post processing data from TSF failed.", tsf->postProcess());

	//check output
	std::vector < cx::SelectDataStringDataAdapterBasePtr > output = tsf->getOutputTypes();
	CPPUNIT_ASSERT_MESSAGE("Number of outputs has changed.", output.size() == 5);
	CPPUNIT_ASSERT_MESSAGE("Centerline volume not generated.", output[0]->getData());
	CPPUNIT_ASSERT_MESSAGE("Centerline vtkPolydata not generated.",	output[1]->getData());
	//TODO not all presets generate these...
//	CPPUNIT_ASSERT_MESSAGE("Segmented centerline not generated.", output[2]->getData());
//	CPPUNIT_ASSERT_MESSAGE("Segmented centerlines surface not generated.", output[3]->getData());
	CPPUNIT_ASSERT_MESSAGE("TDF volume not generated.", output[4]->getData());

	//TODO clean up temp folder...

}
