/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <ctkPluginContext.h>
#include "cxDilationFilter.h"
#include "cxDataLocations.h"
#include "cxSelectDataStringProperty.h"
#include "cxData.h"
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxSessionStorageService.h"
#include "cxVisServices.h"
#include "cxtestVisServices.h"
#include "cxLogicManager.h"
#include "cxFileManagerServiceProxy.h"
#include "cxtestPatientModelServiceMock.h"

TEST_CASE("DilationFilter: execute", "[unit][modules][Algorithm][DilationFilter]")
{
	cx::LogicManager::initialize();
	cx::DataLocations::setTestMode();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	{
		cxtest::TestVisServicesPtr dummyservices = cxtest::TestVisServices::create();

		// Setup filter
		cx::DilationFilterPtr filter(new cx::DilationFilter(dummyservices));
		REQUIRE(filter);
		filter->getInputTypes();
		filter->getOutputTypes();
		filter->getOptions();

		QString filename = cx::DataLocations::getTestDataPath()+ "/testing/DilationFilter/helix_seg.mhd";
		QString info;
		cx::DataPtr data = boost::dynamic_pointer_cast<cxtest::PatientModelServiceMock>(dummyservices->patient())->importDataMock(filename, info, filemanager);
		REQUIRE(data);

		//set input
		std::vector < cx::SelectDataStringPropertyBasePtr > input =filter->getInputTypes();
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
            INFO("Preprocessing DilationFilter failed.");
			REQUIRE(filter->preProcess());
		}

		{
			REQUIRE(filter->execute());
		}
		{
            INFO("Post processing data from Dilation Filter failed.");
			REQUIRE(filter->postProcess());
		}

		// Check output
		std::vector < cx::SelectDataStringPropertyBasePtr > output = filter->getOutputTypes();
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
	cx::LogicManager::shutdown();
}
