/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXDISPLAYTIMERWIDGET_H_
#define CXDISPLAYTIMERWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include <QDateTime>
class QTimer;
class QLabel;
class QDateTime;

namespace cx
{

/**\brief A second counter widget.
 *
 * \ingroup cx_resource_widgets
 * \date Jan 27, 2012
 * \author Christian Askeland, SINTEF
 */
class cxResourceWidgets_EXPORT DisplayTimerWidget : public QWidget
{
	Q_OBJECT
public:
	DisplayTimerWidget(QWidget* parent);

	void setFontSize(int fontSize);
	void start();
	void stop();
    void failed();
	int elaspedSeconds() const;

private slots:
	void timeoutSlot();
private:
	void printText(QString color);
	QTimer* mTimer;
	QLabel* mLabel;
	QDateTime mStartTime;
	int mFontSize;
};
}

#endif /* CXDISPLAYTIMERWIDGET_H_ */
