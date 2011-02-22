#ifndef CXCENTERLINE_H_
#define CXCENTERLINE_H_

#include "cxTimedAlgorithm.h"

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
class Centerline : public ThreadedTimedAlgorithm<vtkImageDataPtr>
{
  Q_OBJECT

public:
  Centerline();
  virtual ~Centerline();

  void setInput(ssc::ImagePtr inputImage, QString outputBasePath);
  ssc::ImagePtr getOutput();

private slots:
  virtual void postProcessingSlot();

private:
  virtual vtkImageDataPtr calculate();

  QString       mOutputBasePath;
  ssc::ImagePtr mInput;
  ssc::ImagePtr mOutput;
};

}//namespace cx
#endif /* CXCENTERLINE_H_ */
