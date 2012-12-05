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

#include "cxSavingVideoRecorder.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <vtkImageChangeInformation.h>
#include <vtkImageLuminance.h>
#include <vtkImageData.h>
#include "vtkImageAppend.h"
#include "vtkMetaImageWriter.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
#include "sscXmlOptionItem.h"

namespace cx
{

VideoRecorderSaveThread::VideoRecorderSaveThread(QObject* parent, QString saveFolder, QString prefix, bool compressed, bool writeColor) :
	QThread(parent),
	mSaveFolder(saveFolder),
	mPrefix(prefix),
	mImageIndex(0),
	mMutex(QMutex::Recursive),
	mStop(false),
	mTimestampsFile(saveFolder+"/"+prefix+".fts"),
	mCompressed(compressed),
	mWriteColor(writeColor)
{
}

void VideoRecorderSaveThread::addData(double timestamp, vtkImageDataPtr data)
{
	if (!data)
		return;

	vtkImageDataPtr frame = vtkImageDataPtr::New();
	frame->DeepCopy(data);

	{
		QMutexLocker sentry(&mMutex);
		mPendingData.push_back(std::make_pair(timestamp, frame));
	}
}

void VideoRecorderSaveThread::stop()
{
	mStop = true;
}

bool VideoRecorderSaveThread::openTimestampsFile()
{
	if(!mTimestampsFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
	  ssc::messageManager()->sendError("Cannot open "+mTimestampsFile.fileName());
	  return false;
	}
	return true;
}


bool VideoRecorderSaveThread::closeTimestampsFile()
{
	mTimestampsFile.close();

	QFileInfo info(mTimestampsFile);
	ssc::messageManager()->sendInfo(QString("Saved %1 timestamps to file %2")
									.arg(mImageIndex)
									.arg(info.fileName()));
	return true;
}

void VideoRecorderSaveThread::write(vtkImageDataPtr data)
{
	QString filename = QString("%1/%2_%3.mhd").arg(mSaveFolder).arg(mPrefix).arg(mImageIndex++);

	// convert to 8 bit data if applicable.
	if (!mWriteColor && data->GetNumberOfScalarComponents()>2)
	{
		  vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
		  luminance->SetInput(data);
		  data = luminance->GetOutput();
		  data->Update();
	}

	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
	writer->SetInput(data);
	writer->SetFileName(cstring_cast(filename));
	writer->SetCompression(mCompressed);
	writer->Write();
}

/** Write all pending images to file.
  *
  */
void VideoRecorderSaveThread::writeQueue()
{
	while(!mPendingData.empty())
	{
		DataType current;
		{
			QMutexLocker sentry(&mMutex);
			current = mPendingData.front();
			mPendingData.pop_front();
		}

		QTextStream stream(&mTimestampsFile);
		stream << qstring_cast(current.first);
		stream << endl;

		this->write(current.second);
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


///** Merge all us frames into one vtkImageData
// *
// */
//vtkImageDataPtr USFrameData::mergeFrames(std::vector<vtkImageDataPtr> input) const
//{
//  vtkImageAppendPtr filter = vtkImageAppendPtr::New();
//  filter->SetAppendAxis(2); // append along z-axis

//  for (unsigned i=0; i<input.size(); ++i)
//    filter->SetInput(i, input[i]);

//  filter->Update();
//  return filter->GetOutput();
//}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


SavingVideoRecorder::SavingVideoRecorder(ssc::VideoSourcePtr source, QString saveFolder, QString prefix, bool compressed, bool writeColor) :
	mSource(source)
{
	mSaveThread.reset(new VideoRecorderSaveThread(NULL, saveFolder, prefix, compressed, writeColor));
	mSaveThread->start();
}

SavingVideoRecorder::~SavingVideoRecorder()
{
	mSaveThread->stop();
}

void SavingVideoRecorder::startRecord()
{
	connect(mSource.get(), SIGNAL(newFrame()), this, SLOT(newFrameSlot()));
}

void SavingVideoRecorder::stopRecord()
{
	disconnect(mSource.get(), SIGNAL(newFrame()), this, SLOT(newFrameSlot()));
}

void SavingVideoRecorder::newFrameSlot()
{
	if (!mSource->validData())
		return;

	mSaveThread->addData(mSource->getTimestamp(), mSource->getVtkImageData());

	double timestamp = mSource->getTimestamp();

	vtkImageDataPtr frame = vtkImageDataPtr::New();
	frame->DeepCopy(mSource->getVtkImageData());
	mData[timestamp] = frame;
	frame = NULL;
}

SavingVideoRecorder::DataType SavingVideoRecorder::getRecording()
{
	return mData;
}

void SavingVideoRecorder::completeSave()
{
	mSaveThread->stop();
	mSaveThread->wait(); // wait indefinitely for thread to finish
}

} // namespace cx


