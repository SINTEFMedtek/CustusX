/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTimedAlgorithm.h"

//#include "itkImageFileReader.h"
#include "cxTypeConversions.h"
//#include "vtkMetaImageWriter.h"
#include "cxSettings.h"
#include "cxUtilHelpers.h"
#include "cxLogger.h"
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

