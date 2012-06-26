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

#include "cxTimelineWidget.h"

#include <vtkPiecewiseFunction.h>
#include <QPainter>
#include <QToolTip>
#include <QMouseEvent>
#include "sscHelperWidgets.h"
#include "sscTime.h"
#include "sscMessageManager.h"
#include "sscLogger.h"
#include "sscTypeConversions.h"

namespace cx
{

TimelineWidget::TimelineWidget(QWidget* parent) :
				QWidget(parent), mBorder(4), mStart(0), mStop(0), mPos(0), mCloseToGlyph(false), mTolerance_p(5)
{
	mForward = vtkPiecewiseFunctionPtr::New();
	mBackward = vtkPiecewiseFunctionPtr::New();

//	int s = 255;
//	int v = 192;
//	mEventColors.push_back(QColor::fromHsv(110, s, v));
//	mEventColors.push_back(QColor::fromHsv(80, s, v));
//	mEventColors.push_back(QColor::fromHsv(140, s, v));
//	mEventColors.push_back(QColor::fromHsv(95, s, v));
//	mEventColors.push_back(QColor::fromHsv(125, s, v));

	this->setFocusPolicy(Qt::StrongFocus);
	this->setMouseTracking(true);
}

TimelineWidget::~TimelineWidget()
{

}

void TimelineWidget::setPos(double pos)
{
	mPos = pos;
	this->update();
	emit positionChanged();
}

double TimelineWidget::getPos() const
{
	return mPos;
}

/**Compact free space using a compacting transform
 *
 */
double TimelineWidget::findCompactedTime(double timeInterval, double totalUsedTime, double totalTime) const
{
	// compact free space using t' = t * c, where
	// c = w*c1 + (1-w)*c0
	// w is this free space's fraction of total time
	// c1 is compactingFactor1, set so that even a space using more than 99%
	// of the total time will be reduced to 20%.
	// c0 is compactingFactor0, set to 0.2. Used for small spaces.
	// constant factor: apply even to small spaces
	double compactingFactor0 = 0.2;
	double compactingFactor1 = totalUsedTime/totalTime * 0.2;

	// find compacting factor c as described above
	double w = timeInterval/totalTime;
	double c = w*compactingFactor1 + (1-w)*compactingFactor0;
	// use c to compact, but also set an absolute limit of fraction of total used time. This handles pauses of days or months.
	double compactedTime = std::min(timeInterval * c, 0.3*totalUsedTime);

	return compactedTime;
}

/**Identify intervals containing no events,
 * then compact this time by a factor.
 *
 * The result is given as the mForward and mBackward
 * transforms that is used to convert from time to
 * normalized space and back.
 *
 */
void TimelineWidget::createCompactingTransforms()
{
	// identify empty zones

	std::vector<TimelineEvent> temp = mEvents;

	// grow
	double fat = 2000; // additional space around each event.
	for (unsigned i = 0; i < temp.size(); ++i)
	{
		temp[i].mDescription = "merged";
		temp[i].mStartTime = std::max(temp[i].mStartTime - fat, mStart);
		temp[i].mEndTime = std::min(temp[i].mEndTime + fat, mStop);
	}

	// merge
	for (unsigned i = 0; i < temp.size(); ++i)
	{
		// try to merge other events into this one

		for (unsigned j = i + 1; j < temp.size();)
		{
			if (temp[i].isOverlap(temp[j]))
			{
				temp[i].mStartTime = std::min(temp[i].mStartTime, temp[j].mStartTime);
				temp[i].mEndTime = std::max(temp[i].mEndTime, temp[j].mEndTime);
				temp.erase(temp.begin() + j);
			}
			else
			{
				++j;
			}
		}
	}

	// sort
	std::sort(temp.begin(), temp.end());

	double totalUsedTime = 0;
	for (unsigned i = 0; i < temp.size(); ++i)
		totalUsedTime += temp[i].mEndTime - temp[i].mStartTime;
	// fraction of total time used in events:
	double usageFactor = totalUsedTime / (mStop-mStart);

	// function from real time (x) to compact time (y)
	mForward = vtkPiecewiseFunctionPtr::New();

	double y = mStart;
	mForward->AddPoint(mStart, y);
	double x_last = mStart;
	for (unsigned i = 0; i < temp.size(); ++i)
	{
		// add first point
		y = y + this->findCompactedTime(temp[i].mStartTime - x_last, totalUsedTime, mStop-mStart);
		mForward->AddPoint(temp[i].mStartTime, y);
		// add last point
		y = y + temp[i].mEndTime - temp[i].mStartTime;
		mForward->AddPoint(temp[i].mEndTime, y);

		x_last = temp[i].mEndTime;
	}

	// add endpoint
	y = y + this->findCompactedTime(mStop - x_last, totalUsedTime, mStop-mStart);
	mForward->AddPoint(mStop, y);

	// normalize y:
	double range[2];
	mForward->GetRange(range);
	double y_min = mForward->GetValue(range[0]);
	double y_max = mForward->GetValue(range[1]);
	for (int i = 0; i < mForward->GetSize(); ++i)
	{
		double val[4];
		mForward->GetNodeValue(i, val);
		val[1] = (val[1] - y_min) / (y_max - y_min);
		mForward->SetNodeValue(i, val);
	}

	// create inverse function:
	mBackward = vtkPiecewiseFunctionPtr::New();
	for (int i = 0; i < mForward->GetSize(); ++i)
	{
		double val[4];
		mForward->GetNodeValue(i, val);
		mBackward->AddPoint(val[1], val[0]);
	}

	mNoncompactedIntervals = temp;
}

void TimelineWidget::setEvents(std::vector<TimelineEvent> events)
{
	mEvents = events;

	if (mEvents.empty())
		return;

	this->createCompactingTransforms();

//	//	// test: add to events to display
//	std::copy(mNoncompactedIntervals.begin(), mNoncompactedIntervals.end(), std::back_inserter(mEvents));

	// identify continous events
	for (unsigned i = 0; i < mEvents.size(); ++i)
	{
		if (mEvents[i].isSingular())
			continue;
//		if (!std::count(mContinousEvents.begin(), mContinousEvents.end(), mEvents[i].mDescription))
//			mContinousEvents.push_back(mEvents[i].mDescription);
		if (!std::count(mContinousEvents.begin(), mContinousEvents.end(), mEvents[i].mGroup))
			mContinousEvents.push_back(mEvents[i].mGroup);
	}

}

void TimelineWidget::setRange(double start, double stop)
{
	mStart = start;
	mStop = stop;
}

/**Convert a time position to it corresponding position
 * in the plot area.
 */
int TimelineWidget::mapTime2PlotX(double time) const
{
	double normalized = mForward->GetValue(time);
//	double normalized = (time - mStart) / (mStop - mStart);
	int retval = normalized * mPlotArea.width() + mPlotArea.left();
	return retval;
}

/**inverse of mapTime2PlotX()
 */
double TimelineWidget::mapPlotX2Time(int plotX) const
{
	double normalized = double(plotX - mPlotArea.left()) / mPlotArea.width();
	double retval = mBackward->GetValue(normalized);
//	double retval = mStart + normalized*(mStop - mStart);
	return retval;
}

void TimelineWidget::paintEvent(QPaintEvent* event)
{
	if (!mStart == mStop)
		return;
	QWidget::paintEvent(event);

	QPainter painter(this);
//  QPen pointPen, pointLinePen;
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);

