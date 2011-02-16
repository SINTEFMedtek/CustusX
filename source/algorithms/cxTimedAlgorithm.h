#ifndef CXTIMEDALGORITHM_H_
#define CXTIMEDALGORITHM_H_

#include <QObject>
#include <QDateTime>
#include <QTimer>

#include <itkImage.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkBinaryThinningImageFilter3D.h>
#include "ItkVtkGlue/itkImageToVTKImageFilter.h"
#include "ItkVtkGlue/itkVTKImageToImageFilter.h"

#include <vtkImageCast.h>

#include "sscMessageManager.h"
#include "sscImage.h"

const unsigned int Dimension = 3;
typedef unsigned short PixelType;
typedef itk::Image< PixelType, Dimension >  itkImageType;
typedef itk::ImageToVTKImageFilter<itkImageType> itkToVtkFilterType;
typedef itk::VTKImageToImageFilter<itkImageType> itkVTKImageToImageFilterType;

namespace cx
{
/**
 * \class AlgorithmHelper
 *
 * \brief Class with helper functions for algorithms.
 *
 *  * \date Feb 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class AlgorithmHelper
{
public:
  static itkImageType::ConstPointer getITKfromSSCImage(ssc::ImagePtr image);
};


/**
 * \class TimedAlgorithm
 *
 * \brief Base class for algorithms that wants to time their
 * execution.
 *
 * \date Feb 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class TimedAlgorithm : public QObject
{
  Q_OBJECT

public:
  TimedAlgorithm(QString product, int secondsBetweenAnnounce);
  virtual ~TimedAlgorithm();

  virtual void generate() = 0;

signals:
  void finished();

protected:
  void startTiming();
  void stopTiming();

protected slots:
  virtual void finishedSlot() = 0;

private slots:
  void timeoutSlot();

private:
  QTime getTimePassed();

  QTimer*    mTimer;
  QDateTime mStartTime;
  QString   mProduct;
};
}//namespace
#endif /* CXTIMEDALGORITHM_H_ */
