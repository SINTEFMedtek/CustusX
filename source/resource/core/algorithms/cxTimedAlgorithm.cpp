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

#include "cxTimedAlgorithm.h"

//#include "itkImageFileReader.h"
#include "cxTypeConversions.h"
//#include "vtkMetaImageWriter.h"
#include "cxSettings.h"
#include "cxUtilHelpers.h"
#include "cxReporter.h"
//#include "cxImage.h"

namespace cx
{

TimedBaseAlgorithm::TimedBaseAlgorithm(QString product, int secondsBetweenAnnounce) :
    QObject(),
    mProduct(product),
    mUseDefaultMessages(true)
{
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
  mTimer->setInterval(secondsBetweenAnnounce*1000);
}

TimedBaseAlgorithm::~TimedBaseAlgorithm()
{}

void TimedBaseAlgorithm::startTiming()
{
	mStartTime = QDateTime::currentDateTime();
	if (mUseDefaultMessages)
		report(QString("Algorithm %1 started.").arg(mProduct));
	mTimer->start();
}

void TimedBaseAlgorithm::stopTiming()
{
	if (mUseDefaultMessages)
		reportSuccess(QString("Algorithm %1 complete [%2s]").arg(mProduct).arg(this->getSecondsPassedAsString()));
	//mStartTime = QDateTime(); we might need the timing after this call
	mTimer->stop();
}

QTime TimedBaseAlgorithm::getTimePassed()
{
  QTime retval = QTime(0, 0);
  retval = retval.addMSecs(mStartTime.time().msecsTo(QDateTime::currentDateTime().time()));
  return retval;
}

QString TimedBaseAlgorithm::getSecondsPassedAsString() const
{
	double secs = double(mStartTime.msecsTo(QDateTime::currentDateTime()))/1000;
	return QString("%1").arg(secs, 0, 'f', 2);
}

void TimedBaseAlgorithm::timeoutSlot()
{
  report(QString("Still executing %1, [%2] ...").arg(mProduct).arg(this->getTimePassed().toString("m:ss")));
}
//---------------------------------------------------------------------------------------------------------------------


}//namespace cx

