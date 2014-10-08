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