	QColor gray0(240, 240, 240);
	QColor gray01(220, 220, 220);
	QColor gray1(200, 200, 200);
	QColor gray2(170, 170, 170); // darker
	QColor gray3(150, 150, 150); // even darker
	QColor gray4(100, 100, 100); // even darker
	QColor highlight(110, 214, 255); // color around highlighted circle in qslider on KDE.

	// Fill with white background color and grey plot area background color
	QBrush brush(Qt::SolidPattern); // = painter.brush();
	brush.setColor(gray2);
	painter.setBrush(brush);

	painter.drawRoundedRect(this->mFullArea, 4, 4);
//	brush.setColor(gray01);
	brush.setColor(gray1);
	painter.setBrush(brush);
	painter.drawRoundedRect(this->mPlotArea, 4, 4);

	int margin = 1;

	// draw noncompacted interval
	for (unsigned i = 0; i < mNoncompactedIntervals.size(); ++i)
	{
		int start_p = this->mapTime2PlotX(mNoncompactedIntervals[i].mStartTime);
		int stop_p = this->mapTime2PlotX(mNoncompactedIntervals[i].mEndTime);
		QColor color = gray01;
		painter.fillRect(QRect(start_p, mPlotArea.top(), stop_p - start_p, mPlotArea.height()), color);
	}

	// draw all continous events
	for (unsigned i = 0; i < mEvents.size(); ++i)
	{
		if (!mContinousEvents.contains(mEvents[i].mGroup))
			continue;
		int start_p = this->mapTime2PlotX(mEvents[i].mStartTime);
		int stop_p = this->mapTime2PlotX(mEvents[i].mEndTime);
		int level = std::distance(mContinousEvents.begin(),
						std::find(mContinousEvents.begin(), mContinousEvents.end(), mEvents[i].mGroup));
		int level_max = mContinousEvents.size();
		int thisHeight = (mPlotArea.height()) / level_max - margin * (level_max - 1) / level_max;
		int thisTop = mPlotArea.top() + level * thisHeight + level * margin;

//		QColor color = mEventColors[level % mEventColors.size()];
		QColor color = mEvents[i].mColor;

		painter.fillRect(QRect(start_p, thisTop, stop_p - start_p, thisHeight), color);
	}

