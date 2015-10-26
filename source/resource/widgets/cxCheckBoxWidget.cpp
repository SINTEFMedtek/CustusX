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
 * sscCheckBoxWidget.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#include "cxCheckBoxWidget.h"
#include <QGridLayout>
#include "cxHelperWidgets.h"

namespace cx
{

CheckBoxWidget::CheckBoxWidget(QWidget* parent, BoolPropertyBasePtr dataInterface, QGridLayout* gridLayout, int row) :
    OptimizedUpdateWidget(parent)
{
	this->setEnabled(dataInterface->getEnabled());

	mData = dataInterface;
    connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));

    mLabel = new QLabel(this);
    mLabel->setText(dataInterface->getDisplayName());

	mCheckBox = new QCheckBox(this);
	connect(mCheckBox, SIGNAL(toggled(bool)), this, SLOT(valueChanged(bool)));

    if (gridLayout) // add to input gridlayout
    {
        gridLayout->addLayout(mergeWidgetsIntoHBoxLayout(mLabel, addDummyMargin(this)), row, 0);
        gridLayout->addWidget(mCheckBox, row, 1);
    }
    else // add directly to this
    {
        QHBoxLayout* topLayout = new QHBoxLayout;
        topLayout->setMargin(0);
        this->setLayout(topLayout);

        topLayout->addWidget(mLabel);
        topLayout->addWidget(mCheckBox, 1);
    }

    this->setModified();
}

void CheckBoxWidget::valueChanged(bool val)
{
	if (val == mData->getValue())
		return;
	mData->setValue(val);
}

void CheckBoxWidget::prePaintEvent()
{
	mCheckBox->blockSignals(true);

	this->setEnabled(mData->getEnabled());

	mCheckBox->setChecked(mData->getValue());
	mCheckBox->setToolTip(mData->getHelp());
	mLabel->setToolTip(mData->getHelp());

	mCheckBox->blockSignals(false);
}

// --------------------------------------------------------


}
