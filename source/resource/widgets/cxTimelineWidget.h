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

#ifndef CXTIMELINEWIDGET_H_
#define CXTIMELINEWIDGET_H_

#include <QWidget>

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include "cxPlaybackTime.h"


namespace cx
{

/**\brief widget for displaying a timeline for events.
 *
 * The point is to display periods of time where tools
 * are visible in a different color than the visible
 * periods. Also other events like registration, data load.
 *
 * \ingroup cx_resource_widgets
 * \date Mar 29, 2012
 * \author Christian Askeland, SINTEF
 */
class TimelineWidget : public QWidget
{
Q_OBJECT

public:
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
	double findCompactedTime(double timeInterval, double totalUsedTime, double totalTime) const;

//	std::vector<std::pair<double, double> > mValidRegions;
	std::vector<TimelineEvent> mEvents;
	int mBorder;
	double mStart, mStop, mPos;
	QRect mFullArea; ///< The full widget area
	QRect mPlotArea; ///< The plot area
	bool mCloseToGlyph; ///< temporary that is true when mouse cursor is close to glyph and should be highlighted.
	int mTolerance_p; ///< tolerance in pix, used to pick/show short events.
	QStringList mContinousEvents; //< list of all continous events, used for stacked display
	std::vector<TimelineEvent> mNoncompactedIntervals; ///< listing the intervals that are unchanged by the compacting transform.
//	std::vector<QColor> mEventColors; ///< use to color continous events

	vtkPiecewiseFunctionPtr mBackward;
	vtkPiecewiseFunctionPtr mForward;

};

} /* namespace cx */

#endif /* CXTIMELINEWIDGET_H_ */
