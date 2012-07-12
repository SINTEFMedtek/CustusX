// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * sscVideoRecorder.cpp
 *
 *  Created on: Dec 17, 2010
 *      Author: christiana
 */

#include "sscVideoRecorder.h"
#include "vtkImageData.h"
#include "sscTime.h"
#include "sscMessageManager.h"

namespace ssc
{

VideoRecorder::VideoRecorder(VideoSourcePtr source, bool sync) :
	mSource(source)
{
	mSynced = !sync;
	mSyncShift = 0;
}

VideoRecorder::~VideoRecorder()
{
//	std::cout << "deleting VideoRecorder " << mData.size() << std::endl;
//
//  for (DataType::iterator iter=mData.begin(); iter!=mData.end(); ++iter)
//  {
//  	if (iter->second->GetReferenceCount()>1)
//  	{
//  		std::cout << "WARNING found ref count " << iter->second->GetReferenceCount() << std::endl;
//  	}
//  }
}

void VideoRecorder::startRecord()
{
	connect(mSource.get(), SIGNAL(newFrame()), this, SLOT(newFrameSlot()));
}

void VideoRecorder::stopRecord()
{
	disconnect(mSource.get(), SIGNAL(newFrame()), this, SLOT(newFrameSlot()));
}

void VideoRecorder::newFrameSlot()
{
	if (!mSource->validData())
		return;

	double timestamp = mSource->getTimestamp();

	if (!mSynced)
	{
		mSyncShift = ssc::getMilliSecondsSinceEpoch() - timestamp;
		ssc::messageManager()->sendInfo(QString("VideoRecorder found and corrected sync shift: %1 s").arg(double(mSyncShift)/1000, 6, 'f', 3));
//    std::cout << "RealTimeStreamSourceRecorder SyncShift: " << mSyncShift << std::endl;
		mSynced = true;
	}

//  double diff = 0;
//  if (!mData.empty())
//    diff = timestamp - mData.rbegin()->first;
//  std::cout << "timestamp " << timestamp << ", " << diff << std::endl;

	vtkImageDataPtr frame = vtkImageDataPtr::New();
//  std::cout << " RC after construct " << frame->GetReferenceCount() << std::endl;
	frame->DeepCopy(mSource->getVtkImageData());
//  std::cout << " RC after fill " << frame->GetReferenceCount() << std::endl;
	mData[timestamp] = frame;
//  std::cout << " RC after assign " << frame->GetReferenceCount() << std::endl;
//  if (frame->GetReferenceCount()>2)
//  	frame->SetReferenceCount(2);
//  std::cout << " RC after explicit set " << frame->GetReferenceCount() << std::endl;
//  mData.clear();
//  std::cout << " RC after clear map " << frame->GetReferenceCount() << std::endl;
	frame = NULL;
//  std::cout << " RC after construct+clear temp " << mData.find(timestamp)->second->GetReferenceCount() << std::endl;
//  std::cout << "============================" << std::endl << std::endl;
}

VideoRecorder::DataType VideoRecorder::getRecording(double start, double stop)
{
	start -= mSyncShift;
	stop -= mSyncShift;
	VideoRecorder::DataType retval = mData;

//  std::cout << std::endl;
//  RealTimeStreamSourceRecorder::DataType::iterator iter;
//  for (iter=retval.begin(); iter!=retval.end(); ++iter)
//  {
//    std::cout << "timestamp pre: " << (iter->first - start) << std::endl;
//  }

	retval.erase(retval.begin(), retval.lower_bound(start)); // erase all data earlier than start
	retval.erase(retval.upper_bound(stop), retval.end()); // erase all data earlier than start

//  std::cout << std::endl;

//  for (iter=retval.begin(); iter!=retval.end(); ++iter)
//  {
//    std::cout << "timestamp post: " << (iter->first - start) << std::endl;
//  }
//  std::cout << std::endl;
//  std::cout << "recording " << retval.size() << "/" << mData.size() << "   start/stop " << start << ", " << stop << std::endl;
//  std::cout << "length " << stop - start << std::endl;
	return retval;
}


} // ssc
