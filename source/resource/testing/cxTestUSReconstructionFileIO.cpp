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

#include "cxTestUSReconstructionFileIO.h"
#include <QFileInfo>
#include "cxFileHelpers.h"
#include "cxDataLocations.h"
#include "cxUsReconstructionFileMaker.h"
#include "cxUsReconstructionFileReader.h"
#include "sscTypeConversions.h"
#include "sscDummyTool.h"
#include "sscUSFrameData.h"
#include "sscVolumeHelpers.h"
#include "vtkImageData.h"

void TestUSReconstructionFileIO::setUp()
{
	cx::removeNonemptyDirRecursively(this->getDataPath());
}

void TestUSReconstructionFileIO::tearDown()
{
	cx::removeNonemptyDirRecursively(this->getDataPath());
}

void TestUSReconstructionFileIO::testCreateUniqueFolders()
{
	QString sessionName = "test_session";

	QString uniqueFolder = cx::UsReconstructionFileMaker::createUniqueFolder(this->getDataPath(), sessionName);
	this->assertValidFolderForSession(uniqueFolder, sessionName);

	QString uniqueFolder2 = cx::UsReconstructionFileMaker::createUniqueFolder(this->getDataPath(), sessionName);
	this->assertValidFolderForSession(uniqueFolder2, sessionName);
	CPPUNIT_ASSERT(uniqueFolder!=uniqueFolder2);
}

void TestUSReconstructionFileIO::testCreateFolder()
{
	QString sessionName = "test_session";

	QString folder = cx::UsReconstructionFileMaker::createFolder(this->getDataPath(), sessionName);
	this->assertValidFolderForSession(folder, sessionName);

	// assert that a new call gives the same folder
	QString folder2 = cx::UsReconstructionFileMaker::createFolder(this->getDataPath(), sessionName);
	CPPUNIT_ASSERT(folder==folder2);
}

void TestUSReconstructionFileIO::assertValidFolderForSession(QString path, QString sessionName)
{
	QFileInfo info(path);

	CPPUNIT_ASSERT(info.exists());
	// assert path is subfolder of datapath
	CPPUNIT_ASSERT(info.absoluteFilePath().contains(this->getDataPath()));
	CPPUNIT_ASSERT(info.absoluteFilePath().contains(sessionName));
}

void TestUSReconstructionFileIO::testCreateEmptyUSReconstructInputData()
{
	ReconstructionData input = this->createEmptyReconstructData();
	ssc::USReconstructInputData output = this->createUSReconstructData(input);

	CPPUNIT_ASSERT(output.mFrames.empty());
	CPPUNIT_ASSERT(output.mPositions.empty());
	CPPUNIT_ASSERT(!output.mUsRaw);
}

void TestUSReconstructionFileIO::testCreateSampleUSReconstructInputData()
{
	ReconstructionData input = this->createSampleReconstructData();
	ssc::USReconstructInputData output = this->createUSReconstructData(input);

	CPPUNIT_ASSERT(output.mFrames.size() == input.imageTimestamps.size());
	CPPUNIT_ASSERT(output.mPositions.size() == input.trackerData.size());
	CPPUNIT_ASSERT(output.mUsRaw->getDimensions()[2] == input.imageData->size());
}

void TestUSReconstructionFileIO::testSaveAndLoadUSReconstructInputData()
{
	ReconstructionData input = this->createSampleReconstructData();

	QString filename = this->write(input);
	ssc::USReconstructInputData hasBeenRead = this->read(filename);

	this->assertCorrespondence(input, hasBeenRead);
}

QString TestUSReconstructionFileIO::write(ReconstructionData input)
{
	QString path = cx::UsReconstructionFileMaker::createFolder(this->getDataPath(), input.sessionName);
	ssc::USReconstructInputData toBeWritten = this->createUSReconstructData(input);

	cx::UsReconstructionFileMakerPtr fileMaker(new cx::UsReconstructionFileMaker(input.sessionName));
	fileMaker->setReconstructData(toBeWritten);
	bool compress = true;
	fileMaker->writeToNewFolder(path, compress);
	return fileMaker->getReconstructData().mFilename;
}

ssc::USReconstructInputData TestUSReconstructionFileIO::read(QString filename)
{
	cx::UsReconstructionFileReaderPtr fileReader(new cx::UsReconstructionFileReader());
	ssc::USReconstructInputData hasBeenRead = fileReader->readAllFiles(filename, "");
	return hasBeenRead;
}

void TestUSReconstructionFileIO::assertCorrespondence(ReconstructionData input, ssc::USReconstructInputData output)
{
	CPPUNIT_ASSERT( !output.mFilename.isEmpty() );
	CPPUNIT_ASSERT( output.mFrames.size() == input.imageTimestamps.size() );
	CPPUNIT_ASSERT( output.mFrames.size() == input.imageData->size() );
	CPPUNIT_ASSERT( output.mPositions.size() == input.trackerData.size() );

	CPPUNIT_ASSERT( output.mProbeUid == input.tool->getUid() );

	CPPUNIT_ASSERT( output.mProbeData.mData.getType() == input.tool->getProbe()->getData().getType() );
	// might add more here: compare timestamps and transforms, frame sizes, probe sector
}

TestUSReconstructionFileIO::ReconstructionData TestUSReconstructionFileIO::createEmptyReconstructData()
{
	ReconstructionData retval;
	retval.sessionName = "test_session";
	return retval;
}

TestUSReconstructionFileIO::ReconstructionData TestUSReconstructionFileIO::createSampleReconstructData()
{
	ReconstructionData retval;
	retval.sessionName = "test_session";

	retval.rMpr = ssc::createTransformTranslate(ssc::Vector3D(0,0,2));

	// add tracking positions spaced 1 seconds apart
	for (unsigned i=0; i<100; ++i)
		retval.trackerData[i] = ssc::createTransformTranslate(ssc::Vector3D(i,0,0));
	retval.writeColor = true;
	Eigen::Array2i frameSize(100, 50);
	ssc::ProbeData probeData = ssc::DummyToolTestUtilities::createProbeDataLinear(10, 5, frameSize);
	ssc::DummyToolPtr tool = ssc::DummyToolTestUtilities::createDummyTool(probeData);
	retval.tool = tool;

	// add frames spaced 2 seconds apart
	unsigned framesCount = 10;
	vtkImageDataPtr imageData = ssc::generateVtkImageData(
	            Eigen::Array3i(frameSize[0], frameSize[1], framesCount),
	            probeData.getImage().mSpacing,
	            0);
	retval.imageData.reset(new cx::SplitFramesContainer(imageData));
	for (unsigned i=0; i<framesCount; ++i)
		retval.imageTimestamps.push_back(2*i);

	return retval;
}

ssc::USReconstructInputData TestUSReconstructionFileIO::createUSReconstructData(ReconstructionData input)
{
	cx::UsReconstructionFileMakerPtr fileMaker;
	fileMaker.reset(new cx::UsReconstructionFileMaker(input.sessionName));

	ssc::USReconstructInputData reconstructData;
	reconstructData = fileMaker->getReconstructData(input.imageData, input.imageTimestamps, input.trackerData, input.tool, input.writeColor, input.rMpr);
	return reconstructData;
}

QString TestUSReconstructionFileIO::getDataPath()
{
	return cx::DataLocations::getTestDataPath() + "/temp/TestUSReconstructionFileIO/";
}
