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
#include "cxRenderLoop.h"

#include "cxCyclicActionLogger.h"
#include <QTimer>
#include "cxView.h"
#include "vtkRenderWindow.h"
#include "cxTypeConversions.h"
#include "cxGLHelpers.h"
#include "cxMessageManager.h"

namespace cx
{

RenderLoop::RenderLoop() :
	QObject(NULL),
	mTimer(NULL),
	mPreRenderSignalRequested(false),
	mSmartRender(false),
	mLogging(false),
	mBaseRenderInterval(40)
{
	mCyclicLogger.reset(new CyclicActionLogger("Main Render timer"));
}

void RenderLoop::start()
{
	mTimer = new QTimer(this);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
	this->sendRenderIntervalToTimer(mBaseRenderInterval);
}

void RenderLoop::requestPreRenderSignal()
{
	mPreRenderSignalRequested = true;
}

void RenderLoop::setRenderingInterval(int interval)
{
	mBaseRenderInterval = interval;
	this->sendRenderIntervalToTimer(mBaseRenderInterval);
}

void RenderLoop::setLogging(bool on)
{
	mLogging = on;
}

void RenderLoop::sendRenderIntervalToTimer(int interval)
{
	if (!mTimer)
		return;
	if (interval==mTimer->interval())
		return;

	mTimer->stop();
	if (interval == 0)
		interval = 30;
	mTimer->start(interval);
}

void RenderLoop::clearViews()
{
	mViews.clear();
}

void RenderLoop::addView(ViewWidget* view)
{
	mViews.insert(view);
}

void RenderLoop::timeoutSlot()
{
	mCyclicLogger->begin();
	mLastBeginRender = QDateTime::currentDateTime();
	this->sendRenderIntervalToTimer(mBaseRenderInterval);

	this->emitPreRenderIfRequested();

	this->renderViews();

	this->emitFPSIfRequired();

	int timeToNext = this->calculateTimeToNextRender();
	this->sendRenderIntervalToTimer(timeToNext);
}

void RenderLoop::emitPreRenderIfRequested()
{
	if(mPreRenderSignalRequested)
	{
		emit preRender();
		mPreRenderSignalRequested = false;
	}
	//    mRenderTimer->time("prerender");
}

void RenderLoop::renderViews()
{
	bool smart = this->pollForSmartRenderingThisCycle();

	for (ViewWidgetSet::iterator iter = mViews.begin(); iter != mViews.end(); ++iter)
	{
		ViewWidget* current = *iter;
		if (current->isVisible())
		{
			if (smart)
				dynamic_cast<View*>(current)->render(); // render only changed scenegraph (shaky but smooth)
			else
				current->getRenderWindow()->Render(); // previous version: renders even when nothing is changed

			report_gl_error_text(cstring_cast(QString("During rendering of view: ") + current->getName()));
		}
	}
	mCyclicLogger->time("render");
}

bool RenderLoop::pollForSmartRenderingThisCycle()
{
	// do a full render anyway at low rate. This is a convenience hack for rendering
	// occational effects that the smart render is too dumb to see.
	bool smart = mSmartRender;
	int smartInterval = mTimer->interval() * 40;
	if (mLastFullRender.time().msecsTo(QDateTime::currentDateTime().time()) > smartInterval)
		smart = false;
	if (!smart)
		mLastFullRender = QDateTime::currentDateTime();
	return smart;
}

void RenderLoop::emitFPSIfRequired()
{
	if (mCyclicLogger->intervalPassed())
	{
		emit fps(mCyclicLogger->getFPS());
		this->dumpStatistics();
//		static int counter=0;
//		if (++counter%3==0)
//			messageManager()->sendDebug(mCyclicLogger->dumpStatisticsSmall());
		mCyclicLogger->reset();
	}
}

void RenderLoop::dumpStatistics()
{
	if (!mLogging)
		return;

	static int counter=0;
	if (++counter%3==0) // every third event
		messageManager()->sendDebug(mCyclicLogger->dumpStatisticsSmall());
}

int RenderLoop::calculateTimeToNextRender()
{
	// tests show that the application wait between renderings even if the rendering uses more time
	// than the interval. This hack shortens the wait time between renderings but keeps below the
	// input update rate at all times.
	int usage = mLastBeginRender.msecsTo(QDateTime::currentDateTime()); // time spent in rendering
	int leftover = std::max(0, mTimer->interval() - usage); // time left of the rendering interval
	int timeToNext = std::max(1, leftover); // always wait at least 1ms - give others time to do stuff
	//    std::cout << QString("setting interval %1, usage is %2").arg(timeToNext).arg(usage) << std::endl;
	return timeToNext;
}

} // namespace cx

