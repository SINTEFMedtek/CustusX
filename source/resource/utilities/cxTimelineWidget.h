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

#ifndef CXTIMELINEWIDGET_H_
#define CXTIMELINEWIDGET_H_

#include "cxPlaybackTime.h"
#include "sscDoubleDataAdapterXml.h"
#include <QSlider>
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscVector3D.h"

namespace cx
{

/**\brief widget for displaying a timeline for events.
 *
 * The point is to display periods of time where tools
 * are visible in a different color than the visible
 * periods. Also other events like registration, data load.
 *
 * \ingroup cxResourceUtilities
 * \date Mar 29, 2012
 * \author Christian Askeland, SINTEF
 */
class TimelineWidget : public QWidget
{
Q_OBJECT

public:
	class TimelineEvent
	{
	public:
		TimelineEvent() : mStartTime(0), mEndTime(0) {}
		TimelineEvent(QString description, double start, double end) : mDescription(description), mStartTime(start), mEndTime(end) {}
		TimelineEvent(QString description, double time) : mDescription(description), mStartTime(time), mEndTime(time) {}
		QString mDescription;
		double mStartTime;
		double mEndTime;
		/** Return whether time is inside event.
		 *  The minimum length of the event if set to tol_ms.
		 */
		bool isInside(double time, double tol_ms=0) const
		{
			double w = mEndTime - mStartTime;
			double m = mStartTime + w/2;
			return fabs(time - m) < std::max(w, tol_ms)/2;
		}
		bool isSingular() const { return ssc::similar(mEndTime,mStartTime); }
		bool isOverlap(const TimelineEvent& rhs) const
		{
			double w0 = mEndTime - mStartTime;
			double m0 = mStartTime + w0/2;
			double w1 = rhs.mEndTime - rhs.mStartTime;
			double m1 = rhs.mStartTime + w1/2;
			return fabs(m1-m0) < (w1+w0)/2;
		}
		bool operator<(const TimelineEvent& rhs) const
		{
			return mStartTime < rhs.mStartTime;
		}
	};

	TimelineWidget(QWidget* parent);
	virtual ~TimelineWidget();


	void setEvents(std::vector<TimelineEvent> events);
//	void setRegions(std::vector<std::pair<double, double> > validRegions);
	void setRange(double start, double stop);
	void setPos(double pos);
	double getPos() const;

signals:
	void positionChanged();

protected slots:

protected:
	virtual bool event(QEvent *event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void resizeEvent(QResizeEvent* evt); ///< Reimplemented from superclass
	virtual void paintEvent(QPaintEvent* event); ///< Reimplemented from superclass. Paints the transferfunction GUI
	virtual QSize sizeHint() const;
	QSize minimumSizeHint() const;

private:
	int mapTime2PlotX(double time) const;
	double mapPlotX2Time(int plotX) const;
	bool showHelp(QPoint pos);

	void setPositionFromScreenPos(int x, int y);
	void createCompactingTransforms();

//	std::vector<std::pair<double, double> > mValidRegions;
	std::vector<TimelineEvent> mEvents;
	double mStart, mStop, mPos;
	int mBorder;
	QRect mFullArea; ///< The full widget area
	QRect mPlotArea; ///< The plot area
	bool mCloseToGlyph; ///< temporary that is true when mouse cursor is close to glyph and should be highlighted.
	int mTolerance_p; ///< tolerance in pix, used to pick/show short events.
	QStringList mContinousEvents; //< list of all continous events, used for stacked display
	std::vector<TimelineEvent> mNoncompactedIntervals; ///< listing the intervals that are unchanged by the compacting transform.
	std::vector<QColor> mEventColors; ///< use to color continous events

	vtkPiecewiseFunctionPtr mBackward;
	vtkPiecewiseFunctionPtr mForward;

};

} /* namespace cx */

#endif /* CXTIMELINEWIDGET_H_ */
