/*
 * cxStatusBarWidget.h
 *
 *  Created on: Jan 21, 2009
 *      Author: Janne Beate Bakeng, SINTEF
 */

#ifndef CXSTATUSBARWIDGET_H_
#define CXSTATUSBARWIDGET_H_

#include <QWidget>

class QHBoxLayout;

namespace cx
{
class StatusBarWidget : public QWidget
{
public:
	StatusBarWidget();
	~StatusBarWidget();

protected:
	QHBoxLayout* mHBoxLayout;
};
}

#endif /* CXSTATUSBARWIDGET_H_ */
