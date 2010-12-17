/*
 * sscRealTimeStreamSourceRecorder.h
 *
 *  Created on: Dec 17, 2010
 *      Author: christiana
 */

#ifndef SSCREALTIMESTREAMSOURCERECORDER_H_
#define SSCREALTIMESTREAMSOURCERECORDER_H_

#include "vtkSmartPointer.h"
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>
#include "sscRealTimeStreamSource.h"
#include <map>

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace ssc
{

/** Recorder for a RealTimeStreamSource.
 */
class RealTimeStreamSourceRecorder : public QObject
{
  Q_OBJECT
public:
  typedef std::map<double, vtkImageDataPtr> DataType; ///<  <timestamp, frame>
public:
  RealTimeStreamSourceRecorder(RealTimeStreamSourcePtr source);

  virtual void startRecord();
  virtual void stopRecord();
  virtual DataType getRecording(double start, double stop);

private slots:
  void newFrameSlot();
private:
  DataType mData;
  RealTimeStreamSourcePtr mSource;
};

typedef boost::shared_ptr<RealTimeStreamSourceRecorder> RealTimeStreamSourceRecorderPtr;

}

#endif /* SSCREALTIMESTREAMSOURCERECORDER_H_ */
