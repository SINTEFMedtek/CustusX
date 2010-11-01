/*
 * sscRealTimeSource.h
 *
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */

#ifndef SSCREALTIMESOURCE_H_
#define SSCREALTIMESOURCE_H_

#include "vtkSmartPointer.h"
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>
#include "vtkImageData.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace ssc
{

/**Synchronize data with source,
 * provide data as a vtkImageData.
 */
class RealTimeStreamSource : public QObject
{
  Q_OBJECT
public:
  virtual ~RealTimeStreamSource() {}
  virtual vtkImageDataPtr getVtkImageData() = 0;
  virtual QDateTime getTimestamp() = 0;
  virtual bool connected() const = 0;

signals:
  void newData();
};
typedef boost::shared_ptr<RealTimeStreamSource> RealTimeStreamSourcePtr;

} // namespace ssc


#endif /* SSCREALTIMESOURCE_H_ */
