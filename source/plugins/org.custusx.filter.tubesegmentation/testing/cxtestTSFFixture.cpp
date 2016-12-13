/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxtestTSFFixture.h"

#include <fstream>
#include <QDir>
#include <QFile>
#include "cxData.h"
#include "cxLogicManager.h"

#include "cxTubeSegmentationFilterService.h"
#include "cxDataLocations.h"
#include "cxFilterTimedAlgorithm.h"
#include "cxSelectDataStringProperty.h"
#include "cxTypeConversions.h"

#include "parameters.hpp"
#include "tsf-config.h"
#include "SIPL/Exceptions.hpp"
#include "cxSessionStorageService.h"
#include "cxLegacySingletons.h"

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

QString TestTubeSegmentationFramework::getParametersDir()
{
	QString path = cx::DataLocations::findConfigFolder("/tsf", QString(KERNELS_DIR)) + "parameters";

	return QDir::toNativeSeparators(path);
}

void TestTubeSegmentationFramework::testParameters()
{
	std::string path = getParametersDir().toStdString();
	{
		INFO("Could not find parameter file: "+path);
		REQUIRE(QFile::exists(qstring_cast(path)));
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
	std::string path = getParametersDir().toStdString();
	paramList parameters = initParameters(path);
	setParameter(parameters, "parameters", preset.toStdString());
	loadParameterPreset(parameters, path);
	return parameters;
}

void TestTubeSegmentationFramework::runFilter(QString preset)
{
	//setup filter
	cx::TubeSegmentationFilterPtr tsf = cx::TubeSegmentationFilterPtr(new cx::TubeSegmentationFilter(cx::logicManager()->getPluginContext()));
	REQUIRE(tsf);
	tsf->getInputTypes();
	tsf->getOutputTypes();
	tsf->getOptions();

	//create a new patient
	QString filename = cx::DataLocations::getExistingTestData("testing/TubeSegmentationFramework", preset+".mhd");
//	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/"+preset+".mhd";
	cx::sessionStorageService()->load(cx::DataLocations::getTestDataPath()+ "/temp/TubeSegmentationFramework/");
//	cx::patientService()->newPatient(cx::DataLocations::getTestDataPath()+ "/temp/TubeSegmentationFramework/");
	QString info;
	cx::DataPtr data = cx::patientService()->importData(filename, info);
	REQUIRE(data);

	//set input
	std::vector < cx::SelectDataStringPropertyBasePtr > input =tsf->getInputTypes();
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
	std::vector < cx::SelectDataStringPropertyBasePtr > output = tsf->getOutputTypes();
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
