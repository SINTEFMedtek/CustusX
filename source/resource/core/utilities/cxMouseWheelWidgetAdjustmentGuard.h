/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMOUSEWHEELWIDGETADJUSTMENTGUARD_H
#define CXMOUSEWHEELWIDGETADJUSTMENTGUARD_H

#include "cxResourceExport.h"
#include <QObject>
#include <QPointer>

class QAbstractScrollArea;
class QWheelEvent;
class QWidget;

namespace cx
{

/**
 * Application-wide event filter (install on QApplication) that makes the
 * mouse wheel scroll the nearest enclosing QScrollArea when it passes over
 * a widget inside that scroll area's content that doesn't want the wheel
 * event itself -- instead of the gesture silently doing nothing unless the
 * cursor is exactly over the scroll area's own scrollbar.
 *
 * A combo box, spin box, slider or QTabBar is additionally never allowed
 * to consume the wheel event for its own value-adjustment/tab-switching
 * purpose unless it currently has keyboard focus, since those are the
 * controls most likely to sit directly under the cursor while scrolling
 * a view and otherwise steal the gesture. Any other widget with its own
 * genuine wheel handling (e.g. a text edit, list view or nested scroll
 * area) still takes precedence over the enclosing scroll area, since it
 * gets the first chance to accept the event.
 */
class cxResource_EXPORT MouseWheelWidgetAdjustmentGuard : public QObject
{
public:
	explicit MouseWheelWidgetAdjustmentGuard(QObject* parent = 0);

protected:
	virtual bool eventFilter(QObject* watched, QEvent* event);

private:
	QAbstractScrollArea* findEnclosingScrollArea(QWidget* widget) const;
	void forwardToScrollArea(QAbstractScrollArea* scrollArea, QWheelEvent* event);

	QPointer<QObject> mDeliveringTo;
};

} // namespace cx

#endif // CXMOUSEWHEELWIDGETADJUSTMENTGUARD_H
