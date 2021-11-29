/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <QDir>
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
#include "cxImage.h"
#include "cxSelectDataStringProperty.h"
#include "cxtestUtilities.h"

namespace cxtest
{
typedef boost::shared_ptr<class TestGenericScriptFilter> TestGenericScriptFilterPtr;

class TestGenericScriptFilter : public cx::GenericScriptFilter
{
public:
	bool mGotOutput;
	TestGenericScriptFilter() :
		GenericScriptFilter(cx::VisServices::getNullObjects()),
		mGotOutput(false)
	{}
	TestGenericScriptFilter(cx::VisServicesPtr services) :
		GenericScriptFilter(services),
		mGotOutput(false)
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
	cx::ProcessWrapperPtr getProcessWrapper()
	{
		return mCommandLine;
	}
	void connectTestSlotsAndTurnOffOtherReporting()
	{
		mCommandLine->turnOffReporting();
		disconnect(mCommandLine->getProcess(), &QProcess::readyReadStandardOutput, this, &cxtest::TestGenericScriptFilter::processReadyRead);
		connect(this->getProcessWrapper()->getProcess(), &QProcess::readyReadStandardOutput, this, &cxtest::TestGenericScriptFilter::testProcessReadyRead);
	}
	bool testReadGeneratedSegmentationFile()
	{
		return readGeneratedSegmentationFiles(true, true);
	}

	void setTestScriptFile(bool useLungsFile = false)
	{
		QString configPath = cx::DataLocations::getRootConfigPath();
		//CX_LOG_DEBUG() << "config path: " << configPath;
		QString scriptFile = configPath + "/profiles/Laboratory/filter_scripts/python_test.ini";
		if(useLungsFile)
			scriptFile = configPath + "/profiles/Laboratory/filter_scripts/python_Lungs_test.ini";
		CX_LOG_DEBUG() << "Using script file: " << scriptFile;

		mScriptFile->setValueFromVariant(scriptFile);
	}
	void testSetupOutputColors(QStringList colorList)
	{
		setupOutputColors(colorList);
	}
	QList<QColor> getOutputColors()
	{
		return mOutputColors;
	}
	void addOutputClass(QString outputClass)
	{
		mOutputClasses << outputClass;
	}
	QColor testGetDefaultColor()
	{
		return getDefaultColor();
	}

	cx::ImagePtr getOutputImage()
	{
		return mOutputImage;
	}
	cx::SelectDataStringPropertyBasePtr getOutputMeshSelectMeshPtr()
	{
		return mOutputMeshSelectMeshPtr;
	}
	cx::CommandStringVariables testCreateCommandStringVariables(cx::ImagePtr input)
	{
		return createCommandStringVariables(input);
	}
	QString testStandardCommandString(cx::CommandStringVariables variables)
	{
		return standardCommandString(variables);
	}
	QString testDeepSintefCommandString(cx::CommandStringVariables variables)
	{
		return deepSintefCommandString(variables);
	}
	bool testIsUsingDeepSintefEngine(cx::CommandStringVariables variables)
	{
		return isUsingDeepSintefEngine(variables);
	}
	
	bool testEnvironmentExist(QString environmentPath)
	{
		return environmentExist(environmentPath);
	}
	bool testCreateVirtualPythonEnvironment(QString environmentPath, QString requirementsPath)
	{
		return createVirtualPythonEnvironment(environmentPath, requirementsPath);
	}

	QString getParameterFilePath()
	{
		return mScriptFile->getEmbeddedPath().getAbsoluteFilepath();
	}
	QColor testCreateColor(QStringList color)
	{
		return createColor(color);
	}
	QString testGetEnvironmentBasePath(QString environmentPath)
	{
		return getEnvironmentBasePath(environmentPath);
	}
	QString testGetEnvironmentPath(cx::CommandStringVariables variables)
	{
		return getEnvironmentPath(variables);
	}
	QString testGetFixedEnvironmentSubdir()
	{
		return getFixedEnvironmentSubdir();
	}

public slots:
	void testProcessReadyRead()
	{
		mGotOutput = true;
		QString output = QString(this->getProcessWrapper()->getProcess()->readAllStandardOutput());
		if(output.size() == 0)
			mGotOutput = false;
		CX_LOG_DEBUG() << "cxtest::ProcessReadyRead: " << output;
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

	cx::DataPtr getImportedTestData(cx::PatientModelServicePtr patient)
	{
		QString filename = cx::DataLocations::getTestDataPath()+ "/testing/DilationFilter/helix_seg.mhd";
		QString info;
		cx::DataPtr data = patient->importData(filename, info);
		REQUIRE(data);
		return data;
	}

	cx::DataPtr getImportedThoraxCT(cx::PatientModelServicePtr patient)
	{
		QString filename = cx::DataLocations::getTestDataPath()+ "/ThoraxCT/Patient_016/pat016.mhd";
		QString info;
		cx::DataPtr data = patient->importData(filename, info);
		REQUIRE(data);
		return data;
	}
} //cxtest

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
TEST_CASE("GenericScriptFilter: Set input and execute", "[unit][not_win64]")
{
	cx::LogicManager::initialize();
	cx::DataLocations::setTestMode();
	cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());

