/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxGenericScriptFilter.h"
#include "cxtestVisServices.h"
#include "cxProperty.h"
#include "cxLogger.h"
#include "cxSelectDataStringPropertyBase.h"
#include "cxDataLocations.h"
#include "cxtestPatientModelServiceMock.h"
#include "cxLogicManager.h"
#include "cxFileManagerServiceProxy.h"

namespace cxtest
{

	void checkFilterInit(cx::FilterPtr filter, bool print = false)
	{
		REQUIRE(filter);

		std::vector < cx::SelectDataStringPropertyBasePtr > input = filter->getInputTypes();
		std::vector < cx::SelectDataStringPropertyBasePtr > output = filter->getOutputTypes();
		std::vector<cx::PropertyPtr> options = filter->getOptions();
		CHECK(input.size() > 0);
		CHECK(output.size() > 0);
		CHECK(options.size() > 0);

		if(!print)
			return;

		//TODO: Check if the if(print) statements below is nessecary or can be replaced by a return
		if(print) CX_LOG_DEBUG() << "input.size: " << input.size();
		for(unsigned i = 0; i < input.size(); ++i)
		{
			cx::SelectDataStringPropertyBasePtr property = input[i];
			if(print) CX_LOG_DEBUG() << property->getDisplayName() << ", " << property->getUid() << ", " << property->getValue();
		}

		if(print) CX_LOG_DEBUG() << "output.size: " << output.size();
		for(unsigned i = 0; i < output.size(); ++i)
		{
			cx::SelectDataStringPropertyBasePtr property = output[i];
			if(print) CX_LOG_DEBUG() << property->getDisplayName() << ", " << property->getUid() << ", " << property->getValue();
		}

		if(print) CX_LOG_DEBUG() << "options.size: " << options.size();
		for(unsigned i = 0; i < options.size(); ++i)
		{
			cx::PropertyPtr property = options[i];
			if(print) CX_LOG_DEBUG() << property->getDisplayName() << ", " << property->getUid();// << ", " << property->getValue();
		}
	}

	cx::DataPtr getTestData(cx::PatientModelServicePtr patient, cx::FileManagerServicePtr filemanager)
	{
		QString filename = cx::DataLocations::getTestDataPath()+ "/testing/DilationFilter/helix_seg.mhd";
		QString info;
		cx::DataPtr data = boost::dynamic_pointer_cast<cxtest::PatientModelServiceMock>(patient)->importDataMock(filename, info, filemanager);
		REQUIRE(data);
		return data;
	}
}

TEST_CASE("GenericScriptFilter: Create", "[unit]")
{
	bool debugOutput = true;

	cxtest::TestVisServicesPtr dummyservices = cxtest::TestVisServices::create();

	cx::GenericScriptFilterPtr filter(new cx::GenericScriptFilter(dummyservices));
	cxtest::checkFilterInit(filter, debugOutput);
}

//Used cxtestDilationFilter as example. Maybe possible to combine some code?
TEST_CASE("GenericScriptFilter: Set input and execute", "[unit]")
{
	cx::LogicManager::initialize();
	cx::DataLocations::setTestMode();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());
	cxtest::TestVisServicesPtr dummyservices = cxtest::TestVisServices::create();

	cx::GenericScriptFilterPtr filter(new cx::GenericScriptFilter(dummyservices));
	cxtest::checkFilterInit(filter);
	cx::DataPtr data = cxtest::getTestData(dummyservices->patient(), filemanager);

	//Set input
	std::vector < cx::SelectDataStringPropertyBasePtr > input = filter->getInputTypes();
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
		INFO("Preprocessing GenericScriptFilter failed.");
		REQUIRE(filter->preProcess());
	}
	{
		REQUIRE(filter->execute());
	}
	{
		INFO("Post processing data from GenericScriptFilter failed.");
		//REQUIRE(filter->postProcess());//TODO: Uncomment and fix
	}

	cx::LogicManager::shutdown();
}
