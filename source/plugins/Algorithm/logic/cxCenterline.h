#ifndef CXCENTERLINE_H_
#define CXCENTERLINE_H_

#include "cxThreadedTimedAlgorithm.h"

#include "sscImage.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginAlgorithm
 * @{
 */

/**
 * \class Centerline
 *
 * \brief Threaded centerline algorithm.
 *
 * \date Feb 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class Centerline : public ThreadedTimedAlgorithm<vtkImageDataPtr>
{
  Q_OBJECT

public:
  Centerline();
  virtual ~Centerline();

  bool setInput(ssc::ImagePtr inputImage, QString outputBasePath);
  ssc::DataPtr getOutput();
  void setDefaultColor(QColor color);

private slots:
  virtual void postProcessingSlot();

private:
  virtual vtkImageDataPtr calculate();

  QString       mOutputBasePath;
  ssc::ImagePtr mInput;
  ssc::DataPtr mOutput;
  QColor mDefaultColor;
};
typedef boost::shared_ptr<class Centerline> CenterlinePtr;

/**
 * @}
 */
}//namespace cx
#endif /* CXCENTERLINE_H_ */
