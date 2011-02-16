#ifndef CXCONTOUR_H_
#define CXCONTOUR_H_

#include "cxTimedAlgorithm.h"

#include <QtGui>

namespace cx
{
/**
 * \class Contour
 *
 * \brief Threaded contour algorithm.
 *
 * \date Feb 16, 2011
 * \author Janne Beate Bakeng
 */
class Contour : public TimedAlgorithm
{
  Q_OBJECT

public:
  Contour();
  virtual ~Contour();

  void setInput(ssc::ImagePtr image, QString outputBasePath, int threshold, double decimation=0.8, bool reduceResolution=false, bool smoothing=true);
  ssc::MeshPtr getOutput();

protected slots:
  virtual void finishedSlot();

private:
  virtual void generate();
  vtkPolyDataPtr calculate();

  ssc::ImagePtr mInput;
  QString       mOutputBasePath;
  int           mThreshold;
  double        mDecimation;
  bool          mUseReduceResolution;
  bool          mUseSmoothing;
  ssc::MeshPtr mOutput;

  QFuture<vtkPolyDataPtr> mFutureResult;
  QFutureWatcher<vtkPolyDataPtr> mWatcher;
};

}//namespace cx
#endif /* CXCONTOUR_H_ */
