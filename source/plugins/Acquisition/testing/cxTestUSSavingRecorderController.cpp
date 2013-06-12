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

#include "cxTestUSSavingRecorderController.h"

#include <QTimer>
#include <cppunit/extensions/HelperMacros.h>

#include "sscLogger.h"
#include "sscTypeConversions.h"
#include "sscTestVideoSource.h"
#include "cxUsReconstructionFileReader.h"
#include "sscUSFrameData.h"
#include "cxDataLocations.h"
#include "sscTool.h"
#include "sscTime.h"

TestUSSavingRecorderController::TestUSSavingRecorderController(QObject* parent) : QObject(parent)
{
	mRecorder.reset(new cx::USSavingRecorder());
	mRecorder->setWriteColor(true);
	mRecorder->set_rMpr(ssc::Transform3D::Identity());
	connect(mRecorder.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(dataSaved(QString)));

	QTimer::singleShot(0, this, SLOT(runOperations()));
}

void TestUSSavingRecorderController::addOperation(boost::function0<void> operation)
{
	mOperations.push_back(operation);
}

void TestUSSavingRecorderController::runOperations()
{
	for (unsigned i=0; i<mOperations.size(); ++i)
	{
		mOperations[i]();
		qApp->processEvents();
	}
	QTimer::singleShot(0, qApp, SLOT(quit()));
}

void TestUSSavingRecorderController::setTool(ssc::ToolPtr tool)
{
	mTool = tool;
}

void TestUSSavingRecorderController::addVideoSource(int width, int height)
{
	int index = mVideo.size();
	ssc::TestVideoSourcePtr videoSource(new ssc::TestVideoSource(
											QString("videoSource%1").arg(index),
											QString("Video Source %1").arg(index),
											80, 40));
	videoSource->start();
	mVideo.push_back(videoSource);
}

void TestUSSavingRecorderController::startRecord()
{
	SSC_LOG("");
	double start = QDateTime::currentMSecsSinceEpoch();
	QString uid = QDateTime::currentDateTime().toString(ssc::timestampSecondsFormat());
	mSession.reset(new cx::RecordSession(uid, start, start, "session_0"));
	mRecorder->startRecord(mSession, mTool, mVideo);
}

void TestUSSavingRecorderController::stopRecord()
{
	SSC_LOG("");
	mSession->setStopTime(QDateTime::currentMSecsSinceEpoch());
	mRecorder->stopRecord();
}

void TestUSSavingRecorderController::wait(int time)
{
	SSC_LOG("begin");
	double stop = QDateTime::currentMSecsSinceEpoch() + time;

	while (QDateTime::currentMSecsSinceEpoch() < stop)
	{
		qApp->processEvents();
	}
	SSC_LOG("end");
}

void TestUSSavingRecorderController::saveAndWaitForCompleted()
{
	SSC_LOG("begin");

	bool compressImages = true;
	QString baseFolder = this->getDataPath();
	mRecorder->startSaveData(baseFolder, compressImages);
	SSC_LOG("beginwait");

	while (mRecorder->getNumberOfSavingThreads() > 0)
	{
		qApp->processEvents();
		usleep(10000);
	}
	SSC_LOG("endwait");
}

QString TestUSSavingRecorderController::getDataPath()
{
	return cx::DataLocations::getTestDataPath() + "/temp/TestAcqCoreController/";
}

void TestUSSavingRecorderController::dataSaved(QString filename)
{
	SSC_LOG("");
	mSavedData << filename;
}


void TestUSSavingRecorderController::verifyMemData(QString uid)
{
	SSC_LOG("");
	ssc::USReconstructInputData data = mRecorder->getDataForStream(uid);
	double duration = mSession->getStopTime() - mSession->getStartTime();
	int minFPS = 10;

	std::cout << "filename " << data.mFilename << std::endl;
	std::cout << "data.mFrames.size() " << data.mFrames.size() << std::endl;

	CPPUNIT_ASSERT(!data.mFilename.isEmpty());
	CPPUNIT_ASSERT(data.mFrames.size() > duration/1000*minFPS);
}

void TestUSSavingRecorderController::verifySaveData()
{
	SSC_LOG("");

	CPPUNIT_ASSERT( mSavedData.size() == mVideo.size() );

	for (int i=0; i<mSavedData.size(); ++i)
		this->verifySaveData(mSavedData[i]);
}

void TestUSSavingRecorderController::verifySaveData(QString filename)
{
	SSC_LOG("");

	cx::UsReconstructionFileReaderPtr fileReader(new cx::UsReconstructionFileReader());
	ssc::USReconstructInputData hasBeenRead = fileReader->readAllFiles(filename, "");

	CPPUNIT_ASSERT( hasBeenRead.mFilename == filename );
	CPPUNIT_ASSERT( !hasBeenRead.mFrames.empty() );
	CPPUNIT_ASSERT( hasBeenRead.mUsRaw->getDimensions()[0] > 0 );
	CPPUNIT_ASSERT( hasBeenRead.mUsRaw->getDimensions()[1] > 0 );
	CPPUNIT_ASSERT( hasBeenRead.mUsRaw->getDimensions()[2] > 0 );

	if (mTool)
	{
		CPPUNIT_ASSERT( !hasBeenRead.mPositions.empty() );
		CPPUNIT_ASSERT( hasBeenRead.mProbeUid == mTool->getUid() );
		CPPUNIT_ASSERT( hasBeenRead.mProbeData.mData.getType() == mTool->getProbe()->getProbeData().getType() );
	}
	else
	{
		CPPUNIT_ASSERT( hasBeenRead.mPositions.empty() );
		CPPUNIT_ASSERT( hasBeenRead.mProbeUid.isEmpty() );
	}
}



