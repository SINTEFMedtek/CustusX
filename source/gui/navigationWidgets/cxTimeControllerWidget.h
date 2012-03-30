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

#ifndef CXTIMECONTROLLERWIDGET_H_
#define CXTIMECONTROLLERWIDGET_H_

#include "cxBaseWidget.h"
#include "cxTimeController.h"

namespace cx
{

/**\brief Widget interface to TimerController
 *
 *
 * \ingroup cxResourceUtilities
 * \date Mar 29, 2012
 * \author Christian Askeland, SINTEF
 */
class TimeControllerWidget : public BaseWidget
{
	Q_OBJECT
public:
	TimeControllerWidget(QWidget* parent);
	virtual ~TimeControllerWidget();

private slots:
	void timeChangedSlot();
	void playSlot();
	void pauseSlot();
	void stopSlot();
	void forwardSlot();
	void rewindSlot();
private:
	virtual QString defaultWhatsThis() const;
	QLabel* mLabel;
	TimeControllerPtr mTimer;
	QAction* mPlayAction;
};

} /* namespace cx */
#endif /* CXTIMECONTROLLERWIDGET_H_ */
