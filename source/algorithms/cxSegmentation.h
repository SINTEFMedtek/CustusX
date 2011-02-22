#ifndef CXSEGMENTATION_H_
#define CXSEGMENTATION_H_

#include "cxTimedAlgorithm.h"

namespace cx
{
/**
 * \class Segmentation
 *
 * \brief Threaded segmentation.
 *
 * \date Feb 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class Segmentation : public ThreadedTimedAlgorithm<vtkImageDataPtr>
{
  Q_OBJECT

public:
  Segmentation();
  virtual ~Segmentation();

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
}//namespace cx
#endif /* CXSEGMENTATION_H_ */
