/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxtestUSReconstructionFileFixture.h"

#include "cxUsReconstructionFileMaker.h"
#include "cxUsReconstructionFileReader.h"
#include "cxUSFrameData.h"
#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include "cxFileManagerServiceProxy.h"


TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Create unique folders", "[unit][resource][usReconstructionTypes]")
{
	cx::DataLocations::setTestMode();
	QString sessionName = "test_session";

	QString uniqueFolder = cx::UsReconstructionFileMaker::createUniqueFolder(this->getDataPath(), sessionName);
	this->assertValidFolderForSession(uniqueFolder, sessionName);

	QString uniqueFolder2 = cx::UsReconstructionFileMaker::createUniqueFolder(this->getDataPath(), sessionName);
	this->assertValidFolderForSession(uniqueFolder2, sessionName);
	CHECK(uniqueFolder!=uniqueFolder2);
}

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Create folder", "[unit][resource][usReconstructionTypes]")
{
	cx::DataLocations::setTestMode();
	QString sessionName = "test_session";

	QString folder = cx::UsReconstructionFileMaker::createFolder(this->getDataPath(), sessionName);
	this->assertValidFolderForSession(folder, sessionName);

	// assert that a new call gives the same folder
	QString folder2 = cx::UsReconstructionFileMaker::createFolder(this->getDataPath(), sessionName);
	CHECK(folder==folder2);
}

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Create empty USReconstructInputData", "[unit][resource][usReconstructionTypes]")
{
	ReconstructionData input = this->createEmptyReconstructData();
	cx::USReconstructInputData output = this->createUSReconstructData(input);

	CHECK(output.mFrames.empty());
	CHECK(output.mPositions.empty());
	CHECK(!output.mUsRaw);
}

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Create sample USReconstructInputData", "[unit][resource][usReconstructionTypes]")
{
	ReconstructionData input = this->createSampleReconstructData();
	cx::USReconstructInputData output = this->createUSReconstructData(input);

	CHECK(output.mFrames.size() == input.imageTimestamps.size());
	CHECK(output.mPositions.size() == input.trackerData.size());
	CHECK(output.mUsRaw->getDimensions()[2] == input.imageData->size());
}

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Save and load USReconstructInputData", "[integration][resource][usReconstructionTypes]")
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());
	ReconstructionData input = this->createSampleReconstructData();

	QString filename = this->write(input);
	cx::USReconstructInputData hasBeenRead = this->read(filename, filemanager);

	this->assertCorrespondence(input, hasBeenRead);
	cx::LogicManager::shutdown();
}
