/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXTHREADEDTIMEDALGORITHM_H_
#define CXTHREADEDTIMEDALGORITHM_H_

#include "cxResourceExport.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "cxTimedAlgorithm.h"

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"

namespace cx
{

/**
 * \brief Base class for algorithms that wants to thread and time their
 * execution. T is the return type of the calculated data in the thread.
 * \ingroup cx_resource_core_algorithms
 *
 * \date Feb 22, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
template <class T>
class cxResource_EXPORT ThreadedTimedAlgorithm : public TimedBaseAlgorithm
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

//template specicalizations
template<>
cxResource_EXPORT void ThreadedTimedAlgorithm<void>::getResult();

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
