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
#include "cxFilePathProperty.h"
#include "cxFilePreviewProperty.h"

namespace cxtest
{
typedef boost::shared_ptr<class TestGenericScriptFilter> TestGenericScriptFilterPtr;

class TestGenericScriptFilter : public cx::GenericScriptFilter
{
public:
	TestGenericScriptFilter() :
		GenericScriptFilter(cx::VisServices::getNullObjects())
	{}
	void testCreateOptions()
	{
		createOptions();
	}
	bool testRunCommandString(QString command)
	{
		return runCommandStringAndWait(command);
	}

	std::vector<cx::PropertyPtr> getOptionsAdapters()
	{
		return mOptionsAdapters;
	}
	bool testCreateProcess()
	{
		return createProcess();
	}
	bool testDeleteProcess()
	{
		return deleteProcess();
	}
};

	void checkFilterInit(cx::FilterPtr filter, bool validInput = true, bool validOutput = true, bool print = false)
	{
		REQUIRE(filter);

		std::vector < cx::SelectDataStringPropertyBasePtr > input = filter->getInputTypes();
		std::vector < cx::SelectDataStringPropertyBasePtr > output = filter->getOutputTypes();
		std::vector<cx::PropertyPtr> options = filter->getOptions();
		CHECK(input.size() > 0);
		CHECK(output.size() > 0);
		CHECK(options.size() > 0);

		//if(!print)
		//	return;

		//TODO: Check if the if(print) statements below is nessecary or can be replaced by a return
		if(print) CX_LOG_DEBUG() << "input.size: " << input.size();
		for(unsigned i = 0; i < input.size(); ++i)
		{
			cx::SelectDataStringPropertyBasePtr property = input[i];
			if(print) CX_LOG_DEBUG() << property->getDisplayName() << ", " << property->getUid() << ", " << property->getValue();
			cx::DataPtr data = property->getData();
			if(validInput)
			{
				REQUIRE(data);
				if(print) CX_LOG_DEBUG() << data->getName();
			}
			else
				CHECK_FALSE(data);
		}

		if(print) CX_LOG_DEBUG() << "output.size: " << output.size();
		for(unsigned i = 0; i < output.size(); ++i)
		{
			cx::SelectDataStringPropertyBasePtr property = output[i];
			if(print) CX_LOG_DEBUG() << property->getDisplayName() << ", " << property->getUid() << ", " << property->getValue();
			cx::DataPtr data = property->getData();
			if(validOutput)
			{
				REQUIRE(data);
				if(print) CX_LOG_DEBUG() << data->getName();
			}
			else
				CHECK_FALSE(data);
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
	bool debugOutput = false;
	bool validInput = false;
	bool validOutput = false;

	cxtest::TestVisServicesPtr dummyservices = cxtest::TestVisServices::create();

	cx::GenericScriptFilterPtr filter(new cx::GenericScriptFilter(dummyservices));
	cxtest::checkFilterInit(filter, validInput, validOutput, debugOutput);

	//Filter should fail with no input
	CHECK_FALSE(filter->execute());
	CHECK_FALSE(filter->postProcess());
}

//Used cxtestDilationFilter as example. Maybe possible to combine some code?
TEST_CASE("GenericScriptFilter: Set input and execute", "[unit]")
{
	cx::LogicManager::initialize();
	cx::DataLocations::setTestMode();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());
	cxtest::TestVisServicesPtr dummyservices = cxtest::TestVisServices::create();

	cx::GenericScriptFilterPtr filter(new cx::GenericScriptFilter(dummyservices));
	cxtest::checkFilterInit(filter, false, false);
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

	cxtest::checkFilterInit(filter, true, false);

	// Execute
	{
		INFO("Preprocessing GenericScriptFilter failed.");
		REQUIRE(filter->preProcess());
	}
	{
		REQUIRE_FALSE(filter->execute());//TODO: Fix when we got valid data
	}
	{
		INFO("Post processing data from GenericScriptFilter failed.");
		//TODO: Uncomment when filter got valid output
		//REQUIRE(filter->postProcess());
		//cxtest::checkFilterInit(filter, true, false);
	}

	cx::LogicManager::shutdown();
}

TEST_CASE("GenericScriptFilter: Detailed test of option adapters", "[unit]")
{
	cxtest::TestGenericScriptFilterPtr filter(new cxtest::TestGenericScriptFilter());

	filter->testCreateOptions();
	std::vector<cx::PropertyPtr> options = filter->getOptionsAdapters();
	CHECK(options.size() == 2);
	options = filter->getOptions();
	REQUIRE(options.size() == 2);

	cx::PropertyPtr option = options[0];
	REQUIRE(option->getUid() == "scriptSelector");
	cx::FilePathPropertyPtr scriptSelectorOption = boost::dynamic_pointer_cast<cx::FilePathProperty>(option);
	REQUIRE(scriptSelectorOption);

	option = options[1];
	cx::FilePreviewPropertyPtr filePreviewOption = boost::dynamic_pointer_cast<cx::FilePreviewProperty>(option);
	REQUIRE(scriptSelectorOption);
}

TEST_CASE("GenericScriptFilter: Test running of external process", "[unit]")
{
	cxtest::TestGenericScriptFilterPtr filter(new cxtest::TestGenericScriptFilter());

	QString invalidCommand("zzz");
	REQUIRE(filter->testCreateProcess());
	REQUIRE_FALSE(filter->testRunCommandString(invalidCommand));
	REQUIRE(filter->testDeleteProcess());

	QString validCommand("date");//or echo
	REQUIRE(filter->testCreateProcess());
	REQUIRE(filter->testRunCommandString(validCommand));
	REQUIRE(filter->testDeleteProcess());
}

TEST_CASE("GenericScriptFilter: Test ProcessWrapper simple usage", "[unit]")
{
	QString command = QString("date");
	cx::ProcessWrapperPtr exe(new cx::ProcessWrapper("ScriptFilter"));
	exe->launch(command);
	REQUIRE(exe->waitForStarted());
	REQUIRE(exe->waitForFinished());
}