	cxtest::TestGenericScriptFilterPtr filter(new cxtest::TestGenericScriptFilter(services));
	cxtest::checkFilterInit(filter, false, false);
	cx::DataPtr data = cxtest::getImportedTestData(services->patient());

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

	filter->setTestScriptFile();

	cxtest::checkFilterInit(filter, true, false);

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
		REQUIRE(filter->postProcess());
		cxtest::checkFilterInit(filter, true, true);
	}

	cx::LogicManager::shutdown();
}

#ifdef CX_CUSTUS_SINTEF
TEST_CASE("GenericScriptFilter: Set input and execute for machine learning", "[unit][hide]")
{
	cx::LogicManager::initialize();
	cx::DataLocations::setTestMode();
	cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());

	cxtest::TestGenericScriptFilterPtr filter(new cxtest::TestGenericScriptFilter(services));
	cxtest::checkFilterInit(filter, false, false);
	cx::DataPtr data = cxtest::getImportedThoraxCT(services->patient());

	//Set input
	std::vector < cx::SelectDataStringPropertyBasePtr > input = filter->getInputTypes();
	{
		INFO("Could not set input to the filter.");
		REQUIRE(input[0]->setValue(data->getUid()));
	}
	{
		INFO("The name of the input data is not as we requested.");
		REQUIRE(input[0]->getData()->getName() == "pat016");
	}

	filter->setTestScriptFile(true);

	cxtest::checkFilterInit(filter, true, false);

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
		REQUIRE(filter->postProcess());
		cxtest::checkFilterInit(filter, true, true);
	}

	cx::LogicManager::shutdown();
}
#endif

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

TEST_CASE("GenericScriptFilter: Test running of external process", "[integration][hide]")
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

TEST_CASE("GenericScriptFilter: Test ProcessWrapper simple usage", "[unit][not_win64]")
{
	QString command = QString("date");
	cx::ProcessWrapperPtr exe(new cx::ProcessWrapper("ScriptFilter"));
	exe->launch(command);
	REQUIRE(exe->waitForStarted());
	REQUIRE(exe->waitForFinished());
}

TEST_CASE("GenericScriptFilter: Get output from process", "[integration][hide]")
{
	cxtest::TestGenericScriptFilterPtr filter(new cxtest::TestGenericScriptFilter());
	QString validCommand("echo  test");
	REQUIRE(filter->testCreateProcess());
	filter->connectTestSlotsAndTurnOffOtherReporting();
	REQUIRE(filter->testRunCommandString(validCommand));
	CHECK(filter->mGotOutput);
	REQUIRE(filter->testDeleteProcess());
}

TEST_CASE("GenericScriptFilter: Read generated file fails with no input", "[unit]")
{
	cxtest::TestGenericScriptFilterPtr filter(new cxtest::TestGenericScriptFilter());
	REQUIRE_FALSE(filter->testReadGeneratedSegmentationFile());
}

TEST_CASE("GenericScriptFilter: Read generated file", "[unit][not_win64]")
{
	cx::LogicManager::initialize();
	cx::DataLocations::setTestMode();
	cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());

	cxtest::TestGenericScriptFilterPtr filter(new cxtest::TestGenericScriptFilter(services));
	cx::DataPtr data = cxtest::getImportedTestData(services->patient());
	REQUIRE(data);

	//Set input and create input variables
	std::vector < cx::SelectDataStringPropertyBasePtr > input = filter->getInputTypes();
	REQUIRE(input[0]->setValue(data->getUid()));

	//Create output variables
	std::vector < cx::SelectDataStringPropertyBasePtr > output = filter->getOutputTypes();
	REQUIRE(output.size() > 0);

	//Create options variables
	filter->getOptions();

	filter->setTestScriptFile();//Init with test ini file

	REQUIRE(filter->preProcess());
	REQUIRE(filter->execute());

	REQUIRE(filter->testReadGeneratedSegmentationFile());
	REQUIRE(filter->getOutputImage());
	//std::cout << "Image uid: " << filter->getOutputImage()->getUid() << std::endl;
	//std::cout << "Image name: " << filter->getOutputImage()->getName() << std::endl;
	REQUIRE(filter->getOutputMeshSelectMeshPtr());
	//std::cout << "Mesh uid: " << filter->getOutputMeshSelectMeshPtr()->getValue() << std::endl;

	cx::LogicManager::shutdown();
}

