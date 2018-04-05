/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
