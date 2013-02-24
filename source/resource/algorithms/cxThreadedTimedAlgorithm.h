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

#ifndef CXTHREADEDTIMEDALGORITHM_H_
#define CXTHREADEDTIMEDALGORITHM_H_

#include <QFutureWatcher>
#include <QtConcurrentRun>

#include "cxTimedAlgorithm.h"

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

namespace cx
{

/**
 * \brief Base class for algorithms that wants to thread and time their
 * execution. T is the return type of the calculated data in the thread.
 * \ingroup cxResourceAlgorithms
 *
 * \date Feb 22, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
template <class T>
class ThreadedTimedAlgorithm : public TimedBaseAlgorithm
{
public:
  ThreadedTimedAlgorithm(QString product, int secondsBetweenAnnounce) :
	  TimedBaseAlgorithm(product, secondsBetweenAnnounce)
  {
	  connect(&mWatcher, SIGNAL(finished()), this, SLOT(finishedSlot()));
	  connect(&mWatcher, SIGNAL(finished()), this, SLOT(postProcessingSlot()));
	  connect(&mWatcher, SIGNAL(finished()), this, SIGNAL(finished()));
  }
  virtual ~ThreadedTimedAlgorithm() {}

  virtual void execute()
  {
  	emit aboutToStart();
	this->preProcessingSlot();
	this->generate();
  }
  virtual bool isFinished() const { return mWatcher.isFinished(); }
  virtual bool isRunning() const { return mWatcher.isRunning(); }


protected:
  virtual void preProcessingSlot() {} ///< This happens before the thread (calculate) is started, here non-thread safe functions can be called
  virtual void postProcessingSlot() = 0; ///< This happens when the thread (calculate) is finished, here non-thread safe functions can be called

protected:
  virtual T calculate() = 0; ///< This is the threaded function, should only contain threadsafe function calls

  void generate() ///< Call generate to execute the algorithm
  {
	  TimedBaseAlgorithm::startTiming();
	  emit started(0); // TODO move to started signal from qtconcurrent??

	  mFutureResult = QtConcurrent::run(this, &ThreadedTimedAlgorithm<T>::calculate);
	  mWatcher.setFuture(mFutureResult);
  }
  T getResult() ///< This gets the result calculated in calculate, should only be used after calculate is finished
  {
	  T result = mWatcher.future().result();
	  return result;
  }

private:
  void finishedSlot()
  {
	  TimedBaseAlgorithm::stopTiming();
  }

  QFuture<T> mFutureResult;
  QFutureWatcher<T> mWatcher;
};

//---------------------------------------------------------------------------------------------------------------------
/**
 * \brief Implementation of ThreadedTimedAlgorithm that shows the minimum implementation of this class.
 * \ingroup cxResourceAlgorithms
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


#endif /* CXTHREADEDTIMEDALGORITHM_H_ */
