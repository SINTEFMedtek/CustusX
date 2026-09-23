/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxMouseWheelWidgetAdjustmentGuard.h"

#include <QAbstractScrollArea>
#include <QAbstractSlider>
#include <QAbstractSpinBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QScrollBar>
#include <QTabBar>
#include <QWheelEvent>
#include <QWidget>

namespace cx
{

MouseWheelWidgetAdjustmentGuard::MouseWheelWidgetAdjustmentGuard(QObject* parent) :
	QObject(parent)
{
}

QAbstractScrollArea* MouseWheelWidgetAdjustmentGuard::findEnclosingScrollArea(QWidget* widget) const
{
	for (QWidget* ancestor = widget->parentWidget(); ancestor; ancestor = ancestor->parentWidget())
	{
		if (QAbstractScrollArea* scrollArea = qobject_cast<QAbstractScrollArea*>(ancestor))
			return scrollArea;
	}
	return NULL;
}

void MouseWheelWidgetAdjustmentGuard::forwardToScrollArea(QAbstractScrollArea* scrollArea, QWheelEvent* event)
{
	QCoreApplication::sendEvent(scrollArea->viewport(), event);
}

bool MouseWheelWidgetAdjustmentGuard::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() != QEvent::Wheel)
		return QObject::eventFilter(watched, event);

	// This exact event is already being manually re-delivered to this exact
	// object below -- let it through untouched instead of reprocessing it.
	if (watched == mDeliveringTo)
		return QObject::eventFilter(watched, event);

	QWidget* widget = qobject_cast<QWidget*>(watched);
	if (!widget)
		return QObject::eventFilter(watched, event);

	// A scroll area's own furniture (viewport, scrollbars) handling its own
	// wheel event is exactly the native behavior we want to end up at --
	// never reprocess that, whether it arrived here naturally or because
	// we just forwarded to it ourselves below.
	if (qobject_cast<QAbstractScrollArea*>(widget)
			|| qobject_cast<QAbstractScrollArea*>(widget->parentWidget())
			|| qobject_cast<QScrollBar*>(widget))
		return QObject::eventFilter(watched, event);

	QAbstractScrollArea* enclosingScrollArea = this->findEnclosingScrollArea(widget);
	if (!enclosingScrollArea)
		return QObject::eventFilter(watched, event);

	QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);

	bool isValueAdjustmentControl = qobject_cast<QComboBox*>(widget)
			|| qobject_cast<QAbstractSpinBox*>(widget)
			|| qobject_cast<QTabBar*>(widget)
			|| (qobject_cast<QAbstractSlider*>(widget) && !qobject_cast<QScrollBar*>(widget));

	if (isValueAdjustmentControl && !widget->hasFocus())
	{
		// Never let an unfocused value-adjustment control consume the wheel
		// event for its own purpose -- go straight to forwarding it.
		wheelEvent->ignore();
	}
	else if (widget->isEnabled())
	{
		// Deliver it normally first, so a widget with genuine wheel handling
		// of its own (a focused control, a text edit, a nested scroll area,
		// ...) still gets first refusal. A plain eventFilter only runs
		// before delivery with no way to react afterward, so do the
		// delivery ourselves in order to inspect whether it was accepted.
		QPointer<QObject> previouslyDelivering = mDeliveringTo;
		mDeliveringTo = watched;
		QCoreApplication::sendEvent(widget, wheelEvent);
		mDeliveringTo = previouslyDelivering;
		if (wheelEvent->isAccepted())
			return true;
	}
	else
	{
		wheelEvent->ignore();
	}

	this->forwardToScrollArea(enclosingScrollArea, wheelEvent);
	return true;
}

} // namespace cx
