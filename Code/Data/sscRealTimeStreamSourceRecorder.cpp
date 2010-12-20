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

RealTimeStreamSourceRecorder::RealTimeStreamSourceRecorder(RealTimeStreamSourcePtr source) :
    mSource(source)
{
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
  vtkImageDataPtr frame = vtkImageDataPtr::New();
  frame->DeepCopy(mSource->getVtkImageData());
  std::cout << "void RealTimeStreamSourceRecorder::newFrameSlot(): frames scalar size " << frame->GetScalarSize() << std::endl;
  std::cout << "void RealTimeStreamSourceRecorder::newFrameSlot(): mSource->getVtkImageData() scalar size " << mSource->getVtkImageData()->GetScalarSize() << std::endl;
  mData[timestamp] = frame;
}

RealTimeStreamSourceRecorder::DataType RealTimeStreamSourceRecorder::getRecording(double start, double stop)
{
  RealTimeStreamSourceRecorder::DataType retval = mData;

  retval.erase(retval.begin(), retval.lower_bound(start)); // erase all data earlier than start
  retval.erase(retval.upper_bound(stop), retval.end()); // erase all data earlier than start
  return retval;
}


} // ssc
