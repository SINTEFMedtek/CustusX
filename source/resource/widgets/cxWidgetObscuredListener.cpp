/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxWidgetObscuredListener.h"

#include <QWidget>
#include <QTimer>

namespace cx
{

WidgetObscuredListener::WidgetObscuredListener(QWidget *listenedTo) : mWidget(listenedTo)
{
	mRemoveTimer = new QTimer(this);
	connect(mRemoveTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
	mRemoveTimer->start(500);

	mObscuredAtLastCheck = this->isObscured();
}

bool WidgetObscuredListener::isObscured() const
{
	return mWidget->visibleRegion().isEmpty();
}

void WidgetObscuredListener::timeoutSlot()
{
	if (mObscuredAtLastCheck == this->isObscured())
		return;

	mObscuredAtLastCheck = this->isObscured();
	emit obscured(mObscuredAtLastCheck);
}

}
