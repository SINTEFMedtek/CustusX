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
  }

  vtkImageDataPtr frame = vtkImageDataPtr::New();
  frame->DeepCopy(mSource->getVtkImageData());
  mData[timestamp] = frame;
}

RealTimeStreamSourceRecorder::DataType RealTimeStreamSourceRecorder::getRecording(double start, double stop)
{
  start -= mSyncShift;
  stop -= mSyncShift;

  RealTimeStreamSourceRecorder::DataType retval = mData;

  retval.erase(retval.begin(), retval.lower_bound(start)); // erase all data earlier than start
  retval.erase(retval.upper_bound(stop), retval.end()); // erase all data earlier than start
  return retval;
}


} // ssc
