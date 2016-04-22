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

#include "cxtestUSReconstructionFileFixture.h"

#include <QFileInfo>
#include "cxFileHelpers.h"
#include "cxDataLocations.h"
#include "cxUsReconstructionFileMaker.h"
#include "cxUsReconstructionFileReader.h"
#include "cxTypeConversions.h"
#include "cxDummyTool.h"
#include "cxUSFrameData.h"
#include "cxVolumeHelpers.h"
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
	CHECK( output.mProbeDefinition.mData.getUid() == input.streamUid );

	CHECK( output.mProbeDefinition.mData.getType() == input.tool->getProbe()->getProbeDefinition(input.streamUid).getType() );
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
	cx::ProbeDefinition probeDefinition = cx::DummyToolTestUtilities::createProbeDefinitionLinear(10, 5, frameSize);
	cx::DummyToolPtr tool = cx::DummyToolTestUtilities::createDummyTool(probeDefinition);
	retval.tool = tool;
	retval.streamUid = probeDefinition.getUid();

	// add frames spaced 2 seconds apart
	unsigned framesCount = 10;
	vtkImageDataPtr imageData = cx::generateVtkImageData(
				Eigen::Array3i(frameSize[0], frameSize[1], framesCount),
			probeDefinition.getSpacing(),
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
	reconstructData = fileMaker->getReconstructData(input.imageData,
													input.imageTimestamps,
													input.trackerData,
													std::map<double, cx::ToolPositionMetadata>(),
													std::map<double, cx::ToolPositionMetadata>(),
													input.tool, input.streamUid,
													input.writeColor,
													input.rMpr);
	return reconstructData;
}

QString USReconstructionFileFixture::getDataPath()
{
	return cx::DataLocations::getTestDataPath() + "/temp/USReconstructionFileFixture/";
}

} // namespace cxtest
