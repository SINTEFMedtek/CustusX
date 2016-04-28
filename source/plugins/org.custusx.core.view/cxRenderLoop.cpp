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
#include "cxRenderLoop.h"

#include "cxCyclicActionLogger.h"
#include <QTimer>
#include "cxView.h"
#include "vtkRenderWindow.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxViewCollectionWidget.h"


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
	mLastFullRender = QDateTime::currentDateTime();
	mCyclicLogger.reset(new CyclicActionLogger("Main Render timer"));

	mTimer = new QTimer(this);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));

	this->sendRenderIntervalToTimer(mBaseRenderInterval);
}

void RenderLoop::start()
{
	mTimer->start();
}

void RenderLoop::stop()
{
	mTimer->stop();
	emit fps(0);
}

bool RenderLoop::isRunning() const
{
	return mTimer->isActive();
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

void RenderLoop::setSmartRender(bool val)
{
	mSmartRender = val;
}

void RenderLoop::sendRenderIntervalToTimer(int interval)
{
	if (interval==mTimer->interval())
		return;
	if (interval == 0)
		interval = 30;

	bool active = this->isRunning();
	if (active)
		mTimer->stop();
	mTimer->setInterval(interval);
	if (active)
		mTimer->start();
}

void RenderLoop::addLayout(ViewCollectionWidget* layout)
{
	mLayoutWidgets.push_back(layout);
}


void RenderLoop::clearViews()
{
	mLayoutWidgets.clear();
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

	for (unsigned i=0; i<mLayoutWidgets.size(); ++i)
	{
		if (mLayoutWidgets[i])
		{
			if (!smart)
				mLayoutWidgets[i]->setModified();
			mLayoutWidgets[i]->render();
		}
	}

	mCyclicLogger->time("render");
	emit renderFinished();
}

bool RenderLoop::pollForSmartRenderingThisCycle()
{
	// do a full render anyway at low rate. This is a convenience hack for rendering
	// occational effects that the smart render is too dumb to see.
	bool smart = mSmartRender;
	int smartInterval = mTimer->interval() * 40;
	int passed = mLastFullRender.time().msecsTo(QDateTime::currentDateTime().time());

	if (passed > smartInterval)
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
//			reportDebug(mCyclicLogger->dumpStatisticsSmall());
		mCyclicLogger->reset();
	}
}

void RenderLoop::dumpStatistics()
{
	if (!mLogging)
		return;

	static int counter=0;
	if (++counter%3==0) // every third event
		reportDebug(mCyclicLogger->dumpStatisticsSmall());
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

