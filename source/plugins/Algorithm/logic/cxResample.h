#ifndef CXRESAMPLE_H_
#define CXRESAMPLE_H_

#include "cxThreadedTimedAlgorithm.h"
#include <boost/function.hpp>

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginAlgorithm
 * @{
 */

/**
 * \class Resample
 *
 * \brief Threaded resampler.
 *
 * \date Feb 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class Resample : public ThreadedTimedAlgorithm<ssc::ImagePtr>
{
  Q_OBJECT

public:
  Resample();
  virtual ~Resample();

  void setInput(ssc::ImagePtr image, ssc::ImagePtr reference, QString outputBasePath, double margin);

  ssc::ImagePtr getOutput();

protected slots:
  virtual void postProcessingSlot();

private:
  virtual ssc::ImagePtr calculate();

  double mMargin;
  QString       mOutputBasePath;
  ssc::ImagePtr mInput;
  ssc::ImagePtr mInputReference;
  ssc::ImagePtr mOutput;
};

/**
 * @}
 */
}//namespace cx
#endif /* CXRESAMPLE_H_ */
