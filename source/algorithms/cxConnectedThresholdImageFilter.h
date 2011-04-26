#ifndef CXCONNECTEDTHRESHOLDIMAGEFILTER_H_
#define CXCONNECTEDTHRESHOLDIMAGEFILTER_H_

#include "cxTimedAlgorithm.h"

namespace cx
{
/**
 * ConnectedThresholdImageFilter
 *
 * \brief
 *
 * \date Apr 26, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class ConnectedThresholdImageFilter : public ThreadedTimedAlgorithm<vtkImageDataPtr>
{
  Q_OBJECT

public:
  ConnectedThresholdImageFilter();
  virtual ~ConnectedThresholdImageFilter();

private slots:
  virtual void postProcessingSlot();

private:
  virtual vtkImageDataPtr calculate();
};

}

#endif /* CXCONNECTEDTHRESHOLDIMAGEFILTER_H_ */
