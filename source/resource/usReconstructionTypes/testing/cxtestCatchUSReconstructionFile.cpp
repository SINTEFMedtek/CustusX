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

#include "catch.hpp"

#include "cxtestUSReconstructionFileFixture.h"

#include "cxUsReconstructionFileMaker.h"
#include "cxUsReconstructionFileReader.h"
#include "sscUSFrameData.h"


TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Create unique folders", "[unit][resource][usReconstructionTypes]")
{
	QString sessionName = "test_session";

	QString uniqueFolder = cx::UsReconstructionFileMaker::createUniqueFolder(this->getDataPath(), sessionName);
	this->assertValidFolderForSession(uniqueFolder, sessionName);

	QString uniqueFolder2 = cx::UsReconstructionFileMaker::createUniqueFolder(this->getDataPath(), sessionName);
	this->assertValidFolderForSession(uniqueFolder2, sessionName);
	CHECK(uniqueFolder!=uniqueFolder2);
}

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Create folder", "[unit][resource][usReconstructionTypes]")
{
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

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Save and load USReconstructInputData", "[unit][resource][usReconstructionTypes]")
{
	ReconstructionData input = this->createSampleReconstructData();

	QString filename = this->write(input);
	cx::USReconstructInputData hasBeenRead = this->read(filename);

	this->assertCorrespondence(input, hasBeenRead);
}
