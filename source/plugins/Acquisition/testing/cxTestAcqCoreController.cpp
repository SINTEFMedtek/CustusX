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

#include "cxTestAcqCoreController.h"

#include <QTimer>
#include <cppunit/extensions/HelperMacros.h>

#include "sscLogger.h"
#include "sscTypeConversions.h"
#include "sscTestVideoSource.h"
#include "cxUsReconstructionFileReader.h"
#include "sscUSFrameData.h"

TestAcqCoreController::TestAcqCoreController(QObject* parent) : QObject(parent)
{
	mAcquisition.reset(new cx::USAcquisitionCore());
	connect(mAcquisition.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(dataSaved(QString)));

	QTimer::singleShot(0, this, SLOT(runOperations()));
}

void TestAcqCoreController::addOperation(boost::function0<void> operation)
{
	mOperations.push_back(operation);
}

void TestAcqCoreController::runOperations()
{
	for (unsigned i=0; i<mOperations.size(); ++i)
	{
		mOperations[i]();
		qApp->processEvents();
	}
	QTimer::singleShot(0, qApp, SLOT(quit()));
}

void TestAcqCoreController::setTool(ssc::ToolPtr tool)
{
	mTool = tool;
}

void TestAcqCoreController::addVideoSource(int width, int height)
{
	int index = mVideo.size();
	ssc::TestVideoSourcePtr videoSource(new ssc::TestVideoSource(
											QString("videoSource%1").arg(index),
											QString("Video Source %1").arg(index),
											80, 40));
	mVideo.push_back(videoSource);
}

void TestAcqCoreController::startRecord()
{
	SSC_LOG("");
	double start = QDateTime::currentMSecsSinceEpoch();
	mSession.reset(new cx::RecordSession("session0", start, start, "session 0"));
	mAcquisition->startRecord(mSession, mTool, mVideo);
}

void TestAcqCoreController::stopRecord()
{
	SSC_LOG("");
	mSession->setStopTime(QDateTime::currentMSecsSinceEpoch());
	mAcquisition->stopRecord();
}

void TestAcqCoreController::wait(int time)
{
	SSC_LOG("");
	double stop = QDateTime::currentMSecsSinceEpoch() + time;

	while (QDateTime::currentMSecsSinceEpoch() < stop)
	{
		qApp->processEvents();
	}
	SSC_LOG("");
}

void TestAcqCoreController::saveAndWaitForCompleted()
{
	SSC_LOG("");
	bool compressImages = true;
	bool writeColor = true;
	mAcquisition->startSaveData(compressImages, writeColor);

	while (mAcquisition->numberOfSavingThreads() > 0)
	{
		qApp->processEvents();
	}
}

void TestAcqCoreController::dataSaved(QString filename)
{
	SSC_LOG("");
	mSavedData << filename;
	std::cout << "saved data " << filename << std::endl;
}


void TestAcqCoreController::verifyMemData(QString uid)
{
	SSC_LOG("");
	ssc::USReconstructInputData data = mAcquisition->getDataForStream(uid);
	double duration = mSession->getStopTime() - mSession->getStartTime();
	int minFPS = 10;

	CPPUNIT_ASSERT(!data.mFilename.isEmpty());
	CPPUNIT_ASSERT(data.mFrames.size() > duration/1000*minFPS);
}

void TestAcqCoreController::verifySaveData()
{
	SSC_LOG("");

	CPPUNIT_ASSERT( mSavedData.size() == mVideo.size() );

	for (int i=0; i<mSavedData.size(); ++i)
		this->verifySaveData(mSavedData[i]);
}

void TestAcqCoreController::verifySaveData(QString filename)
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
		CPPUNIT_ASSERT( hasBeenRead.mProbeData.mData.getType() == mTool->getProbe()->getData().getType() );
	}
	else
	{
		CPPUNIT_ASSERT( hasBeenRead.mPositions.empty() );
		CPPUNIT_ASSERT( hasBeenRead.mProbeUid.isEmpty() );
	}
}



