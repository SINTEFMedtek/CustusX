#ifndef CXSBINARYTHRESHOLDIMAGEFILTER_H_
#define CXSBINARYTHRESHOLDIMAGEFILTER_H_

#include "cxTimedAlgorithm.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginAlgorithm
 * @{
 */

/**
 * \class BinaryThresholdImageFilter
 *
 * \brief Threaded BinaryThresholdImageFilter.
 *
 * \date Feb 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class BinaryThresholdImageFilter : public ThreadedTimedAlgorithm<vtkImageDataPtr>
{
  Q_OBJECT

public:
  BinaryThresholdImageFilter();
  virtual ~BinaryThresholdImageFilter();

  void setInput(ssc::ImagePtr image, QString outputBasePath, int threshold, bool useSmoothing=true, double smoothSigma=0.5);
  ssc::ImagePtr getOutput();

protected slots:
  virtual void postProcessingSlot();

private:
  virtual vtkImageDataPtr calculate();

  QString       mOutputBasePath;
  ssc::ImagePtr mInput;
  ssc::ImagePtr mOutput;
  int           mTheshold;
  bool          mUseSmoothing;
  double        mSmoothingSigma;
};

/**
 * @}
 */
}//namespace cx

#endif /* CXSBINARYTHRESHOLDIMAGEFILTER_H_ */
