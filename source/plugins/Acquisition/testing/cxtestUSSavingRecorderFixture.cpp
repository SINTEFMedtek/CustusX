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

#include "cxtestUSSavingRecorderFixture.h"
#include <QTimer>

#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxTestVideoSource.h"
#include "cxUsReconstructionFileReader.h"
#include "cxUSFrameData.h"
#include "cxDataLocations.h"
#include "cxTool.h"
#include "cxTime.h"
#include "catch.hpp"
#include "cxFileHelpers.h"
#include "cxReporter.h"

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
	SSC_LOG("");
	double start = QDateTime::currentMSecsSinceEpoch();
	QString uid = QDateTime::currentDateTime().toString(cx::timestampSecondsFormat());
	mSession.reset(new cx::RecordSession(uid, start, start, "session_0"));
	mRecorder->startRecord(mSession, mTool, mVideo);
}

void USSavingRecorderFixture::stopRecord()
{
	SSC_LOG("");
	mSession->setStopTime(QDateTime::currentMSecsSinceEpoch());
	mRecorder->stopRecord();
}

void USSavingRecorderFixture::wait(int time)
{
	SSC_LOG("begin");
	double stop = QDateTime::currentMSecsSinceEpoch() + time;

	while (QDateTime::currentMSecsSinceEpoch() < stop)
	{
		qApp->processEvents();
	}
	SSC_LOG("end");
}

void USSavingRecorderFixture::saveAndWaitForCompleted()
{
	SSC_LOG("begin");

	bool compressImages = true;
	QString baseFolder = this->getDataPath();
	mRecorder->startSaveData(baseFolder, compressImages);
	SSC_LOG("beginwait");

	while (mRecorder->getNumberOfSavingThreads() > 0)
	{
		qApp->processEvents();
#ifndef CX_WINDOWS
		usleep(10*1000);
#else
		Sleep(10);
#endif
	}
	SSC_LOG("endwait");
}

QString USSavingRecorderFixture::getDataPath()
{
	return cx::DataLocations::getTestDataPath() + "/temp/TestAcqCoreController/";
}

void USSavingRecorderFixture::dataSaved(QString filename)
{
	SSC_LOG("");
	mSavedData << filename;
}


void USSavingRecorderFixture::verifyMemData(QString uid)
{
	SSC_LOG("");
	cx::USReconstructInputData data = mRecorder->getDataForStream(uid);
	double duration = mSession->getStopTime() - mSession->getStartTime();
	int minFPS = 10;

	std::cout << "filename " << data.mFilename << std::endl;
	std::cout << "data.mFrames.size() " << data.mFrames.size() << std::endl;

	CHECK(!data.mFilename.isEmpty());
	CHECK(data.mFrames.size() > duration/1000*minFPS);
}

void USSavingRecorderFixture::verifySaveData()
{
	SSC_LOG("");

	CHECK( mSavedData.size() == mVideo.size() );

	for (int i=0; i<mSavedData.size(); ++i)
		this->verifySaveData(mSavedData[i]);
}

void USSavingRecorderFixture::verifySaveData(QString filename)
{
	SSC_LOG("");

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
		CHECK( hasBeenRead.mProbeData.mData.getType() == mTool->getProbe()->getProbeData().getType() );
	}
	else
	{
		CHECK( hasBeenRead.mPositions.empty() );
		CHECK( hasBeenRead.mProbeUid.isEmpty() );
	}
}

} // namespace cxtest
