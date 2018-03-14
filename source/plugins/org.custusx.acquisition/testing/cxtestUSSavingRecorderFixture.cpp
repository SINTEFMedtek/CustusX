/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestUSSavingRecorderFixture.h"
#include <QTimer>
#include "cxReporter.h"

#include "cxTypeConversions.h"
#include "cxTestVideoSource.h"
#include "cxUsReconstructionFileReader.h"
#include "cxUSFrameData.h"
#include "cxDataLocations.h"
#include "cxTool.h"
#include "cxTime.h"
#include "catch.hpp"
#include "cxFileHelpers.h"


#ifdef CX_WINDOWS
#include <windows.h>
#endif

namespace cxtest
{

void USSavingRecorderFixture::setUp()
{
	cx::removeNonemptyDirRecursively(this->getDataPath());
	cx::Reporter::initialize();
}

void USSavingRecorderFixture::tearDown()
{
	cx::Reporter::shutdown();
	cx::removeNonemptyDirRecursively(this->getDataPath());
}

USSavingRecorderFixture::USSavingRecorderFixture(QObject* parent) : QObject(parent)
{
	this->setUp();

	mRecorder.reset(new cx::USSavingRecorder());
	mRecorder->setWriteColor(true);
	mRecorder->set_rMpr(cx::Transform3D::Identity());
	connect(mRecorder.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(dataSaved(QString)));

	QTimer::singleShot(0, this, SLOT(runOperations()));
}

USSavingRecorderFixture::~USSavingRecorderFixture()
{
	this->tearDown();
}

void USSavingRecorderFixture::addOperation(boost::function0<void> operation)
{
	mOperations.push_back(operation);
}

void USSavingRecorderFixture::runOperations()
{
	for (unsigned i=0; i<mOperations.size(); ++i)
	{
		mOperations[i]();
		qApp->processEvents();
	}
	QTimer::singleShot(0, qApp, SLOT(quit()));
}

void USSavingRecorderFixture::setTool(cx::ToolPtr tool)
{
	mTool = tool;
}

void USSavingRecorderFixture::addVideoSource(int width, int height)
{
	size_t index = mVideo.size();
	cx::TestVideoSourcePtr videoSource(new cx::TestVideoSource(
											QString("videoSource%1").arg(index),
											QString("Video Source %1").arg(index),
											80, 40));
	videoSource->start();
	mVideo.push_back(videoSource);
}

void USSavingRecorderFixture::startRecord()
{
//	double start = QDateTime::currentMSecsSinceEpoch();
//	QString uid = QDateTime::currentDateTime().toString(cx::timestampSecondsFormat());
	mSession.reset(new cx::RecordSession(0, "session"));
	mSession->startNewInterval();
	mRecorder->startRecord(mSession, mTool, cx::ToolPtr(), mVideo);
}

void USSavingRecorderFixture::stopRecord()
{
	mSession->stopLastInterval();
	mRecorder->stopRecord();
}

void USSavingRecorderFixture::wait(int time)
{
	double stop = QDateTime::currentMSecsSinceEpoch() + time;

	while (QDateTime::currentMSecsSinceEpoch() < stop)
	{
		qApp->processEvents();
	}
}

void USSavingRecorderFixture::saveAndWaitForCompleted()
{
	bool compressImages = true;
	QString baseFolder = this->getDataPath();
	mRecorder->startSaveData(baseFolder, compressImages);

	while (mRecorder->getNumberOfSavingThreads() > 0)
	{
		qApp->processEvents();
#ifndef CX_WINDOWS
		usleep(10*1000);
#else
		Sleep(10);
#endif
	}
}

QString USSavingRecorderFixture::getDataPath()
{
	return cx::DataLocations::getTestDataPath() + "/temp/TestAcqCoreController/";
}

void USSavingRecorderFixture::dataSaved(QString filename)
{
	mSavedData << filename;
}


void USSavingRecorderFixture::verifyMemData(QString uid)
{
	cx::USReconstructInputData data = mRecorder->getDataForStream(uid);
	REQUIRE(mSession->getIntervalCount());
	double duration = mSession->getInterval(0).first.msecsTo(mSession->getInterval(0).second);
//	double duration = mSession->getStopTime() - mSession->getStartTime();
	int minFPS = 10;

	CX_LOG_CHANNEL_INFO("test.acquisition") << "filename " << data.mFilename;
	CX_LOG_CHANNEL_INFO("test.acquisition") << "data.mFrames.size() " << data.mFrames.size();

	CHECK(!data.mFilename.isEmpty());
	CHECK(data.mFrames.size() > duration/1000*minFPS);
}

void USSavingRecorderFixture::verifySaveData()
{
	CHECK( mSavedData.size() == mVideo.size() );

	for (int i=0; i<mSavedData.size(); ++i)
		this->verifySaveData(mSavedData[i]);
}

void USSavingRecorderFixture::verifySaveData(QString filename)
{
	cx::UsReconstructionFileReaderPtr fileReader(new cx::UsReconstructionFileReader());
	cx::USReconstructInputData hasBeenRead = fileReader->readAllFiles(filename, "");

	CHECK( hasBeenRead.mFilename == filename );
	CHECK( !hasBeenRead.mFrames.empty() );
	CHECK( hasBeenRead.mUsRaw->getDimensions()[0] > 0 );
	CHECK( hasBeenRead.mUsRaw->getDimensions()[1] > 0 );
	CHECK( hasBeenRead.mUsRaw->getDimensions()[2] > 0 );

	if (mTool)
	{
		CHECK( !hasBeenRead.mPositions.empty() );
		CHECK( hasBeenRead.mProbeUid == mTool->getUid() );
		CHECK( hasBeenRead.mProbeDefinition.mData.getType() == mTool->getProbe()->getProbeDefinition().getType() );
	}
	else
	{
		CHECK( hasBeenRead.mPositions.empty() );
		CHECK( hasBeenRead.mProbeUid.isEmpty() );
	}
}

} // namespace cxtest
