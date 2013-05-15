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

#include "cxTimedAlgorithm.h"

//#include "itkImageFileReader.h"
#include "sscTypeConversions.h"
//#include "vtkMetaImageWriter.h"
#include "cxSettings.h"
#include "sscUtilHelpers.h"
#include "sscMessageManager.h"
//#include "sscImage.h"

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
		ssc::messageManager()->sendInfo(QString("Algorithm %1 started.").arg(mProduct));
	mTimer->start();
}

void TimedBaseAlgorithm::stopTiming()
{
	if (mUseDefaultMessages)
		ssc::messageManager()->sendSuccess(QString("Algorithm %1 complete [%2s]").arg(mProduct).arg(this->getSecondsPassedAsString()));
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
  ssc::messageManager()->sendInfo(QString("Still executing %1, [%2] ...").arg(mProduct).arg(this->getTimePassed().toString("m:ss")));
}
//---------------------------------------------------------------------------------------------------------------------


}//namespace cx

