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

/*
 * sscCheckBoxWidget.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#include <sscCheckBoxWidget.h>
#include <QGridLayout>

namespace ssc
{

CheckBoxWidget::CheckBoxWidget(QWidget* parent, ssc::BoolDataAdapterPtr dataInterface, QGridLayout* gridLayout, int row) :
    OptimizedUpdateWidget(parent)
{
	mData = dataInterface;
    connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setMargin(0);
	this->setLayout(topLayout);

	mCheckBox = new QCheckBox(this);
	mCheckBox->setText(dataInterface->getValueName());
	topLayout->addWidget(mCheckBox);
	connect(mCheckBox, SIGNAL(toggled(bool)), this, SLOT(valueChanged(bool)));

	if (gridLayout) // add to input gridlayout
	{
		gridLayout->addWidget(mCheckBox, row, 0, 1, 2);
	}
	else // add directly to this
	{
		topLayout->addWidget(mCheckBox);
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

	mCheckBox->setChecked(mData->getValue());
	mCheckBox->setToolTip(mData->getHelp());

	mCheckBox->blockSignals(false);
}

// --------------------------------------------------------


}
