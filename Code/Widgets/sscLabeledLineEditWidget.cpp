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

#include "sscLabeledLineEditWidget.h"
#include <iostream>
#include "sscTypeConversions.h"

namespace ssc
{

///----------------

LabeledLineEditWidget::LabeledLineEditWidget(QWidget* parent, ssc::EditableStringDataAdapterPtr dataInterface,
	QGridLayout* gridLayout, int row) :
    OptimizedUpdateWidget(parent)
{
	mData = dataInterface;
    connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setMargin(0);
	this->setLayout(topLayout);

	mLabel = new QLabel(this);
	mLabel->setText(mData->getValueName());
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

} // namespace ssc
