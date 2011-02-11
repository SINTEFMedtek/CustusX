/*
 * sscRealTimeStreamSourceRecorder.cpp
 *
 *  Created on: Dec 17, 2010
 *      Author: christiana
 */

#include "sscRealTimeStreamSourceRecorder.h"
#include "vtkImageData.h"
#include "sscTime.h"

namespace ssc
{

RealTimeStreamSourceRecorder::RealTimeStreamSourceRecorder(RealTimeStreamSourcePtr source, bool sync) :
    mSource(source)
{
  mSynced = !sync;
  mSyncShift = 0;
}

void RealTimeStreamSourceRecorder::startRecord()
{
  connect(mSource.get(), SIGNAL(newFrame()), this, SLOT(newFrameSlot()));
}

void RealTimeStreamSourceRecorder::stopRecord()
{
  disconnect(mSource.get(), SIGNAL(newFrame()), this, SLOT(newFrameSlot()));
}

void RealTimeStreamSourceRecorder::newFrameSlot()
{
  double timestamp = mSource->getTimestamp();

  if (!mSynced)
  {
    mSyncShift = ssc::getMilliSecondsSinceEpoch() - timestamp;
    std::cout << "RealTimeStreamSourceRecorder SyncShift: " << mSyncShift << std::endl;
    mSynced = true;
  }

//  double diff = 0;
//  if (!mData.empty())
//    diff = timestamp - mData.rbegin()->first;
//  std::cout << "timestamp " << timestamp << ", " << diff << std::endl;

  vtkImageDataPtr frame = vtkImageDataPtr::New();
  frame->DeepCopy(mSource->getVtkImageData());
  mData[timestamp] = frame;
}

RealTimeStreamSourceRecorder::DataType RealTimeStreamSourceRecorder::getRecording(double start, double stop)
{
  start -= mSyncShift;
  stop -= mSyncShift;
  RealTimeStreamSourceRecorder::DataType retval = mData;

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
