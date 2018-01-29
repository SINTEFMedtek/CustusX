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
#ifndef CXRENDERLOOP_H
#define CXRENDERLOOP_H

#include "org_custusx_core_view_Export.h"

#include <QObject>
#include "cxForwardDeclarations.h"
class QTimer;
#include <QDateTime>
#include <set>

namespace cx
{
class ViewCollectionWidget;

/** Render a set of Views in a loop.
 *
 * This is the main render loop in Custus.
 *
 * \ingroup org_custusx_core_view
 * \date 2014-02-06
 * \author christiana
 */
class org_custusx_core_view_EXPORT RenderLoop : public QObject
{
	Q_OBJECT
public:
	RenderLoop();
	void start();
	void stop();
	bool isRunning() const;
	void setRenderingInterval(int interval);
	void setSmartRender(bool val); ///< If set: Render only views with modified props using the given interval, render nonmodified at a slower pace.
	void setLogging(bool on);

	void clearViews();
	void addLayout(ViewCollectionWidget* layout);

	CyclicActionLoggerPtr getRenderTimer() { return mCyclicLogger; }

//public slots:
//	void requestPreRenderSignal();

signals:
	void preRender();
	void fps(int number); ///< Emits number of frames per second
	void renderFinished();

private slots:
	void timeoutSlot();

private:
	void sendRenderIntervalToTimer(int interval);
	void emitPreRenderIfRequested();
	void renderViews();
	bool pollForSmartRenderingThisCycle();
	int calculateTimeToNextRender();
	void emitFPSIfRequired();
	void dumpStatistics();

	QTimer* mTimer; ///< timer that drives rendering
	QDateTime mLastFullRender;
	QDateTime mLastBeginRender;

	CyclicActionLoggerPtr mCyclicLogger;

	bool mSmartRender;
//	bool mPreRenderSignalRequested;
	int mBaseRenderInterval;
	bool mLogging;

//	typedef std::set<ViewPtr> ViewSet;
//	ViewSet mViews;
	std::vector<QPointer<ViewCollectionWidget> > mLayoutWidgets;
};

typedef boost::shared_ptr<RenderLoop> RenderLoopPtr;

} // namespace cx


#endif // CXRENDERLOOP_H
