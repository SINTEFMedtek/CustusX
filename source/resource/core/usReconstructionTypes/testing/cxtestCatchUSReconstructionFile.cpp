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

#include "cxtestUSReconstructionFileFixture.h"

#include "cxUsReconstructionFileMaker.h"
#include "cxUsReconstructionFileReader.h"
#include "cxUSFrameData.h"
#include "cxDataLocations.h"


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

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Save and load USReconstructInputData", "[unit][resource][usReconstructionTypes]")
{
	ReconstructionData input = this->createSampleReconstructData();

	QString filename = this->write(input);
	cx::USReconstructInputData hasBeenRead = this->read(filename);

	this->assertCorrespondence(input, hasBeenRead);
}