TEST_CASE("GenericScriptFilter: Set output colors", "[unit]")
{
	cxtest::TestGenericScriptFilterPtr filter(new cxtest::TestGenericScriptFilter());

	QColor defaultRedColor = filter->testGetDefaultColor();

	QStringList colorListWithError("0,0,255");
	QStringList colorListWithError2("0,0,255, 0, 0");
	QStringList colorList("0,0,255,255");
	QStringList colorListWithWhitespace("0	,0 ,255 ,255");
	QStringList colorListWithTwoColors;
	colorListWithTwoColors << colorList << "0, 255, 255, 255";

	//Test without input classes
	filter->testSetupOutputColors(colorListWithError);
	REQUIRE(filter->getOutputColors().size() == 1);
	CHECK_FALSE(filter->getOutputColors()[0] != defaultRedColor);
	filter->testSetupOutputColors(colorListWithError2);
	CHECK_FALSE(filter->getOutputColors()[0] != defaultRedColor);

	filter->testSetupOutputColors(colorList);
	CHECK(filter->getOutputColors()[0] != defaultRedColor);

	filter->testSetupOutputColors(colorListWithWhitespace);
	CHECK(filter->getOutputColors()[0] != defaultRedColor);

	//Test with an input class
	filter->addOutputClass(QString("testClass"));
	filter->testSetupOutputColors(colorListWithError);
	CHECK_FALSE(filter->getOutputColors()[0] != defaultRedColor);
	filter->testSetupOutputColors(colorList);
	CHECK(filter->getOutputColors()[0] != defaultRedColor);

	filter->testSetupOutputColors(colorListWithTwoColors);
	CHECK(filter->getOutputColors().size() == 1);

	//Test with two input classes
	filter->addOutputClass(QString("testClass2"));
	filter->testSetupOutputColors(colorListWithTwoColors);
	CHECK(filter->getOutputColors().size() == 2);
}

TEST_CASE("GenericScriptFilter: Read python_Lungs_testing.ini file", "[unit]")
{
	cx::LogicManager::initialize();
	cx::DataLocations::setTestMode();
	cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());

	cxtest::TestGenericScriptFilterPtr filter(new cxtest::TestGenericScriptFilter(services));

	//Create options variables. Needed before setting script file
	filter->getOptions();
	filter->setTestScriptFile(true);//Init with python_Lungs_test.ini file

	cx::ImagePtr dummyImage = cxtest::Utilities::create3DImage();

	cx::CommandStringVariables variables = filter->testCreateCommandStringVariables(dummyImage);
	//CX_LOG_DEBUG() << "CommandStringVariables";
	//CX_LOG_DEBUG() << variables.inputFilePath;
	//CX_LOG_DEBUG() << variables.outputFilePath;
	//CX_LOG_DEBUG() << variables.envPath;
	//CX_LOG_DEBUG() << variables.scriptFilePath;
	//CX_LOG_DEBUG() << variables.cArguments;
	//CX_LOG_DEBUG() << variables.scriptEngine;
	//CX_LOG_DEBUG() << variables.model;
	
	//Assuming the variables in "python_Lungs_test.ini" won't change in the future
	REQUIRE_FALSE(variables.inputFilePath.isEmpty());
	REQUIRE_FALSE(variables.outputFilePath.isEmpty());
	REQUIRE_FALSE(variables.envPath.isEmpty());
	REQUIRE_FALSE(variables.scriptFilePath.isEmpty());
	REQUIRE(variables.cArguments.isEmpty());
	REQUIRE(variables.scriptEngine == "DeepSintef");
	REQUIRE(variables.model == "CT_Lungs");

	//CX_LOG_DEBUG() << "ParameterFilePath: " << filter->getParameterFilePath();
	REQUIRE(QFileInfo(filter->getParameterFilePath()).exists());

	cx::OutputVariables outputVariables = cx::OutputVariables(filter->getParameterFilePath());

	//CX_LOG_DEBUG() << "OutputVariables";
	//CX_LOG_DEBUG() << (outputVariables.mCreateOutputVolume);
	//CX_LOG_DEBUG() << (outputVariables.mCreateOutputMesh);
	//CX_LOG_DEBUG() << outputVariables.mOutputColorList.join(";");
	//CX_LOG_DEBUG() << outputVariables.mOutputClasses.join(";");

	//Assuming the variables in "python_Lungs_test.ini" won't change in the future
	REQUIRE(outputVariables.mCreateOutputVolume);
	REQUIRE(outputVariables.mCreateOutputMesh);

	REQUIRE(outputVariables.mOutputColorList.size() > 0);
	REQUIRE(filter->testCreateColor(outputVariables.mOutputColorList[0].split(",")) != filter->testGetDefaultColor());

	REQUIRE(outputVariables.mOutputClasses.size() > 0);
	REQUIRE(outputVariables.mOutputClasses[0] == "Lungs");


	//CX_LOG_DEBUG() << "testStandardCommandString: " << filter->testStandardCommandString(variables);
	//CX_LOG_DEBUG() << "testDeepSintefCommandString: " << filter->testDeepSintefCommandString(variables);
	REQUIRE_FALSE(filter->testStandardCommandString(variables).isEmpty());
	REQUIRE_FALSE(filter->testDeepSintefCommandString(variables).isEmpty());

	REQUIRE(filter->testIsUsingDeepSintefEngine(variables));

	cx::LogicManager::shutdown();
}

