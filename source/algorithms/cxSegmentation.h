#ifndef CXSEGMENTATION_H_
#define CXSEGMENTATION_H_

#include "cxTimedAlgorithm.h"

#include <QtGui>

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
class Segmentation : public TimedAlgorithm
{
  Q_OBJECT

public:
  Segmentation();
  virtual ~Segmentation();

  void setInput(ssc::ImagePtr image, QString outputBasePath, int threshold, bool useSmoothing=true, double smoothSigma=0.5);
  ssc::ImagePtr getOutput();

protected slots:
  virtual void finishedSlot();

private:
  virtual void generate();
  vtkImageDataPtr calculate();

  QString       mOutputBasePath;
  ssc::ImagePtr mInput;
  ssc::ImagePtr mOutput;
  int           mTheshold;
  bool          mUseSmoothing;
  double        mSmoothingSigma;

  QFuture<vtkImageDataPtr> mFutureResult;
  QFutureWatcher<vtkImageDataPtr> mWatcher;
};
}//namespace cx
#endif /* CXSEGMENTATION_H_ */
