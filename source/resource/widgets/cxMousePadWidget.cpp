/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


/*
 * sscMousePadWidget.cpp
 *
 *  Created on: Jul 25, 2011
 *      Author: christiana
 */

#include "cxMousePadWidget.h"
#include <QtWidgets>
#include "cxVector3D.h"

namespace cx
{

MousePadWidgetInternal::MousePadWidgetInternal(QWidget* parent, QSize minimumSize) :
	QFrame(parent), mFixPosX(false), mFixPosY(false), mMinSize(minimumSize)
{
}

MousePadWidgetInternal::~MousePadWidgetInternal()
{
}

void MousePadWidgetInternal::setFixedXPos(bool on)
{
	mFixPosX = on;
	this->fixPos();
}
void MousePadWidgetInternal::setFixedYPos(bool on)
{
	mFixPosY = on;
	this->fixPos();
}

void MousePadWidgetInternal::fixPos()
{
	if (mFixPosX)
	{
		mLastPos.rx() = this->width() / 2;
	}
	if (mFixPosY)
	{
		mLastPos.ry() = this->height() / 2;
	}
}

void MousePadWidgetInternal::showEvent(QShowEvent* event)
{
	mLastPos = QPoint(this->width() / 2, this->height() / 2);
	this->fixPos();
	this->update();
}

void MousePadWidgetInternal::mousePressEvent(QMouseEvent* event)
{
	mLastPos = event->pos();
	this->fixPos();
	this->update();
}

void MousePadWidgetInternal::mouseMoveEvent(QMouseEvent* event)
{
	QPoint delta = event->pos() - mLastPos;

	double padSize = (this->size().width() + this->size().height()) / 2.0; // pixel size of trackpad
	QPointF deltaN(double(delta.x()) / padSize, double(delta.y()) / padSize);
	emit
	mouseMoved(deltaN);

	mLastPos = event->pos();
	this->fixPos();
	this->update();
}

void MousePadWidgetInternal::mouseReleaseEvent(QMouseEvent* event)
{
	mLastPos = QPoint(this->width() / 2, this->height() / 2);
	this->fixPos();
	this->update();
}

void MousePadWidgetInternal::resizeEvent(QResizeEvent* event)
{
	mLastPos = QPoint(this->width() / 2, this->height() / 2);
	this->fixPos();
	this->update();
}

void MousePadWidgetInternal::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	Vector3D center(this->width() / 2, this->height() / 2, 0);
	double radius = center.length();
	QPoint qcenter(this->width() / 2, this->height() / 2);

	QRadialGradient radialGrad(qcenter, radius, mLastPos);
	radialGrad.setColorAt(0.0, QColor("khaki"));
	radialGrad.setColorAt(0.4, QColor("lightgrey"));
	radialGrad.setColorAt(1, QColor("dimgrey"));

	QColor color(146, 0, 146);
	QBrush brush(radialGrad);

	p.setPen(QColor(146, 0, 146));
	p.setBrush(QColor(146, 0, 146));
	p.fillRect(0, 0, width() - 1, height() - 1, brush);
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


MousePadWidget::MousePadWidget(QWidget* parent, QSize minimumSize) :
	QFrame(parent)
{
	mInternal = new MousePadWidgetInternal(this, minimumSize);
	connect(mInternal, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(mouseMoved(QPointF)));

	this->setFrameStyle(QFrame::Panel | QFrame::Raised);
	this->setLineWidth(3);
	QVBoxLayout* fLayout = new QVBoxLayout;
	fLayout->setMargin(0);
	this->setLayout(fLayout);

	//  MousePadWidget* pad = new MousePadWidget(this, minimumSize);
	//  pad->setFixedYPos(true);
	//  pad->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	//  pad->setLineWidth(3);
	fLayout->addWidget(mInternal);
}
MousePadWidget::~MousePadWidget()
{
}
void MousePadWidget::setFixedXPos(bool on)
{
	mInternal->setFixedXPos(on);
}
void MousePadWidget::setFixedYPos(bool on)
{
	mInternal->setFixedYPos(on);
}

}
