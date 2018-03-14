/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
