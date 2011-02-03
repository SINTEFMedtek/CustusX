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
 *
 * The class has an autosync feature that is useful if the realtimestream clock
 * is unsynced with the master clock. The first received frame is used to estimate
 * a shift between the master and source clocks. This shift is then added to the input
 * times in getRecording in order to retrieve data using the source clock.
 */
class RealTimeStreamSourceRecorder : public QObject
{
  Q_OBJECT
public:
  typedef std::map<double, vtkImageDataPtr> DataType; ///<  <timestamp, frame>
public:
  RealTimeStreamSourceRecorder(RealTimeStreamSourcePtr source, bool sync = true);

  virtual void startRecord();
  virtual void stopRecord();
  virtual DataType getRecording(double start, double stop);

private slots:
  void newFrameSlot();
private:
  DataType mData;
  RealTimeStreamSourcePtr mSource;

  bool mSynced;
  double mSyncShift;
};

typedef boost::shared_ptr<RealTimeStreamSourceRecorder> RealTimeStreamSourceRecorderPtr;

}

#endif /* SSCREALTIMESTREAMSOURCERECORDER_H_ */
