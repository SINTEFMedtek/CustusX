/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXWIDGETOBSCUREDLISTENER_H
#define CXWIDGETOBSCUREDLISTENER_H

#include "cxResourceWidgetsExport.h"

#include <QObject>
class QTimer;
#include "boost/shared_ptr.hpp"

namespace cx
{

/**
  * Listen to a widget, then emit signals when it is obscured.
  * This hack solves the issue of tabbed widgets no receiving hide() events,
  * or being !visible at all.
  *
  * \ingroup cx_resource_widgets
  */
class cxResourceWidgets_EXPORT WidgetObscuredListener : public QObject
{
	Q_OBJECT
public:
	WidgetObscuredListener(QWidget* listenedTo);
	bool isObscured() const;

signals:
	void obscured(bool visible);
private slots:
	void timeoutSlot();
private:
	bool mObscuredAtLastCheck;
	QWidget* mWidget;
	QTimer *mRemoveTimer; ///< Timer for removing segmentation preview coloring if widget is not visible
};

}

#endif // CXWIDGETOBSCUREDLISTENER_H
