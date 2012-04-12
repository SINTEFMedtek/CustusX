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

#include "cxPlaybackUSAcquisitionVideo.h"
#include <QStringList>
#include <QDir>
#include "vtkImageImport.h"
#include "sscTypeConversions.h"
#include "cxUsReconstructionFileReader.h"
#include "sscTestVideoSource.h"
#include "sscImageImportVideoSource.h"

namespace cx
{

USAcquisitionVideoPlayback::USAcquisitionVideoPlayback() : QObject(NULL)
{
	mVideoSource.reset(new ssc::ImageImportVideoSource("playbackVideoSource"));
//	mVideoSource.reset(new ssc::TestVideoSource("testvideosource", "testvideosource", 800,600));
	std::cout << "USAcquisitionVideoPlayback::USAcquisitionVideoPlayback() " << mVideoSource.get() << std::endl;

	mImageImport = vtkImageImport::New();
//	mImageImport->SetDataScalarTypeToUnsignedChar();
//	mImageImport->SetNumberOfScalarComponents(3);
//	mImageImport->SetWholeExtent(0, mWidth - 1, 0, mHeight - 1, 0, 0);
//	mImageImport->SetDataExtentToWholeExtent();

	mVideoSource->setImageImport(mImageImport);
}

ssc::VideoSourcePtr USAcquisitionVideoPlayback::getVideoSource()
{
	return mVideoSource;
}

void USAcquisitionVideoPlayback::setTime(PlaybackTimePtr controller)
{
	std::cout << "USAcquisitionVideoPlayback::setTime() " << std::endl;
	if (mTimer)
		disconnect(mTimer.get(), SIGNAL(changed()), this, SLOT(timerChangedSlot()));
	mTimer = controller;
	if (mTimer)
		connect(mTimer.get(), SIGNAL(changed()), this, SLOT(timerChangedSlot()));

	if (controller)
	{
		mVideoSource->start();
	}
	else
	{
		mVideoSource->stop();
	}
}

void USAcquisitionVideoPlayback::setRoot(const QString path)
{
	mRoot = path;

//	QStringList all = this->getAllFiles(mRoot);
//	std::cout << "Found paths: " << all.join("\n") << std::endl;
	mEvents = this->getEvents();
}

std::vector<TimelineEvent> USAcquisitionVideoPlayback::getEvents()
{
	std::vector<TimelineEvent> events;

	QStringList allFiles = this->getAllFiles(mRoot);
	std::cout << "Found paths: " << allFiles.join("\n") << std::endl;
	for (int i=0; i<allFiles.size(); ++i)
	{
		UsReconstructionFileReader reader;
//		ssc::USReconstructInputData data = reader.readAllFiles(allFiles[i]);
		std::vector<ssc::TimedPosition> timestamps = reader.readFrameTimestamps(allFiles[i]);

		if (timestamps.empty())
			continue;

		TimelineEvent current(
						QString("Acquisition %1").arg(QFileInfo(allFiles[i]).fileName()),
						timestamps.front().mTime,
						timestamps.back().mTime);
		current.mUid = allFiles[i];

		events.push_back(current);

	}

	return events;
}

/**Get all mhd files in folder, recursively.
 *
 * Cut and paste from ssc::FileSelectWidget .
 *
 */
QStringList USAcquisitionVideoPlayback::getAllFiles(QString folder)
{
	QStringList mNameFilters;
	mNameFilters << "*.mhd";

	QDir dir(folder);
	QStringList files = dir.entryList(mNameFilters, QDir::Files);

	QStringList retval;
	for (int i = 0; i < files.size(); ++i)
	{
		retval << (dir.absolutePath() + "/" + files[i]);
	}
	QStringList folders = dir.entryList(QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot);

	for (int i = 0; i < folders.size(); ++i)
	{
		files = this->getAllFiles(folder + "/" + folders[i]);
		retval.append(files);
	}

	return retval;
}

void USAcquisitionVideoPlayback::timerChangedSlot()
{
	TimelineEvent event;
	for (unsigned i=0; i<mEvents.size(); ++i)
	{
		if (mEvents[i].isInside(mTimer->getTime().toMSecsSinceEpoch()))
		{
			event = mEvents[i];
			break;
		}
	}


	this->loadFullData(event.mUid);

	this->updateFrame();

	//	mImageImport = vtkImageImport::New();
//		setResolution(mResolution);
	//	mImageTimer = new QTimer(this);
	//	connect(mImageTimer, SIGNAL(timeout()), this, SLOT(processBuffer()));
	//	mBuffer = (uint8_t*)malloc(width * height * 3);
	//
	//	mImageImport->SetDataScalarTypeToUnsignedChar();
	//	mImageImport->SetNumberOfScalarComponents(3);
	//	mImageImport->SetWholeExtent(0, mWidth - 1, 0, mHeight - 1, 0, 0);
	//	mImageImport->SetDataExtentToWholeExtent();


}

void USAcquisitionVideoPlayback::loadFullData(QString filename)
{
	// if same filename, ok and return
	if (filename == mCurrentFilename)
		return;

	// clear data
	mCurrentFilename = "";
	mCurrentData = ssc::USReconstructInputData();

	// if no new data, return
	if (filename.isEmpty())
		return;

	// load new data
	UsReconstructionFileReader reader;
	mCurrentData = reader.readAllFiles(filename);
	mCurrentFilename = filename;
}

void  USAcquisitionVideoPlayback::updateFrame()
{
	int index = 0; // grab first image, for starters
	double timestamp = mTimer->getTime().toMSecsSinceEpoch();
//	unsigned char* getFrame(unsigned int index);
	int* dim = mCurrentData.mUsRaw->getDimensions();
//	int* getDimensions();
	ssc::Vector3D spacing = mCurrentData.mUsRaw->getSpacing();

	mImageImport->SetDataScalarTypeToUnsignedChar();
	mImageImport->SetNumberOfScalarComponents(1);
	mImageImport->SetWholeExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, 0);
	mImageImport->SetDataExtentToWholeExtent();
	mImageImport->SetImportVoidPointer(mCurrentData.mUsRaw->getFrame(index));

	mVideoSource->refresh(timestamp);
}

} // cx

