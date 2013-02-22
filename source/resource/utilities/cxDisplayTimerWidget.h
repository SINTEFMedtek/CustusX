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


#ifndef CXDISPLAYTIMERWIDGET_H_
#define CXDISPLAYTIMERWIDGET_H_

#include <QWidget>
#include <QDateTime>
class QTimer;
class QLabel;
class QDateTime;

namespace cx
{

/**\brief A second counter widget.
 *
 * \ingroup cxResourceUtilities
 * \date Jan 27, 2012
 * \author Christian Askeland, SINTEF
 */
class DisplayTimerWidget : public QWidget
{
	Q_OBJECT
public:
	DisplayTimerWidget(QWidget* parent);

	void setFontSize(int fontSize);
	void start();
	void stop();
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
