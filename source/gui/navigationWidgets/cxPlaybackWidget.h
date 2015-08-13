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
#include "cxLegacySingletons.h"

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
	PlaybackWidget(QWidget* parent);
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

	QLabel* mLabel;
	QLabel* mTotalLengthLabel;
	QLabel* mStartTimeLabel;
	bool mOpen;
	PlaybackTimePtr mTimer;
	QAction* mPlayAction;
	QAction* mOpenAction;
	DoublePropertyPtr mSpeedAdapter;
	TimelineWidget* mToolTimelineWidget;
};





} /* namespace cx */
#endif /* CXPLAYBACKWIDGET_H_ */
