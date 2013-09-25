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

#include "cxtestUSReconstructionFileFixture.h"

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

#include "catch.hpp"

namespace cxtest
{

USReconstructionFileFixture::USReconstructionFileFixture()
{
	cx::removeNonemptyDirRecursively(this->getDataPath());
}

USReconstructionFileFixture::~USReconstructionFileFixture()
{
	cx::removeNonemptyDirRecursively(this->getDataPath());
}


void USReconstructionFileFixture::assertValidFolderForSession(QString path, QString sessionName)
{
	QFileInfo info(path);

	CHECK(info.exists());
	// assert path is subfolder of datapath
	CHECK(info.absoluteFilePath().contains(this->getDataPath()));
	CHECK(info.absoluteFilePath().contains(sessionName));
}


QString USReconstructionFileFixture::write(ReconstructionData input)
{
	QString path = cx::UsReconstructionFileMaker::createFolder(this->getDataPath(), input.sessionName);
	cx::USReconstructInputData toBeWritten = this->createUSReconstructData(input);

	cx::UsReconstructionFileMakerPtr fileMaker(new cx::UsReconstructionFileMaker(input.sessionName));
	fileMaker->setReconstructData(toBeWritten);
	bool compress = true;
	fileMaker->writeToNewFolder(path, compress);
	return fileMaker->getReconstructData().mFilename;
}

cx::USReconstructInputData USReconstructionFileFixture::read(QString filename)
{
	cx::UsReconstructionFileReaderPtr fileReader(new cx::UsReconstructionFileReader());
	cx::USReconstructInputData hasBeenRead = fileReader->readAllFiles(filename, "");
	return hasBeenRead;
}

void USReconstructionFileFixture::assertCorrespondence(ReconstructionData input, cx::USReconstructInputData output)
{
	CHECK( !output.mFilename.isEmpty() );
	CHECK( output.mFrames.size() == input.imageTimestamps.size() );
	CHECK( output.mFrames.size() == input.imageData->size() );
	CHECK( output.mPositions.size() == input.trackerData.size() );

	CHECK( output.mProbeUid == input.tool->getUid() );

	CHECK( output.mProbeData.mData.getType() == input.tool->getProbe()->getProbeData().getType() );
	// might add more here: compare timestamps and transforms, frame sizes, probe sector
}

USReconstructionFileFixture::ReconstructionData USReconstructionFileFixture::createEmptyReconstructData()
{
	ReconstructionData retval;
	retval.sessionName = "test_session";
	return retval;
}

USReconstructionFileFixture::ReconstructionData USReconstructionFileFixture::createSampleReconstructData()
{
	ReconstructionData retval;
	retval.sessionName = "test_session";

	retval.rMpr = cx::createTransformTranslate(cx::Vector3D(0,0,2));

	// add tracking positions spaced 1 seconds apart
	for (unsigned i=0; i<100; ++i)
		retval.trackerData[i] = cx::createTransformTranslate(cx::Vector3D(i,0,0));
	retval.writeColor = true;
	Eigen::Array2i frameSize(100, 50);
	cx::ProbeData probeData = cx::DummyToolTestUtilities::createProbeDataLinear(10, 5, frameSize);
	cx::DummyToolPtr tool = cx::DummyToolTestUtilities::createDummyTool(probeData);
	retval.tool = tool;

	// add frames spaced 2 seconds apart
	unsigned framesCount = 10;
	vtkImageDataPtr imageData = cx::generateVtkImageData(
				Eigen::Array3i(frameSize[0], frameSize[1], framesCount),
				probeData.getImage().mSpacing,
				0);
	retval.imageData.reset(new cx::SplitFramesContainer(imageData));
	for (unsigned i=0; i<framesCount; ++i)
		retval.imageTimestamps.push_back(2*i);

	return retval;
}

cx::USReconstructInputData USReconstructionFileFixture::createUSReconstructData(ReconstructionData input)
{
	cx::UsReconstructionFileMakerPtr fileMaker;
	fileMaker.reset(new cx::UsReconstructionFileMaker(input.sessionName));

	cx::USReconstructInputData reconstructData;
	reconstructData = fileMaker->getReconstructData(input.imageData, input.imageTimestamps, input.trackerData, input.tool, input.writeColor, input.rMpr);
	return reconstructData;
}

QString USReconstructionFileFixture::getDataPath()
{
	return cx::DataLocations::getTestDataPath() + "/temp/USReconstructionFileFixture/";
}

} // namespace cxtest
