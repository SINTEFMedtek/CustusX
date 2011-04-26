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

  void setInput(ssc::ImagePtr image, QString outputBasePath, float lowerThreshold, float upperThreshold, int replaceValue, itkImageType::IndexType seed);
  ssc::ImagePtr getOutput();

private slots:
  virtual void postProcessingSlot();

private:
  virtual vtkImageDataPtr calculate();

  QString       mOutputBasePath;
  ssc::ImagePtr mInput;
  ssc::ImagePtr mOutput;

  float           mLowerThreshold;
  float           mUpperTheshold;
  int             mReplaceValue;
  itkImageType::IndexType mSeed;
};

}

#endif /* CXCONNECTEDTHRESHOLDIMAGEFILTER_H_ */