	// draw all singular events
	for (unsigned i = 0; i < mEvents.size(); ++i)
	{
		if (mContinousEvents.contains(mEvents[i].mGroup))
			continue;

		int start_p = this->mapTime2PlotX(mEvents[i].mStartTime);
//		int stop_p = this->mapTime2PlotX(mEvents[i].mEndTime);

		int glyphWidth = 3;
		QRect rect(start_p - glyphWidth / 2, mPlotArea.top(), glyphWidth, mPlotArea.height());

		brush.setColor(QColor(50, 50, 50));
		painter.setBrush(brush);
		painter.drawRoundedRect(rect, 2, 2);

//		painter.fillRect(rect, gray4);
		if (rect.width() > 2 && rect.height() > 2)
		{
			rect.adjust(1, 1, -1, -1);
			painter.fillRect(rect, gray2);
		}
	}

	int offset_p = this->mapTime2PlotX(mPos);
	QPolygonF glyph;
	int z = 5;
	int h = mPlotArea.height();
	glyph.push_back(QPointF(-z, 0));
	glyph.push_back(QPointF(z, 0));
	glyph.push_back(QPointF(z, 0.7 * h));
	glyph.push_back(QPointF(0, h));
	glyph.push_back(QPointF(-z, 0.7 * h));
	glyph.translate(offset_p, 0);
	if (this->hasFocus() || mCloseToGlyph)
		painter.setPen(highlight);
	else
		painter.setPen(gray4);
//  QBrush brush(Qt::SolidPattern);// = painter.brush();

	QRadialGradient radialGrad(QPointF(offset_p, h / 3), 2 * h / 3);
	radialGrad.setColorAt(0, gray0);
//  radialGrad.setColorAt(0.5, Qt::blue);
	radialGrad.setColorAt(1, gray2);

	brush = QBrush(radialGrad);
//  brush.setColor(gray0);
	painter.setBrush(brush);
	painter.drawPolygon(glyph);
}

void TimelineWidget::resizeEvent(QResizeEvent* evt)
{
	QWidget::resizeEvent(evt);

	// Calculate areas
	this->mFullArea = QRect(0, 0, width(), height());
	this->mPlotArea = QRect(mBorder, mBorder, width() - mBorder * 2, height() - mBorder * 2);
}

void TimelineWidget::setPositionFromScreenPos(int x, int y)
{
	mPos = this->mapPlotX2Time(x);
	this->update();
	emit positionChanged();
}

void TimelineWidget::mousePressEvent(QMouseEvent* event)
{
	QWidget::mousePressEvent(event);

	if (event->button() == Qt::LeftButton)
	{
		this->setPositionFromScreenPos(event->x(), event->y());
	}
}
void TimelineWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);
}

bool TimelineWidget::event(QEvent *event)
{
	if (event->type() == QEvent::ToolTip)
	{
		QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
		if (!this->showHelp(helpEvent->pos()))
		{
			event->ignore();
		}

		return true;
	}
	return QWidget::event(event);
}

bool TimelineWidget::showHelp(QPoint pos)
{
	double mouseTimePos = this->mapPlotX2Time(pos.x());
	double tol_ms = this->mapPlotX2Time(pos.x()+mTolerance_p) - mouseTimePos;

	QStringList text;

	for (unsigned i = 0; i < mEvents.size(); ++i)
	{
		if (mEvents[i].isInside(mouseTimePos, tol_ms))
		{
			text << mEvents[i].mDescription;
//		  std::cout << "inside: " << mEvents[i].mDescription << " [" << mouseTimePos << "] " << mEvents[i].mStartTime << "-" << mEvents[i].mEndTime << " " << tol_ms << std::endl;
		}
	}

	if (text.isEmpty())
	{
		QToolTip::hideText();
		return false;
	}
	else
	{
//	  QToolTip::hideText();
//	  QToolTip::showText(this->mapToGlobal(pos), "", this); // hide - caused undefinable hiding of text after a sec.
		QToolTip::showText(this->mapToGlobal(pos), text.join("\n"), this); // show in new place
		return true;
	}
}

void TimelineWidget::mouseMoveEvent(QMouseEvent* event)
{
	QWidget::mouseMoveEvent(event);

	if (event->buttons() == Qt::LeftButton)
	{
		this->setPositionFromScreenPos(event->x(), event->y());
	}

	bool newCloseToGlyph = fabs((double)(this->mapTime2PlotX(mPos) - event->x())) < 5;
	if (mCloseToGlyph != newCloseToGlyph)
		this->update();

//  this->showHelp(event->pos());

	mCloseToGlyph = newCloseToGlyph;
}

QSize TimelineWidget::sizeHint() const
{
	return QSize(100, 30);
}

QSize TimelineWidget::minimumSizeHint() const
{
	return QSize(100, 20);
}

} /* namespace cx */
