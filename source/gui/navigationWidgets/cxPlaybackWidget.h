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

#ifndef CXPLAYBACKWIDGET_H_
#define CXPLAYBACKWIDGET_H_

#include "cxBaseWidget.h"
#include "sscDoubleDataAdapterXml.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscVector3D.h"
#include "sscRegistrationTransform.h"
#include "cxPlaybackTime.h"

namespace cx
{
class TimelineWidget;


/**\brief Widget interface to PlaybackTime
 *
 *
 * \ingroup cxGUI
 * \date Mar 29, 2012
 * \author Christian Askeland, SINTEF
 */
class PlaybackWidget : public BaseWidget
{
	Q_OBJECT
public:
	PlaybackWidget(QWidget* parent);
	virtual ~PlaybackWidget();

private slots:
	void timeChangedSlot();
	void playSlot();
	void pauseSlot();
	void stopSlot();
	void forwardSlot();
	void rewindSlot();

	void toggleOpenSlot();
	void speedChangedSlot();
	void timeLineWidgetValueChangedSlot();
	void toolManagerInitializedSlot();
	void toggleDetailsSlot();

private:
	virtual QString defaultWhatsThis() const;
	QString stripLeadingZeros(QString time);
	std::vector<TimelineEvent> convertHistoryToEvents(ssc::ToolPtr tool);
	std::vector<TimelineEvent> createEvents();
	std::pair<double,double> findTimeRange(std::vector<TimelineEvent> events);
	std::vector<TimelineEvent> convertRegistrationHistoryToEvents(ssc::RegistrationHistoryPtr reg);
	void showDetails();
	QColor generateRandomToolColor() const;
	QString convertMillisecsToNiceString(qint64 ms) const;

	QLabel* mLabel;
	QLabel* mTotalLengthLabel;
	QLabel* mStartTimeLabel;
	bool mOpen;
	PlaybackTimePtr mTimer;
	QAction* mPlayAction;
	QAction* mOpenAction;
	ssc::DoubleDataAdapterXmlPtr mSpeedAdapter;
	TimelineWidget* mToolTimelineWidget;
};





} /* namespace cx */
#endif /* CXPLAYBACKWIDGET_H_ */
