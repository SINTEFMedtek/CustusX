/*
 * sscRealTimeSourceStream.h
 *
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */

#ifndef SSCREALTIMESTREAMSOURCE_H_
#define SSCREALTIMESTREAMSOURCE_H_

#include "vtkSmartPointer.h"
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>
//#include "vtkImageData.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace ssc
{

/**Synchronize data with source,
 * provide data as a vtkImageData.
 *
 * states:
 *   running
 *   frozen
 *   stopped
 *   unconnected
 *   timeout
 *
 */
class RealTimeStreamSource : public QObject
{
  Q_OBJECT
public:
  virtual ~RealTimeStreamSource() {}

  virtual QString getUid() = 0;
  virtual QString getName() = 0;
  virtual vtkImageDataPtr getVtkImageData() = 0;
  virtual QDateTime getTimestamp() = 0;

  virtual QString getInfoString() const = 0; ///< an information text intended to be visible along with the data stream at all times
  virtual QString getStatusString() const = 0; ///< status text describing the stream state, display instead of stream when the stream is invalid.

  virtual void start() = 0;
  virtual void pause() = 0;
  virtual void stop()  = 0;

  virtual bool validData() const = 0; ///< return true is data stream is ok to display
  virtual bool connected() const = 0; ///< return true when a connection to the data source is established. (Use???)

//  virtual startRecord() = 0;
//  virtual vector<ImagesAndTimestamps> stopRecord() = 0;

signals:
  void changed();
};
typedef boost::shared_ptr<RealTimeStreamSource> RealTimeStreamSourcePtr;

} // namespace ssc


#endif /* SSCREALTIMESTREAMSOURCE_H_ */
