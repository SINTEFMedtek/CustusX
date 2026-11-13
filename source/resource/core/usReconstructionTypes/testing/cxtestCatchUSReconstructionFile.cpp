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
#include "cxFileManagerServiceNull.h"
#include "cxImageDataContainer.h"
#include "cxMessageListener.h"
#include "cxReporter.h"
#include "cxUtilHelpers.h"
#include <QFile>
#include <QFileInfo>
#include <QElapsedTimer>
#include <QThread>


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

namespace
{
bool waitForText(cx::MessageListenerPtr messageListener, const QString& text)
{
	QElapsedTimer timer;
	timer.start();
	bool found = messageListener->containsText(text);
	while (!found && timer.elapsed() < 1000)
	{
		QThread::msleep(10);
		found = messageListener->containsText(text);
	}
	return found;
}
} // namespace

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Save skips images that can't be read from the recording cache", "[unit][resource][usReconstructionTypes]")
{
	cx::DataLocations::setTestMode();
	ReconstructionData input = this->createSampleReconstructData();
	std::vector<QString> missingCacheFiles;
	for (unsigned i=0; i<input.imageTimestamps.size(); ++i)
	{
		missingCacheFiles.push_back(this->getDataPath() + QString("missing_cache_%1.mhd").arg(i));
	}
	cx::FileManagerServicePtr nullFileManager(new cx::FileManagerServiceNull());
	input.imageData.reset(new cx::CachedImageDataContainer(missingCacheFiles, nullFileManager));
	cx::Reporter::initialize();
	cx::MessageListenerPtr messageListener = cx::MessageListener::createWithQueue();

	QString filename = this->write(input);

	CHECK(QFileInfo(filename).exists());
	CHECK(!this->imageFileExists(filename, input.sessionName, 0));
	CHECK(waitForText(messageListener, "these frames are not saved"));
	messageListener.reset();
	cx::Reporter::shutdown();
}

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Save images from the recording cache without the file manager", "[unit][resource][usReconstructionTypes]")
{
	cx::DataLocations::setTestMode();
	ReconstructionData input = this->createSampleReconstructData();
	QString cacheFilename = this->write(input);
	std::vector<QString> cacheFiles;
	for (unsigned i=0; i<input.imageTimestamps.size(); ++i)
	{
		cacheFiles.push_back(QString("%1/%2_%3.mhd").arg(QFileInfo(cacheFilename).absolutePath()).arg(input.sessionName).arg(i));
	}
	cx::FileManagerServicePtr nullFileManager(new cx::FileManagerServiceNull());
	input.imageData.reset(new cx::CachedImageDataContainer(cacheFiles, nullFileManager));
	input.sessionName = "saved_session";

	QString filename = this->write(input);

	CHECK(this->imageFileExists(filename, input.sessionName, 0));
	CHECK(this->imageFileExists(filename, input.sessionName, cacheFiles.size()-1));
}

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Save and load a recording with a frame missing from the recording cache", "[unit][resource][usReconstructionTypes]")
{
	cx::DataLocations::setTestMode();
	ReconstructionData input = this->createSampleReconstructData();
	QString cacheFilename = this->write(input);
	std::vector<QString> cacheFiles;
	for (unsigned i=0; i<input.imageTimestamps.size(); ++i)
	{
		cacheFiles.push_back(QString("%1/%2_%3.mhd").arg(QFileInfo(cacheFilename).absolutePath()).arg(input.sessionName).arg(i));
	}
	REQUIRE(QFile::remove(cacheFiles[3]));
	cx::FileManagerServicePtr nullFileManager(new cx::FileManagerServiceNull());
	input.imageData.reset(new cx::CachedImageDataContainer(cacheFiles, nullFileManager));
	input.sessionName = "saved_session";

	QString filename = this->write(input);
	cx::USReconstructInputData hasBeenRead = this->read(filename, nullFileManager);

	REQUIRE(hasBeenRead.mUsRaw);
	CHECK(!hasBeenRead.mFilename.isEmpty());
	REQUIRE(hasBeenRead.mFrames.size() == cacheFiles.size()-1);
	CHECK(hasBeenRead.mUsRaw->getNumImages() == cacheFiles.size()-1);
	CHECK(cx::similar(hasBeenRead.mFrames[2].mTime, input.imageTimestamps[2].getAcquisitionTime()));
	CHECK(cx::similar(hasBeenRead.mFrames[3].mTime, input.imageTimestamps[4].getAcquisitionTime()));
}

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Save and load a recording with more frame positions than images", "[unit][resource][usReconstructionTypes]")
{
	cx::DataLocations::setTestMode();
	ReconstructionData input = this->createSampleReconstructData();
	unsigned imageCount = input.imageData->size();
	input.imageTimestamps.push_back(2*imageCount);
	input.imageTimestamps.push_back(2*imageCount+2);
	cx::FileManagerServicePtr nullFileManager(new cx::FileManagerServiceNull());

	QString filename = this->write(input);
	cx::USReconstructInputData hasBeenRead = this->read(filename, nullFileManager);

	REQUIRE(hasBeenRead.mUsRaw);
	CHECK(!hasBeenRead.mFilename.isEmpty());
	REQUIRE(hasBeenRead.mFrames.size() == imageCount);
	CHECK(hasBeenRead.mUsRaw->getNumImages() == imageCount);
	CHECK(cx::similar(hasBeenRead.mFrames[0].mTime, input.imageTimestamps[0].getAcquisitionTime()));
	CHECK(cx::similar(hasBeenRead.mFrames[imageCount-1].mTime, input.imageTimestamps[imageCount-1].getAcquisitionTime()));
}

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Save only images that have a frame position", "[unit][resource][usReconstructionTypes]")
{
	cx::DataLocations::setTestMode();
	ReconstructionData input = this->createSampleReconstructData();
	cx::USReconstructInputData toBeWritten = this->createUSReconstructData(input);
	unsigned framesCount = toBeWritten.mFrames.size() / 2;
	toBeWritten.mFrames.resize(framesCount);

	QString filename = this->write(toBeWritten, input.sessionName);

	CHECK(this->imageFileExists(filename, input.sessionName, framesCount-1));
	CHECK(!this->imageFileExists(filename, input.sessionName, framesCount));
}

TEST_CASE_METHOD(cxtest::USReconstructionFileFixture, "USReconstructionFile: Save data without images", "[unit][resource][usReconstructionTypes]")
{
	cx::DataLocations::setTestMode();
	ReconstructionData input = this->createEmptyReconstructData();

	QString filename = this->write(input);

	CHECK(QFileInfo(filename).exists());
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
