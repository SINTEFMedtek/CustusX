// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "cxOptimizedUpdateWidget.h"
#include <iostream>

namespace cx
{


OptimizedUpdateWidget::OptimizedUpdateWidget(QWidget* parent) :
    QWidget(parent), mModified(true)
{
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
