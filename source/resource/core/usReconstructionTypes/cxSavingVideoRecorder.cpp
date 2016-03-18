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


