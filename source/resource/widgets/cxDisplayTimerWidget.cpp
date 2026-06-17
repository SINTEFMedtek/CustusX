/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDisplayTimerWidget.h"

#include <QTimer>
#include <QHBoxLayout>
#include <QLabel>
#include "cxStyles.h"

namespace cx
{


DisplayTimerWidget::DisplayTimerWidget(QWidget* parent) : QWidget(parent),
	mFontSize(8)
{
	mTimer = new QTimer(this);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
	mTimer->setInterval(1000);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setMargin(0);

	mLabel = new QLabel;
	layout->addWidget(mLabel);
}

void DisplayTimerWidget::setFontSize(int fontSize)
{
	mFontSize = fontSize;
}

void DisplayTimerWidget::start()
{
	if (mTimer->isActive())
		return;
	mStartTime = QDateTime::currentDateTime();
	mTimer->start();
	timeoutSlot();
}

void DisplayTimerWidget::stop()
{
	mTimer->stop();
	this->printText(QString("green"));
}

void DisplayTimerWidget::failed()
{
	if(mTimer)
		mTimer->stop();
	this->printText(QString("red"));
}

void DisplayTimerWidget::timeoutSlot()
{
	this->printText(Styles::getTextColor());
}

void DisplayTimerWidget::printText(QString color)
{
	int secs = this->elaspedSeconds();
	int mins = secs / 60;
	int remainingSecs = secs % 60;
	QString timeText = mins > 0
		? QString("%1 m %2 s").arg(mins).arg(remainingSecs)
		: QString("%1 s").arg(secs);
	mLabel->setText(QString("<font size=%1 color=%2><b>%3</b></font>").arg(mFontSize).arg(color).arg(timeText));
}

void DisplayTimerWidget::printText(QColor color)
{
	this->printText(color.name());
}

int DisplayTimerWidget::elaspedSeconds() const
{
	return mStartTime.secsTo(QDateTime::currentDateTime());
}


}
