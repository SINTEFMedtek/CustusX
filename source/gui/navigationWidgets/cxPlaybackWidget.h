/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPLAYBACKWIDGET_H_
#define CXPLAYBACKWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include "cxDoubleProperty.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include "cxRegistrationTransform.h"
#include "cxPlaybackTime.h"

namespace cx
{
class TimelineWidget;


/**\brief Widget interface to PlaybackTime
 *
 *
 * \ingroup cx_gui
 * \date Mar 29, 2012
 * \author Christian Askeland, SINTEF
 */
class cxGui_EXPORT PlaybackWidget : public BaseWidget
{
	Q_OBJECT
public:
	PlaybackWidget(TrackingServicePtr trackingService, VideoServicePtr videoService, PatientModelServicePtr patientModelService, QWidget* parent);
	virtual ~PlaybackWidget();

private slots:
	void timeChangedSlot();
	void playSlot();
//	void pauseSlot();
	void stopSlot();
	void forwardSlot();
	void rewindSlot();

	void toggleOpenSlot();
	void speedChangedSlot();
	void timeLineWidgetValueChangedSlot();
	void toolManagerInitializedSlot();
	void toggleDetailsSlot();

private:
//	QString stripLeadingZeros(QString time);
	std::vector<TimelineEvent> convertHistoryToEvents(ToolPtr tool);
	std::vector<TimelineEvent> createEvents();
	std::pair<double,double> findTimeRange(std::vector<TimelineEvent> events);
	std::vector<TimelineEvent> convertRegistrationHistoryToEvents(RegistrationHistoryPtr reg);
	void showDetails();
	QColor generateRandomToolColor() const;
	QString convertMillisecsToNiceString(qint64 ms) const;
	bool isInterestingTool(ToolPtr tool) const;

	QLabel* mLabel;
	QLabel* mTotalLengthLabel;
	QLabel* mStartTimeLabel;
	bool mOpen;
	PlaybackTimePtr mTimer;
	QAction* mPlayAction;
	QAction* mOpenAction;
	DoublePropertyPtr mSpeedAdapter;
	TimelineWidget* mToolTimelineWidget;
	TrackingServicePtr mTrackingService;
	VideoServicePtr mVideoService;
	PatientModelServicePtr mPatientModelService;
};





} /* namespace cx */
#endif /* CXPLAYBACKWIDGET_H_ */
