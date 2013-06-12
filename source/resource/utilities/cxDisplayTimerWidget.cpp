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

#include "cxDisplayTimerWidget.h"

#include <QTimer>
#include <QHBoxLayout>
#include <QLabel>

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
	this->printText("green");
}

void DisplayTimerWidget::timeoutSlot()
{
	this->printText("black");
}

void DisplayTimerWidget::printText(QString color)
{
	int secs = this->elaspedSeconds(); // SmStartTime.secsTo(QDateTime::currentDateTime());
	mLabel->setText(QString("<font size=%1 color=%2><b>%3 s</b></font>").arg(mFontSize).arg(color).arg(secs));
}

int DisplayTimerWidget::elaspedSeconds() const
{
	return mStartTime.secsTo(QDateTime::currentDateTime());
}


}
