// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifdef CX_USE_TSF
#include "testing/cxTestTubeSegmentationFramework.h"

#include <fstream>
#include "cxData.h"
#include "cxLogicManager.h"
#include "cxReporter.h"
#include "cxDataInterface.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxTubeSegmentationFilter.h"
#include "cxDataLocations.h"
#include "cxFilterTimedAlgorithm.h"
#include "cxSelectDataStringDataAdapter.h"

#include "parameters.hpp"
#include "tsf-config.h"
#include "SIPL/Exceptions.hpp"

#include "catch.hpp"

TestTubeSegmentationFramework::TestTubeSegmentationFramework() 
{
	cx::LogicManager::initialize();
}

TestTubeSegmentationFramework::~TestTubeSegmentationFramework() 
{
	cx::LogicManager::shutdown();
}


void TestTubeSegmentationFramework::testConstructor()
{
}

void TestTubeSegmentationFramework::testParameters()
{
	std::string path = std::string(PARAMETERS_DIR);
	{
		INFO("Could not find parameter file.");
		REQUIRE(ifstream(path.c_str()));
	}

	{
		INFO("Inititalizing parameters with default values failed.");
		REQUIRE_NOTHROW(initParameters(path));
	}
	paramList defaultParameters = initParameters(path);

	{
		INFO("No default string parameters found.");
		REQUIRE(!defaultParameters.strings.empty());
	}

	{
		INFO("No default bool parameters found.");
		REQUIRE(!defaultParameters.bools.empty());
	}

	{
		INFO("No default numeric parameters found.");
		REQUIRE(!defaultParameters.numerics.empty());
	}
	{
		INFO("Gpu not validated as device.");
		REQUIRE(defaultParameters.strings["device"].validate("gpu"));
	}
	{
		INFO("250 not a valid gvf-iterations value.");
		REQUIRE(defaultParameters.numerics["gvf-iterations"].validate(250));
	}
	{
		INFO("Set parameter parameter to Lung-Airways-CT failed.");
		REQUIRE_NOTHROW(setParameter(defaultParameters, "parameters", "Lung-Airways-CT"));
	}

	{
		INFO("Load presets failed.");
		REQUIRE_NOTHROW(loadParameterPreset(defaultParameters, path));
	}
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

void TestTubeSegmentationFramework::testLiverVesselsCTPreset()
{
	runFilter(QString("Liver-Vessels-CT"));
}

void TestTubeSegmentationFramework::testLiverVesselsMRPreset()
{
	runFilter(QString("Liver-Vessels-MR"));
}

void TestTubeSegmentationFramework::testLoadParameterFile()
{
	paramList preset = loadPreset(QString("Neuro-Vessels-USA"));

	INFO("sphere-segmentation not set to true in Neuro-Vessels-USA");
	REQUIRE(getParamBool(preset,"sphere-segmentation") == true);
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
	REQUIRE(tsf);
	tsf->getInputTypes();
	tsf->getOutputTypes();
	tsf->getOptions();

	//create a new patient
	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/"+preset+".mhd";
	cx::patientService()->getPatientData()->newPatient(cx::DataLocations::getTestDataPath()+ "/temp/TubeSegmentationFramework/");
	QString info;
	cx::DataPtr data = cx::patientService()->getPatientData()->importData(filename, info);
	REQUIRE(data);

	//set input
	std::vector < cx::SelectDataStringDataAdapterBasePtr > input =tsf->getInputTypes();
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
		REQUIRE(input[0]->getData()->getName() == preset);
	}
	tsf->requestSetPresetSlot(preset);

	//execute
	{
		INFO("Preprocessing TubeSegmentationFilter failed.");
		REQUIRE(tsf->preProcess());
	}

	{
		INFO("Executed TSF on "+preset.toStdString()+" with "+preset.toStdString()+" parameters failed.");
		REQUIRE(tsf->execute());
	}
	{
		INFO("Post processing data from TSF failed.");
		REQUIRE(tsf->postProcess());
	}

	//check output
	std::vector < cx::SelectDataStringDataAdapterBasePtr > output = tsf->getOutputTypes();
	{
		INFO("Number of outputs has changed.");
		REQUIRE(output.size() == 5);
	}
	{
		INFO("Centerline volume not generated.");
		REQUIRE(output[0]->getData());
	}
	{
		INFO("Centerline vtkPolydata not generated.");
		REQUIRE(output[1]->getData());
	}
	//TODO not all presets generate these...
//	CPPUNIT_ASSERT_MESSAGE("Segmented centerline not generated.", output[2]->getData());
//	CPPUNIT_ASSERT_MESSAGE("Segmented centerlines surface not generated.", output[3]->getData());
	{
		INFO("TDF volume not generated.");
		REQUIRE(output[4]->getData());
	}
	//TODO clean up temp folder...

}

#endif //CX_USE_TSF
