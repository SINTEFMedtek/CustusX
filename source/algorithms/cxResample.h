#ifndef CXRESAMPLE_H_
#define CXRESAMPLE_H_

#include "cxTimedAlgorithm.h"

#include <QtGui>

namespace cx
{
/**
 * \class Resample
 *
 * \brief Threaded resampler.
 *
 * \date Feb 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class Resample : public TimedAlgorithm
{
  Q_OBJECT

public:
  Resample();
  virtual ~Resample();

  void setInput(ssc::ImagePtr image, ssc::ImagePtr reference, QString outputBasePath, double margin);
  ssc::ImagePtr getOutput();

protected slots:
  virtual void finishedSlot();

private:
  virtual void generate();
  ssc::ImagePtr calculate();

  double mMargin;
  QString       mOutputBasePath;
  ssc::ImagePtr mInput;
  ssc::ImagePtr mInputReference;
  ssc::ImagePtr mOutput;

  QFuture<ssc::ImagePtr> mFutureResult;
  QFutureWatcher<ssc::ImagePtr > mWatcher;
};
}//namespace cx
#endif /* CXRESAMPLE_H_ */
