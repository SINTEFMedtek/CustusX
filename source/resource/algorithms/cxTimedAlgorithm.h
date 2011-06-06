#ifndef CXTIMEDALGORITHM_H_
#define CXTIMEDALGORITHM_H_

#include <QtGui>

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

private:
  static itkImageType::ConstPointer getITKfromSSCImageViaFile(ssc::ImagePtr image);
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
  void finished(); ///< should be emitted when at the end of postProcessingSlot

protected:
  void startTiming();
  void stopTiming();

protected slots:
  virtual void finishedSlot() = 0;
  virtual void postProcessingSlot() = 0;

private slots:
  void timeoutSlot();

private:
  QTime getTimePassed();

  QTimer*    mTimer;
  QDateTime mStartTime;
  QString   mProduct;
};

/**
 * \class ThreadedTimedAlgorithm
 *
 * \brief Base class for algorithms that wants to thread and time their
 * execution. T is the return type of the calculated data in the thread.
 *
 * \date Feb 22, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
template <class T>
class ThreadedTimedAlgorithm : public TimedAlgorithm
{
public:
  ThreadedTimedAlgorithm(QString product, int secondsBetweenAnnounce);
  virtual ~ThreadedTimedAlgorithm();

protected:
  virtual void postProcessingSlot() = 0; ///< This happens when the thread (calculate) is finished, here non-thread safe functions can be called

protected:
  virtual T calculate() = 0; ///< This is the threaded function, should only contain threadsafe function calls

  void generate(); ///< Call generate to execute the algorithm
  T getResult(); ///< This gets the result calculated in calculate, should only be used after calculate is finished

private:
  void finishedSlot();

  QFuture<T> mFutureResult;
  QFutureWatcher<T> mWatcher;
};

//---------------------------------------------------------------------------------------------------------------------
/**
 * \class Example
 *
 * \brief Implementation of ThreadedTimedAlgorithm that shows the minimum implementation of this class.
 *
 * \date Feb 22, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class Example : public ThreadedTimedAlgorithm<QString>
{
  Q_OBJECT
public:
  Example();
  virtual ~Example();

private slots:
  virtual void postProcessingSlot();

private:
  virtual QString calculate();
};
}//namespace
#endif /* CXTIMEDALGORITHM_H_ */