TEST_CASE("GenericScriptFilter: Test environment", "[unit][not_win64][hide]")
{
	cx::LogicManager::initialize();
	cx::DataLocations::setTestMode();
	cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());

	cxtest::TestGenericScriptFilterPtr filter(new cxtest::TestGenericScriptFilter(services));

	//Create options variables. Needed before setting script file
	filter->getOptions();
	filter->setTestScriptFile();//Use python_test.ini
	//filter->setTestScriptFile(true);//Init with python_Lungs_test.ini file

	cx::ImagePtr dummyImage = cxtest::Utilities::create3DImage();

	cx::CommandStringVariables variables = filter->testCreateCommandStringVariables(dummyImage);
	CHECK(filter->testEnvironmentExist(filter->testGetEnvironmentPath(variables)));
	
	//filter->setTestScriptFile(true);//Init with python_Lungs_test.ini file
	//variables = filter->testCreateCommandStringVariables(dummyImage);
	//CHECK(filter->testEnvironmentExist(filter->testGetEnvironmentPath(variables)));
	
	cx::LogicManager::shutdown();
}

#ifdef CX_CUSTUS_SINTEF
TEST_CASE("GenericScriptFilter: Create environment", "[integration][not_win32][not_win64][hide]")
{
	cx::LogicManager::initialize();
	cx::DataLocations::setTestMode();
	cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());

	cxtest::TestGenericScriptFilterPtr filter(new cxtest::TestGenericScriptFilter(services));

	//Create options variables. Needed before setting script file
	filter->getOptions();
	filter->setTestScriptFile(true);//Init with python_Lungs_test.ini file
	cx::ImagePtr dummyImage = cxtest::Utilities::create3DImage();
	cx::CommandStringVariables variables = filter->testCreateCommandStringVariables(dummyImage);
	
	QString requirementsPath = filter->testGetEnvironmentPath(variables);
	// Create new venv in the temptorary test folder instead of using path from ini-file.
	QString environmentPath = cx::DataLocations::getTestDataPath() + "/" + filter->testGetFixedEnvironmentSubdir();
	QString environmentBasePath = filter->testGetEnvironmentBasePath(environmentPath);
	requirementsPath = filter->testGetEnvironmentBasePath(requirementsPath);
	
	CX_LOG_DEBUG() << "Test environmentPath: " << environmentPath;
	CX_LOG_DEBUG() << "requirementsPath: " << requirementsPath;
	CX_LOG_DEBUG() << "environmentBasePath: " << environmentBasePath;
	
	requirementsPath = QFileInfo(requirementsPath).absolutePath();
	CX_LOG_DEBUG() << "Absolute requirementsPath: " << requirementsPath;
		
	REQUIRE(QFileInfo(environmentBasePath).exists());
	REQUIRE(QFileInfo(requirementsPath).exists());
	CHECK_FALSE(filter->testEnvironmentExist(environmentPath));

	CHECK(filter->testCreateVirtualPythonEnvironment(environmentPath, requirementsPath));
	CHECK(filter->testEnvironmentExist(environmentPath));
	
	QString venvPath = environmentBasePath + "venv";
	QDir dir(venvPath);
	CX_LOG_DEBUG() << "Going to delete newly created venv: " << dir.absolutePath();
	dir.removeRecursively();
	
	cx::LogicManager::shutdown();
}
#endif
