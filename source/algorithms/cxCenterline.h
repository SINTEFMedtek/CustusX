#ifndef CXCENTERLINE_H_
#define CXCENTERLINE_H_

#include "cxTimedAlgorithm.h"

#include <QtGui>
#include "sscImage.h"

namespace cx
{
/**
 * \class Centerline
 *
 * \brief Threaded centerline algorithm.
 *
 * \date Feb 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class Centerline : public TimedAlgorithm
{
  Q_OBJECT

public:
  Centerline();
  virtual ~Centerline();

  void setInput(ssc::ImagePtr inputImage, QString outputBasePath);
  ssc::ImagePtr getOutput();

protected slots:
  virtual void finishedSlot();

private:
  virtual void generate();
  ssc::ImagePtr calculate();

  QString       mOutputBasePath;
  ssc::ImagePtr mInput;
  ssc::ImagePtr mOutput;

  QFuture<ssc::ImagePtr> mFutureResult;
  QFutureWatcher<ssc::ImagePtr > mWatcher;
};

}//namespace cx
#endif /* CXCENTERLINE_H_ */
