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
#ifndef CXRENDERLOOP_H
#define CXRENDERLOOP_H

#include <QObject>
#include "cxForwardDeclarations.h"
class QTimer;
#include <QDateTime>
#include <set>

namespace cx
{
typedef boost::shared_ptr<class CyclicActionLogger> CyclicActionLoggerPtr;


/** Render a set of ViewWidgets in a loop.
 *
 * This is the main render loop in Custus.
 *
 * \ingroup cx_service_visualization
 * \date 2014-02-06
 * \author christiana
 */
class RenderLoop : public QObject
{
	Q_OBJECT
public:
	RenderLoop();
	void start();
	void setRenderingInterval(int interval);
	/** If set: Render only views with modified props using the given interval,
	  * render nonmodified at a slower pace. */
	void setSmartRender(bool val) { mSmartRender = val; }
	void setLogging(bool on);

	void clearViews();
	void addView(ViewWidget* view);

	CyclicActionLoggerPtr getRenderTimer() { return mCyclicLogger; }

public slots:
	void requestPreRenderSignal();

signals:
	void preRender();
	void fps(int number); ///< Emits number of frames per second

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
	bool mPreRenderSignalRequested;
	int mBaseRenderInterval;
	bool mLogging;

	typedef std::set<QPointer<ViewWidget> > ViewWidgetSet;
	ViewWidgetSet mViews;
};

typedef boost::shared_ptr<RenderLoop> RenderLoopPtr;

} // namespace cx


#endif // CXRENDERLOOP_H
