/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestUSReconstructionFileFixture.h"

#include <QFileInfo>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include "cxFileHelpers.h"
#include "cxDataLocations.h"
#include "cxUsReconstructionFileMaker.h"
#include "cxUsReconstructionFileReader.h"
#include "cxTypeConversions.h"
#include "cxDummyTool.h"
#include "cxUSFrameData.h"
#include "cxVolumeHelpers.h"

#include "catch.hpp"

namespace cxtest
{

namespace
{
class RecordingTestTool : public cx::DummyTool
{
public:
	RecordingTestTool(QString uid, cx::Transform3D sMt) : cx::DummyTool(uid), m_sMt(sMt) {}
	virtual cx::Transform3D getCalibration_sMt() const { return m_sMt; }
	virtual QString getOpenIGTLinkTransformId() const { return this->getUid() + "_transform"; }
	virtual QString getOpenIGTLinkImageId() const { return this->getUid() + "_image"; }
private:
	cx::Transform3D m_sMt;
};
} // namespace

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

	REQUIRE(info.exists());
	{
		INFO(info.absoluteFilePath() + "   ( info.absoluteFilePath() )");
		INFO("contains");
//		INFO(this->getDataPath() + "   ( this->getDataPath() )");
		QFileInfo dataPath(this->getDataPath());
		INFO(dataPath.absoluteFilePath() + "   ( dataPath.absoluteFilePath() )");
		// assert path is subfolder of datapath
		CHECK(info.absoluteFilePath().contains(dataPath.absoluteFilePath()));
	}
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

cx::USReconstructInputData USReconstructionFileFixture::read(QString filename, cx::FileManagerServicePtr filemanagerservice)
{
	cx::UsReconstructionFileReaderPtr fileReader(new cx::UsReconstructionFileReader(filemanagerservice));
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
	this->assertRecordingInfoCorrespondence(input, output.mRecordingInfo);
	// might add more here: compare timestamps and transforms, frame sizes, probe sector
}

void USReconstructionFileFixture::assertRecordingInfoCorrespondence(ReconstructionData input, cx::USRecordingInfo output)
{
	CHECK( output.mProbeOpenIGTLinkTransformId == input.tool->getOpenIGTLinkTransformId() );
	CHECK( output.mProbeOpenIGTLinkImageId == input.tool->getOpenIGTLinkImageId() );
	CHECK( cx::similar(output.mProbe_sMt, input.tool->getCalibration_sMt(), 1.0E-8) );
	CHECK( output.mReferenceUid == input.reference->getUid() );
	CHECK( output.mReferenceOpenIGTLinkTransformId == input.reference->getOpenIGTLinkTransformId() );
	CHECK( cx::similar(output.rMpr, input.rMpr, 1.0E-8) );
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

	retval.rMpr = cx::createTransformRotateZ(0.3) * cx::createTransformTranslate(cx::Vector3D(0.123456789,0,2));

	// add tracking positions spaced 1 seconds apart
	for (unsigned i=0; i<100; ++i)
		retval.trackerData[i] = cx::createTransformTranslate(cx::Vector3D(i,0,0));
	retval.writeColor = true;
	Eigen::Array2i frameSize(100, 50);
	cx::ProbeDefinition probeDefinition = cx::DummyToolTestUtilities::createProbeDefinitionLinear(10, 5, frameSize);
	cx::Transform3D sMt = cx::createTransformRotateX(0.1) * cx::createTransformTranslate(cx::Vector3D(1.5,-2.25,30.0000001));
	cx::DummyToolPtr tool(new RecordingTestTool("test_probe", sMt));
	tool->setProbeSector(probeDefinition);
	tool->setVisible(true);
	retval.tool = tool;
	retval.reference.reset(new RecordingTestTool("test_reference", cx::Transform3D::Identity()));
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
													input.rMpr,
													input.reference);
	return reconstructData;
}

QString USReconstructionFileFixture::getDataPath()
{
	return cx::DataLocations::getTestDataPath() + "/temp/USReconstructionFileFixture/";
}

} // namespace cxtest
