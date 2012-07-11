#ifndef CXCONTOUR_H_
#define CXCONTOUR_H_

#include "cxThreadedTimedAlgorithm.h"
#include "cxThreadedTimedAlgorithm.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginAlgorithm
 * @{
 */

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

  void setInput(ssc::ImagePtr image, QString outputBasePath, int threshold, double decimation = 0.8,
			bool reduceResolution = false, bool smoothing = true, bool preserveTopology = true);
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
  bool					mPreserveTopology;
  ssc::MeshPtr mOutput;
};
typedef boost::shared_ptr<class Contour> ContourPtr;



/**
 * @}
 */
}//namespace cx
#endif /* CXCONTOUR_H_ */
