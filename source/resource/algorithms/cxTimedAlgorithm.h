// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXTIMEDALGORITHM_H_
#define CXTIMEDALGORITHM_H_

//#include <QtGui>

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
 * \ingroup cxResourceAlgorithms
 *
 * \date Feb 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class TimedBaseAlgorithm : public QObject
{
  Q_OBJECT

public:
  TimedBaseAlgorithm(QString product, int secondsBetweenAnnounce);
  virtual ~TimedBaseAlgorithm();

  virtual QString getProduct() const { return mProduct; }
  /**Execute the algorithm.
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
	void started(int maxSteps); /// < emitted at start of run. \param maxSteps is an input to a QProgressBar, set to zero if unknown.
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
