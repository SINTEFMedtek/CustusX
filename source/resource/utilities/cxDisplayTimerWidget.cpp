/*
 * cxDisplayTimerWidget.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: christiana
 */

#include <cxDisplayTimerWidget.h>

namespace cx
{


DisplayTimerWidget::DisplayTimerWidget(QWidget* parent) : QWidget(parent),
	mFontSize(8)
{
	mTimer = new QTimer;
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
	mStartTime = QDateTime::currentDateTime();
	mTimer->start();
	timeoutSlot();
}

void DisplayTimerWidget::stop()
{
	mTimer->stop();
	this->printText("green");
}

void DisplayTimerWidget::timeoutSlot()
{
	this->printText("black");
}

void DisplayTimerWidget::printText(QString color)
{
	int secs = mStartTime.secsTo(QDateTime::currentDateTime());
	mLabel->setText(QString("<font size=%1 color=%2><b>%3 s</b></font>").arg(mFontSize).arg(color).arg(secs));
}



}
