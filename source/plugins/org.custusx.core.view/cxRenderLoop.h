/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
