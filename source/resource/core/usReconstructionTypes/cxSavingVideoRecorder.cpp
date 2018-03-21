/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSavingVideoRecorder.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <vtkImageChangeInformation.h>
#include <vtkImageLuminance.h>
#include <vtkImageData.h>
#include "vtkImageAppend.h"
#include "vtkMetaImageWriter.h"

#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxSettings.h"
#include "cxXmlOptionItem.h"
#include "cxImageDataContainer.h"
#include "cxVideoSource.h"

namespace cx
{

VideoRecorderSaveThread::VideoRecorderSaveThread(QObject* parent, QString saveFolder, QString prefix, bool compressed, bool writeColor) :
	QThread(parent),
	mSaveFolder(saveFolder),
	mPrefix(prefix),
	mImageIndex(0),
	mMutex(QMutex::Recursive),
	mStop(false),
	mCancel(false),
	mTimestampsFile(saveFolder+"/"+prefix+".fts"),
	mCompressed(compressed),
	mWriteColor(writeColor)
{
	this->setObjectName("org.custusx.resource.videorecordersave"); // becomes the thread name
}

VideoRecorderSaveThread::~VideoRecorderSaveThread()
{
}

QString VideoRecorderSaveThread::addData(TimeInfo timestamp, vtkImageDataPtr image)
{
	if (!image)
		return "";

	DataType data;
	data.mTimestamp = timestamp;
	data.mImage = vtkImageDataPtr::New();
	data.mImage->DeepCopy(image);
	data.mImageFilename = QString("%1/%2_%3.mhd").arg(mSaveFolder).arg(mPrefix).arg(mImageIndex++);

	{
		QMutexLocker sentry(&mMutex);
		mPendingData.push_back(data);
	}

	return data.mImageFilename;
}

void VideoRecorderSaveThread::stop()
{
	mStop = true;
}

void VideoRecorderSaveThread::cancel()
{
	mCancel = true;
	mStop = true;
}

bool VideoRecorderSaveThread::openTimestampsFile()
{
	if(!mTimestampsFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
	  reportError("Cannot open "+mTimestampsFile.fileName());
	  return false;
	}
	return true;
}


bool VideoRecorderSaveThread::closeTimestampsFile()
{
	mTimestampsFile.close();

//	QFileInfo info(mTimestampsFile);
//	if (!mCancel)
//	{
//		report(QString("Saved %1 timestamps to file %2")
//										.arg(mImageIndex)
//										.arg(info.fileName()));
//	}
	return true;
}

void VideoRecorderSaveThread::write(VideoRecorderSaveThread::DataType data)
{
	this->writeTimeStampsFile(data.mTimestamp);

	// convert to 8 bit data if applicable.
	if (!mWriteColor && data.mImage->GetNumberOfScalarComponents()>2)
	{
		  vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
		  luminance->SetInputData(data.mImage);
		  luminance->Update();
		  data.mImage = luminance->GetOutput();
//		  data.mImage->Update();
	}

	// write image
	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
	writer->SetInputData(data.mImage);
	writer->SetFileName(cstring_cast(data.mImageFilename));
	writer->SetCompression(mCompressed);
	writer->Write();
}

void VideoRecorderSaveThread::writeTimeStampsFile(TimeInfo timeStamps)
{
	QTextStream stream(&mTimestampsFile);
	stream << qstring_cast(timeStamps.getAcquisitionTime());
	stream << endl;
}

/** Write all pending images to file.
  *
  */
void VideoRecorderSaveThread::writeQueue()
{
	while(!mPendingData.empty())
	{
		if (mCancel)
			return;

		DataType current;

		{
			QMutexLocker sentry(&mMutex);
			current = mPendingData.front();
			mPendingData.pop_front();
		}

		this->write(current);
	}
}

void VideoRecorderSaveThread::run()
{
	this->openTimestampsFile();
	while (!mStop)
	{
		this->writeQueue();
		this->msleep(20);
	}

	this->writeQueue();
	this->closeTimestampsFile();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


SavingVideoRecorder::SavingVideoRecorder(VideoSourcePtr source, QString saveFolder, QString prefix, bool compressed, bool writeColor) :
//	mLastPurgedImageIndex(-1),
	mSource(source)
{
	mImages.reset(new cx::CachedImageDataContainer());
	mImages->setDeleteFilesOnRelease(true);

	mPrefix = prefix;
	mSaveFolder = saveFolder;
	mSaveThread.reset(new VideoRecorderSaveThread(NULL, saveFolder, prefix, compressed, writeColor));
	mSaveThread->start();
}

SavingVideoRecorder::~SavingVideoRecorder()
{
	mSaveThread->cancel();
	mSaveThread->wait(); // wait indefinitely for thread to finish
}

void SavingVideoRecorder::startRecord()
{
	connect(mSource.get(), &VideoSource::newFrame, this, &SavingVideoRecorder::newFrameSlot);
}

void SavingVideoRecorder::stopRecord()
{
	disconnect(mSource.get(), &VideoSource::newFrame, this, &SavingVideoRecorder::newFrameSlot);
}

void SavingVideoRecorder::newFrameSlot()
{
	if (!mSource->validData())
		return;

	vtkImageDataPtr image = mSource->getVtkImageData();
	TimeInfo timestamp = mSource->getAdvancedTimeInfo();
	QString filename = mSaveThread->addData(timestamp, image);

	mImages->append(filename);
	mTimestamps.push_back(timestamp);
}

CachedImageDataContainerPtr SavingVideoRecorder::getImageData()
{
	return mImages;
}

std::vector<TimeInfo> SavingVideoRecorder::getTimestamps()
{
	return mTimestamps;
}

void SavingVideoRecorder::cancel()
{
	this->stopRecord();

	mSaveThread->cancel();
	mSaveThread->wait(); // wait indefinitely for thread to finish

	this->deleteFolder(mSaveFolder);
}

/** Delete folder and all contents that have been written by savers.
  */
void SavingVideoRecorder::deleteFolder(QString folder)
{
	QStringList filters;
	filters << "*.fts" << "*.mhd" << "*.raw" << "*.zraw";
	for (int i=0; i<filters.size(); ++i) // prepend prefix, ensuring files from other savers are not deleted.
		filters[i] = mPrefix + filters[i];

	QDir dir(folder);
	QStringList files = dir.entryList(filters);

	for (int i=0; i<files.size(); ++i)
		dir.remove(files[i]);
	dir.rmdir(folder);
}

void SavingVideoRecorder::completeSave()
{
	mSaveThread->stop();
	mSaveThread->wait(); // wait indefinitely for thread to finish
}

} // namespace cx


