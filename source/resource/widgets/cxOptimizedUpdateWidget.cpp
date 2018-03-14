/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxOptimizedUpdateWidget.h"
#include <iostream>

namespace cx
{


OptimizedUpdateWidget::OptimizedUpdateWidget(QWidget* parent) :
    QWidget(parent), mModified(true)
{
}

void OptimizedUpdateWidget::forcePrePaint()
{
	this->prePaintEventPrivate();
}

void OptimizedUpdateWidget::paintEvent(QPaintEvent* event)
{
    this->prePaintEventPrivate();
    QWidget::paintEvent(event);
}

void OptimizedUpdateWidget::setModified()
{
    mModified = true;
    this->update();
}

void OptimizedUpdateWidget::prePaintEventPrivate()
{
    if (!mModified)
        return;

    this->prePaintEvent();

    mModified = false;
}

} // namespace cx
