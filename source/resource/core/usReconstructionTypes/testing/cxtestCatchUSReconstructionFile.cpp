/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <vtkPolyData.h>

#include "cxtestUSReconstructionFileFixture.h"
#include "cxUsReconstructionFileMaker.h"
#include "cxUsReconstructionFileReader.h"
#include "cxUSFrameData.h"
#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include "cxFileManagerServiceProxy.h"
#include "cxUtilHelpers.h"
#include <QFile>
#include <QFileInfo>


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

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Save and read recording info", "[unit][resource][usReconstructionTypes]")
{
	cx::DataLocations::setTestMode();
	ReconstructionData input = this->createSampleReconstructData();

	QString filename = this->write(input);
	cx::USRecordingInfo hasBeenRead = cx::UsReconstructionFileReader::readRecordingInfo(filename);

	this->assertRecordingInfoCorrespondence(input, hasBeenRead);
	CHECK(!QFileInfo(cx::changeExtension(filename, "probe.toolmeta")).exists());
	CHECK(!QFileInfo(cx::changeExtension(filename, "ref.toolmeta")).exists());
}

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Read recording info from an older recording", "[unit][resource][usReconstructionTypes]")
{
	cx::DataLocations::setTestMode();
	QString folder = cx::UsReconstructionFileMaker::createFolder(this->getDataPath(), "old_session");
	QString filename = folder + "/old_session.fts";
	QFile probeData(cx::changeExtension(filename, "probedata.xml"));
	REQUIRE(probeData.open(QIODevice::WriteOnly));
	probeData.write("<root>\n"
					"    <configuration type=\"1\" uid=\"BK\"/>\n"
					"    <tool toolID=\"old_probe\"/>\n"
					"</root>\n");
	probeData.close();

	cx::USRecordingInfo hasBeenRead = cx::UsReconstructionFileReader::readRecordingInfo(filename);

	CHECK(hasBeenRead.mProbeOpenIGTLinkTransformId.isEmpty());
	CHECK(hasBeenRead.mProbeOpenIGTLinkImageId.isEmpty());
	CHECK(hasBeenRead.mReferenceUid.isEmpty());
	CHECK(cx::similar(hasBeenRead.mProbe_sMt, cx::Transform3D::Identity()));
	CHECK(cx::similar(hasBeenRead.rMpr, cx::Transform3D::Identity()));
	CHECK(cx::UsReconstructionFileReader::readProbeDefinitionFromFile(filename).first == "old_probe");
}
