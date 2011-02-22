#ifndef CXCONTOUR_H_
#define CXCONTOUR_H_

#include "cxTimedAlgorithm.h"

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
class Contour : public ThreadedTimedAlgorithm<vtkPolyDataPtr>
{
  Q_OBJECT

public:
  Contour();
  virtual ~Contour();

  void setInput(ssc::ImagePtr image, QString outputBasePath, int threshold, double decimation=0.8, bool reduceResolution=false, bool smoothing=true);
  ssc::MeshPtr getOutput();

protected slots:
  virtual void postProcessingSlot();

private:
  virtual vtkPolyDataPtr calculate();

  ssc::ImagePtr mInput;
  QString       mOutputBasePath;
  int           mThreshold;
  double        mDecimation;
  bool          mUseReduceResolution;
  bool          mUseSmoothing;
  ssc::MeshPtr mOutput;
};


}//namespace cx
#endif /* CXCONTOUR_H_ */
