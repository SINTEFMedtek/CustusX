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

#include "catch.hpp"
#include "cxLevelSetFilterService.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxManualTool.h"
#include "cxTrackingService.h"
#include "cxData.h"
#include "cxImage.h"
#include "cxtestUtilities.h"
//#include "cxPatientData.h"
#include "cxDataLocations.h"
#include "cxSelectDataStringProperty.h"
#include "cxLogicManager.h"
#include "cxProperty.h"
#include "cxDoublePropertyBase.h"
#include "cxRegistrationTransform.h"
#include "cxDoubleProperty.h"
#include "cxPatientModelService.h"
#include "cxSessionStorageService.h"
#include "cxVisServices.h"

namespace cxtest {

void setSeedPoint(cx::Vector3D point)
{
	cx::ToolPtr tool = cx::trackingService()->getManualTool();
    tool->set_prMt(cx::createTransformTranslate(point));

}

TEST_CASE("LevelSetFilter: getSeedPointFromTool", "[unit][modules][Algorithm][LevelSetFilter]")
{
	cx::LogicManager::initialize();
	cx::Vector3D toolTipPoint;
    toolTipPoint.setRandom();
	setSeedPoint(toolTipPoint);
	cx::ImagePtr image = cxtest::Utilities::create3DImage();

	cx::VisServicesPtr vs = cx::VisServices::create(cx::logicManager()->getPluginContext());
	cx::Vector3D point = cx::LevelSetFilter::getSeedPointFromTool(vs->spaceProvider(), image);
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
	cx::LevelSetFilterPtr lsf = cx::LevelSetFilterPtr(new cx::LevelSetFilter(cx::logicManager()->getPluginContext()));
	REQUIRE(lsf);
	lsf->getInputTypes();
	lsf->getOutputTypes();
	lsf->getOptions();

    //create a new patient
	QString filename = cx::DataLocations::getTestDataPath()+ "/testing/TubeSegmentationFramework/Default.mhd";
	cx::sessionStorageService()->load(cx::DataLocations::getTestDataPath()+ "/temp/LevelSetFilter/");
	QString info;
	cx::DataPtr data = cx::patientService()->importData(filename, info);
	data->get_rMd_History()->setRegistration(cx::Transform3D::Identity());
	REQUIRE(data);

	//set input
	std::vector < cx::SelectDataStringPropertyBasePtr > input =lsf->getInputTypes();
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
	std::vector < cx::SelectDataStringPropertyBasePtr > output = lsf->getOutputTypes();
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
