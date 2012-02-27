/*
 * cxDisplayTimerWidget.h
 *
 *  Created on: Jan 27, 2012
 *      Author: christiana
 */

#ifndef CXDISPLAYTIMERWIDGET_H_
#define CXDISPLAYTIMERWIDGET_H_

#include <QtGui>

namespace cx
{

/**\brief A second counter widget.
 * \ingroup cxResourceUtilities
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
