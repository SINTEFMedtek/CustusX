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

#include "sscLabeledComboBoxWidget.h"
#include <iostream>
#include "sscTypeConversions.h"
#include "sscHelperWidgets.h"
#include "sscLogger.h"

namespace ssc
{

///----------------

LabeledComboBoxWidget::LabeledComboBoxWidget(QWidget* parent, ssc::StringDataAdapterPtr dataInterface,
	QGridLayout* gridLayout, int row) :
    OptimizedUpdateWidget(parent)
{
	SSC_ASSERT(dataInterface->getAllowOnlyValuesInRange()==true);

	this->setEnabled(dataInterface->getEnabled());

	mData = dataInterface;
    connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));

	mLabel = new QLabel(this);
	mLabel->setText(mData->getValueName());

	mCombo = new QComboBox(this);
	connect(mCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));

	if (gridLayout) // add to input gridlayout
	{
        gridLayout->addLayout(mergeWidgetsIntoHBoxLayout(mLabel, addDummyMargin(this)), row, 0);
		gridLayout->addWidget(mCombo, row, 1);
	}
	else // add directly to this
	{
        QHBoxLayout* topLayout = new QHBoxLayout;
        topLayout->setMargin(0);
        this->setLayout(topLayout);

        topLayout->addWidget(mLabel);
		topLayout->addWidget(mCombo, 1);
	}

    this->setModified();
}

void LabeledComboBoxWidget::comboIndexChanged(int index)
{
	mData->setValue(mCombo->itemData(index).toString());
}

void LabeledComboBoxWidget::showLabel(bool on)
{
	mLabel->setVisible(on);
}

void LabeledComboBoxWidget::prePaintEvent()
{
	mCombo->blockSignals(true);
	mCombo->clear();

	this->setEnabled(mData->getEnabled());
	mLabel->setEnabled(mData->getEnabled());
	mCombo->setEnabled(mData->getEnabled());

	QString currentValue = mData->getValue();
	QStringList range = mData->getValueRange();
	int currentIndex = -1;
	for (int i = 0; i < range.size(); ++i)
	{
		mCombo->addItem(mData->convertInternal2Display(range[i]));
		mCombo->setItemData(i, range[i]);
		if (range[i] == currentValue)
			currentIndex = i;
		//      mCombo->setCurrentIndex(i);
	}
	mCombo->setCurrentIndex(currentIndex);

	mCombo->setToolTip(mData->getHelp());
	mLabel->setToolTip(mData->getHelp());
	mCombo->blockSignals(false);
}

} // namespace ssc
