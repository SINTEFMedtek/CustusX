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


#include "cxLabeledLineEditWidget.h"
#include <iostream>
#include "cxTypeConversions.h"
#include "cxLogger.h"


namespace cx
{

///----------------

LabeledLineEditWidget::LabeledLineEditWidget(QWidget* parent, StringPropertyBasePtr dataInterface,
	QGridLayout* gridLayout, int row) :
    OptimizedUpdateWidget(parent)
{
	CX_ASSERT(dataInterface->getAllowOnlyValuesInRange()==false);
	mData = dataInterface;
    connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setMargin(0);
	this->setLayout(topLayout);

	mLabel = new QLabel(this);
	mLabel->setText(mData->getDisplayName());
	topLayout->addWidget(mLabel);

	mLine = new QLineEdit(this);
	topLayout->addWidget(mLine);
	connect(mLine, SIGNAL(editingFinished()), this, SLOT(editingFinished()));

	if (gridLayout) // add to input gridlayout
	{
		gridLayout->addWidget(mLabel, row, 0);
		gridLayout->addWidget(mLine, row, 1);
	}
	else // add directly to this
	{
		topLayout->addWidget(mLabel);
		topLayout->addWidget(mLine, 1);
	}

    this->setModified();
}

void LabeledLineEditWidget::editingFinished()
{
	mData->setValue(mLine->text());
}

void LabeledLineEditWidget::prePaintEvent()
{
	mLine->blockSignals(true);
	mLine->setReadOnly(mData->isReadOnly());
	mLine->setText(mData->getValue());
	mLine->setToolTip(mData->getHelp());
	mLabel->setToolTip(mData->getHelp());
	mLine->blockSignals(false);
}

} // namespace cx
