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

#include "cxColorSelectWidget.h"
#include "cxHelperWidgets.h"

namespace cx
{

ColorSelectWidget::ColorSelectWidget(QWidget* parent, ColorPropertyBasePtr dataInterface, QGridLayout* gridLayout, int row) :
    OptimizedUpdateWidget(parent)
{
	this->setEnabled(dataInterface->getEnabled());

    mData = dataInterface;
    connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));

    mLabel = new QLabel(this);
    mLabel->setText(dataInterface->getDisplayName());

    mColorButton = new cx::ColorSelectButton("");

	// emit as queued signal because of crash issues related to handling the signal
	// in the modal loop of the color select dialog of cx:ColorSelectButton.
	connect(mColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(valueChanged(QColor)), Qt::QueuedConnection);

    if (gridLayout) // add to input gridlayout
    {
        gridLayout->addLayout(mergeWidgetsIntoHBoxLayout(mLabel, addDummyMargin(this)), row, 0);
        gridLayout->addWidget(mColorButton, row, 1);
    }
    else // add directly to this
    {
        QHBoxLayout* topLayout = new QHBoxLayout;
        topLayout->setMargin(0);
        this->setLayout(topLayout);

        topLayout->addWidget(mLabel);
        topLayout->addWidget(mColorButton, 1);
    }

    this->setModified();
}

void ColorSelectWidget::valueChanged(QColor val)
{
    if (val == mData->getValue())
        return;
    mData->setValue(val);
}

void ColorSelectWidget::prePaintEvent()
{
    mColorButton->blockSignals(true);

    this->setEnabled(mData->getEnabled());

    mColorButton->setColor(mData->getValue());
    mColorButton->setToolTip(mData->getHelp());
	mLabel->setToolTip(mData->getHelp());

    mColorButton->blockSignals(false);
}


} // namespace cx

