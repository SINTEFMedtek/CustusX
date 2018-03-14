/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTIMEDALGORITHM_H_
#define CXTIMEDALGORITHM_H_

#include "cxResourceExport.h"

//#include <QtWidgets>


#include <QObject>
#include <QDateTime>
#include <QTimer>
#include <boost/function.hpp>
#include <vector>
#include <iostream>
#include "boost/shared_ptr.hpp"

namespace cx
{



/**
 * \brief Base class for algorithms that wants to time their
 * execution.
 * \ingroup cx_resource_core_algorithms
 * \date Feb 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT TimedBaseAlgorithm : public QObject
{
  Q_OBJECT

public:
  TimedBaseAlgorithm(QString product, int secondsBetweenAnnounce);
  virtual ~TimedBaseAlgorithm();

  virtual QString getProduct() const { return mProduct; }
  /** Execute the algorithm.
   * Parts of the execution can be in another thread, thus the call
   * usually returns after preprocessing is complete.
   * The exec starts by emitting aboutToStart(), which can be used by
   * callers to set input arguments.
   */
  virtual void execute() = 0;
  /**
   * Returns true after the finished() signal is emitted.
   */
  virtual bool isFinished() const = 0;
  /**
   * Returns true if preprocess is started and postprocess not complete.
   * (Right after aboutToStart, right before finished())
   */
  virtual bool isRunning() const = 0;

signals:
	void aboutToStart(); ///< emitted at start of execute. Use to perform preprocessing
	void started(int maxSteps); ///< emitted at start of run. \param maxSteps is an input to a QProgressBar, set to zero if unknown.
	void finished(); ///< should be emitted when at the end of postProcessingSlot
	void productChanged(); ///< emitted whenever product string has changed
//	void progress();

protected:
  void startTiming();
  void stopTiming();
  bool mUseDefaultMessages;

  QString getSecondsPassedAsString() const;

protected slots:
  virtual void finishedSlot() {}
  virtual void postProcessingSlot() {}

private slots:
  void timeoutSlot();

private:
  QTime getTimePassed();

  QTimer*    mTimer;
  QDateTime mStartTime;
  QString   mProduct;
};
typedef boost::shared_ptr<TimedBaseAlgorithm> TimedAlgorithmPtr;

}//namespace

#endif /* CXTIMEDALGORITHM_H_ */
