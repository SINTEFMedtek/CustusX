/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


/*
 * sscMousePadWidget.h
 *
 *  Created on: Jul 25, 2011
 *      Author: christiana
 */

#ifndef CXMOUSEPADWIDGET_H_
#define CXMOUSEPADWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include <QFrame>

namespace cx
{

/**
 * Internal class for the MousePadWidget
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT MousePadWidgetInternal: public QFrame
{
Q_OBJECT

public:
	MousePadWidgetInternal(QWidget* parent, QSize minimumSize);
	virtual ~MousePadWidgetInternal();
	void setFixedXPos(bool on);
	void setFixedYPos(bool on);
	virtual QSize minimumSizeHint() const
	{
		return mMinSize;
	}
signals:
	void mouseMoved(QPointF deltaN);
protected:
	void paintEvent(QPaintEvent* event);
	virtual void showEvent(QShowEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void resizeEvent(QResizeEvent* event);

private:
	QPoint mLastPos;
	bool mFixPosX;
	bool mFixPosY;
	QSize mMinSize;
	void fixPos();
};

/**\brief A touchpad-friendly area  for performing 1D/2D scroll operations.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT MousePadWidget: public QFrame
{
Q_OBJECT

public:
	MousePadWidget(QWidget* parent, QSize minimumSize);
	virtual ~MousePadWidget();
	void setFixedXPos(bool on);
	void setFixedYPos(bool on);
signals:
	void mouseMoved(QPointF deltaN);
protected:
private:
	class MousePadWidgetInternal* mInternal;
};

}

#endif /* CXMOUSEPADWIDGET_H_ */
