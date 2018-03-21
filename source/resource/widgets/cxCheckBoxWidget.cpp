/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
