/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOOLPOSITIONRECORDERWIDGET_H_
#define CXTOOLPOSITIONRECORDERWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include "cxToolPositionRecorder.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QTimer;

namespace cx
{

/** \brief Records tool positions to a CSV file, e.g. while moving tools in and out of the measurement volume.
 *
 * \ingroup cx_gui
 */
class cxGui_EXPORT ToolPositionRecorderWidget : public BaseWidget
{
	Q_OBJECT
public:
	ToolPositionRecorderWidget(TrackingServicePtr trackingService, QWidget* parent = NULL);
	virtual ~ToolPositionRecorderWidget();

private:
	void browse();
	void toggleRecording();
	void updateStatus();
	static QString createDefaultFilename();

	TrackingServicePtr mTrackingService;
	ToolPositionRecorder mRecorder;
	QLineEdit* mFilename;
	QLineEdit* mLabel;
	QPushButton* mRecordButton;
	QLabel* mStatus;
	QTimer* mStatusTimer;
};

} // namespace cx

#endif /* CXTOOLPOSITIONRECORDERWIDGET_H_ */
